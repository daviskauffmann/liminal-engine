#version 330 core

const int NUM_POINT_LIGHTS = 4;

in struct Vertex
{
    vec2 uv;
} vertex;

uniform struct Geometry
{
    sampler2D position;
    sampler2D normal;
    sampler2D albedospecular;
    sampler2D light_space_position;
} geometry;

uniform struct Camera
{
    mat4 projection;
    mat4 view;
    vec3 position;
} camera;

uniform struct Sun
{
    mat4 projection;
    mat4 view;
} sun;

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

vec3 calc_directional_light(DirectionalLight light, vec3 position, vec3 normal, vec3 diffuse, float specular, vec4 light_space_position, vec3 view_direction);
vec3 calc_point_light(PointLight light, vec3 position, vec3 normal, vec3 diffuse, float specular, vec4 light_space_position, vec3 view_direction);
vec3 calc_spot_light(SpotLight light, vec3 position, vec3 normal, vec3 diffuse, float specular, vec4 light_space_position, vec3 view_direction);

void main()
{
    vec3 position = texture(geometry.position, vertex.uv).rgb;
    vec3 normal = texture(geometry.normal, vertex.uv).rgb;
    vec3 diffuse = texture(geometry.albedospecular, vertex.uv).rgb;
    float specular = texture(geometry.albedospecular, vertex.uv).a;
    vec4 light_space_position = sun.projection * sun.view * vec4(position, 1.0);
    vec3 view_direction = normalize(camera.position - position);

    vec3 color;

    // ambient light
    color += ambient * diffuse;

    // directional light
    color += calc_directional_light(directional_light, position, normal, diffuse, specular, light_space_position, view_direction);

    // point lights
    for (int i = 0; i < NUM_POINT_LIGHTS; i++)
    {
        color += calc_point_light(point_lights[i], position, normal, diffuse, specular, light_space_position, view_direction);
    }

    // spot light
    color += calc_spot_light(spot_light, position, normal, diffuse, specular, light_space_position, view_direction);

    // emission
    // color += vec3(texture(material.emission, vertex.uv)) * material.glow;

    gl_FragColor = vec4(color, 1.0);
}

vec3 calc_directional_light(DirectionalLight light, vec3 position, vec3 normal, vec3 diffuse, float specular, vec4 light_space_position, vec3 view_direction)
{
    // ambient
    vec3 final_ambient = light.ambient * diffuse;

    // diffuse
    vec3 light_direction = normalize(-light.direction);
    float diffuse_factor = max(dot(normal, light_direction), 0.0);
    vec3 final_diffuse = light.diffuse * diffuse * diffuse_factor;

    // specular
    vec3 reflect_direction = reflect(-light_direction, normal);
    float specular_factor = pow(max(dot(view_direction, reflect_direction), 0.0), 16.0);
    vec3 final_specular = light.specular * specular * specular_factor;

    // shadow
    vec3 proj_coords = (light_space_position.xyz / light_space_position.w) * 0.5 + 0.5;
    float closest_depth = texture(depthmap.texture, proj_coords.xy).r;
    float current_depth = proj_coords.z;
    float bias = max(0.05 * (1.0 - dot(normal, light_direction)), 0.005); 
    float shadow = current_depth - bias > closest_depth ? 1.0 : 0.0;
    if (proj_coords.z > 1.0) shadow = 0.0;

    return (final_ambient + (1.0 - shadow) * (final_diffuse + final_specular));

    // return final_ambient + final_diffuse + final_specular;
}

vec3 calc_point_light(PointLight light, vec3 position, vec3 normal, vec3 diffuse, float specular, vec4 light_space_position, vec3 view_direction)
{
    // ambient
    vec3 final_ambient = light.ambient * diffuse;

    // diffuse
    vec3 light_direction = normalize(light.position - position);
    float diffuse_factor = max(dot(normal, light_direction), 0.0);
    vec3 final_diffuse = light.diffuse * diffuse * diffuse_factor;

    // specular
    vec3 reflect_direction = reflect(-light_direction, normal);
    float specular_factor = pow(max(dot(view_direction, reflect_direction), 0.0), 16.0);
    vec3 final_specular = light.specular * specular * specular_factor;

    // attenuation
    float light_distance = length(light.position - position);
    float attenuation = 1.0 / (light.constant + light.linear * light_distance + light.quadratic * pow(light_distance, 2));

    return (final_ambient + final_diffuse + final_specular) * attenuation;
}

vec3 calc_spot_light(SpotLight light, vec3 position, vec3 normal, vec3 diffuse, float specular, vec4 light_space_position, vec3 view_direction)
{
    // ambient
    vec3 final_ambient = light.ambient * diffuse;

    // diffuse
    vec3 light_direction = normalize(light.position - position);
    float diffuse_factor = max(dot(normal, light_direction), 0.0);
    vec3 final_diffuse = light.diffuse * diffuse * diffuse_factor;

    // specular
    vec3 reflect_direction = reflect(-light_direction, normal);
    float specular_factor = pow(max(dot(view_direction, reflect_direction), 0.0), 16.0);
    vec3 final_specular = light.specular * specular * specular_factor;

    // attenuation
    float light_distance = length(light.position - position);
    float attenuation = 1.0 / (light.constant + light.linear * light_distance + light.quadratic * pow(light_distance, 2));

    // intensity
    float theta = dot(light_direction, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    return (final_ambient + final_diffuse + final_specular) * attenuation * intensity;
}
