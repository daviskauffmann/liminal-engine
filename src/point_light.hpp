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
    int depth_cube_width;
    int depth_cube_height;
    GLuint depth_cubemap_fbo_id;
    GLuint depth_cubemap_texture_id;

    point_light(
        glm::vec3 position,
        glm::vec3 color,
        int depth_cube_width, int depth_cube_height);
    ~point_light();

    void set_depth_cube_size(int depth_cube_width, int depth_cube_height);
};
} // namespace pk

#endif
