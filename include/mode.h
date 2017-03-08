#ifdef __cplusplus
extern "C" {
#endif

#pragma once

typedef enum
{
  DISARMED,
  ARMED,
  DISARMED_FAILSAFE,
  ARMED_FAILSAFE
} armed_state_t;
extern armed_state_t _armed_state;

void init_mode(void);
bool check_mode(uint64_t now);

#ifdef __cplusplus
}
#endif

