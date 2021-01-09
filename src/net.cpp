#include "net.hpp"

#include <spdlog/spdlog.h>

pk::net::net()
{
    if (SDLNet_Init() != 0)
    {
        spdlog::error("Failed to initialize SDL_net: {}", SDLNet_GetError());
        return;
    }
}

pk::net::~net()
{
    SDLNet_Quit();
}
