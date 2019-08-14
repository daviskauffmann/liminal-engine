#ifndef GAME_DIRECTIONAL_LIGHT_H
#define GAME_DIRECTIONAL_LIGHT_H

struct DirectionalLight
{
    vec3 direction;
    vec3 ambientColor;
    vec3 diffuseColor;
    vec3 specularColor;

    DirectionalLight(
        vec3 direction,
        vec3 ambientColor,
        vec3 diffuseColor,
        vec3 specularColor);
    ~DirectionalLight();
};

#endif
