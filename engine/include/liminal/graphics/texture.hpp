#ifndef LIMINAL_GRAPHICS_TEXTURE_HPP
#define LIMINAL_GRAPHICS_TEXTURE_HPP

#include <GL/glew.h>
#include <string>

namespace liminal
{
    class texture
    {
    public:
        texture(const std::string &filename, const bool srgb = false, const bool filter = true);
        ~texture();

        void bind(const unsigned int index) const;

    private:
        GLuint texture_id;
    };
}

#endif
