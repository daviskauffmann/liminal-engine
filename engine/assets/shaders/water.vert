#version 460 core

layout (location = 0) in vec2 position;

uniform struct Camera
{
	mat4 projection;
	mat4 view;
} camera;

uniform struct Water
{
	mat4 model;
} water;

void main()
{
    gl_Position = camera.projection * camera.view * water.model * vec4(position.x, 0.0, position.y, 1.0);
}
