#include "display/interfaces/sevseg/m74hc595/multi/onecolor/display.hpp"
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
            auto dev{"/dev/spidev0.0"};
            std::chrono::microseconds delayus{5s};
            auto loglvl =
                (bool)atoi(argv[1]) ? logs::level::debug : logs::level::info;

            auto logconsole = logs::Factory::create<logs::console::Log,
                                                    logs::console::config_t>(
                {loglvl, logs::time::hide, logs::tags::hide});
            auto logstorage = logs::Factory::create<logs::storage::Log,
                                                    logs::storage::config_t>(
                {loglvl, logs::time::show, logs::tags::show, {}});
            auto logif =
                logs::Factory::create<logs::group::Log, logs::group::config_t>(
                    {logconsole, logstorage});

            using namespace display::sevseg::m74hc595::multi::onecolor;
            auto iface = display::Factory::create<Display, config_t, param_t>(
                dev, {commontype::anode, 1ms, logif});

            iface->show(argv[2]);
            usleep((uint32_t)delayus.count());

            if (argc > 3)
            {
                iface->show(argv[3], 200ms);
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
