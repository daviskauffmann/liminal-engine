#include <liminal/graphics/mesh.hpp>

#include <assimp/scene.h>

liminal::mesh::mesh(
    std::vector<liminal::vertex> vertices,
    std::vector<GLuint> indices,
    std::vector<std::vector<liminal::texture *>> textures)
    : vertices_size((GLsizei)(vertices.size() * sizeof(liminal::vertex))),
      indices_size((GLsizei)(indices.size() * sizeof(GLuint))),
      textures(textures)
{
    glGenVertexArrays(1, &vao_id);
    glBindVertexArray(vao_id);
    {
        glGenBuffers(1, &vbo_id);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_id);
        glBufferData(GL_ARRAY_BUFFER, vertices_size, vertices.data(), GL_STATIC_DRAW);

        glGenBuffers(1, &ebo_id);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_id);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_size, indices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(liminal::vertex), (void *)offsetof(liminal::vertex, position));
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(liminal::vertex), (void *)offsetof(liminal::vertex, normal));
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(liminal::vertex), (void *)offsetof(liminal::vertex, uv));
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(liminal::vertex), (void *)offsetof(liminal::vertex, tangent));
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(liminal::vertex), (void *)offsetof(liminal::vertex, bitangent));
        glVertexAttribIPointer(5, 4, GL_INT, sizeof(liminal::vertex), (void *)offsetof(liminal::vertex, bone_ids));
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(liminal::vertex), (void *)offsetof(liminal::vertex, bone_weights));

        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
        glEnableVertexAttribArray(3);
        glEnableVertexAttribArray(4);
        glEnableVertexAttribArray(5);
        glEnableVertexAttribArray(6);
    }
    glBindVertexArray(0);
}

liminal::mesh::~mesh()
{
    glDeleteVertexArrays(1, &vao_id);
    glDeleteBuffers(1, &vbo_id);
    glDeleteBuffers(1, &ebo_id);
}

void liminal::mesh::draw(liminal::program *program) const
{
    // TODO: support multiple textures per type in the shader?

    if (textures.size() > 0 && textures[aiTextureType_DIFFUSE].size() > 0)
    {
        textures[aiTextureType_DIFFUSE][0]->bind(0);
    }
    else
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    if (textures.size() > 0 && textures[aiTextureType_NORMALS].size() > 0)
    {
        program->set_int("material.has_normal_map", 1);
        textures[aiTextureType_NORMALS][0]->bind(1);
    }
    else
    {
        program->set_int("material.has_normal_map", 0);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    if (textures.size() > 0 && textures[aiTextureType_SHININESS].size() > 0)
    {
        program->set_int("material.has_metallic_map", 1);
        textures[aiTextureType_SHININESS][0]->bind(2);
    }
    else
    {
        program->set_int("material.has_metallic_map", 0);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    if (textures.size() > 0 && textures[aiTextureType_OPACITY].size() > 0)
    {
        program->set_int("material.has_roughness_map", 1);
        textures[aiTextureType_OPACITY][0]->bind(3);
    }
    else
    {
        program->set_int("material.has_roughness_map", 0);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    if (textures.size() > 0 && textures[aiTextureType_AMBIENT].size() > 0)
    {
        program->set_int("material.has_occlusion_map", 1);
        textures[aiTextureType_AMBIENT][0]->bind(4);
    }
    else
    {
        program->set_int("material.has_occlusion_map", 0);
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    if (textures.size() > 0 && textures[aiTextureType_HEIGHT].size() > 0)
    {
        program->set_int("material.has_height_map", 1);
        textures[aiTextureType_HEIGHT][0]->bind(5);
    }
    else
    {
        program->set_int("material.has_height_map", 0);
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    glBindVertexArray(vao_id);
    glDrawElements(GL_TRIANGLES, indices_size, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, 0);
}
