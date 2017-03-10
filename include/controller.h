#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "mux.h"
#include "param.h"

namespace rosflight {

typedef struct
{
  uint16_t kp_param_id;
  uint16_t ki_param_id;
  uint16_t kd_param_id;

  float *current_x;
  float *current_xdot;
  float *commanded_x;
  float *output;

  float max;
  float min;

  float integrator;
  float prev_time;
  float prev_x;
  float differentiator;
  float tau;
} pid_t;

extern pid_t pid_roll;
extern pid_t pid_roll_rate;
extern pid_t pid_pitch;
extern pid_t pid_pitch_rate;
extern pid_t pid_yaw_rate;
extern pid_t pid_altitude;

void init_pid(pid_t *pid, uint16_t kp_param_id, uint16_t ki_param_id, uint16_t kd_param_id, float *current_x,
              float *current_xdot, float *commanded_x, float *output, float max, float min);
void run_pid(pid_t *pid, float dt);

void run_controller();
void init_controller();

}
