#include "display/interfaces/sevseg/m74hc595/single/onecolor/display.hpp"

#include "display/helpers/helpers.hpp"
#include "display/helpers/sevseg/charcodes.hpp"

#include <fcntl.h>
#include <linux/spi/spidev.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <bitset>
#include <source_location>

namespace display::sevseg::m74hc595::single::onecolor
{

using namespace display::helpers;

struct Display::Handler
{
  public:
    Handler(const std::string& dev, const config_t& config) :
        logif{std::get<std::shared_ptr<logs::LogIf>>(config)},
        spifd{open(dev.c_str(), O_RDWR)}, type{std::get<commontype>(config)}
    {
        if (spifd < 0)
            throw std::runtime_error("Cannot open device: " + dev);
        log(logs::level::info,
            "Created onecolor single 7segm display [dev/typ/size/speedhz]: " +
                dev + "/" + str((int32_t)type) + "/" + "/" + str(textsize) +
                "/" + str(speedhz));
    }
    ~Handler()
    {
        show(" ", {});
        close(spifd);
        log(logs::level::info, "Released onecolor single 7segm display");
    }

    bool show(const std::string& text) const
    {
        return show(text, {});
    }

    bool show(const std::string& text, const param_t&) const
    {
        log(logs::level::debug,
            "Requested text to display: '" + text + "'/" + str(text.size()));
        if (text.size() == textsize)
        {
            auto digit = text.at(0);
            if (uint8_t code{}; getcode(digit, code))
            {
                return showdigit(code);
            }
            throw std::runtime_error("Cannot get code for digit: '" +
                                     str(digit) + "'");
        }
        throw std::runtime_error("Text to big to display: '" + text + "'/" +
                                 str(text.size()));
    }

  private:
    const std::shared_ptr<logs::LogIf> logif;
    int32_t spifd;
    commontype type;
    const uint8_t textsize{1};
    const uint32_t speedhz{500000};

    bool getcode(const char ch, uint8_t& code) const
    {
        using namespace display::sevseg::charmaps::onecolor;
        auto str = std::string{ch};
        if (charmap.contains(str))
        {
            code = charmap.at(str);
            return true;
        }
        return false;
    }

    bool showdigit(uint8_t code) const
    {
        uint8_t tosendcode = type == commontype::cathode ? code : ~code;
        auto tosenddata = std::to_array({tosendcode}),
             torecvdata = std::to_array({uint8_t{}});
        struct spi_ioc_transfer spiinfo = {
            .tx_buf = (uint64_t)&tosenddata[0],
            .rx_buf = (uint64_t)&torecvdata[0],
            .len = tosenddata.size(),
            .speed_hz = speedhz,
            .delay_usecs = 0,
            .bits_per_word = 8,
        };

        auto ret = 0 == ioctl(spifd, SPI_IOC_MESSAGE(1), &spiinfo);
        log(logs::level::debug,
            "Code sent to display(" + std::string(ret == 0 ? "ok" : "err") +
                "): " + std::bitset<8 * sizeof(uint8_t)>(code).to_string() +
                " -> " +
                std::bitset<8 * sizeof(uint8_t)>(tosendcode).to_string());

        return ret;
    }

    void log(
        logs::level level, const std::string& msg,
        const std::source_location loc = std::source_location::current()) const
    {
        if (logif)
            logif->log(level, std::string{loc.function_name()}, msg);
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
