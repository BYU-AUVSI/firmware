#pragma GCC diagnostic ignored "-Wstrict-aliasing"
#pragma GCC diagnostic ignored "-Wswitch"

#include <stdint.h>
#include "mavlink.h"

namespace rosflight {


// function definitions
void Mavlink::update_param(uint16_t param_id)
{
  if (param_id < PARAMS_COUNT)
  {
    MAV_PARAM_TYPE type;
    switch (params_->get_param_type(param_id))
    {
    case PARAM_TYPE_INT32:
      type = MAV_PARAM_TYPE_INT32;
      break;
    case PARAM_TYPE_FLOAT:
      type = MAV_PARAM_TYPE_REAL32;
      break;
    default:
      return;
    }

    mavlink_message_t msg;
    mavlink_msg_param_value_pack(params_->get_param_int(PARAM_SYSTEM_ID), 0, &msg,
                                 params_->get_param_name(param_id), params_->get_param_float(param_id), type, PARAMS_COUNT, param_id);
    send_message(msg);
  }
}

void Mavlink::mavlink_handle_msg_param_request_list(void)
{
  send_params_index = 0;
}

void Mavlink::mavlink_handle_msg_param_request_read(const mavlink_message_t *const msg)
{
  mavlink_param_request_read_t read;
  mavlink_msg_param_request_read_decode(msg, &read);

  if (read.target_system == (uint8_t) params_->get_param_int(PARAM_SYSTEM_ID)) // TODO check if component id matches?
  {
    uint16_t id = (read.param_index < 0) ? params_->lookup_param_id(read.param_id) : (uint16_t) read.param_index;

    if (id < PARAMS_COUNT)
      update_param(id);
  }
}

void Mavlink::mavlink_handle_msg_param_set(const mavlink_message_t *const msg)
{
  mavlink_param_set_t set;
  mavlink_msg_param_set_decode(msg, &set);

  if (set.target_system == (uint8_t) params_->get_param_int(PARAM_SYSTEM_ID)) // TODO check if component id matches?
  {
    uint16_t id = params_->lookup_param_id(set.param_id);

    if (id < PARAMS_COUNT)
    {
      param_type_t candidate_type;
      switch (set.param_type)
      {
      case MAV_PARAM_TYPE_INT32:
        candidate_type = PARAM_TYPE_INT32;
        break;
      case MAV_PARAM_TYPE_REAL32:
        candidate_type = PARAM_TYPE_FLOAT;
        break;
      default:
        candidate_type = PARAM_TYPE_INVALID;
        break;
      }

      if (candidate_type == params_->get_param_type(id))
      {
        switch (candidate_type)
        {
        case PARAM_TYPE_INT32:
          params_->set_param_int(id, *(int32_t *) &set.param_value);
          break;
        case PARAM_TYPE_FLOAT:
          params_->set_param_float(id, set.param_value);
          break;
        }
      }
    }
  }
}

void Mavlink::mavlink_send_next_param(void)
{
  if (send_params_index < PARAMS_COUNT)
  {
    update_param((uint16_t) send_params_index);
    send_params_index++;
  }
}

}
