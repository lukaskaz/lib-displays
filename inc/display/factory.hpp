#pragma once

#include "display/interfaces/display.hpp"

#include <memory>
#include <tuple>

namespace display
{

class DisplayFactory
{
  public:
    template <typename T, typename C, typename P>
    static std::shared_ptr<DisplayIf<P>> create(const std::string& text,
                                                const C& config)
    {
        return std::shared_ptr<T>(new T(text, config));
    }
};

} // namespace display
