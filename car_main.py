"""
car_main.py — Raspberry Pi 3 Brain 
=======================================================================
"""

import curses
import serial
import threading
import time
import sys
import math
import csv
import os
import firebase_admin
from firebase_admin import credentials, db

# ── CONFIGURATION ─────────────────────────────────────────────────────────────
UART_PORT            = "/dev/serial0"
UART_BAUDRATE        = 9600
AEB_THRESHOLD_CM     = 25
DEFAULT_SPEED        = 85
TELEM_TIMEOUT_S      = 5.0
CHASSIS_LENGTH_CM    = 30
PARK_GAP_RATIO       = 1.5
PARK_SPEED           = 70
PARK_SCAN_CM         = 50     
PARK_OVERSHOOT_S     = 0.8   
PARK_ANGLE_S         = 0.65  
SENSOR_BLIND_CM      = 3
WHEEL_DIA_CM         = 6.5
TRACK_WIDTH_CM       = 20.0
SENSOR_DEATH_TIMEOUT = 3.0
LOG_FILE             = "ai_training_data.csv"
FIREBASE_DB_URL      = "https://autonomous-car-869bf-default-rtdb.firebaseio.com"

HB_BYTE = 0xFF

SPEED_ZONES = [
    (60, 9999, 100, "FULL"),
    (45,   60,  85, "75%"),
    (30,   45,  70, "50%"),
    (20,   30,  60, "CRAWL"),
    ( 1,   20,   0, "AEB"),
]

BACK_DANGER_CM  = 15
BACK_WARN_CM    = 30
BACK_CAUTION_CM = 50

AVOID_SPEED      = 75
AVOID_TURN_SPEED = 85
AVOID_MAX_TIME_S = 3.5
AVOID_COOLDOWN_S = 2.0

# ── PROTOCOL ──────────────────────────────────────────────────────────────────
CMD_STOP    = 0x00
CMD_FORWARD = 0x01
CMD_REVERSE = 0x02
CMD_LEFT    = 0x03
CMD_RIGHT   = 0x04
CMD_BEEP    = 0x05

CMD_NAMES = {
    CMD_STOP:    "STOP",
    CMD_FORWARD: "FORWARD",
    CMD_REVERSE: "REVERSE",
    CMD_LEFT:    "LEFT",
    CMD_RIGHT:   "RIGHT",
}

FIREBASE_TO_CMD = {
    'Forward': CMD_FORWARD, 'Reverse': CMD_REVERSE,
    'Left':    CMD_LEFT,    'Right':   CMD_RIGHT,   'Stop': CMD_STOP,
}

KEY_MAP = {
    ord("w"): CMD_FORWARD, ord("s"): CMD_REVERSE,
    ord("a"): CMD_LEFT,    ord("d"): CMD_RIGHT,    ord("x"): CMD_STOP,
}

# ── SHARED STATE ──────────────────────────────────────────────────────────────
class State:
    def __init__(self):
        self._lock           = threading.Lock()
        self.front           = 255
        self.back            = 255
        self.right           = 255
        self.left_rpm        = 0.0
        self.right_rpm       = 0.0
        self.telem_time      = 0.0
        self.active_cmd      = CMD_STOP
        self.active_speed    = DEFAULT_SPEED
        self.sent_speed      = 0
        self.x               = 0.0
        self.y               = 0.0
        self.theta           = 0.0
        self.status_conn     = "WAITING..."
        self.status_speed    = "FULL"
        self.status_back     = ""
        self.status_park     = "OFF"
        self.status_stall    = ""
        self.status_ai       = "RULE MODE (Pi 3 Optimized)"
        self.last_key        = "-"
        self.autopark_on     = False
        self._is_avoiding    = False
        self.avoid_cooldown_until = 0.0
        self.front_zero_time = 0.0
        self.front_dead      = False

state        = State()
stop_event   = threading.Event()
ser_global   = None
uart_lock    = threading.Lock()
stall_global = None

