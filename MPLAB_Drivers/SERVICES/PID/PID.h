#ifndef PID_H
#define PID_H

#include "../STD_TYPES.h"

/* PID State Structure */
typedef struct {
    f32 kp;
    f32 ki;
    f32 kd;
    f32 previous_error;
    f32 integral;
    f32 output_limit;
} PID_Controller_t;

void PID_Init(PID_Controller_t *pid, f32 kp, f32 ki, f32 kd, f32 limit);
f32 PID_Compute(PID_Controller_t *pid, f32 setpoint, f32 measured_value, f32 dt);

#endif /* PID_H */