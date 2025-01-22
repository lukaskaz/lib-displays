#pragma once

#include <array>
#include <cstdint>
#include <string>
#include <unordered_map>

namespace display::monocolor
{

extern const std::unordered_map<std::string, uint8_t> charmap;
extern const std::array<uint8_t, 96> chararray;

} // namespace display::monocolor
