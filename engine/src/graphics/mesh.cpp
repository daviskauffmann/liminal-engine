#include <liminal/graphics/mesh.hpp>

#include <assimp/scene.h>

liminal::mesh::mesh(
    const std::vector<liminal::vertex> &vertices,
    const std::vector<GLuint> &indices,
    const std::vector<std::vector<std::shared_ptr<liminal::texture>>> &textures)
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

        glVertexAttribPointer(
            0,
            sizeof(liminal::vertex::position) / sizeof(liminal::vertex::position[0]),
            GL_FLOAT,
            GL_FALSE,
            sizeof(liminal::vertex),
            reinterpret_cast<void *>(offsetof(liminal::vertex, position)));
        glVertexAttribPointer(
            1,
            sizeof(liminal::vertex::normal) / sizeof(liminal::vertex::normal[0]),
            GL_FLOAT,
            GL_FALSE,
            sizeof(liminal::vertex),
            reinterpret_cast<void *>(offsetof(liminal::vertex, normal)));
        glVertexAttribPointer(
            2,
            sizeof(liminal::vertex::uv) / sizeof(liminal::vertex::uv[0]),
            GL_FLOAT,
            GL_FALSE,
            sizeof(liminal::vertex),
            reinterpret_cast<void *>(offsetof(liminal::vertex, uv)));
        glVertexAttribPointer(
            3,
            sizeof(liminal::vertex::tangent) / sizeof(liminal::vertex::tangent[0]),
            GL_FLOAT,
            GL_FALSE,
            sizeof(liminal::vertex),
            reinterpret_cast<void *>(offsetof(liminal::vertex, tangent)));
        glVertexAttribPointer(
            4,
            sizeof(liminal::vertex::bitangent) / sizeof(liminal::vertex::bitangent[0]),
            GL_FLOAT,
            GL_FALSE,
            sizeof(liminal::vertex),
            reinterpret_cast<void *>(offsetof(liminal::vertex, bitangent)));
        glVertexAttribPointer(
            5,
            sizeof(liminal::vertex::color) / sizeof(liminal::vertex::color[0]),
            GL_FLOAT,
            GL_FALSE,
            sizeof(liminal::vertex),
            reinterpret_cast<void *>(offsetof(liminal::vertex, color)));
        glVertexAttribIPointer(
            6,
            sizeof(liminal::vertex::bone_ids) / sizeof(liminal::vertex::bone_ids[0]),
            GL_UNSIGNED_INT,
            sizeof(liminal ::vertex),
            reinterpret_cast<void *>(offsetof(liminal::vertex, bone_ids)));
        glVertexAttribPointer(
            7,
            sizeof(liminal::vertex::bone_weights) / sizeof(liminal::vertex::bone_weights[0]),
            GL_FLOAT,
            GL_FALSE,
            sizeof(liminal::vertex),
            reinterpret_cast<void *>(offsetof(liminal::vertex, bone_weights)));

        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
        glEnableVertexAttribArray(3);
        glEnableVertexAttribArray(4);
        glEnableVertexAttribArray(5);
        glEnableVertexAttribArray(6);
        glEnableVertexAttribArray(7);
    }
    glBindVertexArray(0);
}

liminal::mesh::~mesh()
{
    glDeleteVertexArrays(1, &vao_id);
    glDeleteBuffers(1, &vbo_id);
    glDeleteBuffers(1, &ebo_id);
}

void liminal::mesh::draw(const liminal::program &program) const
{
    // TODO: support multiple textures per type in the shader?

    if (textures.size() > 0 && textures.at(aiTextureType_DIFFUSE).size() > 0)
    {
        textures.at(aiTextureType_DIFFUSE).at(0)->bind(0);
    }
    else
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    if (textures.size() > 0 && textures.at(aiTextureType_NORMALS).size() > 0)
    {
        program.set_int("material.has_normal_map", 1);
        textures.at(aiTextureType_NORMALS).at(0)->bind(1);
    }
    else
    {
        program.set_int("material.has_normal_map", 0);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    if (textures.size() > 0 && textures.at(aiTextureType_SHININESS).size() > 0)
    {
        program.set_int("material.has_metallic_map", 1);
        textures.at(aiTextureType_SHININESS).at(0)->bind(2);
    }
    else
    {
        program.set_int("material.has_metallic_map", 0);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    if (textures.size() > 0 && textures.at(aiTextureType_OPACITY).size() > 0)
    {
        program.set_int("material.has_roughness_map", 1);
        textures.at(aiTextureType_OPACITY).at(0)->bind(3);
    }
    else
    {
        program.set_int("material.has_roughness_map", 0);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    if (textures.size() > 0 && textures.at(aiTextureType_AMBIENT).size() > 0)
    {
        program.set_int("material.has_occlusion_map", 1);
        textures.at(aiTextureType_AMBIENT).at(0)->bind(4);
    }
    else
    {
        program.set_int("material.has_occlusion_map", 0);
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    if (textures.size() > 0 && textures.at(aiTextureType_HEIGHT).size() > 0)
    {
        program.set_int("material.has_height_map", 1);
        textures.at(aiTextureType_HEIGHT).at(0)->bind(5);
    }
    else
    {
        program.set_int("material.has_height_map", 0);
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
