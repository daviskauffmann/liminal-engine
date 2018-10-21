#version 330 core

const int NUM_POINT_LIGHTS = 4;

in struct Vertex
{
    vec3 position;
    vec3 normal;
    vec2 uv;
    vec4 light_space_position;
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

uniform vec3 ambient;

uniform struct DirectionalLight
{
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
} directional_light;

uniform struct PointLight
{
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float constant;
    float linear;
    float quadratic;
} point_lights[NUM_POINT_LIGHTS];

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

uniform struct Depthmap {
    sampler2D texture;
} depthmap;

vec3 calc_directional_light(DirectionalLight light, vec3 normal, vec3 view_direction);
vec3 calc_point_light(PointLight light, vec3 normal, vec3 view_direction);
vec3 calc_spot_light(SpotLight light, vec3 normal, vec3 view_direction);

void main()
{
    vec3 normal = normalize(vertex.normal);
    vec3 view_direction = normalize(camera.position - vertex.position);

    vec3 color;

    // ambient light
    color += ambient * vec3(texture(material.diffuse, vertex.uv)) * material.color;

    // directional light
    color += calc_directional_light(directional_light, normal, view_direction);

    // point lights
    for (int i = 0; i < NUM_POINT_LIGHTS; i++)
    {
        color += calc_point_light(point_lights[i], normal, view_direction);
    }

    // spot light
    color += calc_spot_light(spot_light, normal, view_direction);

    // emission
    color += vec3(texture(material.emission, vertex.uv)) * material.glow;

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

    // shadow
    vec3 proj_coords = (vertex.light_space_position.xyz / vertex.light_space_position.w) * 0.5 + 0.5;
    float current_depth = proj_coords.z;
    float closest_depth = texture(depthmap.texture, proj_coords.xy).r;
    float bias = max(0.05 * (1.0 - dot(vertex.normal, light_direction)), 0.005); 
    float shadow = current_depth - bias > closest_depth  ? 1.0 : 0.0;
    if (proj_coords.z > 1.0) shadow = 0.0;

    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular));

    return lighting;
}

vec3 calc_point_light(PointLight light, vec3 normal, vec3 view_direction)
{
    // attenuation
    float light_distance = length(light.position - vertex.position);
    float attenuation = 1.0 / (light.constant + light.linear * light_distance + light.quadratic * pow(light_distance, 2));

    // ambient
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, vertex.uv)) * material.color * attenuation;

    // diffuse
    vec3 light_direction = normalize(light.position - vertex.position);
    float diff = max(dot(normal, light_direction), 0.0);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, vertex.uv)) * material.color * attenuation;

    // specular
    vec3 reflect_direction = reflect(-light_direction, normal);
    float spec = pow(max(dot(view_direction, reflect_direction), 0.0), material.shininess);
    vec3 specular = light.specular * spec * vec3(texture(material.specular, vertex.uv)) * material.color * attenuation;

    return ambient + diffuse + specular;
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
