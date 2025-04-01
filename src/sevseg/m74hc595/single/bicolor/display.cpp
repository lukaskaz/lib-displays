#include "display/interfaces/sevseg/m74hc595/single/bicolor/display.hpp"

#include "display/helpers/helpers.hpp"
#include "display/helpers/sevseg/charcodes.hpp"

#include <fcntl.h>
#include <linux/spi/spidev.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <bitset>
#include <source_location>

namespace display::sevseg::m74hc595::single::bicolor
{

using namespace display::helpers;

struct Display::Handler
{
  public:
    Handler(const std::string& dev, const config_t& config) :
        logif{std::get<std::shared_ptr<logs::LogIf>>(config)}, spifd{open(
                                                                   dev.c_str(),
                                                                   O_RDWR)},
        type{std::get<commontype>(config)}, color{std::get<colortype>(config)}
    {
        if (spifd < 0)
            throw std::runtime_error("Cannot open device: " + dev);
        log(logs::level::info, "Created bicolor single 7segm display "
                               "[dev/typ/col/size/speedhz]: " +
                                   dev + "/" + str((int32_t)type) + "/" +
                                   str((int32_t)color) + "/" + str(textsize) +
                                   "/" + str(speedhz));
    }
    ~Handler()
    {
        show(" ", {});
        close(spifd);
        log(logs::level::info, "Released bicolor single 7segm display");
    }

    bool show(const std::string& text) const
    {
        return show(text, color);
    }

    bool show(const std::string& text, const param_t& color) const
    {
        log(logs::level::debug,
            "Requested text to display(col:" + str((int32_t)color) + "): '" +
                text + "'/" + str(text.size()));
        if (text.size() == textsize)
        {
            auto digit = color == colortype::first ? text : text + colorswitch;
            if (uint8_t code{}; getcode(digit, code))
                return showdigit(code);
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
    colortype color;
    const char colorswitch{'.'};
    const uint8_t textsize{1};
    const uint32_t speedhz{500000};

    bool getcode(const std::string& str, uint8_t& code) const
    {
        using namespace display::sevseg::charmaps::bicolor;
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

bool Display::show(const std::string& text, const param_t& color)
{
    return handler->show(text, color);
}

} // namespace display::sevseg::m74hc595::single::bicolor
