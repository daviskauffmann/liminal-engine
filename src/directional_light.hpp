#ifndef DIRECTIONAL_LIGHT_HPP
#define DIRECTIONAL_LIGHT_HPP

#include <glm/matrix.hpp>
#include <glm/vec3.hpp>
#include <GL/glew.h>

namespace pk
{
    struct directional_light
    {
        static float shadow_map_size;
        static float near_plane;
        static float far_plane;

        glm::vec3 direction;
        glm::vec3 color;
        int depth_map_size;
        GLuint depth_map_fbo_id;
        GLuint depth_map_texture_id;
        glm::mat4 transformation_matrix;

        directional_light(
            glm::vec3 direction,
            glm::vec3 color,
            int depth_map_size);
        ~directional_light();

        void set_depth_map_size(int depth_map_size);

        void update_transformation_matrix(glm::vec3 center);
    };
} // namespace pk

#endif