# ── HELPERS ───────────────────────────────────────────────────────────────────
def safe_uart_write(ser, *bts):
    if ser is None:
        return
    with uart_lock:
        try:
            ser.write(bytes(bts))
            ser.flush()          
        except Exception:
            pass

def apply_speed_limit(requested: int, front: int, cmd: int):
    if cmd != CMD_FORWARD:
        return requested, "FULL"
    if state.front_dead:
        return min(requested, 35), "LIMP MODE"
    if front == 0:           
        return requested, "FULL"
    for lo, hi, cap, label in SPEED_ZONES:
        if lo <= front < hi:
            return min(requested, cap), label
    return requested, "FULL"

def send_command(ser, cmd: int, speed: int = -1):
    with state._lock:
        if speed < 0:
            speed = state.active_speed
    speed         = max(0, min(100, speed))
    clamped, zone = apply_speed_limit(speed, state.front, cmd)
    clamped       = max(0, min(100, clamped))
    with state._lock:
        state.active_cmd   = cmd
        state.active_speed = speed
        state.sent_speed   = clamped
        state.status_speed = zone
    safe_uart_write(ser, cmd & 0xFF, clamped & 0xFF)

def send_beep(ser):
    safe_uart_write(ser, CMD_BEEP, 0x00)

def update_back_warning():
    with state._lock:
        cmd  = state.active_cmd
        back = state.back
    if cmd != CMD_REVERSE:
        with state._lock:
            state.status_back = ""
        return
    if   back > 0 and back < BACK_DANGER_CM:  msg = "!! DANGER - STOP NOW !!"
    elif back > 0 and back < BACK_WARN_CM:    msg = f"WARNING - object at {back} cm"
    elif back > 0 and back < BACK_CAUTION_CM: msg = f"CAUTION - object at {back} cm"
    else:                                      msg = ""
    with state._lock:
        state.status_back = msg

# ── THREADS ───────────────────────────────────────────────────────────────────
def heartbeat_thread(ser):
    while not stop_event.is_set():
        with state._lock:
            avoiding = state._is_avoiding
            cmd      = state.active_cmd
            spd      = state.sent_speed
        if avoiding:
            safe_uart_write(ser, HB_BYTE)
        elif cmd != CMD_STOP and spd > 0:
            safe_uart_write(ser, cmd & 0xFF, spd & 0xFF)
        else:
            safe_uart_write(ser, CMD_STOP, 0)
        stop_event.wait(0.2)

def receiver_thread(ser):
    buf    = bytearray()
    last_t = time.time()
    while not stop_event.is_set():
        try:
            byte = ser.read(1)
            if not byte:
                continue
            buf.append(byte[0])
            if len(buf) > 9:
                buf.pop(0)
            if len(buf) == 9 and buf[0] == 0xAA and buf[8] == 0x55:
                now    = time.time()
                dt     = now - last_t
                last_t = now
                lrpm      = float((buf[4] << 8) | buf[5])
                rrpm      = float((buf[6] << 8) | buf[7])
                new_front = buf[1]
                with state._lock:
                    cmd = state.active_cmd
                    if new_front == 0 and cmd == CMD_FORWARD:
                        state.front_zero_time += dt
                        if state.front_zero_time > SENSOR_DEATH_TIMEOUT:
                            state.front_dead = True
                    else:
                        state.front_zero_time = 0.0
                        state.front_dead      = False
                    dir_l = dir_r = 0
                    if   cmd == CMD_FORWARD: dir_l, dir_r =  1,  1
                    elif cmd == CMD_REVERSE: dir_l, dir_r = -1, -1
                    elif cmd == CMD_RIGHT:   dir_l, dir_r =  1, -1
                    elif cmd == CMD_LEFT:    dir_l, dir_r = -1,  1
                    v_l   = (lrpm / 60.0) * math.pi * WHEEL_DIA_CM * dir_l
                    v_r   = (rrpm / 60.0) * math.pi * WHEEL_DIA_CM * dir_r
                    v     = (v_l + v_r) / 2.0
                    omega = (v_r - v_l) / TRACK_WIDTH_CM
                    state.theta += omega * dt
                    state.x     += v * math.cos(state.theta) * dt
                    state.y     += v * math.sin(state.theta) * dt
                    state.front       = new_front
                    state.back        = buf[2]
                    state.right       = buf[3]
                    state.left_rpm    = lrpm
                    state.right_rpm   = rrpm
                    state.telem_time  = now
                    state.status_conn = "CONNECTED"
                buf.clear()
        except serial.SerialException:
            time.sleep(0.1)

