#ifndef ENGINE_PROGRAM_H
#define ENGINE_PROGRAM_H

struct program
{
    GLuint program_id;
};

struct program *program_create(const char *vertex_filename, const char *fragment_filename);
GLint program_get_location(struct program *program, const char *name);
void program_bind(struct program *program);
void program_set_int(struct program *program, const char *name, int value);
void program_set_unsigned_int(struct program *program, const char *name, unsigned int value);
void program_set_float(struct program *program, const char *name, float value);
void program_set_vec3(struct program *program, const char *name, vec3 vec);
void program_set_vec4(struct program *program, const char *name, vec4 vec);
void program_set_mat4(struct program *program, const char *name, mat4 mat);
void program_unbind(void);
void program_destroy(struct program *program);

#endif
