#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <turbovec.h>

#include "board.h"
#include "param.h"

namespace rosflight
{

// forward declarations of circular dependencies
class Mavlink;
class Params;

class Sensors
{

private:
  Board* board_;
  Params* params_;
  float accel[3];
  float gyro[3];

  bool calibrating_acc_flag;
  bool calibrating_gyro_flag;
  void calibrate_accel(void);
  void calibrate_gyro(void);
  void correct_imu(void);
  void correct_mag(void);
  void imu_ISR(void);
  bool update_imu(void);
  uint32_t last_time_look_for_disarmed_sensors;
  uint32_t last_imu_update_ms;
  uint16_t gyro_calibration_count;
  vector_t gyro_sum;
  uint16_t accel_calibration_count;
  vector_t acc_sum;
  const vector_t gravity = {0.0f, 0.0f, 9.80665f};
  float acc_temp_sum = 0.0f;


public:

  Sensors();

  // global variable declarations
  vector_t _accel;
  vector_t _gyro;
  float _imu_temperature;
  uint64_t _imu_time;
  bool new_imu_data;

  bool _diff_pressure_present;
  float _diff_pressure_velocity, _diff_pressure, _diff_pressure_temp;

  bool _baro_present;
  float _baro_altitude;
  float _baro_pressure;
  float _baro_temperature;

  bool _sonar_present;
  float _sonar_range;

  bool _mag_present;
  vector_t _mag;

  // function declarations
  void init_sensors(Board* _board, Params* _params);
  bool update_sensors();
  void IMU_ISR(void);

  bool start_imu_calibration(void);
  bool start_gyro_calibration(void);
  void start_baro_calibration(void);
  void start_airspeed_calibration(void);
  bool gyro_calibration_complete(void);
};

}
