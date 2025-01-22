#pragma once

#include "display/factory.hpp"

namespace display::sevsegbi
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

using param_t = colortype;
using config_t = std::tuple<commontype, param_t>;

class Display : public DisplayIf<param_t>
{
  public:
    ~Display();
    bool show(const std::string&) override;
    bool show(const std::string&, const param_t&) override;

  private:
    friend class display::DisplayFactory;
    Display(const std::string&, const config_t&);

    struct Handler;
    std::unique_ptr<Handler> handler;
};

} // namespace display::sevsegbi
