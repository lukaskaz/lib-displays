#include "display/interfaces/sevseg/m74hc595/multi/onecolor/display.hpp"

#include "display/helpers/sevseg/charcodes.hpp"

#include <fcntl.h>
#include <linux/spi/spidev.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <functional>
#include <future>
#include <mutex>

namespace display::sevseg::m74hc595::multi::onecolor
{

struct Display::Handler
{
  public:
    Handler(const std::string& dev, const config_t& config) :
        spifd{open(dev.c_str(), O_RDWR)}, type{std::get<commontype>(config)},
        digitmplextime{std::get<std::chrono::milliseconds>(config)}
    {
        if (spifd < 0)
            throw std::runtime_error("Cannot open device: " + dev);
        start();
    }

    ~Handler()
    {
        stop();
    }

    bool start()
    {
        async = std::async(
            std::launch::async,
            std::bind(
                [this](std::stop_token running) {
                    clear();
                    while (!running.stop_requested())
                    {
                        if (auto text = gettextfordisplay(); !text.empty())
                        {
                            if (!showtext(text))
                            {
                                throw std::runtime_error(
                                    "Cannot display text: " + inputtext);
                            }
                        }
                        else
                        {
                            usleep(10 * 1000);
                        }
                    }
                },
                running.get_token()));
        return true;
    }

    bool stop()
    {
        auto ret = running.request_stop();
        async.wait();
        clear();
        return ret;
    }

    bool show(const std::string& text)
    {
        return show(text, digitshifttime);
    }

    bool show(const std::string& text, const param_t& timems)
    {
        std::lock_guard lock(mtx);
        shiftpos = 0;
        shiftdir = dirtype::left;
        digitshifttime = timems;
        inputtext = text;
        return true;
    }

  private:
    int32_t spifd;
    commontype type;
    std::chrono::milliseconds digitmplextime{1ms};
    std::chrono::milliseconds digitshifttime{500ms};
    const uint8_t dispsize{4};
    const uint32_t speedhz{500000};
    std::stop_source running;
    std::future<void> async;
    std::string inputtext;
    uint8_t shiftpos{0};
    enum class dirtype
    {
        left,
        right
    } shiftdir{dirtype::left};
    std::mutex mtx;

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

    std::string gettextfordisplay()
    {
        std::lock_guard lock(mtx);
        if (inputtext.size() <= dispsize)
        {
            return inputtext;
        }
        else
        {
            auto text = inputtext.substr(shiftpos, std::string::npos);
            if (shiftdir == dirtype::left)
            {
                shiftdir = ++shiftpos < inputtext.size() - dispsize
                               ? shiftdir
                               : dirtype::right;
            }
            else
            {
                shiftdir = --shiftpos > 0 ? shiftdir : dirtype::left;
            }
            return text;
        }
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
        auto start = std::chrono::steady_clock::now();
        auto runningtime{0ms};
        while (runningtime < digitshifttime)
        {
            for (uint8_t idx{0}; idx < dispsize && idx < text.size(); idx++)
            {
                auto digit = text.at(idx);
                uint8_t code{};
                getcode(digit, code);
                auto segnum = dispsize < text.size() ? dispsize : text.size();
                showdigit((uint8_t)segnum - idx, code);
                usleep((uint32_t)digitmplextime.count() *
                       1000); // multiplexing @ digitmplextime[ms]
            }
            runningtime = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::steady_clock::now() - start);
        }
        return true;
    }

    bool clear()
    {
        if (uint8_t code{}; getcode(' ', code))
        {
            for (auto cnt = dispsize; cnt; cnt--)
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
