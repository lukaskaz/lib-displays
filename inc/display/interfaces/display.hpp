#pragma once

#include <string>

namespace display
{

template <typename P>
class DisplayIf
{
  public:
    virtual ~DisplayIf() = default;
    virtual bool show(const std::string&) = 0;
    virtual bool show(const std::string&, const P&) = 0;
};

} // namespace display
