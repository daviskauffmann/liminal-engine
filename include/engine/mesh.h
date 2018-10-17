#ifndef MESH_H
#define MESH_H

#include <GL/glew.h>

struct mesh
{
    GLuint vao;
    GLuint vbo;
    GLuint ebo;
    int vertices_size;
    int indices_size;
};

struct mesh *mesh_create(
    float *vertices,
    unsigned int vertices_size,
    unsigned int *indices,
    unsigned int indices_size);
struct mesh *mesh_create_obj(const char *file);
void mesh_draw(struct mesh *mesh);
void mesh_destroy(struct mesh *mesh);

#endif
