#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "mux.h"
#include "param.h"
#include "estimator.h"
#include "arming_fsm.h"

namespace rosflight {

class Arming_FSM;
class CommLink;
class Estimator;

class Controller
{
public:
  void run_controller();
  void init_controller(Arming_FSM*_fsm, Board* _board, //Mux* _mux, Mixer* _mixer,
                       Estimator* _estimator, Params* _params);
private:
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
    float prev_x;
    float differentiator;
    float tau;
  } pid_t;

  pid_t pid_roll;
  pid_t pid_roll_rate;
  pid_t pid_pitch;
  pid_t pid_pitch_rate;
  pid_t pid_yaw_rate;
  pid_t pid_altitude;

  Estimator* estimator;
  Arming_FSM* fsm;
  Mux* mux;
  Mixer* mixer;
  Params* params;
  Board* board;
  CommLink* comm_link;

  void init_pid(pid_t *pid, uint16_t kp_param_id, uint16_t ki_param_id, uint16_t kd_param_id, float *current_x,
                float *current_xdot, float *commanded_x, float *output, float max, float min);
  void run_pid(pid_t *pid, float dt);

  float prev_time;

};
}
