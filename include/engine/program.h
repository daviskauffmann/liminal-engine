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
void program_set_int(struct program *program, const char *name, int value);
void program_set_float(struct program *program, const char *name, float value);
void program_set_vec3(struct program *program, const char *name, vec3 vec);
void program_set_mat4(struct program *program, const char *name, mat4 mat);
void program_set_texture(struct program *program, const char *name, int num, GLuint texture);
void program_unbind(void);
void program_destroy(struct program *program);

#endif
