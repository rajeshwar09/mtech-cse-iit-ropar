#include "Device.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <cstdlib>
#include <ctime>

bool KeyboardDevice::onRead(std::uint32_t addr, std::vector<std::uint8_t>& out) {
    (void)addr;
    // Read a single token from stdin (like scanf("%s"))
    std::string token;
    if (!(std::cin >> token)) return false;
    // include NUL terminator
    out.assign(token.begin(), token.end());
    out.push_back('\0');
    if (out.size() > FRAME_DATA_MAX) out.resize(FRAME_DATA_MAX);
    return true;
}

bool DisplayDevice::onWrite(std::uint32_t addr, const std::uint8_t* data, std::size_t len) {
    (void)addr;
    if (!data || len == 0) return false;
    // If last byte is NUL, print as C-string; else write raw
    if (data[len - 1] == '\0') {
        std::cout << reinterpret_cast<const char*>(data);
    } else {
        std::cout.write(reinterpret_cast<const char*>(data), static_cast<std::streamsize>(len));
    }
    std::cout.flush();
    return true;
}

bool AlarmDevice::onWrite(std::uint32_t addr, const std::uint8_t* data, std::size_t len) {
    (void)addr;
    if (!data || len == 0) return false;
    unsigned times = data[0];
    for (unsigned i = 0; i < times; ++i) {
        std::cout << "Alarm" << std::endl;
    }
    return true;
}

bool ButtonDevice::onRead(std::uint32_t addr, std::vector<std::uint8_t>& out) {
    (void)addr;
    static bool seeded = false;
    if (!seeded) { std::srand(static_cast<unsigned>(std::time(nullptr))); seeded = true; }
    std::uint8_t v = static_cast<std::uint8_t>(std::rand() & 1);
    out.clear();
    out.push_back(v);
    return true;
}
