#include "mesh.hpp"

namespace pk
{
mesh::mesh(std::vector<pk::vertex> vertices, std::vector<unsigned int> indices)
    : vertices_size((GLsizei)(vertices.size() * sizeof(pk::vertex))),
      indices_size((GLsizei)(indices.size() * sizeof(unsigned int)))
{
    glGenVertexArrays(1, &vao_id);
    glBindVertexArray(vao_id);
    glGenBuffers(1, &vbo_id);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_id);
    glBufferData(GL_ARRAY_BUFFER, vertices_size, vertices.data(), GL_STATIC_DRAW);
    glGenBuffers(1, &ebo_id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_size, indices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(pk::vertex), (void *)offsetof(pk::vertex, position));
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(pk::vertex), (void *)offsetof(pk::vertex, normal));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(pk::vertex), (void *)offsetof(pk::vertex, uv));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);
}

mesh::~mesh()
{
    glDeleteBuffers(1, &ebo_id);
    glDeleteBuffers(1, &vbo_id);
    glDeleteVertexArrays(1, &vao_id);
}

void mesh::draw() const
{
    glBindVertexArray(vao_id);
    glDrawElements(GL_TRIANGLES, indices_size, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}
} // namespace pk