def firebase_sync_thread():
    ref = db.reference('telemetry')
    while not stop_event.is_set():
        with state._lock:
            cmd    = state.active_cmd
            snt    = state.sent_speed
            sstall = state.status_stall
            apark  = state.autopark_on
            f_dead = state.front_dead
            front  = state.front
            back   = state.back
            right  = state.right
            lrpm   = state.left_rpm
            rrpm   = state.right_rpm
            x      = state.x
            y      = state.y
            theta  = state.theta
        if f_dead:
            html_status = "SENSOR FAULT (Limp Mode)"
        elif apark:
            html_status = "Parking"
        elif "!!" in sstall:
            html_status = "Object Detected"
        elif 0 < front < AEB_THRESHOLD_CM and cmd == CMD_FORWARD:
            html_status = "Object Detected"
        else:
            html_status = "Safe"
        try:
            ref.set({
                'battery_percent': 100,
                'front_distance':  front,
                'back_distance':   back,
                'right_distance':  right,
                'left_rpm':        lrpm,
                'right_rpm':       rrpm,
                'current_speed':   snt,
                'status':          html_status,
                'slam_x':          x,
                'slam_y':          y,
                'slam_theta':      theta,
                'front_dead':      f_dead,
            })
        except Exception:
            pass
        time.sleep(0.3)

def data_logger_thread():
    if not os.path.exists(LOG_FILE):
        with open(LOG_FILE, 'w', newline='') as f:
            csv.writer(f).writerow(['Front', 'Back', 'Right', 'LeftRPM', 'RightRPM', 'Command', 'Source'])
    while not stop_event.is_set():
        with state._lock:
            cmd      = state.active_cmd
            spd      = state.active_speed
            apark    = state.autopark_on
            avoiding = state._is_avoiding
            front    = state.front
            back     = state.back
            right    = state.right
            lrpm     = state.left_rpm
            rrpm     = state.right_rpm
        if not apark and spd > 0 and cmd != CMD_STOP:
            f   = SENSOR_BLIND_CM if front == 0 else front
            b   = SENSOR_BLIND_CM if back  == 0 else back
            r   = SENSOR_BLIND_CM if right == 0 else right
            src = "RULE" if avoiding else "HUMAN"
            try:
                with open(LOG_FILE, 'a', newline='') as fp:
                    csv.writer(fp).writerow([f, b, r, lrpm, rrpm, cmd, src])
            except OSError:
                pass
        time.sleep(0.1)

def voice_command_thread():
    voice_ref = db.reference('voice')
    last_ts = 0
    def push_response(text):
        print(f"[VOICE] Speaking: '{text}'")
        try:
            voice_ref.update({"response": text, "response_ts": int(time.time() * 1000)})
        except Exception:
            pass
    while not stop_event.is_set():
        try:
            voice_data = voice_ref.get()
            if voice_data:
                ts  = voice_data.get("timestamp", 0)
                cmd = voice_data.get("command", "Unknown")
                if ts > last_ts and cmd != "Unknown":
                    last_ts = ts
                    if cmd == "FrontDistance":
                        with state._lock: 
                            dist = state.front
                        if dist == 0: push_response("Front sensor is clear.")
                        elif dist < 15: push_response(f"Warning! Object at {dist} centimeters.")
                        else: push_response(f"Front sensor reads {dist} centimeters.")
                    elif cmd == "Battery":
                        push_response("Battery is at 100 percent. Power levels optimal.")
                    elif cmd == "Speed":
                        with state._lock: spd = state.sent_speed
                        push_response(f"Current motor speed is {spd} percent.")
                    elif cmd == "Status":
                        with state._lock:
                            if state.front_dead: st = "Sensor Fault, Limp Mode"
                            elif state.front > 0 and state.front < 25: st = "Object Detected"
                            else: st = "Safe"
                        push_response(f"Vehicle status is {st}.")
                    elif cmd in ["Forward", "Reverse", "Left", "Right", "Stop", "Park", "Autonomous", "Manual"]:
                        push_response(f"Executing {cmd.lower()} command.")
        except Exception as e:
            pass
        time.sleep(0.3)

