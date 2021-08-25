#ifndef POINT_LIGHT_HPP
#define POINT_LIGHT_HPP

#include <glm/matrix.hpp>
#include <glm/vec3.hpp>
#include <GL/glew.h>
#include <vector>

namespace liminal
{
    struct point_light
    {
        static float near_plane;
        static float far_plane;

        glm::vec3 color;
        GLsizei depth_cube_size;
        GLuint depth_cubemap_fbo_id;
        GLuint depth_cubemap_texture_id;
        std::vector<glm::mat4> transformation_matrices;

        point_light(
            glm::vec3 color,
            GLsizei depth_cube_size);
        ~point_light();

        void set_depth_cube_size(GLsizei depth_cube_size);

        void update_transformation_matrices(const glm::vec3 &position);
    };
}

#endif
