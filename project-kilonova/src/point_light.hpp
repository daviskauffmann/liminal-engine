#ifndef GAME_POINT_LIGHT_HPP
#define GAME_POINT_LIGHT_HPP

#include <GL/glew.h>

#include <glm/vec3.hpp>

namespace pk
{
struct point_light
{
    glm::vec3 position;
    glm::vec3 color;
    GLuint depth_cubemap_fbo_id;
    GLuint depth_cubemap_texture_id;

    point_light(
        glm::vec3 position,
        glm::vec3 color);
    ~point_light();
};
} // namespace pk

#endif
