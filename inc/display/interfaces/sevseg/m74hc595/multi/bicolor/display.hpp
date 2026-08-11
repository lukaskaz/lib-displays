#pragma once

#include "display/factory.hpp"
#include "logs/interfaces/logs.hpp"

#include <chrono>

using namespace std::chrono_literals;

namespace display::sevseg::m74hc595::multi::bicolor
{

enum class commontype
{
    cathode,
    anode
};

enum class colortype
{
    first,
    second
};

using param_t = std::chrono::milliseconds;
using config_t = std::tuple<commontype, colortype, std::chrono::microseconds,
                            uint8_t, std::shared_ptr<logs::LogIf>>;

class Display : public DisplayIf<param_t>
{
  public:
    ~Display();
    bool show(const std::string&) override;
    bool show(const std::string&, const param_t&) override;

  private:
    friend class display::Factory;
    Display(const std::string&, const config_t&);

    struct Handler;
    std::unique_ptr<Handler> handler;
};

} // namespace display::sevseg::m74hc595::multi::bicolor
