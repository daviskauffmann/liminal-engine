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
    glm::mat4 projection;
    glm::mat4 view;
    GLuint depthmap_fbo_id;
    GLuint depthmap_texture_id;

    directional_light(
        glm::vec3 direction,
        glm::vec3 color);
    ~directional_light();

    glm::mat4 calc_projection() const;
    glm::mat4 calc_view(glm::vec3 center) const;
};
} // namespace pk

#endif
