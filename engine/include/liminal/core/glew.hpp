#ifndef LIMINAL_CORE_GLEW_HPP
#define LIMINAL_CORE_GLEW_HPP

namespace liminal
{
    class glew
    {
    public:
        glew();
        ~glew() = default;
        glew(const glew &other) = delete;
        glew &operator=(const glew &other) = delete;
        glew(glew &&other) = delete;
        glew &operator=(glew &&other) = delete;
    };
}

#endif
