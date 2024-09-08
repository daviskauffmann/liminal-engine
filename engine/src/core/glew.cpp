#include <liminal/core/glew.hpp>

#include <gl/glew.h>
#include <stdexcept>

liminal::core::glew::glew()
{
    const auto error = glewInit();
    if (error != GLEW_OK)
    {
        throw std::runtime_error(reinterpret_cast<const char *>(glewGetErrorString(error)));
    }
}
