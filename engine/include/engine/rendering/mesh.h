#ifndef ENGINE_MESH_H
#define ENGINE_MESH_H

struct mesh
{
    GLuint vao_id;
    GLuint vbo_id;
    GLuint ebo_id;
    unsigned int vertices_size;
    unsigned int indices_size;
};

struct mesh ENGINE_API *mesh_create(
    float *vertices,
    unsigned int vertices_size,
    unsigned int *indices,
    unsigned int indices_size);
void ENGINE_API mesh_draw(struct mesh *mesh);
void ENGINE_API mesh_destroy(struct mesh *mesh);

#endif
