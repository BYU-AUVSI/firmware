#include <stdint.h>

#include "board.h"
#include "mavlink_receive.h"
#include "param.h"

#include "mavlink.h"
#include "mavlink_log.h"

// global variable definitions
mavlink_system_t mavlink_system;

// function definitions
void init_mavlink(void)
{
  serial_init(get_param_int(PARAM_BAUD_RATE));

  mavlink_system.sysid = get_param_int(PARAM_SYSTEM_ID);
  mavlink_system.compid = 250;

  _offboard_control_time = 0;

  mavlink_log_warning("rebooting");
}

void send_message(const mavlink_message_t &msg)
{
  uint8_t data[MAVLINK_MAX_PACKET_LEN];
  uint16_t len = mavlink_msg_to_send_buffer(data, &msg);
  for (int i = 0; i < len; i++)
  {
    serial_write(data[i]);
  }
}

