#pragma once

extern "C"
{
#include <mavlink/v1.0/rosflight/mavlink.h>
}

#include "board.h"
#include "param.h"

#define CALL_MEMBER_FN(object,ptrToMember)  ((object).*(ptrToMember))

namespace rosflight {

typedef enum
{
  MAVLINK_STREAM_ID_HEARTBEAT,

  MAVLINK_STREAM_ID_ATTITUDE,

  MAVLINK_STREAM_ID_IMU,
  MAVLINK_STREAM_ID_DIFF_PRESSURE,
  MAVLINK_STREAM_ID_BARO,
  MAVLINK_STREAM_ID_SONAR,
  MAVLINK_STREAM_ID_MAG,

  MAVLINK_STREAM_ID_SERVO_OUTPUT_RAW,
  MAVLINK_STREAM_ID_RC_RAW,
  MAVLINK_STREAM_ID_LOW_PRIORITY,
  MAVLINK_STREAM_COUNT
} mavlink_stream_id_t;

class Mavlink
{
private:
  uint64_t _offboard_control_time;
  Params* params_;

  typedef  void (Mavlink::*MavlinkStreamFcn)(void);

  typedef struct
  {
    uint32_t period_us;
    uint64_t last_time_us;
    MavlinkStreamFcn send_function;
  } mavlink_stream_t;

  void mavlink_handle_msg_param_request_list(void);
  void mavlink_handle_msg_param_request_read(const mavlink_message_t *const msg);
  void mavlink_handle_msg_param_set(const mavlink_message_t *const msg);
  void mavlink_send_next_param(void);

  void handle_mavlink_message(void);

  void mavlink_handle_msg_rosflight_cmd(const mavlink_message_t *const msg);
  void mavlink_handle_msg_timesync(const mavlink_message_t *const msg);
  void mavlink_handle_msg_offboard_control(const mavlink_message_t *const msg);

  void mavlink_send_heartbeat(void);
  void mavlink_send_attitude(void);
  void mavlink_send_imu(void);
  void mavlink_send_servo_output_raw(void);
  void mavlink_send_rc_raw(void);
  void mavlink_send_diff_pressure(void);
  void mavlink_send_baro(void);
  void mavlink_send_sonar(void);
  void mavlink_send_mag(void);
  void mavlink_send_low_priority(void);

  mavlink_stream_t mavlink_streams[MAVLINK_STREAM_COUNT] = {
  //  period_us    last_time_us   send_function
    { 1000000,     0,             &rosflight::Mavlink::mavlink_send_heartbeat },
    { 200000,      0,             &rosflight::Mavlink::mavlink_send_attitude },
    { 1000,        0,             &rosflight::Mavlink::mavlink_send_imu },
    { 200000,      0,             &rosflight::Mavlink::mavlink_send_diff_pressure },
    { 200000,      0,             &rosflight::Mavlink::mavlink_send_baro },
    { 100000,      0,             &rosflight::Mavlink::mavlink_send_sonar },
    { 6250,        0,             &rosflight::Mavlink::mavlink_send_mag },
    { 0,           0,             &rosflight::Mavlink::mavlink_send_servo_output_raw },
    { 0,           0,             &rosflight::Mavlink::mavlink_send_rc_raw },
    { 10000,       0,             &rosflight::Mavlink::mavlink_send_low_priority }
  };


public:
  Mavlink(Board* _board, Params* _params);

  // Generic
  void init_mavlink(void);
  void send_message(const mavlink_message_t &msg);

  // Parameters
  void mavlink_send_param(param_id_t id);

  // Receive
  void mavlink_receive(void);

  // Stream
  void mavlink_stream(uint64_t time_us);
  void mavlink_stream_set_rate(mavlink_stream_id_t stream_id, uint32_t rate);
  void mavlink_stream_set_period(mavlink_stream_id_t stream_id, uint32_t period_us);
};

}
