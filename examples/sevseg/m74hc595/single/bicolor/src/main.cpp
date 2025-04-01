#include "display/interfaces/sevseg/m74hc595/single/bicolor/display.hpp"
#include "logs/interfaces/console/logs.hpp"
#include "logs/interfaces/group/logs.hpp"
#include "logs/interfaces/storage/logs.hpp"

#include <chrono>
#include <iostream>

using namespace std::chrono_literals;

int main(int argc, char** argv)
{
    try
    {
        if (argc > 2)
        {
            std::chrono::microseconds delayus{100ms};
            auto dev{"/dev/spidev0.0"};
            auto loglvl =
                (bool)atoi(argv[1]) ? logs::level::debug : logs::level::info;
            auto text = argv[2];

            auto logconsole = logs::Factory::create<logs::console::Log,
                                                    logs::console::config_t>(
                {loglvl, logs::time::hide, logs::tags::hide});
            auto logstorage = logs::Factory::create<logs::storage::Log,
                                                    logs::storage::config_t>(
                {loglvl, logs::time::show, logs::tags::show, {}});
            auto logif =
                logs::Factory::create<logs::group::Log, logs::group::config_t>(
                    {logconsole, logstorage});

            using namespace display::sevseg::m74hc595::single::bicolor;
            auto iface = display::Factory::create<Display, config_t, param_t>(
                dev, {commontype::anode, {}, logif});

            if (argc == 3)
            {
                iface->show(text);
                usleep((uint32_t)delayus.count());
            }
            else if (argc == 4)
            {
                iface->show(text, *argv[3] == '1' ? colortype::first
                                                  : colortype::second);
                usleep((uint32_t)delayus.count());
            }
        }
    }
    catch (std::exception& err)
    {
        std::cerr << "[ERROR] " << err.what() << '\n';
    }
    return 0;
}
