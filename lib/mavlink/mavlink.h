#pragma once

#include <mavlink/v1.0/rosflight/mavlink.h>

#include "board.h"
#include "param.h"
#include "sensors.h"

#include "commlink.h"

#define CALL_MEMBER_FN(object,ptrToMember)  ((object).*(ptrToMember))

namespace rosflight {

// Forward Declaraion of Required Classes
class Params;
class Board;
class Sensors;

class Mavlink : public CommLink
{
private:
  uint32_t sysid;
  uint32_t compid;
  uint64_t _offboard_control_time;
  Params* params_;
  Sensors* sensors_;
  Board* board_;
  uint8_t send_params_index;
  mavlink_message_t in_buf;
  mavlink_status_t status;

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
  void send_message(const mavlink_message_t &msg);
  void mavlink_stream_set_period(uint8_t stream_id, uint32_t period_us);


  // Debugging Utils
  void mavlink_send_named_value_int(const char *const name, int32_t value);
  void mavlink_send_named_value_float(const char *const name, float value);
//  void mavlink_send_named_command_struct(const char *const name, control_t command_struct);

  mavlink_stream_t mavlink_streams[STREAM_COUNT] = {
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
  Mavlink();

  void init(Board* _board, Params* _params, Sensors *_sensors);
  void receive(void);
  void stream();
  void update_param(uint16_t param_id);
  void set_streaming_rate(uint8_t stream_id, int32_t rate);
};

}
