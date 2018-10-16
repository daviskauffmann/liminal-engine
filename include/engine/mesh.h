#ifndef MESH_H
#define MESH_H

#include <GL/glew.h>

struct mesh
{
    GLuint vao;
    GLuint vbo;
    GLuint ebo;
    int num_vertices;
    int num_indices;
};

struct mesh *mesh_create(
    float *vertices,
    unsigned int num_vertices,
    unsigned int *indices,
    unsigned int num_indices);
struct mesh *mesh_create_obj(const char *file);
void mesh_draw(struct mesh *mesh);
void mesh_destroy(struct mesh *mesh);

#endif
