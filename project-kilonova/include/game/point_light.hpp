#ifndef GAME_POINT_LIGHT_HPP
#define GAME_POINT_LIGHT_HPP

struct PointLight
{
    vec3 position;
    vec3 ambientColor;
    vec3 diffuseColor;
    vec3 specularColor;
    vec3 attenuation;

    PointLight(
        vec3 position,
        vec3 ambientColor,
        vec3 diffuseColor,
        vec3 specularColor,
        vec3 attenuation);
    ~PointLight();
};

#endif
