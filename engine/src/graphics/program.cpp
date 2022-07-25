#include <liminal/graphics/program.hpp>

#include <fstream>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <stb_include.h>

liminal::program::program(
    const std::string &vertex_filename,
    const std::string &geometry_filename,
    const std::string &fragment_filename)
    : vertex_filename(vertex_filename),
      geometry_filename(geometry_filename),
      fragment_filename(fragment_filename)
{
    program_id = create_program();
}

liminal::program::program(
    const std::string &vertex_filename,
    const std::string &fragment_filename)
    : program(vertex_filename, "", fragment_filename)
{
}

liminal::program::~program()
{
    glDeleteProgram(program_id);
}

void liminal::program::reload()
{
    // TODO: it'd be cool if this just watched the files and automatically reloaded
    // would need some way to update texture samplers, but only the renderer knows that information right now
    const auto new_program_id = create_program();
    if (new_program_id)
    {
        glDeleteProgram(program_id);
        program_id = new_program_id;
    }
}

void liminal::program::bind() const
{
    glUseProgram(program_id);
}

void liminal::program::unbind(void) const
{
    glUseProgram(0);
}

void liminal::program::set_int(const std::string &name, GLint value) const
{
    glUniform1i(get_location(name), value);
}

void liminal::program::set_unsigned_int(const std::string &name, GLuint value) const
{
    glUniform1ui(get_location(name), value);
}

void liminal::program::set_float(const std::string &name, GLfloat value) const
{
    glUniform1f(get_location(name), value);
}

void liminal::program::set_vec3(const std::string &name, const glm::vec3 &vec3) const
{
    glUniform3fv(get_location(name), 1, glm::value_ptr(vec3));
}

void liminal::program::set_vec4(const std::string &name, const glm::vec4 &vec4) const
{
    glUniform4fv(get_location(name), 1, glm::value_ptr(vec4));
}

void liminal::program::set_mat4(const std::string &name, const glm::mat4 &mat4) const
{
    glUniformMatrix4fv(get_location(name), 1, GL_FALSE, glm::value_ptr(mat4));
}

void liminal::program::set_mat4_vector(const std::string &name, const std::vector<glm::mat4> &mat4_vector) const
{
    glUniformMatrix4fv(get_location(name), (GLsizei)mat4_vector.size(), GL_FALSE, glm::value_ptr(mat4_vector[0]));
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
    if (!geometry_filename.empty())
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
            glGetProgramInfoLog(new_program_id, length, &length, &info_log[0]);

            std::cerr << "Error: Failed to link program: " << &info_log[0] << std::endl;
            return 0;
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
            glGetProgramInfoLog(new_program_id, length, &length, &info_log[0]);

            std::cerr << "Error: Failed to validate program: " << &info_log[0] << std::endl;
            return 0;
        }
    }

    return new_program_id;
}

GLuint liminal::program::create_shader(GLenum type, const std::string &filename) const
{
    const auto shader_id = glCreateShader(type);

    char error[256];
    const auto source = stb_include_file(
        const_cast<char *>(filename.c_str()),
        NULL,
        const_cast<char *>("assets/shaders"),
        error);
    if (!source)
    {
        std::cerr << "Error: Failed to preprocess shader: " << error << std::endl;
        return 0;
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
        glGetShaderInfoLog(shader_id, length, &length, &info_log[0]);

        std::cerr << "Error: Failed to compile shader: " << &info_log[0] << std::endl;
        return 0;
    }

    return shader_id;
}

GLint liminal::program::get_location(const std::string &name) const
{
    if (uniforms.find(name) == uniforms.end())
    {
        uniforms[name] = glGetUniformLocation(program_id, name.c_str());
    }

    return uniforms[name];
}
