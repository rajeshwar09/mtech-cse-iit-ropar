#include "bus.h"
#include "driver.h"
#include "app.h"
int main(void)
{
  Bus b;
  bus_init(&b);
  DeviceDriver dd;
  dd_init(&dd, &b);
  run_app(&dd);
  return 0;
}
