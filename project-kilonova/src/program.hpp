#ifndef GAME_PROGRAM_HPP
#define GAME_PROGRAM_HPP

#include <string>
#include <unordered_map>

#include <cglm/cglm.h>
#include <GL/glew.h>

namespace pk
{
    class program
    {
    public:
        program(const std::string &vertex_filename, const std::string &fragment_filename);
        ~program();

        void bind() const;
        void unbind(void) const;
        GLint get_location(const std::string &name) const;
        void set_int(const std::string &name, int value) const;
        void set_unsigned_int(const std::string &name, unsigned int value) const;
        void set_float(const std::string &name, float value) const;
        void set_vec3(const std::string &name, vec3 vec) const;
        void set_vec4(const std::string &name, vec4 vec) const;
        void set_mat4(const std::string &name, mat4 mat) const;
    private:
        GLuint program_id;
        mutable std::unordered_map<std::string, GLint> uniforms;

        GLuint create_shader(GLenum type, const std::string &filename) const;
    };
}

#endif
