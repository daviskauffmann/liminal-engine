#version 460 core

in struct Vertex
{
	vec3 position;
} vertex;

uniform samplerCube environment_cubemap;

out vec4 frag_color;

const float PI = 3.14159265359;

void main()
{		
    vec3 normal = normalize(vertex.position);
    vec3 up = vec3(0.0, 1.0, 0.0);
    vec3 right = cross(up, normal);
    up = cross(normal, right);
       
    vec3 irradiance = vec3(0.0);
    float sample_delta = 0.025;
    float num_samples = 0.0;
    for(float phi = 0.0; phi < 2.0 * PI; phi += sample_delta)
    {
        for(float theta = 0.0; theta < 0.5 * PI; theta += sample_delta)
        {
            vec3 tangent_sample = vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
            vec3 sample = tangent_sample.x * right + tangent_sample.y * up + tangent_sample.z * normal; 
            irradiance += texture(environment_cubemap, sample).rgb * cos(theta) * sin(theta);
            num_samples++;
        }
    }
    irradiance = PI * irradiance * (1.0 / float(num_samples));
    
    frag_color = vec4(irradiance, 1.0);
}
