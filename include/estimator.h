#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <math.h>

#include <turbovec.h>
#include <turbotrig.h>

#include "param.h"
#include "sensors.h"

namespace rosflight {

class Estimator
{

public:
  quaternion_t q;
  vector_t omega;
  float roll;
  float pitch;
  float yaw;
  float altitude;
  uint64_t now_us;
  vector_t _adaptive_gyro_bias;

  void reset_state();
  void reset_adaptive_bias();
  void init_estimator(Params *_params, Sensors *_sensors);
  void run_estimator();

private:
  Params* params_;
  Sensors* sensors_;

  vector_t w1;
  vector_t w2;
  vector_t wbar;
  vector_t wfinal;
  vector_t w_acc;
  const vector_t g = {0.0f, 0.0f, -1.0f};
  vector_t b;
  quaternion_t q_tilde;
  quaternion_t q_hat;
  uint64_t last_time;

  bool mat_exp;
  bool quad_int;
  bool use_acc;

  vector_t _accel_LPF;
  vector_t _gyro_LPF;

  void run_LPF();
};

}
