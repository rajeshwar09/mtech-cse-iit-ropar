#include "bus.h"

void bus_init(Bus* b) {
    if (!b) return;
    device_init(&b->dev);
}

bool bus_transact(Bus* b, const Frame* req, Frame* resp) {
    if (!b || !req || !resp) return false;
    if (req->device_id != DEV_ID_STORAGE) return false;

    switch (req->packet_type) {
        case PKT_TYPE_DATA:
            return device_handle_data(&b->dev, req, resp);
        case PKT_TYPE_PRINT:
            return device_handle_print(&b->dev, req, resp);
        case PKT_TYPE_ACKNAK: // not valid host->device op
        default:
            return false;
    }
}