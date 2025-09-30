#include "bus.h"
#include "driver.h"
#include "app.h"

int main(void) {
    Bus bus;
    bus_init(&bus);

    DeviceDriver dd;
    dd_init(&dd, &bus);

    run_application(&dd);
    return 0;
}