#include "display/interfaces/sevseg/m74hc595/single/onecolor/display.hpp"

#include "display/helpers/sevseg/charcodes.hpp"

#include <fcntl.h>
#include <linux/spi/spidev.h>
#include <sys/ioctl.h>
#include <unistd.h>

namespace display::sevseg::m74hc595::single::onecolor
{

struct Display::Handler
{
  public:
    Handler(const std::string& dev, const config_t& config) :
        spifd{open(dev.c_str(), O_RDWR)}, type{std::get<commontype>(config)}
    {
        if (spifd < 0)
            throw std::runtime_error("Cannot open device: " + dev);
    }
    ~Handler()
    {
        show(" ", {});
    }

    bool show(const std::string& text) const
    {
        return show(text, {});
    }

    bool show(const std::string& text, const param_t&) const
    {
        if (text.size() == textsize)
        {
            auto digit = text.at(0);
            if (uint8_t code{}; getcode(digit, code))
            {
                return showdigit(code);
            }
        }
        throw std::runtime_error("Cannot display text: " + text);
    }

  private:
    int32_t spifd;
    commontype type;
    const uint8_t textsize{1};
    const uint32_t speedhz{500000};

    bool getcode(const char ch, uint8_t& code) const
    {
        uint32_t pos = ch - ' ';
        if (pos < chararray.size())
        {
            code = chararray.at(pos);
            return true;
        }
        return false;
    }

    bool showdigit(uint8_t code) const
    {
        if (type == commontype::anode)
            code = ~code;

        auto tosend = std::to_array({code}), torecv = std::to_array({code});
        struct spi_ioc_transfer spiinfo = {
            .tx_buf = (uint64_t)&tosend[0],
            .rx_buf = (uint64_t)&torecv[0],
            .len = tosend.size(),
            .speed_hz = speedhz,
            .delay_usecs = 0,
            .bits_per_word = 8,
        };
        return 0 == ioctl(spifd, SPI_IOC_MESSAGE(1), &spiinfo);
    }
};

Display::Display(const std::string& text, const config_t& config) :
    handler{std::make_unique<Handler>(text, config)}
{}
Display::~Display() = default;

bool Display::show(const std::string& text)
{
    return handler->show(text);
}

bool Display::show(const std::string& text, const param_t&)
{
    return show(text);
}

} // namespace display::sevseg::m74hc595::single::onecolor
