#ifndef LIMINAL_COMPONENTS_WATER_HPP
#define LIMINAL_COMPONENTS_WATER_HPP

namespace liminal
{
    struct water
    {
        float tiling = 1;
        float speed = 0.02f;
        float wave_strength = 0.01f;
        float reflectivity = 0.5f;
        float shine_damper = 100;
    };
}

#endif