# ── FIREBASE CALLBACKS ─────────────────────────────────────────────────────────
def fb_direction_handler(event):
    if not event.data: return
    val = event.data
    with state._lock:
        avoiding = state._is_avoiding
        apark    = state.autopark_on
    if avoiding and val != 'Stop': return
    if val == 'Park':
        state.autopark_on = True
        send_command(ser_global, CMD_STOP)
        return
    if val not in FIREBASE_TO_CMD or (apark and val != 'Stop'): return
    state.autopark_on = False
    new_cmd = FIREBASE_TO_CMD[val]
    if new_cmd == CMD_STOP:
        with state._lock:
            state.active_speed  = 0
            state._is_avoiding  = False
        if stall_global is not None: stall_global.reset()
        send_command(ser_global, CMD_STOP, 0)
    else:
        with state._lock:
            if state.active_speed == 0: state.active_speed = DEFAULT_SPEED
        send_command(ser_global, new_cmd)

def fb_speed_handler(event):
    if event.data is None: return
    spd = int(event.data)
    with state._lock:
        state.active_speed = spd
        cmd = state.active_cmd
    if cmd != CMD_STOP:
        send_command(ser_global, cmd, spd)

# ── RULE-BASED AVOIDANCE ──────────────────────────────────────────────────────
class StallDetector:
    def __init__(self, ser):
        self._ser = ser
        self.reset()
    def reset(self):
        self._state    = "IDLE"
        self._step     = 0
        self._avoid_t  = 0.0
        self._turn_dir = CMD_RIGHT
        with state._lock:
            state.status_stall = ""
    def is_avoiding(self):
        return self._state != "IDLE"
    def force_avoidance(self, right_dist: int):
        if self.is_avoiding() or state.front_dead: return
        send_beep(self._ser)
        send_command(self._ser, CMD_STOP)
        self._turn_dir = CMD_RIGHT if right_dist > 30 else CMD_LEFT
        self._state    = "AVOIDING"
        self._step     = 0
        self._avoid_t  = time.time()
        with state._lock: state.status_stall = "RULE: Wall — reversing"
    def tick(self):
        if self._state == "IDLE": return
        elapsed = time.time() - self._avoid_t
        with state._lock:
            front = state.front
            back  = state.back
            right = state.right
        if self._step == 0:
            with state._lock: state.status_stall = f"RULE: Reversing ({elapsed:.1f}s)"
            send_command(self._ser, CMD_REVERSE, AVOID_SPEED)
            back_danger = 0 < back < BACK_DANGER_CM
            if (elapsed >= 1.0 and front > AEB_THRESHOLD_CM + 10) or back_danger or elapsed > 3.0:
                send_command(self._ser, CMD_STOP)
                time.sleep(0.15)
                self._step    = 1
                self._avoid_t = time.time()
        elif self._step == 1:
            ds = "right" if self._turn_dir == CMD_RIGHT else "left"
            with state._lock: state.status_stall = f"RULE: Turning {ds} ({elapsed:.1f}s)"
            send_command(self._ser, self._turn_dir, AVOID_TURN_SPEED)
            front_clear = (front == 0) or (front > AEB_THRESHOLD_CM + 10)
            if front_clear or elapsed >= 0.9:
                send_command(self._ser, CMD_STOP)
                time.sleep(0.15)
                self._step    = 2
                self._avoid_t = time.time()
        elif self._step == 2:
            if 0 < front < AEB_THRESHOLD_CM:
                send_command(self._ser, CMD_STOP)
                time.sleep(0.10)
                with state._lock: state.status_stall = "RULE: Still blocked — more turning"
                self._step    = 1
                self._avoid_t = time.time()
                return
            with state._lock: state.status_stall = "RULE: Clearing obstacle"
            send_command(self._ser, CMD_FORWARD, AVOID_SPEED)
            front_clear = (front == 0) or (front > AEB_THRESHOLD_CM + 15)
            if front_clear or right > 45 or elapsed > 2.0:
                send_command(self._ser, CMD_STOP)
                time.sleep(0.15)
                self._step    = 3
                self._avoid_t = time.time()
        elif self._step == 3:
            ret = CMD_LEFT if self._turn_dir == CMD_RIGHT else CMD_RIGHT
            with state._lock: state.status_stall = "RULE: Correcting heading"
            send_command(self._ser, ret, AVOID_TURN_SPEED)
            if elapsed >= 0.8:
                send_command(self._ser, CMD_STOP)
                time.sleep(0.15)
                if 0 < front < AEB_THRESHOLD_CM:
                    with state._lock: state.status_stall = "RULE: Still blocked — restarting"
                    self._step    = 0       
                    self._avoid_t = time.time()
                    return
                with state._lock:
                    state.status_stall         = "RULE: Path clear — resuming"
                    state.avoid_cooldown_until = time.time() + AVOID_COOLDOWN_S
                spd = state.active_speed or DEFAULT_SPEED
                send_command(self._ser, CMD_FORWARD, spd)
                self.reset()

