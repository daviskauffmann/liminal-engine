#ifndef LIMINAL_GRAPHICS_PROGRAM_HPP
#define LIMINAL_GRAPHICS_PROGRAM_HPP

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>
#include <unordered_map>
#include <vector>

namespace liminal
{
    class program
    {
    public:
        program(
            const std::string &vertex_filename,
            const std::string &geometry_filename,
            const std::string &fragment_filename);
        program(
            const std::string &vertex_filename,
            const std::string &fragment_filename);
        ~program();

        void reload();

        void bind() const;
        void unbind() const;

        void set_int(const std::string &name, const GLint value) const;
        void set_unsigned_int(const std::string &name, const GLuint value) const;
        void set_float(const std::string &name, const GLfloat value) const;
        void set_vec3(const std::string &name, const glm::vec3 &vec3) const;
        void set_vec4(const std::string &name, const glm::vec4 &vec4) const;
        void set_mat4(const std::string &name, const glm::mat4 &mat4) const;
        void set_mat4_vector(const std::string &name, const std::vector<glm::mat4> &mat4_vector) const;

    private:
        const std::string vertex_filename;
        const std::string geometry_filename;
        const std::string fragment_filename;

        GLuint program_id;

        mutable std::unordered_map<std::string, GLint> uniforms;

        GLuint create_program() const;
        GLuint create_shader(const GLenum type, const std::string &filename) const;

        GLint get_location(const std::string &name) const;
    };
}

#endif
