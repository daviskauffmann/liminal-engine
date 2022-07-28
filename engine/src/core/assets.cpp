#include <liminal/core/assets.hpp>

liminal::assets *liminal::assets::instance = nullptr;

liminal::assets::assets()
{
    instance = this;
}

liminal::assets::~assets()
{
    for (const auto &pair : cache)
    {
        delete pair.second;
    }
}
