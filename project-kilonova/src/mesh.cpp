#include "mesh.hpp"

namespace pk
{
    mesh::mesh(std::vector<pk::vertex> vertices, std::vector<unsigned int> indices)
        : vertices(vertices)
        , indices(indices)
    {
        glGenVertexArrays(1, &this->vao_id);
        glGenBuffers(1, &this->vbo_id);
        glGenBuffers(1, &this->ebo_id);

        glBindVertexArray(this->vao_id);

        // setup vertex buffer
        glBindBuffer(GL_ARRAY_BUFFER, this->vbo_id);
        glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(pk::vertex), vertices.data(), GL_STATIC_DRAW);

        // setup element buffer
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ebo_id);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        // setup vertex attributes
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)offsetof(pk::vertex, position));
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)offsetof(pk::vertex, normal));
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)offsetof(pk::vertex, uv));

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

        glDrawElements(GL_TRIANGLES, (GLsizei)(this->indices.size() * sizeof(unsigned int)), GL_UNSIGNED_INT, nullptr);

        glBindVertexArray(0);
    }
}
