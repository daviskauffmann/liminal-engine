#ifndef LIMINAL_GRAPHICS_PROGRAM_HPP
#define LIMINAL_GRAPHICS_PROGRAM_HPP

#include <gl/glew.h>
#include <glm/glm.hpp>
#include <unordered_map>
#include <utility>
#include <vector>

namespace liminal
{
    class program
    {
    public:
        static void unbind();

        program(
            const char *vertex_filename,
            const char *geometry_filename,
            const char *fragment_filename);
        program(
            const char *vertex_filename,
            const char *fragment_filename);
        ~program();
        program(const program &other) = delete;
        program &operator=(const program &other) = delete;
        program(program &&other) = delete;
        program &operator=(program &&other) = delete;

        void reload();

        void bind() const;

        void set_int(const char *name, GLint value) const;
        void set_unsigned_int(const char *name, GLuint value) const;
        void set_float(const char *name, GLfloat value) const;
        void set_vec3(const char *name, const glm::vec3 &vec3) const;
        void set_vec4(const char *name, const glm::vec4 &vec4) const;
        void set_mat4(const char *name, const glm::mat4 &mat4) const;
        void set_mat4_vector(const char *name, const std::vector<glm::mat4> &mat4_vector) const;
        void set_sampler(const char *name, GLint value) const;
        void set_samplers(const std::vector<std::pair<const char *, GLint>> &sampler_pairs) const;

    private:
        const char *vertex_filename;
        const char *geometry_filename;
        const char *fragment_filename;

        GLuint program_id;

        mutable std::unordered_map<const char *, GLint> uniforms;
        mutable std::unordered_map<const char *, GLint> samplers;

        GLuint create_program() const;
        GLuint create_shader(GLenum type, const char *filename) const;

        GLint get_location(const char *name) const;
    };
}

#endif
