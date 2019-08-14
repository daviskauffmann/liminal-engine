#ifndef GAME_MESH_HPP
#define GAME_MESH_HPP

struct Mesh
{
    GLuint vaoId;
    GLuint vboId;
    GLuint eboId;
    unsigned int verticesSize;
    unsigned int indicesSize;

    Mesh(
        float *vertices,
        unsigned int verticesSize,
        unsigned int *indices,
        unsigned int indicesSize);
    ~Mesh();

    void draw() const;
};

#endif
