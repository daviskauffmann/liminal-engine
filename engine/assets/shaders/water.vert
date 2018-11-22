#version 460 core

layout (location = 0) in vec2 position;

uniform struct Camera
{
    mat4 projection;
    mat4 view;
    vec3 position;
} camera;

uniform struct Water
{
	mat4 model;
} water;

out struct Vertex
{
    vec3 position;
    vec3 normal;
} vertex;

void main()
{
    gl_Position = camera.projection * camera.view * water.model * vec4(position.x, 0.0, position.y, 1.0);
    vertex.position = vec3(water.model * vec4(position.x, 0.0, position.y, 1.0));
    vertex.normal = mat3(transpose(inverse(water.model))) * vec3(0.0, 1.0, 0.0);
}
