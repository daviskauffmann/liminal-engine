#ifndef GAME_SUN_HPP
#define GAME_SUN_HPP

namespace pk
{
    struct sun
    {
        vec3 direction;
        vec3 ambient_color;
        vec3 diffuse_color;
        vec3 specular_color;

        sun(
            vec3 direction,
            vec3 ambient_color,
            vec3 diffuse_color,
            vec3 specular_color);
        ~sun();
    };
}

#endif
