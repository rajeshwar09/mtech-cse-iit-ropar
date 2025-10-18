#include "Bus.hpp"
#include <cstring>

Bus::Bus() = default;

void Bus::init() {
    devices_.clear();
    devices_.emplace_back(new KeyboardDevice());
    devices_.emplace_back(new DisplayDevice());
    devices_.emplace_back(new AlarmDevice());
    devices_.emplace_back(new ButtonDevice());
}

int Bus::route(std::uint32_t a) const {
    for (std::size_t i = 0; i < devices_.size(); ++i) {
        auto& d = devices_[i];
        std::uint32_t base = d->base();
        std::uint32_t size = d->size();
        if (a >= base && a < base + size) return static_cast<int>(i);
    }
    return -1;
}

int Bus::send(const Frame& req, Frame& resp) {
    // default NACK response template
    resp = Frame{};
    resp.packet_type  = PacketType::ACK;
    resp.request_type = req.request_type;
    resp.mem_addr     = req.mem_addr;
    resp.length       = 0;

    int idx = route(req.mem_addr);
    if (idx < 0) return 0; // invalid address

    Device* d = devices_[idx].get();

    if (req.request_type == RequestType::READ) {
        if (!d->canRead()) return 0;
        std::vector<std::uint8_t> payload;
        if (!d->onRead(req.mem_addr, payload)) return 0;
        if (payload.size() > FRAME_DATA_MAX) payload.resize(FRAME_DATA_MAX);
        resp.length = static_cast<std::uint8_t>(payload.size());
        std::memcpy(resp.data.data(), payload.data(), resp.length);
        resp.computeChecksum();
        return 1;
    } else { // WRITE
        if (!d->canWrite()) return 0;
        if (!req.validateChecksum()) return 0; // verify request checksum before touching device
        if (!d->onWrite(req.mem_addr, req.data.data(), req.length)) return 0;
        resp.length = 0;
        resp.computeChecksum(); // CRC over empty data is fine
        return 1;
    }
}
