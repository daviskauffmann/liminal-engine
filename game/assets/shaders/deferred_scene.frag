#version 460 core

in struct Vertex
{
    vec2 uv;
} vertex;

uniform struct Geometry
{
    sampler2D position;
    sampler2D normal;
    sampler2D albedo_specular;
} geometry;

uniform struct Camera
{
    vec3 position;
} camera;

uniform struct Scene
{
	vec3 sun_direction;
	vec3 sun_ambient;
	vec3 sun_diffuse;
	vec3 sun_specular;
	mat4 sun_projection;
	mat4 sun_view;
} scene;

uniform struct Depthmap {
    sampler2D texture;
} depthmap;

out vec4 frag_color;

void main()
{
    vec3 position = texture(geometry.position, vertex.uv).rgb;
    vec3 normal = texture(geometry.normal, vertex.uv).rgb;
    vec3 diffuse = texture(geometry.albedo_specular, vertex.uv).rgb;
    float specular = texture(geometry.albedo_specular, vertex.uv).a;
	float shininess = 16.0; // TODO: find a place in gbuffer to put material shininess
    vec3 view_direction = normalize(camera.position - position);

	// ambient
	vec3 final_ambient = scene.sun_ambient * diffuse;

    // diffuse
    vec3 light_direction = normalize(-scene.sun_direction);
    float diffuse_factor = max(dot(normal, light_direction), 0.0);
    vec3 final_diffuse = scene.sun_diffuse * diffuse * diffuse_factor;

    // specular
    vec3 halfway_direction = normalize(light_direction + view_direction);
	float specular_angle = max(dot(normal, halfway_direction), 0.0);
    float specular_factor = pow(specular_angle, shininess);
    vec3 final_specular = scene.sun_specular * specular * specular_factor;

    // shadow
    vec4 shadow_position = scene.sun_projection * scene.sun_view * vec4(position, 1.0);
    vec3 proj_coords = (shadow_position.xyz / shadow_position.w) * 0.5 + 0.5;
    float current_depth = proj_coords.z;
    float bias = max(0.05 * (1.0 - dot(normal, light_direction)), 0.005); 
    float shadow = 0.0;
    vec2 texel_size = 1.0 / textureSize(depthmap.texture, 0);
    for (int x = -1; x <= 1; x++)
    {
        for (int y = -1; y <= 1; y++)
        {
            float pcf_depth = texture(depthmap.texture, proj_coords.xy + vec2(x, y) * texel_size).r;
            shadow += current_depth - bias > pcf_depth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;
    if (proj_coords.z > 1.0) shadow = 0.0;

    frag_color = vec4((final_ambient + (1.0 - shadow) * (final_diffuse + final_specular)), 1.0);
}
