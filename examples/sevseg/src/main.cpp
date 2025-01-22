#include "display/interfaces/m74hc595/sevsegbicolor.hpp"

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
        using namespace display::sevsegbi::m74hc595;
        auto dev{"/dev/spidev0.0"};
        auto text = argv[1];
        auto iface = display::DisplayFactory::create<Display>(
            dev, config_t{commontype::anode, colortype{}});

        iface->show(text);
        sleep(2);
        if (argc > 2)
        {
            iface->show(text,
                        *argv[2] == '1' ? colortype::first : colortype::second);
            sleep(2);
        }
    }
    return 0;
}
