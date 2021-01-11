#include <cxxopts.hpp>
#include <SDL2/SDL.h>

#include "server.hpp"
#include "client.hpp"

#define VERSION "v0.0.1"

int main(int argc, char *argv[])
{
    try
    {
        cxxopts::Options options("pk");

        auto option_adder = options.add_options();
        option_adder("height", "Set window height", cxxopts::value<int>()->default_value("720"));
        option_adder("h,help", "Print usage");
        option_adder("scale", "Set render scale", cxxopts::value<float>()->default_value("1.0"));
        option_adder("s,server", "Run as headless server");
        option_adder("v,version", "Print version");
        option_adder("width", "Set window width", cxxopts::value<int>()->default_value("1280"));

        auto result = options.parse(argc, argv);
        if (result.count("help"))
        {
            std::cout << options.help() << std::endl;
            return 0;
        }
        if (result.count("server"))
        {
            return pk::server_main(result);
        }
        else
        {
            return pk::client_main(result);
        }
        if (result.count("version"))
        {
            std::cout << VERSION << std::endl;
            return 0;
        }
    }
    catch (std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}
