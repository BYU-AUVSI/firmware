#include <stdbool.h>

#include "board.h"
#include "mavlink.h"
#include "mavlink_param.h"
#include "mixer.h"
#include "sensors.h"
#include "estimator.h"
#include "param.h"
#include "arming_fsm.h"
#include "rc.h"

#include "mavlink_stream.h"
#include "mavlink_util.h"
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
  mavlink_msg_heartbeat_pack(params_->get_param_int(PARAM_SYSTEM_ID), 0, &msg,
                             params_->get_param_int(PARAM_FIXED_WING) ? MAV_TYPE_FIXED_WING : MAV_TYPE_QUADROTOR,
                             MAV_AUTOPILOT_GENERIC,
                             armed_mode,
                             control_mode,
                             MAV_STATE_STANDBY);
  send_message(msg);
}

 void Mavlink::mavlink_send_attitude(void)
{
  mavlink_message_t msg;
//  mavlink_msg_attitude_quaternion_pack(sysid, compid, &msg,
//                                        board_->clock_millis(),
//                                        _current_state.q.w,
//                                        _current_state.q.x,
//                                        _current_state.q.y,
//                                        _current_state.q.z,
//                                        _current_state.omega.x,
//                                        _current_state.omega.y,
//                                        _current_state.omega.z);
  send_message(msg);
}

void Mavlink::mavlink_send_imu(void)
{
  mavlink_message_t msg;
//  mavlink_msg_small_imu_pack(sysid, compid, &msg,
//                             _imu_time,
//                             _accel.x,
//                             _accel.y,
//                             _accel.z,
//                             _gyro.x,
//                             _gyro.y,
//                             _gyro.z,
//                             _imu_temperature);
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
  send_message(msg);
}

 void Mavlink::mavlink_send_rc_raw(void)
{
  mavlink_message_t msg;
//  mavlink_msg_rc_channels_pack(sysid, compid, &msg,
//                               board_->clock_millis(),
//                               0,
//                               board_->pwm_read(0),
//                               board_->pwm_read(1),
//                               board_->pwm_read(2),
//                               board_->pwm_read(3),
//                               board_->pwm_read(4),
//                               board_->pwm_read(5),
//                               board_->pwm_read(6),
//                               board_->pwm_read(7),
//                               0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 0);
  send_message(msg);
}

void Mavlink::mavlink_send_diff_pressure(void)
{
  if (board_->diff_pressure_present())
  {
    mavlink_message_t msg;
//    mavlink_msg_diff_pressure_pack(sysid, compid, &msg,
//                                   _diff_pressure_velocity,
//                                   _diff_pressure,
//                                   _diff_pressure_temp);
    send_message(msg);
  }
}

void Mavlink::mavlink_send_baro(void)
{
  if (board_->baro_present())
  {
    mavlink_message_t msg;
//    mavlink_msg_small_baro_pack(sysid, compid, &msg,
//                                _baro_altitude,
//                                _baro_pressure,
//                                _baro_temperature);
    send_message(msg);
  }
}

void Mavlink::mavlink_send_sonar(void)
{
  if (board_->sonar_present())
  {
    mavlink_message_t msg;
//    mavlink_msg_small_sonar_pack(sysid, compid, &msg,
//                                 _sonar_range,
//                                 8.0,
//                                 0.25);
    send_message(msg);
  }
}

void Mavlink::mavlink_send_mag(void)
{
  if (board_->mag_present())
  {
    mavlink_message_t msg;
//    mavlink_msg_small_mag_pack(sysid, compid, &msg,
//                               _mag.x,
//                               _mag.y,
//                               _mag.z);
    send_message(msg);
  }
}

void Mavlink::mavlink_send_low_priority(void)
{
  mavlink_send_next_param();
}

// function definitions
void Mavlink::mavlink_stream(uint64_t time_us)
{
  for (int i = 0; i < MAVLINK_STREAM_COUNT; i++)
  {
    if (mavlink_streams[i].period_us && time_us - mavlink_streams[i].last_time_us >= mavlink_streams[i].period_us)
    {
      // if we took too long, set the last_time_us to be where it should have been
      mavlink_streams[i].last_time_us += mavlink_streams[i].period_us;
      (this->*mavlink_streams[i].send_function)();
    }
  }
}

void Mavlink::mavlink_stream_set_rate(mavlink_stream_id_t stream_id, uint32_t rate)
{
  mavlink_streams[stream_id].period_us = (rate == 0 ? 0 : 1000000/rate);
}

void Mavlink::mavlink_stream_set_period(mavlink_stream_id_t stream_id, uint32_t period_us)
{
  mavlink_streams[stream_id].period_us = period_us;
}

}
