#ifndef LIMINAL_GRAPHICS_SKYBOX_HPP
#define LIMINAL_GRAPHICS_SKYBOX_HPP

#include <GL/glew.h>
#include <string>

namespace liminal
{
    struct skybox
    {
        GLuint environment_cubemap_id;
        GLuint irradiance_cubemap_id;
        GLuint prefilter_cubemap_id;

        skybox(const std::string &filename);
        ~skybox();

        void set_cubemap(const std::string &filename);
    };
}

#endif
