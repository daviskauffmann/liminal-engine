#ifndef LIMINAL_COMPONENTS_POINT_LIGHT_HPP
#define LIMINAL_COMPONENTS_POINT_LIGHT_HPP

#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/matrix.hpp>
#include <glm/vec3.hpp>

namespace liminal
{
    class point_light
    {
    public:
        glm::vec3 color = glm::vec3(1);

        glm::mat4 view_projection_matrices[6] = {};
        GLuint depth_cubemap_texture_id = 0;
    };
}

#endif
