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
        auto text = argv[1];
        auto iface =
            display::DisplayFactory::create<Display, config_t, param_t>(
                dev, {commontype::anode, 10ms});
        iface->show(text, 3000ms);
    }
    return 0;
}
