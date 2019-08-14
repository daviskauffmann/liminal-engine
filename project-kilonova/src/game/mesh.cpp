#include <game/game.hpp>

namespace pk
{
    mesh::mesh(
        float *vertices,
        unsigned int vertices_size,
        unsigned int *indices,
        unsigned int indices_size)
    {
        glGenVertexArrays(1, &this->vao_id);
        glGenBuffers(1, &this->vbo_id);
        glGenBuffers(1, &this->ebo_id);
        this->vertices_size = vertices_size;
        this->indices_size = indices_size;

        glBindVertexArray(this->vao_id);

        // setup vertex buffer
        glBindBuffer(GL_ARRAY_BUFFER, this->vbo_id);
        glBufferData(GL_ARRAY_BUFFER, vertices_size, vertices, GL_STATIC_DRAW);

        // setup element buffer
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ebo_id);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_size, indices, GL_STATIC_DRAW);

        // setup vertex attributes
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)(0 * sizeof(GLfloat))); // position
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)(3 * sizeof(GLfloat))); // normal
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)(6 * sizeof(GLfloat))); // uv

        // enable vertex attributes for this vertex array
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);

        glBindVertexArray(0);
    }

    mesh::~mesh()
    {
        glDeleteBuffers(1, &this->ebo_id);
        glDeleteBuffers(1, &this->vbo_id);
        glDeleteVertexArrays(1, &this->vao_id);
    }

    void mesh::draw() const
    {
        glBindVertexArray(this->vao_id);

        glDrawElements(GL_TRIANGLES, this->indices_size, GL_UNSIGNED_INT, NULL);

        glBindVertexArray(0);
    }
}
