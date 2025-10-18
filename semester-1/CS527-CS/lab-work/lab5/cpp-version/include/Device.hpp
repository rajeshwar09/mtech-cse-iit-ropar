#pragma once
#include <cstdint>
#include <cstddef>
#include <vector>
#include "Constants.hpp"

class Device {
public:
    Device(std::uint32_t base, std::uint32_t size) : base_(base), size_(size) {}
    virtual ~Device() = default;

    std::uint32_t base() const { return base_; }
    std::uint32_t size() const { return size_; }

    virtual bool canRead()  const { return false; }
    virtual bool canWrite() const { return false; }

    // onRead fills 'out' with up to 255 bytes. Return true on success.
    virtual bool onRead (std::uint32_t addr, std::vector<std::uint8_t>& out) { (void)addr; (void)out; return false; }
    // onWrite consumes 'len' bytes from data. Return true on success.
    virtual bool onWrite(std::uint32_t addr, const std::uint8_t* data, std::size_t len) { (void)addr; (void)data; (void)len; return false; }

private:
    std::uint32_t base_;
    std::uint32_t size_;
};

class KeyboardDevice : public Device {
public:
    KeyboardDevice() : Device(KEYBOARD_BASE, KEYBOARD_MEM_BYTES) {}
    bool canRead() const override { return true; }
    bool onRead(std::uint32_t addr, std::vector<std::uint8_t>& out) override;
};

class DisplayDevice : public Device {
public:
    DisplayDevice() : Device(DISPLAY_BASE, DISPLAY_MEM_BYTES) {}
    bool canWrite() const override { return true; }
    bool onWrite(std::uint32_t addr, const std::uint8_t* data, std::size_t len) override;
};

class AlarmDevice : public Device {
public:
    AlarmDevice() : Device(ALARM_ADDR, ALARM_MEM_BYTES) {}
    bool canWrite() const override { return true; }
    bool onWrite(std::uint32_t addr, const std::uint8_t* data, std::size_t len) override;
};

class ButtonDevice : public Device {
public:
    ButtonDevice() : Device(BUTTON_ADDR, BUTTON_MEM_BYTES) {}
    bool canRead() const override { return true; }
    bool onRead(std::uint32_t addr, std::vector<std::uint8_t>& out) override;
};
