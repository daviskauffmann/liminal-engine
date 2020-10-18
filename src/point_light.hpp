#ifndef POINT_LIGHT_HPP
#define POINT_LIGHT_HPP

#include <glm/matrix.hpp>
#include <glm/vec3.hpp>
#include <GL/glew.h>
#include <vector>

namespace pk
{
    struct point_light
    {
        static float near_plane;
        static float far_plane;

        glm::vec3 position;
        glm::vec3 color;
        int depth_cube_size;
        GLuint depth_cubemap_fbo_id;
        GLuint depth_cubemap_texture_id;

        point_light(
            glm::vec3 position,
            glm::vec3 color,
            int depth_cube_size);
        ~point_light();

        void set_depth_cube_size(int depth_cube_size);

        std::vector<glm::mat4> calc_mvps() const;
    };
} // namespace pk

#endif
