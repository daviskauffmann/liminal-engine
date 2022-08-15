#ifndef LIMINAL_GRAPHICS_TEXTURE_HPP
#define LIMINAL_GRAPHICS_TEXTURE_HPP

#include <gl/glew.h>

namespace liminal
{
    class texture
    {
    public:
        texture(const char *filename, bool srgb = false, bool filter = true);
        ~texture();

        void bind(unsigned int index) const;

    private:
        GLuint texture_id;
    };
}

#endif
