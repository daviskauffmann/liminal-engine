#include "mesh.hpp"

#include <assimp/scene.h>

pk::mesh::mesh(std::vector<pk::vertex> vertices, std::vector<unsigned int> indices, std::vector<std::vector<pk::texture *>> textures)
    : vertices_size((GLsizei)(vertices.size() * sizeof(pk::vertex))),
      indices_size((GLsizei)(indices.size() * sizeof(unsigned int))),
      textures(textures)
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
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(pk::vertex), (void *)offsetof(pk::vertex, tangent));
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(pk::vertex), (void *)offsetof(pk::vertex, bitangent));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
    glEnableVertexAttribArray(4);
    glBindVertexArray(0);
}

pk::mesh::~mesh()
{
    glDeleteVertexArrays(1, &vao_id);
    glDeleteBuffers(1, &vbo_id);
    glDeleteBuffers(1, &ebo_id);
}

void pk::mesh::draw() const
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textures.size() ? textures[aiTextureType_DIFFUSE].size() > 0 ? textures[aiTextureType_DIFFUSE][0]->texture_id : 0 : 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textures.size() ? textures[aiTextureType_NORMALS].size() > 0 ? textures[aiTextureType_NORMALS][0]->texture_id : 0 : 0);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, textures.size() ? textures[aiTextureType_SHININESS].size() > 0 ? textures[aiTextureType_SHININESS][0]->texture_id : 0 : 0);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, textures.size() ? textures[aiTextureType_OPACITY].size() > 0 ? textures[aiTextureType_OPACITY][0]->texture_id : 0 : 0);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, textures.size() ? textures[aiTextureType_AMBIENT].size() > 0 ? textures[aiTextureType_AMBIENT][0]->texture_id : 0 : 0);
    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, textures.size() ? textures[aiTextureType_HEIGHT].size() > 0 ? textures[aiTextureType_HEIGHT][0]->texture_id : 0 : 0);

    glBindVertexArray(vao_id);
    glDrawElements(GL_TRIANGLES, indices_size, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}
