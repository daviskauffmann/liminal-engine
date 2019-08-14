#ifndef GAME_SPOT_LIGHT_HPP
#define GAME_SPOT_LIGHT_HPP

struct SpotLight
{
    vec3 position;
    vec3 direction;
    vec3 ambientColor;
    vec3 diffuseColor;
    vec3 specularColor;
    vec3 attenuation;
    float innerCutoff;
    float outerCutoff;

    SpotLight(
        vec3 position,
        vec3 direction,
        vec3 ambientColor,
        vec3 diffuseColor,
        vec3 specularColor,
        vec3 attenuation,
        float innerCutoff,
        float outerCutoff);
    ~SpotLight();
};

#endif
