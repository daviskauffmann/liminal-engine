#include <game/game.hpp>

static GLuint create_shader(GLenum type, const std::string &filename)
{
    // create shader
    GLuint shader = glCreateShader(type);

    // open shader file
    SDL_RWops *io = SDL_RWFromFile(filename.c_str(), "rb");

    if (!io)
    {
        std::cout << "Error: Couldn't open file " << filename << std::endl;

        return 0;
    }

    // get file size
    SDL_RWseek(io, 0, RW_SEEK_END);
    size_t size = SDL_RWtell(io);
    SDL_RWseek(io, 0, RW_SEEK_SET);

    // file buffer
    char *source = (char *)malloc(size + 1);

    if (!source)
    {
        std::cout << "Error: Couldn't allocate size " << size << std::endl;

        return 0;
    }

    // read the file into the buffer
    if (SDL_RWread(io, source, sizeof(char), size) != size)
    {
        std::cout << "Error: Couldn't read file " << filename << std::endl;

        return 0;
    }

    // null terminate
    source[size] = '\0';

    SDL_RWclose(io);

    // set the shader source
    glShaderSource(shader, 1, &source, NULL);

    free(source);

    // compile the shader
    glCompileShader(shader);

    // check errors
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        GLchar info_log[512];
        glGetShaderInfoLog(shader, sizeof(info_log), NULL, info_log);
        std::cout << info_log << std::endl;

        return 0;
    }

    return shader;
}

namespace pk
{
    program::program(const std::string &vertex_filename, const std::string &fragment_filename)
    {
        this->program_id = glCreateProgram();

        // compile shaders and attach to program
        GLuint vertex_shader = create_shader(GL_VERTEX_SHADER, vertex_filename);

        if (!vertex_shader)
        {
            throw std::exception("Couldn't compile vertex shader");
        }

        glAttachShader(this->program_id, vertex_shader);

        GLuint fragment_shader = create_shader(GL_FRAGMENT_SHADER, fragment_filename);

        if (!fragment_shader)
        {
            throw std::exception("Couldn't compile fragment shader");
        }

        glAttachShader(this->program_id, fragment_shader);

        // link program
        glLinkProgram(this->program_id);
        {
            GLint success;
            glGetProgramiv(this->program_id, GL_LINK_STATUS, &success);

            if (!success)
            {
                GLchar info_log[512];
                glGetProgramInfoLog(this->program_id, sizeof(info_log), NULL, info_log);
                std::cout << info_log << std::endl;

                throw std::exception("Program linking failed... see log");
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
                GLchar info_log[512];
                glGetProgramInfoLog(this->program_id, sizeof(info_log), NULL, info_log);
                std::cout << info_log << std::endl;

                throw std::exception("Program validation failed... see log");
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

    GLint program::get_location(const std::string &name)
    {
        GLint location;

        if (this->uniforms.find(name) == this->uniforms.end())
        {
            location = this->uniforms[name];
        }
        else
        {
            location = glGetUniformLocation(this->program_id, name.c_str());

            this->uniforms[name] = location;
        }

        return location;
    }


    void program::set_int(const std::string &name, int value)
    {
        glUniform1i(this->get_location(name), value);
    }

    void program::set_unsigned_int(const std::string &name, unsigned int value)
    {
        glUniform1ui(this->get_location(name), value);
    }

    void program::set_float(const std::string &name, float value)
    {
        glUniform1f(this->get_location(name), value);
    }

    void program::set_vec3(const std::string &name, vec3 vec)
    {
        glUniform3fv(this->get_location(name), 1, (GLfloat *)vec);
    }

    void program::set_vec4(const std::string &name, vec4 vec)
    {
        glUniform4fv(this->get_location(name), 1, (GLfloat *)vec);
    }

    void program::set_mat4(const std::string &name, mat4 mat)
    {
        glUniformMatrix4fv(this->get_location(name), 1, GL_FALSE, (GLfloat *)mat);
    }
}