# ── AUTO-PARK ─────────────────────────────────────────────────────────────────
class AutoPark:
    def __init__(self, ser):
        self._ser   = ser
        self._gap_w = 0.0
        self.reset()
    def reset(self):
        self._state    = "SCANNING"
        self._was_wall = False
        self._phase_t  = None
        self._gap_w    = 0.0
        with state._lock: state.status_park = "SCANNING"
    def is_done(self):
        return self._state == "DONE"
    def tick(self):
        with state._lock:
            right = state.right
            back  = state.back
            front = state.front
        if self._state == "SCANNING":
            if 0 < right < PARK_SCAN_CM:
                self._was_wall = True
                self._phase_t  = None
                send_command(self._ser, CMD_FORWARD, PARK_SPEED)
                with state._lock: state.status_park = "SCANNING (passing parked car)"
                return
            if self._was_wall:
                if self._phase_t is None: self._phase_t = time.time()
                send_command(self._ser, CMD_FORWARD, PARK_SPEED)
                with state._lock: avg_rpm = (state.left_rpm + state.right_rpm) / 2.0
                speed_cms = max((avg_rpm / 60.0) * math.pi * WHEEL_DIA_CM, PARK_SPEED * 0.25)
                gap_width = (time.time() - self._phase_t) * speed_cms
                self._gap_w = gap_width
                with state._lock: state.status_park = f"GAP {gap_width:.0f} cm"
                if gap_width >= CHASSIS_LENGTH_CM * PARK_GAP_RATIO:
                    send_command(self._ser, CMD_STOP)
                    time.sleep(0.4)
                    self._state   = "OVERSHOOT"
                    self._phase_t = time.time()
                    with state._lock: state.status_park = "Positioning past gap..."
                return
            send_command(self._ser, CMD_FORWARD, PARK_SPEED)
            with state._lock: state.status_park = "SCANNING (searching for gap)"
        elif self._state == "OVERSHOOT":
            send_command(self._ser, CMD_FORWARD, PARK_SPEED)
            if time.time() - self._phase_t >= PARK_OVERSHOOT_S:
                send_command(self._ser, CMD_STOP)
                time.sleep(0.4)
                self._state   = "ANGLE_R"
                self._phase_t = time.time()
                with state._lock: state.status_park = "Angling into spot (right)..."
        elif self._state == "ANGLE_R":
            send_command(self._ser, CMD_RIGHT, AVOID_TURN_SPEED)
            if time.time() - self._phase_t >= PARK_ANGLE_S:
                send_command(self._ser, CMD_STOP)
                time.sleep(0.3)
                self._state   = "REVERSING"
                self._phase_t = time.time()
                with state._lock: state.status_park = "Reversing into spot..."
        elif self._state == "REVERSING":
            send_command(self._ser, CMD_REVERSE, PARK_SPEED)
            spot_reached = 0 < back < CHASSIS_LENGTH_CM // 2
            timeout      = time.time() - self._phase_t >= 2.5
            if spot_reached or timeout:
                send_command(self._ser, CMD_STOP)
                time.sleep(0.3)
                self._state   = "ANGLE_L"
                self._phase_t = time.time()
                with state._lock: state.status_park = "Straightening (left)..."
        elif self._state == "ANGLE_L":
            send_command(self._ser, CMD_LEFT, AVOID_TURN_SPEED)
            if time.time() - self._phase_t >= PARK_ANGLE_S:
                send_command(self._ser, CMD_STOP)
                time.sleep(0.3)
                self._state   = "CENTERING"
                self._phase_t = time.time()
                with state._lock: state.status_park = "ACTIVE CENTERING"
        elif self._state == "CENTERING":
            if front == 0 and back == 0:
                if time.time() - self._phase_t > 3.0:
                    send_command(self._ser, CMD_STOP)
                    self._state = "DONE"
                    with state._lock: state.status_park = "PARKED (Sensor Timeout)"
                return
            f    = SENSOR_BLIND_CM if front == 0 else front
            b    = SENSOR_BLIND_CM if back  == 0 else back
            diff = f - b
            done = abs(diff) <= 5 or time.time() - self._phase_t > 4.0
            if done:
                send_command(self._ser, CMD_STOP)
                self._state = "DONE"
                with state._lock: state.status_park = "PARKED PERFECTLY!"
            elif diff > 0:
                send_command(self._ser, CMD_FORWARD, 55)
                with state._lock: state.status_park = f"CENTERING fwd (d{diff}cm)"
            else:
                send_command(self._ser, CMD_REVERSE, 55)
                with state._lock: state.status_park = f"CENTERING rev (d{abs(diff)}cm)"

