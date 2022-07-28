#ifndef LIMINAL_GRAPHICS_SKYBOX_HPP
#define LIMINAL_GRAPHICS_SKYBOX_HPP

#include <GL/glew.h>
#include <string>

namespace liminal
{
    class skybox
    {
    public:
        GLuint environment_cubemap_id = 0;
        GLuint irradiance_cubemap_id = 0;
        GLuint prefilter_cubemap_id = 0;

        skybox(const std::string &filename);
        ~skybox();

        void set_cubemap(const std::string &filename);
    };
}

#endif
