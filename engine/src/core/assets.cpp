#include <liminal/core/assets.hpp>

liminal::assets *liminal::assets::instance = nullptr;

liminal::assets::assets()
{
    instance = this;
}

liminal::assets::~assets()
{
    for (auto it = cache.begin(); it != cache.end(); it++)
    {
        delete it->second;
    }
}
