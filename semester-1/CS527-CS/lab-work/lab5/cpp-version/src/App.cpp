#include "App.hpp"
#include <cstring>

App& App::instance() {
    static App app;
    return app;
}

void App::init() {
    bus_.init();
}

void App::read(unsigned int address, char* message, unsigned int size) {
    if (!message || size == 0) return;
    Frame req{}, resp{};
    req.packet_type  = PacketType::DATA;
    req.request_type = RequestType::READ;
    req.mem_addr     = static_cast<std::uint32_t>(address);
    req.length       = 0;
    req.checksum     = 0;

    if (bus_.send(req, resp) && resp.packet_type == PacketType::ACK) {
        unsigned n = resp.length;
        if (n >= size) n = size - 1;
        if (n > 0) std::memcpy(message, resp.data.data(), n);
        message[n] = '\0';
    } else {
        message[0] = '\0';
    }
}

void App::write(unsigned int address, char* message, unsigned int size) {
    if (!message || size == 0) return;
    Frame req{}, resp{};
    req.packet_type  = PacketType::DATA;
    req.request_type = RequestType::WRITE;
    req.mem_addr     = static_cast<std::uint32_t>(address);

    unsigned n = size;
    if (n > FRAME_DATA_MAX) n = FRAME_DATA_MAX;
    std::memcpy(req.data.data(), message, n);
    req.length = static_cast<std::uint8_t>(n);
    req.computeChecksum();

    (void)bus_.send(req, resp); // ignore NACK per Lab-5 baseline
}

// ---- Optional global C-style wrappers to match exact PDF signatures ----
extern "C" {
void read(unsigned int address, char* message, unsigned int size) {
    App::instance().read(address, message, size);
}
void write(unsigned int address, char* message, unsigned int size) {
    App::instance().write(address, message, size);
}
}
