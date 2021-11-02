#ifndef LIMINAL_GRAPHICS_TEXTURE_HPP
#define LIMINAL_GRAPHICS_TEXTURE_HPP

#include <GL/glew.h>
#include <string>

namespace liminal
{
    class texture
    {
    public:
        texture(const std::string &filename, bool srgb = false);
        ~texture();

        void bind(unsigned int index) const;

    private:
        GLuint texture_id;
    };
}

#endif
