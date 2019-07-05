#include <engine/engine.h>

// TODO: geometry shader

// TODO: split up shader_create into shader_create_from_string and shader_create_from_file

static GLuint shader_create(GLenum type, const char *filename);

struct program *program_create(const char *vertex_filename, const char *fragment_filename)
{
    struct program *program = malloc(sizeof(struct program));

    if (!program)
    {
        printf("Error: Couldn't allocate program\n");

        return NULL;
    }

    program->program_id = glCreateProgram();

    // compile shaders and attach to program
    GLuint vertex_shader = 0;
    if (vertex_filename)
    {
        vertex_shader = shader_create(GL_VERTEX_SHADER, vertex_filename);

        if (!vertex_shader)
        {
            printf("Error: Couldn't compile vertex shader\n");

            return NULL;
        }

        glAttachShader(program->program_id, vertex_shader);
    }

    GLuint fragment_shader = 0;
    if (fragment_filename)
    {
        fragment_shader = shader_create(GL_FRAGMENT_SHADER, fragment_filename);

        if (!fragment_shader)
        {
            printf("Error: Couldn't compile fragment shader\n");

            return NULL;
        }

        glAttachShader(program->program_id, fragment_shader);
    }

    // link program
    glLinkProgram(program->program_id);
    {
        GLint success;
        glGetProgramiv(program->program_id, GL_LINK_STATUS, &success);

        if (!success)
        {
            GLchar info_log[512];
            glGetProgramInfoLog(program->program_id, sizeof(info_log), NULL, info_log);

            printf("Error: Program linking failed\n%s\n", info_log);

            return NULL;
        }
    }

    // detach and delete shaders, we're done with them now
    if (vertex_shader)
    {
        glDetachShader(program->program_id, vertex_shader);
        glDeleteShader(vertex_shader);
    }

    if (fragment_shader)
    {
        glDetachShader(program->program_id, fragment_shader);
        glDeleteShader(fragment_shader);
    }

    // check for errors
    glValidateProgram(program->program_id);
    {
        GLint success;
        glGetProgramiv(program->program_id, GL_VALIDATE_STATUS, &success);

        if (!success)
        {
            GLchar info_log[512];
            glGetProgramInfoLog(program->program_id, sizeof(info_log), NULL, info_log);

            printf("Error: Program validation failed\n%s\n", info_log);

            return NULL;
        }
    }

    return program;
}

GLint program_get_location(struct program *program, const char *name)
{
    return glGetUniformLocation(program->program_id, name);
}

void program_bind(struct program *program)
{
    glUseProgram(program->program_id);
}

void program_set_int(struct program *program, const char *name, int value)
{
    glUniform1i(program_get_location(program, name), value);
}

void program_set_unsigned_int(struct program *program, const char *name, unsigned int value)
{
    glUniform1ui(program_get_location(program, name), value);
}

void program_set_float(struct program *program, const char *name, float value)
{
    glUniform1f(program_get_location(program, name), value);
}

void program_set_vec3(struct program *program, const char *name, vec3 vec)
{
    glUniform3fv(program_get_location(program, name), 1, (GLfloat *)vec);
}

void program_set_vec4(struct program *program, const char *name, vec4 vec)
{
    glUniform4fv(program_get_location(program, name), 1, (GLfloat *)vec);
}

void program_set_mat4(struct program *program, const char *name, mat4 mat)
{
    glUniformMatrix4fv(program_get_location(program, name), 1, GL_FALSE, (GLfloat *)mat);
}

void program_unbind(void)
{
    glUseProgram(0);
}

void program_destroy(struct program *program)
{
    glDeleteProgram(program->program_id);

    free(program);
}

static GLuint shader_create(GLenum type, const char *filename)
{
    // create shader
    GLuint shader = glCreateShader(type);

    // open shader file
    FILE *file;
    fopen_s(&file, filename, "rb");

    if (!file)
    {
        printf("Error: Couldn't open file %s\n", filename);

        return 0;
    }

    // get file size
    fseek(file, 0, SEEK_END);
    size_t size = (size_t)ftell(file);
    rewind(file);

    // file buffer
    char *source = malloc(size + 1);

    if (!source)
    {
        printf("Error: Couldn't allocate size %ld\n", (long)size);

        return 0;
    }

    // read the file into the buffer
    if (fread(source, sizeof(char), size, file) != size)
    {
        printf("Error: Couldn't read file %s\n", filename);

        return 0;
    }

    // null terminate
    source[size] = '\0';

    fclose(file);

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

        printf("Error: Shader compilation failed\n%s\n", info_log);

        return 0;
    }

    return shader;
}
