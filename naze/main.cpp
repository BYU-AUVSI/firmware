#include "rosflight.h"
#include "naze32.h"
#include "param.h"

int main(void)
{
  rosflight::Naze32 board;
  rosflight::Params params(&board);
  rosflight::Mavlink mavlink(&board, &params);
  rosflight::ROSflight firmware(&board, &params, &mavlink);
  firmware.rosflight_init();

  while(1)
  {
    firmware.rosflight_run();
  }
  return 0;
}
