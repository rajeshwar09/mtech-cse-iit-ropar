#pragma once
#include <cstdint>
#include <cstddef>

// ----- Device address map (per Lab-5 PDF) -----
static constexpr std::uint32_t KEYBOARD_BASE      = 0xFFFF0000u;
static constexpr std::uint32_t KEYBOARD_MEM_BYTES = 128u;

static constexpr std::uint32_t DISPLAY_BASE       = 0xFFFF0100u;
static constexpr std::uint32_t DISPLAY_MEM_BYTES  = 512u;   // [0xFFFF0100 .. 0xFFFF02FF]

static constexpr std::uint32_t ALARM_ADDR         = 0xFFFF0300u; // 1 byte
static constexpr std::uint32_t ALARM_MEM_BYTES    = 1u;

static constexpr std::uint32_t BUTTON_ADDR        = 0xFFFF0301u; // 1 byte
static constexpr std::uint32_t BUTTON_MEM_BYTES   = 1u;

// Packet fields (protocol same as Lab-4)
enum class PacketType  : std::uint8_t { DATA = 0, ACK = 1 };
enum class RequestType : std::uint8_t { READ = 0, WRITE = 1 };

// Protocol limits
static constexpr std::size_t FRAME_DATA_MAX = 255u;