# ── TERMINAL DASHBOARD ─────────────────────────────────────────────────────────
BAR_W = 18
def bar(v: float, m: float) -> str:
    n = max(0, min(BAR_W, int((v / m) * BAR_W) if m else 0))
    return "[" + "#" * n + "." * (BAR_W - n) + "]"

def draw(scr):
    scr.erase()
    h, w = scr.getmaxyx()
    G = curses.color_pair(1)
    Y = curses.color_pair(2)
    R = curses.color_pair(3)
    C = curses.color_pair(4)
    N = curses.A_NORMAL
    with state._lock:
        front   = state.front;    back    = state.back;    right  = state.right
        lrpm    = state.left_rpm; rrpm    = state.right_rpm
        telem_t = state.telem_time
        cmd     = state.active_cmd
        aspd    = state.active_speed;  snt  = state.sent_speed
        sspd    = state.status_speed;  sback = state.status_back
        sstall  = state.status_stall;  spark = state.status_park
        sai     = state.status_ai;    lkey  = state.last_key
        f_dead  = state.front_dead;   apark = state.autopark_on
    connected  = (time.time() - telem_t if telem_t else 999) <= TELEM_TIMEOUT_S
    aeb_active = connected and 0 < front < AEB_THRESHOLD_CM and not f_dead
    speed_attr = {"FULL": G, "75%": G, "50%": Y | curses.A_BOLD, "CRAWL": R | curses.A_BOLD, "AEB": R | curses.A_BOLD, "LIMP MODE": R | curses.A_BOLD}.get(sspd, N)
    back_attr  = (R | curses.A_BOLD if "DANGER" in sback else Y | curses.A_BOLD if "WARN" in sback else Y if "CAUT" in sback else G)
    stall_attr = (R | curses.A_BOLD if "!!" in sstall else Y | curses.A_BOLD if sstall else G)
    ai_attr    = (Y | curses.A_BOLD)
    def da(v): return (Y | curses.A_BOLD if v == 0 else R | curses.A_BOLD if v < 20 else Y | curses.A_BOLD if v < 40 else G)
    row = [0]
    def put(t, a=N):
        if row[0] < h - 1:
            try: scr.addstr(row[0], 0, t[:w - 1], a)
            except curses.error: pass
        row[0] += 1
    def sep(): put("+" + "-" * 58 + "+")
    sep()
    put("|   4WD CAR  --  TERMINAL DASHBOARD (Pi 3 Edition)        |", C | curses.A_BOLD)
    sep()
    put("| SAFETY & SYSTEM STATUS                                   |", C)
    put(f"|  UART      : {'CONNECTED' if connected else '!! NO TELEMETRY !!':<42}|", G | curses.A_BOLD if connected else Y | curses.A_BOLD)
    put(f"|  Sensor    : {'FAULT (Limp Mode)' if f_dead else 'HEALTHY':<42}|", R | curses.A_BOLD if f_dead else G | curses.A_BOLD)
    put(f"|  AI Mode   : {sai:<42}|", ai_attr)
    put(f"|  Front AEB : {'BRAKING' if aeb_active else 'CLEAR':<42}|", R | curses.A_BOLD if aeb_active else G)
    put(f"|  Spd Zone  : {sspd} - sending {snt}% (want {aspd}%){'':>12}|", speed_attr)
    put(f"|  Rear Prox : {(sback or 'CLEAR'):<42}|", back_attr)
    put(f"|  Avoidance : {(sstall or 'OK'):<42}|", stall_attr)
    sep()
    put("| COMMAND                                                   |")
    put(f"|   Direction : {CMD_NAMES.get(cmd, '?'):<44}|")
    put(f"|   Requested : {bar(aspd, 100)}  {aspd:>3}%              |")
    put(f"|   Actual    : {bar(snt,  100)}  {snt:>3}%  <- limiter        |")
    sep()
    put("| ULTRASONIC DISTANCES                                      |")
    for label, val in [("Front", front), ("Back ", back), ("Right", right)]:
        if row[0] < h - 1:
            try:
                scr.addstr(row[0], 0, f"|   {label} : {bar(min(val, 100), 100)}  ")
                scr.addstr(f"{val:>4} cm", da(val))
                scr.addstr("          |")
            except curses.error: pass
        row[0] += 1
    sep()
    put("| WHEEL RPM & ODOMETRY                                      |")
    put(f"|   Left  : {bar(lrpm, 300)}  {lrpm:>5.0f} RPM           |")
    put(f"|   Right : {bar(rrpm, 300)}  {rrpm:>5.0f} RPM           |")
    sep()
    park_attr = (G if "PERFECTLY" in spark else C if spark not in ("OFF", "PARKED!") else Y)
    put(f"|  AUTO-PARK  : {spark:<43}|", park_attr)
    sep()
    put("| KEYS: w=Fwd  s=Rev  a=Left  d=Right  x=Stop              |", C)
    put("|       p=Park  r=Reset odometry  +=Spd+  -=Spd-  q=Quit   |", C)
    put(f"|  Last key: [ {lkey} ]                                      |")
    sep()
    scr.refresh()

