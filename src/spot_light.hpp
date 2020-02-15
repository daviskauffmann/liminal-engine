#ifndef SPOT_LIGHT_HPP
#define SPOT_LIGHT_HPP

#include <GL/glew.h>

#include <glm/matrix.hpp>
#include <glm/vec3.hpp>

namespace pk
{
struct spot_light
{
    glm::vec3 position;
    glm::vec3 direction;
    glm::vec3 color;
    float inner_cutoff;
    float outer_cutoff;
    int depth_map_width;
    int depth_map_height;
    glm::mat4 projection;
    glm::mat4 view;
    GLuint depth_map_fbo_id;
    GLuint depth_map_texture_id;

    spot_light(
        glm::vec3 position,
        glm::vec3 direction,
        glm::vec3 color,
        float inner_cutoff,
        float outer_cutoff,
        int depth_map_width, int depth_map_height);
    ~spot_light();

    void set_depth_map_size(int depth_map_width, int depth_map_height);

    glm::mat4 calc_projection() const;
    glm::mat4 calc_view() const;
};
} // namespace pk

#endif
