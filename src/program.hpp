#ifndef PROGRAM_HPP
#define PROGRAM_HPP

#include <glm/glm.hpp>
#include <GL/glew.h>
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
        void unbind(void) const;

        void set_int(const std::string &name, GLint value) const;
        void set_unsigned_int(const std::string &name, GLuint value) const;
        void set_float(const std::string &name, GLfloat value) const;
        void set_vec3(const std::string &name, glm::vec3 vec3) const;
        void set_vec4(const std::string &name, glm::vec4 vec4) const;
        void set_mat4(const std::string &name, glm::mat4 mat4) const;
        void set_mat4_vector(const std::string &name, std::vector<glm::mat4> mat4_vector) const;

    private:
        const std::string vertex_filename;
        const std::string geometry_filename;
        const std::string fragment_filename;

        GLuint program_id;

        mutable std::unordered_map<std::string, GLint> uniforms;

        GLuint create_program() const;
        GLuint create_shader(GLenum type, const std::string &filename) const;

        GLint get_location(const std::string &name) const;
    };
}

#endif
