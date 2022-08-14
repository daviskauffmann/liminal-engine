#include <liminal/graphics/program.hpp>

#include <fstream>
#include <glm/gtc/type_ptr.hpp>
#include <stb_include.h>
#include <stdexcept>

liminal::program::program(
    const char *const vertex_filename,
    const char *const geometry_filename,
    const char *const fragment_filename)
    : vertex_filename(vertex_filename),
      geometry_filename(geometry_filename),
      fragment_filename(fragment_filename)
{
    program_id = create_program();
}

liminal::program::program(
    const char *const vertex_filename,
    const char *const fragment_filename)
    : program(vertex_filename, nullptr, fragment_filename)
{
}

liminal::program::~program()
{
    glDeleteProgram(program_id);
}

void liminal::program::reload()
{
    // TODO: it'd be cool if this just watched the files and automatically reloaded
    const auto new_program_id = create_program();
    if (new_program_id)
    {
        glDeleteProgram(program_id);
        program_id = new_program_id;

        uniforms.clear();

        bind();
        for (const auto [name, value] : samplers)
        {
            set_int(name, value);
        }
        unbind();
    }
}

void liminal::program::bind() const
{
    glUseProgram(program_id);
}

void liminal::program::unbind() const
{
    glUseProgram(0);
}

void liminal::program::set_int(const char *name, const GLint value) const
{
    glUniform1i(get_location(name), value);
}

void liminal::program::set_unsigned_int(const char *name, GLuint value) const
{
    glUniform1ui(get_location(name), value);
}

void liminal::program::set_float(const char *name, const GLfloat value) const
{
    glUniform1f(get_location(name), value);
}

void liminal::program::set_vec3(const char *name, const glm::vec3 &vec3) const
{
    glUniform3fv(get_location(name), 1, glm::value_ptr(vec3));
}

void liminal::program::set_vec4(const char *name, const glm::vec4 &vec4) const
{
    glUniform4fv(get_location(name), 1, glm::value_ptr(vec4));
}

void liminal::program::set_mat4(const char *name, const glm::mat4 &mat4) const
{
    glUniformMatrix4fv(get_location(name), 1, GL_FALSE, glm::value_ptr(mat4));
}

void liminal::program::set_mat4_vector(const char *name, const std::vector<glm::mat4> &mat4_vector) const
{
    glUniformMatrix4fv(get_location(name), static_cast<GLsizei>(mat4_vector.size()), GL_FALSE, glm::value_ptr(mat4_vector.at(0)));
}

void liminal::program::set_sampler(const char *name, const GLint value) const
{
    if (!samplers.contains(name))
    {
        samplers.insert({name, value});
    }

    set_int(name, value);
}

void liminal::program::set_samplers(const std::vector<std::pair<const char *, GLint>> &sampler_pairs) const
{
    for (const auto [name, value] : sampler_pairs)
    {
        set_sampler(name, value);
    }
}

GLuint liminal::program::create_program() const
{
    const auto new_program_id = glCreateProgram();

    const auto vertex_shader_id = create_shader(GL_VERTEX_SHADER, vertex_filename);
    if (!vertex_shader_id)
    {
        return 0;
    }
    glAttachShader(new_program_id, vertex_shader_id);

    GLuint geometry_shader_id = 0;
    if (geometry_filename)
    {
        geometry_shader_id = create_shader(GL_GEOMETRY_SHADER, geometry_filename);
        if (!geometry_shader_id)
        {
            return 0;
        }
        glAttachShader(new_program_id, geometry_shader_id);
    }

    const auto fragment_shader_id = create_shader(GL_FRAGMENT_SHADER, fragment_filename);
    if (!fragment_shader_id)
    {
        return 0;
    }
    glAttachShader(new_program_id, fragment_shader_id);

    glLinkProgram(new_program_id);
    {
        GLint success;
        glGetProgramiv(new_program_id, GL_LINK_STATUS, &success);
        if (!success)
        {
            GLint length;
            glGetProgramiv(new_program_id, GL_INFO_LOG_LENGTH, &length);

            std::vector<GLchar> info_log(length);
            glGetProgramInfoLog(new_program_id, length, &length, info_log.data());

            throw std::runtime_error(info_log.data());
        }
    }

    glDetachShader(new_program_id, vertex_shader_id);
    glDeleteShader(vertex_shader_id);

    if (geometry_shader_id)
    {
        glDetachShader(new_program_id, geometry_shader_id);
        glDeleteShader(geometry_shader_id);
    }

    glDetachShader(new_program_id, fragment_shader_id);
    glDeleteShader(fragment_shader_id);

    glValidateProgram(new_program_id);
    {
        GLint success;
        glGetProgramiv(new_program_id, GL_VALIDATE_STATUS, &success);
        if (!success)
        {
            GLint length;
            glGetProgramiv(new_program_id, GL_INFO_LOG_LENGTH, &length);

            std::vector<GLchar> info_log(length);
            glGetProgramInfoLog(new_program_id, length, &length, info_log.data());

            throw std::runtime_error(info_log.data());
        }
    }

    return new_program_id;
}

GLuint liminal::program::create_shader(const GLenum type, const char *const filename) const
{
    const auto shader_id = glCreateShader(type);

    char error[256];
    const auto source = stb_include_file(
        const_cast<char *>(filename),
        nullptr,
        const_cast<char *>("assets/shaders"),
        error);
    if (!source)
    {
        throw std::runtime_error(error);
    }

    glShaderSource(shader_id, 1, &source, nullptr);
    glCompileShader(shader_id);

    free(source);

    GLint success;
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        GLint length;
        glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &length);

        std::vector<GLchar> info_log(length);
        glGetShaderInfoLog(shader_id, length, &length, info_log.data());

        throw std::runtime_error(info_log.data());
    }

    return shader_id;
}

GLint liminal::program::get_location(const char *name) const
{
    if (!uniforms.contains(name))
    {
        uniforms.insert({name, glGetUniformLocation(program_id, name)});
    }

    return uniforms.at(name);
}
