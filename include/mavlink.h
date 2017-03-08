#pragma once

extern "C"
{
#include <mavlink/v1.0/rosflight/mavlink.h>
}

// function declarations
void init_mavlink(void);
void send_message(const mavlink_message_t &msg);
