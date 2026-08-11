#pragma once

#include <array>
#include <cstdint>
#include <string>
#include <unordered_map>

namespace display::sevseg::charmaps
{

namespace onecolor
{

extern const std::unordered_map<std::string, uint8_t> charmap;

}

namespace bicolor
{

extern const std::unordered_map<std::string, uint8_t> charmap;
// extern const std::array<uint8_t, 96> chararray;

} // namespace bicolor

namespace multi::bicolor
{

extern const std::unordered_map<std::string, uint8_t> charmap;

} // namespace multi::bicolor

} // namespace display::sevseg::charmaps
