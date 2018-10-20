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

uniform struct DirectionalLight
{
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
} directional_light;

vec3 calc_directional_light(DirectionalLight light, vec3 normal, vec3 view_direction);

void main()
{
    vec3 normal = normalize(vertex.normal);
    vec3 view_direction = normalize(camera.position - vertex.position);

    vec3 color;

    // directional light
    color += calc_directional_light(directional_light, normal, view_direction);

    gl_FragColor = vec4(color, 1.0);
}

vec3 calc_directional_light(DirectionalLight light, vec3 normal, vec3 view_direction)
{
    // ambient
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, vertex.uv)) * material.color;

    // diffuse
    vec3 light_direction = normalize(-light.direction);
    float diff = max(dot(normal, light_direction), 0.0);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, vertex.uv)) * material.color;

    // specular
    vec3 reflect_direction = reflect(-light_direction, normal);
    float spec = pow(max(dot(view_direction, reflect_direction), 0.0), material.shininess);
    vec3 specular = light.specular * spec * vec3(texture(material.specular, vertex.uv)) * material.color;

    return ambient + diffuse + specular;
}
