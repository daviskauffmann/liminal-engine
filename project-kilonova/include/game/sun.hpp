#ifndef GAME_SUN_HPP
#define GAME_SUN_HPP

struct Sun
{
    vec3 direction;
    vec3 ambientColor;
    vec3 diffuseColor;
    vec3 specularColor;

    Sun(
        vec3 direction,
        vec3 ambientColor,
        vec3 diffuseColor,
        vec3 specularColor);
    ~Sun();
};

#endif
