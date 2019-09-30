#ifndef GAME_PROGRAM_HPP
#define GAME_PROGRAM_HPP

#include <string>
#include <unordered_map>

#include <GL/glew.h>
#include <glm/glm.hpp>

namespace pk
{
class program
{
public:
    program(
        const std::string &vertex_filename,
        const std::string &geometry_filename,
        const std::string &fragment_filename);
    ~program();

    void bind() const;
    void unbind(void) const;
    GLint get_location(const std::string &name) const;
    void set_int(const std::string &name, int value) const;
    void set_unsigned_int(const std::string &name, unsigned int value) const;
    void set_float(const std::string &name, float value) const;
    void set_vec3(const std::string &name, glm::vec3 vec3) const;
    void set_vec4(const std::string &name, glm::vec4 vec4) const;
    void set_mat4(const std::string &name, glm::mat4 mat4) const;

private:
    GLuint program_id;
    mutable std::unordered_map<std::string, GLint> uniforms;

    GLuint create_shader(GLenum type, const std::string &filename) const;
};
} // namespace pk

#endif
