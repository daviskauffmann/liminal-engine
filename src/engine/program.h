#ifndef PROGRAM_H
#define PROGRAM_H

#include <cglm/cglm.h>
#include <GL/glew.h>

struct program
{
    GLuint program;
};

struct program *program_create(const char *vertex_file, const char *fragment_file);
GLint program_get_location(struct program *program, const char *name);
void program_bind(struct program *program);
void program_set_int(GLint location, int value);
void program_set_float(GLint location, float value);
void program_set_vec3(GLint location, vec3 vec);
void program_set_mat4(GLint location, mat4 mat);
void program_unbind(void);
void program_destroy(struct program *program);

#endif