# ── CURSES MAIN ───────────────────────────────────────────────────────────────
def curses_main(scr):
    global stall_global
    curses.start_color()
    curses.use_default_colors()
    for i, c in enumerate([curses.COLOR_GREEN, curses.COLOR_YELLOW, curses.COLOR_RED, curses.COLOR_CYAN]):
        curses.init_pair(i + 1, c, -1)
    curses.curs_set(0)
    scr.nodelay(True)
    scr.timeout(50)

    ser   = ser_global
    park  = AutoPark(ser)
    stall = StallDetector(ser)
    stall_global = stall

    while not stop_event.is_set():
        update_back_warning()
        with state._lock:
            cmd         = state.active_cmd
            apark       = state.autopark_on
            front       = state.front
            right       = state.right          
            cooldown_ok = time.time() > state.avoid_cooldown_until
            f_dead      = state.front_dead
        with state._lock:
            state._is_avoiding = stall.is_avoiding()

        if (cmd == CMD_FORWARD and 0 < front < AEB_THRESHOLD_CM
                and not apark and not stall.is_avoiding()
                and cooldown_ok and not f_dead):
            with state._lock: state._is_avoiding = True
            stall.force_avoidance(right)       

        if not apark: stall.tick()
        else: stall.reset()

        with state._lock: apark = state.autopark_on
        if apark and not park.is_done(): park.tick()
        elif park.is_done(): state.autopark_on = False

        draw(scr)

        key = scr.getch()
        if key == -1: continue

        with state._lock: state.last_key = chr(key) if 32 <= key < 127 else f"#{key}"
        with state._lock: apark = state.autopark_on
        avoiding = stall.is_avoiding()

        if key == ord("q"):
            stop_event.set()
            break
        elif key == ord("p"):
            state.autopark_on = not state.autopark_on
            if state.autopark_on: park.reset(); stall.reset()
            else:
                with state._lock: state.status_park = "OFF"
                send_command(ser, CMD_STOP)
        elif key == ord("r"):
            with state._lock: state.x = state.y = state.theta = 0.0
        elif key == ord("x"):
            with state._lock:
                state.active_speed = 0
                state._is_avoiding = False
            stall.reset()
            send_command(ser, CMD_STOP, 0)
        elif not apark and not avoiding:
            if key in KEY_MAP:
                new_cmd = KEY_MAP[key]
                if new_cmd != CMD_STOP:
                    with state._lock:
                        if state.active_speed == 0: state.active_speed = DEFAULT_SPEED
                else:
                    with state._lock: state.active_speed = 0
                send_command(ser, new_cmd)
                if new_cmd not in (CMD_FORWARD, CMD_STOP): stall.reset()
            elif key == ord("+"):
                with state._lock:
                    state.active_speed = min(100, state.active_speed + 10)
                    c, s = state.active_cmd, state.active_speed
                send_command(ser, c, s)
            elif key == ord("-"):
                with state._lock:
                    state.active_speed = max(0, state.active_speed - 10)
                    c, s = state.active_cmd, state.active_speed
                send_command(ser, c, s)

