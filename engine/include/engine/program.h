#ifndef PROGRAM_H
#define PROGRAM_H

#include <cglm/cglm.h>
#include <GL/glew.h>

struct program
{
    GLuint program;
};

struct program ENGINE_API *program_create(const char *vertex_file, const char *fragment_file);
GLint ENGINE_API program_get_location(struct program *program, const char *name);
void ENGINE_API program_bind(struct program *program);
void ENGINE_API program_set_int(GLint location, int value);
void ENGINE_API program_set_float(GLint location, float value);
void ENGINE_API program_set_vec3(GLint location, vec3 vec);
void ENGINE_API program_set_mat4(GLint location, mat4 mat);
void ENGINE_API program_unbind(void);
void ENGINE_API program_destroy(struct program *program);

#endif
