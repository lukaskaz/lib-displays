#pragma once

#include "display/factory.hpp"
#include "logs/interfaces/logs.hpp"

namespace display::sevseg::m74hc595::single::onecolor
{

enum class commontype
{
    cathode,
    anode
};

enum class colortype
{
    first
};

using param_t = colortype;
using config_t = std::tuple<commontype, std::shared_ptr<logs::LogIf>>;

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

} // namespace display::sevseg::m74hc595::single::onecolor
