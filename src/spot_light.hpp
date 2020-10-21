#ifndef SPOT_LIGHT_HPP
#define SPOT_LIGHT_HPP

#include <glm/matrix.hpp>
#include <glm/vec3.hpp>
#include <GL/glew.h>

namespace pk
{
    struct spot_light
    {
        static float near_plane;
        static float far_plane;

        glm::vec3 position;
        glm::vec3 direction;
        glm::vec3 color;
        float inner_cutoff;
        float outer_cutoff;
        GLsizei depth_map_size;
        GLuint depth_map_fbo_id;
        GLuint depth_map_texture_id;
        glm::mat4 transformation_matrix;

        spot_light(
            glm::vec3 position,
            glm::vec3 direction,
            glm::vec3 color,
            float inner_cutoff,
            float outer_cutoff,
            GLsizei depth_map_size);
        ~spot_light();

        void set_depth_map_size(GLsizei depth_map_size);

        void update_transformation_matrix();
    };
} // namespace pk

#endif
