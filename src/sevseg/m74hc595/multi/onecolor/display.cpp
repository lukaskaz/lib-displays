#include "display/interfaces/sevseg/m74hc595/multi/onecolor/display.hpp"

#include "display/helpers/sevseg/charcodes.hpp"

#include <fcntl.h>
#include <linux/spi/spidev.h>
#include <sys/ioctl.h>
#include <unistd.h>

namespace display::sevseg::m74hc595::multi::onecolor
{

struct Display::Handler
{
  public:
    Handler(const std::string& dev, const config_t& config) :
        spifd{open(dev.c_str(), O_RDWR)}, type{std::get<commontype>(config)},
        timems{std::get<std::chrono::milliseconds>(config)}
    {
        if (spifd < 0)
            throw std::runtime_error("Cannot open device: " + dev);
    }
    ~Handler()
    {
        clear();
    }

    bool show(const std::string& text) const
    {
        return show(text, timems);
    }

    bool show(const std::string& text, const param_t& timems) const
    {
        auto start = std::chrono::steady_clock::now();
        auto runningtime = 0ms;
        while (runningtime < timems)
        {
            if (!showtext(text))
            {
                throw std::runtime_error("Cannot display text: " + text);
            }
            runningtime = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::steady_clock::now() - start);
        }
        return true;
    }

  private:
    int32_t spifd;
    commontype type;
    std::chrono::milliseconds timems;
    const uint8_t textsize{4};
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

    bool showdigit(uint8_t segnum, uint8_t code) const
    {
        if (type == commontype::anode)
            code = ~code;
        segnum = segnum > 0 ? 1 << (segnum - 1) : 1;

        auto tosend = std::to_array<uint8_t>({code, segnum}),
             torecv = std::to_array<uint8_t>({0x00, 0x00});
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

    bool showtext(const std::string& text) const
    {
        if (text.size() <= textsize)
        {
            for (uint8_t idx{}; idx < text.size(); idx++)
            {
                auto digit = text.at(idx);
                uint8_t code{};
                getcode(digit, code);
                showdigit(textsize - idx, code);
                usleep(1 * 1000); // multiplexing @ 1ms
            }
            return true;
        }
        return false;
    }

    bool clear()
    {
        if (uint8_t code{}; getcode(' ', code))
        {
            for (auto cnt = textsize; cnt; cnt--)
                showdigit(cnt, code);
            return true;
        }
        return false;
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

bool Display::show(const std::string& text, const param_t& timems)
{
    return handler->show(text, timems);
}

} // namespace display::sevseg::m74hc595::multi::onecolor
