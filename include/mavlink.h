#pragma once

extern "C"
{
#include <mavlink/v1.0/rosflight/mavlink.h>
}

#include "board.h"
#include "param.h"

namespace rosflight {

class Mavlink
{
private:
  void mavlink_handle_msg_param_request_list(void);
  void mavlink_handle_msg_param_request_read(const mavlink_message_t *const msg);
  void mavlink_handle_msg_param_set(const mavlink_message_t *const msg);
  void mavlink_send_next_param(void);


public:
  Mavlink(Board* _board);
  // function declarations
  void init_mavlink(void);
  void send_message(const mavlink_message_t &msg);
//  void mavlink_send_param(param_id_t id);
  void mavlink_receive(void);
};

}
