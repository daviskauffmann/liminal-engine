#ifndef LIMINAL_GRAPHICS_SKYBOX_HPP
#define LIMINAL_GRAPHICS_SKYBOX_HPP

#include <gl/glew.h>

namespace liminal
{
    class skybox
    {
    public:
        skybox(const char *filename);
        ~skybox();
        skybox(const skybox &other) = delete;
        skybox &operator=(const skybox &other) = delete;
        skybox(skybox &&other) = delete;
        skybox &operator=(skybox &&other) = delete;

        void set_cubemaps(const char *filename);

        void bind_environment_cubemap(unsigned int index) const;
        void bind_irradiance_cubemap(unsigned int index) const;
        void bind_prefilter_cubemap(unsigned int index) const;

    private:
        GLuint environment_cubemap_id = 0;
        GLuint irradiance_cubemap_id = 0;
        GLuint prefilter_cubemap_id = 0;
    };
}

#endif
