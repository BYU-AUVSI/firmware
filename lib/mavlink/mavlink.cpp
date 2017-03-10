#include <stdint.h>

#include "mavlink.h"

namespace rosflight {

Mavlink::Mavlink(Board* _board)
{
  board_ = _board;
}

// function definitions
void Mavlink::init_mavlink(Params *_params, Sensors* _sensors)
{
  params_ = _params;
  sensors_ = _sensors;
  board_->serial_init(params_->get_param_int(PARAM_BAUD_RATE));

  sysid = params_->get_param_int(PARAM_SYSTEM_ID);
  compid = 250;

  _offboard_control_time = 0;
  send_params_index = PARAMS_COUNT;

//  mavlink_log_warning("rebooting");
}

void Mavlink::send_message(const mavlink_message_t &msg)
{
  uint8_t data[MAVLINK_MAX_PACKET_LEN];
  uint16_t len = mavlink_msg_to_send_buffer(data, &msg);
  for (int i = 0; i < len; i++)
  {
    board_->serial_write(data[i]);
  }
}

}

