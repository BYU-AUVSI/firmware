/*
 *
 * BSD 3-Clause License
 *
 * Copyright (c) 2017, James Jackson and Daniel Koch, BYU MAGICC Lab, Provo UT
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * * Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdbool.h>

#include "board.h"
#include "mavlink.h"
#include "mixer.h"
#include "sensors.h"
#include "estimator.h"
#include "param.h"
#include "mode.h"
#include "rc.h"
#include "mavlink_log.h"

namespace rosflight
{

// local function definitions
 void Mavlink::mavlink_send_heartbeat(void)
{
  MAV_MODE armed_mode = MAV_MODE_ENUM_END; // used for failsafe
//  if (_armed_state == ARMED)
//    armed_mode = MAV_MODE_MANUAL_ARMED;
//  else if (_armed_state == DISARMED)
//    armed_mode = MAV_MODE_MANUAL_DISARMED;

  uint8_t control_mode = 0;
//  if (params_->get_param_int(PARAM_FIXED_WING))
//  {
//    control_mode = MODE_PASS_THROUGH;
//  }
//  else if (rc_switch(params_->get_param_int(PARAM_RC_F_CONTROL_TYPE_CHANNEL)))
//  {
//    control_mode = MODE_ROLL_PITCH_YAWRATE_ALTITUDE;
//  }
//  else
//  {
//    control_mode = rc_switch(params_->get_param_int(PARAM_RC_ATT_CONTROL_TYPE_CHANNEL)) ? MODE_ROLL_PITCH_YAWRATE_THROTTLE :
//                   MODE_ROLLRATE_PITCHRATE_YAWRATE_THROTTLE;
//  }

  mavlink_message_t msg;
  mavlink_msg_heartbeat_pack(RF_->params_.get_param_int(PARAM_SYSTEM_ID), 0, &msg,
                             RF_->params_.get_param_int(PARAM_FIXED_WING) ? MAV_TYPE_FIXED_WING : MAV_TYPE_QUADROTOR,
                             MAV_AUTOPILOT_GENERIC,
                             armed_mode,
                             control_mode,
                             MAV_STATE_STANDBY);
  send_message(msg);
}

 void Mavlink::mavlink_send_attitude(void)
{
  mavlink_message_t msg;
  mavlink_msg_attitude_quaternion_pack(sysid, compid, &msg,
                                        RF_->estimator_.get_estimator_timestamp()/1000,
                                        RF_->estimator_.get_attitude().w,
                                        RF_->estimator_.get_attitude().x,
                                        RF_->estimator_.get_attitude().y,
                                        RF_->estimator_.get_attitude().z,
                                        RF_->estimator_.get_angular_velocity().x,
                                        RF_->estimator_.get_angular_velocity().y,
                                        RF_->estimator_.get_angular_velocity().z);
  send_message(msg);
}

void Mavlink::mavlink_send_imu(void)
{
  mavlink_message_t msg;
  vector_t accel = RF_->sensors_.get_accel();
  vector_t gyro = RF_->sensors_.get_gyro();
  mavlink_msg_small_imu_pack(sysid, compid, &msg,
                             RF_->sensors_.get_imu_time(),
                             accel.x,
                             accel.y,
                             accel.z,
                             gyro.x,
                             gyro.y,
                             gyro.z,
                             RF_->sensors_.get_imu_temp());
  send_message(msg);

}

void Mavlink::mavlink_send_servo_output_raw(void)
{
  mavlink_message_t msg;
//  mavlink_msg_servo_output_raw_pack(sysid, compid, &msg,
//                                    board_->clock_micros(),
//                                    0,
//                                    _outputs[0],
//                                    _outputs[1],
//                                    _outputs[2],
//                                    _outputs[3],
//                                    _outputs[4],
//                                    _outputs[5],
//                                    _outputs[6],
//                                    _outputs[7]);
//  send_message(msg);
}

 void Mavlink::mavlink_send_rc_raw(void)
{
  mavlink_message_t msg;
  mavlink_msg_rc_channels_pack(sysid, compid, &msg,
                               RF_->board_->clock_millis(),
                               0,
                               RF_->board_->pwm_read(0),
                               RF_->board_->pwm_read(1),
                               RF_->board_->pwm_read(2),
                               RF_->board_->pwm_read(3),
                               RF_->board_->pwm_read(4),
                               RF_->board_->pwm_read(5),
                               RF_->board_->pwm_read(6),
                               RF_->board_->pwm_read(7),
                               0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 0);
  send_message(msg);
}

void Mavlink::mavlink_send_diff_pressure(void)
{
  if (RF_->board_->diff_pressure_present())
  {
    mavlink_message_t msg;
    mavlink_msg_diff_pressure_pack(sysid, compid, &msg,
                                   RF_->sensors_.get_differential_pressure_velocity(),
                                   RF_->sensors_.get_differential_pressure(),
                                   RF_->sensors_.get_differential_pressure_temperature());
    send_message(msg);
  }
}

void Mavlink::mavlink_send_baro(void)
{
  if (RF_->board_->baro_present())
  {
    mavlink_message_t msg;
    mavlink_msg_small_baro_pack(sysid, compid, &msg,
                                RF_->sensors_.get_barometer_altitude(),
                                RF_->sensors_.get_barometer_pressure(),
                                RF_->sensors_.get_barometer_temperature());
    send_message(msg);
  }
}

void Mavlink::mavlink_send_sonar(void)
{
  if (RF_->board_->sonar_present())
  {
    mavlink_message_t msg;
    mavlink_msg_small_sonar_pack(sysid, compid, &msg,
                                 RF_->sensors_.get_sonar_range(),
                                 8.0,
                                 0.25);
    send_message(msg);
  }
}

void Mavlink::mavlink_send_mag(void)
{
  if (RF_->board_->mag_present())
  {
    mavlink_message_t msg;
    mavlink_msg_small_mag_pack(sysid, compid, &msg,
                               RF_->sensors_.get_mag().x,
                               RF_->sensors_.get_mag().y,
                               RF_->sensors_.get_mag().z);
    send_message(msg);
  }
}

void Mavlink::mavlink_send_low_priority(void)
{
  mavlink_send_next_param();
}

// function definitions
void Mavlink::stream()
{
  uint64_t time_us = RF_->board_->clock_micros();
  for (int i = 0; i < STREAM_COUNT; i++)
  {
    if (mavlink_streams[i].period_us && time_us - mavlink_streams[i].last_time_us >= mavlink_streams[i].period_us)
    {
      // if we took too long, set the last_time_us to be where it should have been
      mavlink_streams[i].last_time_us += mavlink_streams[i].period_us;
      (this->*mavlink_streams[i].send_function)();
    }
  }
}

void Mavlink::set_streaming_rate(uint8_t stream_id, int16_t param_id)
{
  mavlink_streams[stream_id].period_us = (RF_->params_.get_param_int(param_id) == 0 ? 0 : 1000000/RF_->params_.get_param_int(param_id));
}

void Mavlink::mavlink_stream_set_period(uint8_t stream_id, uint32_t period_us)
{
  mavlink_streams[stream_id].period_us = period_us;
}

}
