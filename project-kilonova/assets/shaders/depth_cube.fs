#version 460 core

in vec4 frag_position;

uniform struct Light
{
    vec3 position;
} light;

uniform float far_plane;

void main()
{
    float light_distance = length(frag_position.xyz - light.position);

    light_distance = light_distance / far_plane;

    gl_FragDepth = light_distance;
}
