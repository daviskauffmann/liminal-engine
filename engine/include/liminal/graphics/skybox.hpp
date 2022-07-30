#ifndef LIMINAL_GRAPHICS_SKYBOX_HPP
#define LIMINAL_GRAPHICS_SKYBOX_HPP

#include <GL/glew.h>

namespace liminal
{
    class skybox
    {
    public:
        GLuint environment_cubemap_id = 0;
        GLuint irradiance_cubemap_id = 0;
        GLuint prefilter_cubemap_id = 0;

        skybox(const char *filename);
        ~skybox();

        void set_cubemap(const char *filename);
    };
}

#endif
