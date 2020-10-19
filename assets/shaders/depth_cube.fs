#version 460 core

in vec4 frag_position;

uniform struct Light
{
    float far_plane;
    vec3 position;
    mat4 transformation_matrices[6];
} light;

void main()
{
    float light_distance = length(frag_position.xyz - light.position);
    light_distance = light_distance / light.far_plane;
    gl_FragDepth = light_distance;
}
