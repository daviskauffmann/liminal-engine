#ifndef GAME_MESH_HPP
#define GAME_MESH_HPP

namespace pk
{
    struct mesh
    {
        GLuint vao_id;
        GLuint vbo_id;
        GLuint ebo_id;
        unsigned int vertices_size;
        unsigned int indices_size;

        mesh(
            float *vertices,
            unsigned int vertices_size,
            unsigned int *indices,
            unsigned int indices_size);
        ~mesh();

        void draw() const;
    };
}

#endif
