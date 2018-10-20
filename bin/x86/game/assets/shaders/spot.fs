#version 330 core

in struct Vertex
{
    vec3 position;
    vec3 normal;
    vec2 uv;
} vertex;

uniform struct Camera
{
    mat4 projection;
    mat4 view;
    vec3 position;
} camera;

uniform struct Material
{
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emission;
    vec3 color;
    float shininess;
    float glow;
} material;

uniform struct SpotLight
{
    vec3 position;
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float constant;
    float linear;
    float quadratic;    
    float cutOff;
    float outerCutOff;
} spot_light;

vec3 calc_spot_light(SpotLight light, vec3 normal, vec3 view_direction);

void main()
{
    vec3 normal = normalize(vertex.normal);
    vec3 view_direction = normalize(camera.position - vertex.position);

    vec3 color;

    // spot light
    color += calc_spot_light(spot_light, normal, view_direction);

    gl_FragColor = vec4(color, 1.0);
}

vec3 calc_spot_light(SpotLight light, vec3 normal, vec3 view_direction)
{
    // attenuation
    float light_distance = length(light.position - vertex.position);
    float attenuation = 1.0 / (light.constant + light.linear * light_distance + light.quadratic * pow(light_distance, 2));

    // intensity
    vec3 light_direction = normalize(light.position - vertex.position);
    float theta = dot(light_direction, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    // ambient
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, vertex.uv)) * material.color * attenuation * intensity;

    // diffuse
    float diff = max(dot(normal, light_direction), 0.0);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, vertex.uv)) * material.color * attenuation * intensity;

    // specular
    vec3 reflect_direction = reflect(-light_direction, normal);
    float spec = pow(max(dot(view_direction, reflect_direction), 0.0), material.shininess);
    vec3 specular = light.specular * spec * vec3(texture(material.specular, vertex.uv)) * material.color * attenuation * intensity;

    return ambient + diffuse + specular;
}
