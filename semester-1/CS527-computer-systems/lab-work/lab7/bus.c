#include "bus.h"
void bus_init(Bus *b) { device_init(&b->dev); }
bool bus_transact(Bus *b, const Frame *req, Frame *resp)
{
  if (!b || !req || !resp)
    return false;
  if (req->device_id != DEV_ID_STORAGE)
    return false;
  if (req->packet_type == PKT_TYPE_DATA)
    return device_data(&b->dev, req, resp);
  if (req->packet_type == PKT_TYPE_PRINT)
    return device_print(&b->dev, req, resp);
  return false;
}
