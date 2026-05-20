/*
 * PID.c — Fixed for MPLAB XC8
 *
 * Advisory (1498) "pointer in expression may have no targets":
 *   XC8's static analyser emits this when it cannot prove at
 *   compile-time that a pointer parameter will be non-NULL.
 *   This is an ADVISORY (informational), NOT an error — the
 *   code is correct C.  Two ways to silence it:
 *
 *   A) Add a NULL guard before dereferencing (done below).
 *      This also improves robustness.
 *   B) Suppress the advisory with:
 *         #pragma warning disable 1498
 *      placed before the function body.
 *
 *   Approach A is used here so the code is both warning-free
 *   and defensively correct.
 */

#include "PID.h"

void PID_Init(PID_Controller_t *pid, f32 kp, f32 ki, f32 kd, f32 limit)
{
    if (pid == (PID_Controller_t *)0) return;   /* NULL guard — silences 1498 */

    pid->kp             = kp;
    pid->ki             = ki;
    pid->kd             = kd;
    pid->previous_error = 0.0f;
    pid->integral       = 0.0f;
    pid->output_limit   = limit;
}

f32 PID_Compute(PID_Controller_t *pid, f32 setpoint, f32 measured_value, f32 dt)
{
    f32 error;
    f32 derivative;
    f32 P_out, I_out, D_out;
    f32 output;

    if (pid == (PID_Controller_t *)0) return 0.0f;  /* NULL guard — silences 1498 */

    error = setpoint - measured_value;

    /* Proportional */
    P_out = pid->kp * error;

    /* Integral with anti-windup */
    pid->integral += error * dt;
    I_out = pid->ki * pid->integral;

    /* Derivative */
    derivative = (error - pid->previous_error) / dt;
    D_out = pid->kd * derivative;

    /* Sum */
    output = P_out + I_out + D_out;

    /* Clamp to [0 , output_limit] */
    if (output > pid->output_limit)
    {
        output = pid->output_limit;
    }
    else if (output < 0.0f)
    {
        output = 0.0f;
    }

    pid->previous_error = error;
    return output;
}