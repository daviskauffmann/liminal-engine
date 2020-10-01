#ifndef SPOT_LIGHT_HPP
#define SPOT_LIGHT_HPP

#include <glm/matrix.hpp>
#include <glm/vec3.hpp>
#include <GL/glew.h>

namespace pk
{
    struct spot_light
    {
        glm::vec3 position;
        glm::vec3 direction;
        glm::vec3 color;
        float inner_cutoff;
        float outer_cutoff;
        int depth_map_size;
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
            int depth_map_size);
        ~spot_light();

        void set_depth_map_size(int depth_map_size);

        glm::mat4 calc_projection() const;
        glm::mat4 calc_view() const;
    };
} // namespace pk

#endif
