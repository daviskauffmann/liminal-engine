#version 460 core

#include "glsl/math.glsl"

in struct Vertex
{
	vec3 position;
} vertex;

out vec4 frag_color;

uniform samplerCube environment_cubemap;

void main()
{		
    vec3 normal = normalize(vertex.position);
    vec3 up = vec3(0, 1, 0);
    vec3 right = cross(up, normal);
    up = cross(normal, right);
       
    vec3 irradiance = vec3(0);
    float sample_delta = 0.025;
    float num_samples = 0;
    for (float phi = 0; phi < 2 * PI; phi += sample_delta)
    {
        for (float theta = 0; theta < 0.5 * PI; theta += sample_delta)
        {
            vec3 tangent_sample = vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
            vec3 sample_position = tangent_sample.x * right + tangent_sample.y * up + tangent_sample.z * normal; 
            irradiance += texture(environment_cubemap, sample_position).rgb * cos(theta) * sin(theta);
            num_samples++;
        }
    }
    irradiance = PI * irradiance * (1 / float(num_samples));
    
    frag_color = vec4(irradiance, 1);
}
