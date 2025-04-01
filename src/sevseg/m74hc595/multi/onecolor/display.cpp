#include "display/interfaces/sevseg/m74hc595/multi/onecolor/display.hpp"

#include "display/helpers/helpers.hpp"
#include "display/helpers/sevseg/charcodes.hpp"

#include <fcntl.h>
#include <linux/spi/spidev.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <bitset>
#include <functional>
#include <future>
#include <mutex>
#include <source_location>

namespace display::sevseg::m74hc595::multi::onecolor
{

using namespace display::helpers;

struct Display::Handler
{
  public:
    Handler(const std::string& dev, const config_t& config) :
        logif{std::get<std::shared_ptr<logs::LogIf>>(config)},
        spifd{open(dev.c_str(), O_RDWR)}, type{std::get<commontype>(config)},
        digitmplextime{std::get<std::chrono::microseconds>(config)}
    {
        if (spifd < 0)
            throw std::runtime_error("Cannot open device: " + dev);
        start();
        log(logs::level::info,
            "Created onecolor multi 7segm display w/ shifting "
            "[dev/typ/mxtimeus/shtimems/speedhz]: " +
                dev + "/" + str((int32_t)type) + "/" +
                str(digitmplextime.count()) + "/" +
                str(digitshifttime.count()) + "/" + str(dispsize) + "/" +
                str(speedhz));
    }

    ~Handler()
    {
        stop();
        close(spifd);
        log(logs::level::info, "Released onecolor multi 7segm display");
    }

    bool start()
    {
        async = std::async(
            std::launch::async,
            std::bind(
                [this](std::stop_token running) {
                    clear();
                    log(logs::level::info, "Display multiplexing started");
                    while (!running.stop_requested())
                    {
                        if (auto text = gettextfordisplay(); !text.empty())
                        {
                            if (!showtext(text))
                                throw std::runtime_error(
                                    "Cannot display text: '" + inputtext +
                                    "'/" + str(inputtext.size()));
                        }
                        else
                            usleep(10 * 1000);
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
        log(logs::level::info, "Display multiplexing stopped");
        return ret;
    }

    bool show(const std::string& text)
    {
        return show(text, digitshifttime);
    }

    bool show(const std::string& text, const param_t& timems)
    {
        log(logs::level::debug,
            "Requested text to display: '" + text + "'/" + str(text.size()));
        std::lock_guard lock(mtx);
        shiftpos = 0;
        shiftdir = dirtype::left;
        digitshifttime = timems;
        inputtext = text;
        return true;
    }

  private:
    const std::shared_ptr<logs::LogIf> logif;
    int32_t spifd;
    commontype type;
    std::chrono::microseconds digitmplextime{1ms};
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
        uint8_t tosendcode = (type == commontype::cathode) ? code : ~code;
        segnum = segnum > 0 ? 1 << (segnum - 1) : 1;

        auto tosenddata = std::to_array<uint8_t>({tosendcode, segnum}),
             torecvdata = std::to_array<uint8_t>({0x00, 0x00});
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
                // multiplexing @ digitmplextime[us]
                usleep((uint32_t)digitmplextime.count());
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

bool Display::show(const std::string& text, const param_t& timems)
{
    return handler->show(text, timems);
}

} // namespace display::sevseg::m74hc595::multi::onecolor
