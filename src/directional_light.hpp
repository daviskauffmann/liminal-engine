#ifndef GAME_DIRECTIONAL_LIGHT_HPP
#define GAME_DIRECTIONAL_LIGHT_HPP

#include <GL/glew.h>

#include <glm/matrix.hpp>
#include <glm/vec3.hpp>

namespace pk
{
struct directional_light
{
    glm::vec3 direction;
    glm::vec3 color;
    int depth_map_width;
    int depth_map_height;
    glm::mat4 projection;
    glm::mat4 view;
    GLuint depth_map_fbo_id;
    GLuint depth_map_texture_id;

    directional_light(
        glm::vec3 direction,
        glm::vec3 color,
        int depth_map_width, int depth_map_height);
    ~directional_light();

    void set_depth_map_size(int depth_map_width, int depth_map_height);

    glm::mat4 calc_projection() const;
    glm::mat4 calc_view(glm::vec3 center) const;
};
} // namespace pk

#endif