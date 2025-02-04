#include "display/interfaces/sevseg/m74hc595/multi/onecolor/display.hpp"

#include <fcntl.h>
#include <linux/spi/spidev.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <array>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <unordered_map>

int main(int argc, char** argv)
{
    if (argc > 1)
    {
        using namespace display::sevseg::m74hc595::multi::onecolor;
        auto dev{"/dev/spidev0.0"};
        auto delay = 10s;
        auto iface = display::Factory::create<Display, config_t, param_t>(
            dev, {commontype::anode, 1ms});
        for (uint8_t idx{1}; idx < argc; idx++)
        {
            // iface->show(argv[idx], 500ms);
            iface->show(argv[idx]);
            sleep((uint32_t)delay.count());
        }
    }
    return 0;
}