# ── ENTRY POINT ───────────────────────────────────────────────────────────────
def main():
    global ser_global
    print(f"Connecting to PIC on {UART_PORT} @ {UART_BAUDRATE} baud ...")
    try:
        cred = credentials.Certificate("serviceAccountKey.json")
        firebase_admin.initialize_app(cred, {'databaseURL': FIREBASE_DB_URL})
        print("Firebase connected.")
        db.reference('controls/direction').listen(fb_direction_handler)
        db.reference('controls/max_speed').listen(fb_speed_handler)
    except Exception as e:
        print(f"[WARNING] Firebase disabled: {e}")
    try:
        ser_global = serial.Serial(
            port=UART_PORT, baudrate=UART_BAUDRATE,
            bytesize=serial.EIGHTBITS, parity=serial.PARITY_NONE,
            stopbits=serial.STOPBITS_ONE, timeout=1.0,
        )
    except serial.SerialException as e:
        print(f"\n[ERROR] {e}")
        sys.exit(1)

    threads = [
        (receiver_thread,      "RX",     (ser_global,)),
        (heartbeat_thread,     "HB",     (ser_global,)),
        (firebase_sync_thread, "FB",     ()),
        (data_logger_thread,   "LOGGER", ()),
        (voice_command_thread, "VOICE",  ()),
    ]
    for fn, name, args in threads:
        threading.Thread(target=fn, args=args, daemon=True, name=name).start()

    send_command(ser_global, CMD_STOP)

    try:
        curses.wrapper(curses_main)
    finally:
        stop_event.set()
        if ser_global:
            try:
                safe_uart_write(ser_global, CMD_STOP, 0)
                time.sleep(0.2)
                ser_global.close()
            except Exception: pass
        print("Stopped. Goodbye.")

if __name__ == "__main__":
    main()