#include "driver.h"
#include "crc16.h"
#include <string.h>

void dd_init(DeviceDriver* dd, Bus* bus) { dd->bus = bus; }

static bool send_recv(DeviceDriver* dd, const Frame* req, Frame* resp) {
    return bus_transact(dd->bus, req, resp);
}

bool dd_read_bytes(DeviceDriver* dd, uint32_t addr, uint32_t len, uint8_t* out) {
    if (!dd || !out) return false;
    if (!device_range_ok(addr, len)) return false;

    Frame req = {0}, resp = {0};
    req.device_id   = DEV_ID_STORAGE;
    req.packet_type = PKT_TYPE_DATA; // 00 for both
    req.rw_flag     = RW_READ;
    req.start_addr  = addr;
    req.length      = len;
    req.crc         = 0u; // no payload in request

    for (unsigned a = 0; a <= MAX_RETRIES; ++a) {
        memset(&resp, 0, sizeof(resp));
        if (!send_recv(dd, &req, &resp)) continue;
        if (resp.packet_type != PKT_TYPE_ACKNAK) continue;
        if (resp.length != len) continue;
        if (crc16_payload(resp.payload, resp.length) != resp.crc) continue;
        memcpy(out, resp.payload, len);
        return true;
    }
    return false;
}

bool dd_write_bytes(DeviceDriver* dd, uint32_t addr, const uint8_t* in, uint32_t len) {
    if (!dd || (!in && len>0)) return false;
    if (!device_range_ok(addr, len)) return false;

    Frame req = {0}, resp = {0};
    req.device_id   = DEV_ID_STORAGE;
    req.packet_type = PKT_TYPE_DATA; // 00 for both
    req.rw_flag     = RW_WRITE;
    req.start_addr  = addr;
    req.length      = len;
    if (len) memcpy(req.payload, in, len);
    req.crc         = crc16_payload(req.payload, req.length);

    for (unsigned a = 0; a <= MAX_RETRIES; ++a) {
        memset(&resp, 0, sizeof(resp));
        if (!send_recv(dd, &req, &resp)) continue;
        if (resp.packet_type != PKT_TYPE_ACKNAK) continue;
        if (resp.length != 1u) continue;
        if (crc16_payload(resp.payload, resp.length) != resp.crc) continue;
        if (resp.payload[0] == 1u) return true;
    }
    return false;
}

bool dd_read_int(DeviceDriver* dd, uint32_t byte_addr, int32_t* out) {
    return dd_read_bytes(dd, byte_addr, INT_BYTES, (uint8_t*)out);
}

bool dd_write_int(DeviceDriver* dd, uint32_t byte_addr, int32_t value) {
    return dd_write_bytes(dd, byte_addr, (const uint8_t*)&value, INT_BYTES);
}

bool dd_print_device(DeviceDriver* dd) {
    if (!dd) return false;
    Frame req = {0}, resp = {0};
    req.device_id   = DEV_ID_STORAGE;
    req.packet_type = PKT_TYPE_PRINT;
    req.rw_flag     = RW_READ;
    req.start_addr  = 0u;
    req.length      = 0u;
    req.crc         = 0u;

    if (!send_recv(dd, &req, &resp)) return false;
    if (resp.packet_type != PKT_TYPE_ACKNAK) return false;
    if (resp.length != 1u) return false;
    if (crc16_payload(resp.payload, resp.length) != resp.crc) return false;
    return resp.payload[0] == 1u;
}