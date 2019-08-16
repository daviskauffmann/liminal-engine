#include "program.hpp"

#include <fstream>
#include <iostream>
#include <sstream>

#include <glm/gtc/type_ptr.hpp>

namespace pk
{
    program::program(const std::string &vertex_filename, const std::string &fragment_filename)
    {
        this->program_id = glCreateProgram();

        // compile shaders and attach to program
        GLuint vertex_shader = create_shader(GL_VERTEX_SHADER, vertex_filename);
        glAttachShader(this->program_id, vertex_shader);

        GLuint fragment_shader = create_shader(GL_FRAGMENT_SHADER, fragment_filename);
        glAttachShader(this->program_id, fragment_shader);

        // link program
        glLinkProgram(this->program_id);
        {
            GLint success;
            glGetProgramiv(this->program_id, GL_LINK_STATUS, &success);

            if (!success)
            {
                GLchar info_log[1024];
                glGetProgramInfoLog(this->program_id, sizeof(info_log), nullptr, info_log);

                std::cout << "Error: Program linking failed\n" << info_log << std::endl;
            }
        }

        // detach and delete shaders, we're done with them now
        glDetachShader(this->program_id, vertex_shader);
        glDeleteShader(vertex_shader);

        glDetachShader(this->program_id, fragment_shader);
        glDeleteShader(fragment_shader);

        // check for errors
        glValidateProgram(this->program_id);
        {
            GLint success;
            glGetProgramiv(this->program_id, GL_VALIDATE_STATUS, &success);

            if (!success)
            {
                GLchar info_log[1024];
                glGetProgramInfoLog(this->program_id, sizeof(info_log), nullptr, info_log);

                std::cout << "Error: Program validation failed\n" << info_log << std::endl;
            }
        }
    }

    program::~program()
    {
        glDeleteProgram(this->program_id);
    }

    void program::bind() const
    {
        glUseProgram(this->program_id);
    }

    void program::unbind(void) const
    {
        glUseProgram(0);
    }

    GLint program::get_location(const std::string &name) const
    {
        GLint location;

        if (this->uniforms.find(name) == this->uniforms.end())
        {
            location = glGetUniformLocation(this->program_id, name.c_str());

            this->uniforms[name] = location;
        }
        else
        {
            location = this->uniforms[name];
        }

        return location;
    }


    void program::set_int(const std::string &name, int value) const
    {
        glUniform1i(this->get_location(name), value);
    }

    void program::set_unsigned_int(const std::string &name, unsigned int value) const
    {
        glUniform1ui(this->get_location(name), value);
    }

    void program::set_float(const std::string &name, float value) const
    {
        glUniform1f(this->get_location(name), value);
    }

    void program::set_vec3(const std::string &name, glm::vec3 vec3) const
    {
        glUniform3fv(this->get_location(name), 1, glm::value_ptr(vec3));
    }

    void program::set_vec4(const std::string &name, glm::vec4 vec4) const
    {
        glUniform4fv(this->get_location(name), 1, glm::value_ptr(vec4));
    }

    void program::set_mat4(const std::string &name, glm::mat4 mat4) const
    {
        glUniformMatrix4fv(this->get_location(name), 1, GL_FALSE, glm::value_ptr(mat4));
    }

    GLuint program::create_shader(GLenum type, const std::string &filename) const
    {
        // create shader
        GLuint shader = glCreateShader(type);

        // read file
        std::ifstream file;
        file.open(filename);

        std::stringstream stream;
        stream << file.rdbuf();

        file.close();

        std::string source_str = stream.str();
        const char *source_c_str = source_str.c_str();

        // set shader source
        glShaderSource(shader, 1, &source_c_str, nullptr);

        // compile the shader
        glCompileShader(shader);

        // check errors
        GLint success;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

        if (!success)
        {
            GLchar info_log[1024];
            glGetShaderInfoLog(shader, sizeof(info_log), nullptr, info_log);

            std::cout << "Error: Shader compilation failed\n" << info_log << std::endl;
        }

        return shader;
    }
}
