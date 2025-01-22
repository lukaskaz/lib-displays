#pragma once

#include "display/interfaces/display.hpp"

#include <memory>
#include <tuple>

namespace display
{

class DisplayFactory
{
  public:
    template <typename T, typename U, typename P>
    static std::shared_ptr<DisplayIf<P>> create(const std::string& text,
                                                const std::tuple<U, P>& config)
    {
        return std::shared_ptr<T>(new T(text, config));
    }
};

} // namespace display
