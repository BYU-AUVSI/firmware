#include <stdbool.h>
#include <stdint.h>

#include "sensors.h"
#include "arming_fsm.h"

#include <turbovec.h>

rosflight::Sensors* IMU_ptr;
void IMU_ISR_wrapper(void)
{
  IMU_ptr->IMU_ISR();
}

namespace rosflight
{

Sensors::Sensors(){}

void Sensors::init_sensors(Board *_board, Params *_params)
{
  board_ = _board;
  params_ = _params;
  IMU_ptr = this;
  new_imu_data = false;
  board_->sensors_init(params_->get_param_int(PARAM_BOARD_REVISION));
  board_->imu_register_callback(&IMU_ISR_wrapper);
}


bool Sensors::update_sensors()
{
  // Look for disabled sensors while disarmed (poll every 0.5 seconds)
  // These sensors need power to respond, so they might not have been
  // detected on startup, but will be detected whenever power is applied
  // to the 5V rail.
  //  if ((_armed_state & ARMED) == 0)
  if(0)
  {
    uint32_t now = board_->clock_millis();
    if (now > (last_time_look_for_disarmed_sensors + 500))
    {
      last_time_look_for_disarmed_sensors = now;
      if(!board_->sonar_present())
      {
        if(board_->sonar_check())
        {
          //          mavlink_log_info("FOUND SONAR", NULL);
        }
      }
      if(!board_->diff_pressure_present())
      {
        if(board_->diff_pressure_check())
        {
          //          mavlink_log_info("FOUND DIFF PRESS", NULL);
        }
      }
      if (!board_->mag_present())
      {
        if (board_->mag_check())
        {
          //          mavlink_log_info("FOUND MAG", NULL);
        }
      }
    }
  }

  if(board_->baro_present())
  {
    board_->baro_read(&_baro_altitude, &_baro_pressure, &_baro_temperature);
  }

  if(board_->diff_pressure_present())
  {
    if(board_->baro_present())
    {
      board_->diff_pressure_set_atm(_baro_pressure);
    }
    board_->diff_pressure_read(&_diff_pressure, &_diff_pressure_temp, &_diff_pressure_velocity);
  }

  if (board_->sonar_present())
  {
    _sonar_range = board_->sonar_read();
  }

  if (board_->mag_present())
  {
    float mag[3];
    board_->mag_read(mag);
    _mag.x = mag[0];
    _mag.y = mag[1];
    _mag.z = mag[2];
    correct_mag();
  }

  // Return whether or not we got new IMU data
  return update_imu();
}


bool Sensors::start_imu_calibration(void)
{
  start_gyro_calibration();

  calibrating_acc_flag = true;
  params_->set_param_float(PARAM_ACC_X_BIAS, 0.0);
  params_->set_param_float(PARAM_ACC_Y_BIAS, 0.0);
  params_->set_param_float(PARAM_ACC_Z_BIAS, 0.0);
  return true;
}

bool Sensors::start_gyro_calibration(void)
{
  calibrating_gyro_flag = true;
  params_->set_param_float(PARAM_GYRO_X_BIAS, 0.0);
  params_->set_param_float(PARAM_GYRO_Y_BIAS, 0.0);
  params_->set_param_float(PARAM_GYRO_Z_BIAS, 0.0);
  return true;
}

bool Sensors::gyro_calibration_complete(void)
{
  return !calibrating_gyro_flag;
}




//==================================================================
// local function definitions
void Sensors::IMU_ISR(void)
{
  _imu_time = board_->clock_micros();
  new_imu_data = true;
}


bool Sensors::update_imu(void)
{
  if (new_imu_data)
  {
    last_imu_update_ms = board_->clock_millis();
    board_->imu_read_accel(accel);
    board_->imu_read_gyro(gyro);
    _imu_temperature = board_->imu_read_temperature();
    new_imu_data = false;

    _accel.x = accel[0] * params_->get_param_float(PARAM_ACCEL_SCALE);
    _accel.y = accel[1] * params_->get_param_float(PARAM_ACCEL_SCALE);
    _accel.z = accel[2] * params_->get_param_float(PARAM_ACCEL_SCALE);

    _gyro.x = gyro[0];
    _gyro.y = gyro[1];
    _gyro.z = gyro[2];

    if (calibrating_acc_flag == true)
      calibrate_accel();
    if (calibrating_gyro_flag)
      calibrate_gyro();

    correct_imu();
    return true;
  }
  else
  {
    // if we have lost 1000 IMU messages then something is wrong
    if(board_->clock_millis() > last_imu_update_ms + 1000)
    {
      // change board revision and reset IMU
      last_imu_update_ms = board_->clock_millis();
      params_->set_param_int(PARAM_BOARD_REVISION, (params_->get_param_int(PARAM_BOARD_REVISION) >= 4) ? 2 : 5);
      board_->sensors_init(params_->get_param_int(PARAM_BOARD_REVISION));
    }
    return false;
  }
}


void Sensors::calibrate_gyro()
{
  gyro_sum.x = 0.0f;
  gyro_sum.y = 0.0f;
  gyro_sum.z = 0.0f;
  gyro_sum = vector_add(gyro_sum, _gyro);
  gyro_calibration_count++;

  if (gyro_calibration_count > 100)
  {
    // Gyros are simple.  Just find the average during the calibration
    vector_t gyro_bias = scalar_multiply(1.0/(float)gyro_calibration_count, gyro_sum);

    if (sqrd_norm(gyro_bias) < 1.0)
    {
      params_->set_param_float(PARAM_GYRO_X_BIAS, gyro_bias.x);
      params_->set_param_float(PARAM_GYRO_Y_BIAS, gyro_bias.y);
      params_->set_param_float(PARAM_GYRO_Z_BIAS, gyro_bias.z);

      // Tell the estimator to reset it's bias estimate, because it should be zero now
//      reset_adaptive_bias();
    }
    else
    {
      //      mavlink_log_error("Too much movement for gyro cal", NULL);
    }

    // reset calibration in case we do it again
    calibrating_gyro_flag = false;
    gyro_calibration_count = 0;
    gyro_sum.x = 0.0f;
    gyro_sum.y = 0.0f;
    gyro_sum.z = 0.0f;
  }
}


void Sensors::calibrate_accel(void)
{
  acc_sum.x = 0.0f;
  acc_sum.y = 0.0f;
  acc_sum.z = 0.0f;
  acc_temp_sum = 0.0f;

  acc_sum = vector_add(vector_add(acc_sum, _accel), gravity);
  acc_temp_sum += _imu_temperature;
  accel_calibration_count++;

  if (accel_calibration_count > 1000)
  {
    // The temperature bias is calculated using a least-squares regression.
    // This is computationally intensive, so it is done by the onboard computer in
    // fcu_io and shipped over to the flight controller.
    vector_t accel_temp_bias =
    {
      params_->get_param_float(PARAM_ACC_X_TEMP_COMP),
      params_->get_param_float(PARAM_ACC_Y_TEMP_COMP),
      params_->get_param_float(PARAM_ACC_Z_TEMP_COMP)
    };

    // Figure out the proper accel bias.
    // We have to consider the contribution of temperature during the calibration,
    // Which is why this line is so confusing. What we are doing, is first removing
    // the contribution of temperature to the measurements during the calibration,
    // Then we are dividing by the number of measurements.
    vector_t accel_bias = scalar_multiply(1.0/(float)accel_calibration_count, vector_sub(acc_sum, scalar_multiply(acc_temp_sum, accel_temp_bias)));

    // Sanity Check -
    // If the accelerometer is upside down or being spun around during the calibration,
    // then don't do anything
    if (sqrd_norm(accel_bias) < 4.5)
    {
      params_->set_param_float(PARAM_ACC_X_BIAS, accel_bias.x);
      params_->set_param_float(PARAM_ACC_Y_BIAS, accel_bias.y);
      params_->set_param_float(PARAM_ACC_Z_BIAS, accel_bias.z);
      //      mavlink_log_info("IMU offsets captured", NULL);

      // reset the estimated state
      //      reset_state();
      calibrating_acc_flag = false;
    }
    else
    {
      // check for bad _accel_scale
      if (sqrd_norm(accel_bias) > 4.5*4.5 && sqrd_norm(accel_bias) < 5.5*5.5)
      {
        //        mavlink_log_error("Detected bad IMU accel scale value", 0);
        params_->set_param_float(PARAM_ACCEL_SCALE, 2.0 * params_->get_param_float(PARAM_ACCEL_SCALE));
        params_->write_params();
      }
      else if (sqrd_norm(accel_bias) > 9.0*9.0 && sqrd_norm(accel_bias) < 11.0*11.0)
      {
        //        mavlink_log_error("Detected bad IMU accel scale value", 0);
        params_->set_param_float(PARAM_ACCEL_SCALE, 0.5 * params_->get_param_float(PARAM_ACCEL_SCALE));
        params_->write_params();
      }
      else
      {
        //        mavlink_log_error("Too much movement for IMU cal", NULL);
        calibrating_acc_flag = false;
      }
    }

    // reset calibration in case we do it again
    accel_calibration_count = 0;
    acc_sum.x = 0.0f;
    acc_sum.y = 0.0f;
    acc_sum.z = 0.0f;
    acc_temp_sum = 0.0f;
  }
}

void Sensors::correct_imu(void)
{
  // correct according to known biases and temperature compensation
  _accel.x -= params_->get_param_float(PARAM_ACC_X_TEMP_COMP)*_imu_temperature + params_->get_param_float(PARAM_ACC_X_BIAS);
  _accel.y -= params_->get_param_float(PARAM_ACC_Y_TEMP_COMP)*_imu_temperature + params_->get_param_float(PARAM_ACC_Y_BIAS);
  _accel.z -= params_->get_param_float(PARAM_ACC_Z_TEMP_COMP)*_imu_temperature + params_->get_param_float(PARAM_ACC_Z_BIAS);

  _gyro.x -= params_->get_param_float(PARAM_GYRO_X_BIAS);
  _gyro.y -= params_->get_param_float(PARAM_GYRO_Y_BIAS);
  _gyro.z -= params_->get_param_float(PARAM_GYRO_Z_BIAS);
}

void Sensors::correct_mag(void)
{
  // correct according to known hard iron bias
  float mag_hard_x = _mag.x - params_->get_param_float(PARAM_MAG_X_BIAS);
  float mag_hard_y = _mag.y - params_->get_param_float(PARAM_MAG_Y_BIAS);
  float mag_hard_z = _mag.z - params_->get_param_float(PARAM_MAG_Z_BIAS);

  // correct according to known soft iron bias - converts to nT
  _mag.x = params_->get_param_float(PARAM_MAG_A11_COMP)*mag_hard_x + params_->get_param_float(PARAM_MAG_A12_COMP)*mag_hard_y +
      params_->get_param_float(PARAM_MAG_A13_COMP)*mag_hard_z;
  _mag.y = params_->get_param_float(PARAM_MAG_A21_COMP)*mag_hard_x + params_->get_param_float(PARAM_MAG_A22_COMP)*mag_hard_y +
      params_->get_param_float(PARAM_MAG_A23_COMP)*mag_hard_z;
  _mag.z = params_->get_param_float(PARAM_MAG_A31_COMP)*mag_hard_x + params_->get_param_float(PARAM_MAG_A32_COMP)*mag_hard_y +
      params_->get_param_float(PARAM_MAG_A33_COMP)*mag_hard_z;
}

}
