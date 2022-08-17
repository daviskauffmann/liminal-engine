#include <liminal/graphics/mesh.hpp>

#include <assimp/scene.h>
#include <liminal/graphics/program.hpp>
#include <liminal/graphics/texture.hpp>

void liminal::mesh::vertex::add_bone_data(const unsigned int id, const float weight)
{
    for (std::size_t bone_index = 0; bone_index < num_bones; bone_index++)
    {
        if (bone_ids.at(bone_index) == num_bones)
        {
            bone_ids.at(bone_index) = id;
            bone_weights.at(bone_index) = weight;
            return;
        }
    }
}

liminal::mesh::mesh(
    const std::vector<liminal::mesh::vertex> &vertices,
    const std::vector<GLuint> &indices,
    const std::array<std::vector<std::shared_ptr<liminal::texture>>, num_textures> &textures)
    : vertices_size((GLsizei)(vertices.size() * sizeof(liminal::mesh::vertex))),
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
            sizeof(vertex::position) / sizeof(vertex::position[0]),
            GL_FLOAT,
            GL_FALSE,
            sizeof(vertex),
            reinterpret_cast<void *>(offsetof(vertex, position)));
        glVertexAttribPointer(
            1,
            sizeof(vertex::normal) / sizeof(vertex::normal[0]),
            GL_FLOAT,
            GL_FALSE,
            sizeof(vertex),
            reinterpret_cast<void *>(offsetof(vertex, normal)));
        glVertexAttribPointer(
            2,
            sizeof(vertex::uv) / sizeof(vertex::uv[0]),
            GL_FLOAT,
            GL_FALSE,
            sizeof(vertex),
            reinterpret_cast<void *>(offsetof(vertex, uv)));
        glVertexAttribPointer(
            3,
            sizeof(vertex::tangent) / sizeof(vertex::tangent[0]),
            GL_FLOAT,
            GL_FALSE,
            sizeof(vertex),
            reinterpret_cast<void *>(offsetof(vertex, tangent)));
        glVertexAttribPointer(
            4,
            sizeof(vertex::bitangent) / sizeof(vertex::bitangent[0]),
            GL_FLOAT,
            GL_FALSE,
            sizeof(vertex),
            reinterpret_cast<void *>(offsetof(vertex, bitangent)));
        glVertexAttribPointer(
            5,
            sizeof(vertex::color) / sizeof(vertex::color[0]),
            GL_FLOAT,
            GL_FALSE,
            sizeof(vertex),
            reinterpret_cast<void *>(offsetof(vertex, color)));
        glVertexAttribIPointer(
            6,
            sizeof(vertex::bone_ids) / sizeof(vertex::bone_ids[0]),
            GL_UNSIGNED_INT,
            sizeof(vertex),
            reinterpret_cast<void *>(offsetof(vertex, bone_ids)));
        glVertexAttribPointer(
            7,
            sizeof(vertex::bone_weights) / sizeof(vertex::bone_weights[0]),
            GL_FLOAT,
            GL_FALSE,
            sizeof(vertex),
            reinterpret_cast<void *>(offsetof(vertex, bone_weights)));

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

    if (textures.at(aiTextureType_DIFFUSE).size() > 0)
    {
        textures.at(aiTextureType_DIFFUSE).at(0)->bind(0);
    }
    else
    {
        liminal::texture::unbind(0);
    }

    if (textures.at(aiTextureType_NORMALS).size() > 0)
    {
        program.set_int("material.has_normal_map", 1);
        textures.at(aiTextureType_NORMALS).at(0)->bind(1);
    }
    else
    {
        program.set_int("material.has_normal_map", 0);
        liminal::texture::unbind(1);
    }

    if (textures.at(aiTextureType_SHININESS).size() > 0)
    {
        program.set_int("material.has_metallic_map", 1);
        textures.at(aiTextureType_SHININESS).at(0)->bind(2);
    }
    else
    {
        program.set_int("material.has_metallic_map", 0);
        liminal::texture::unbind(2);
    }

    if (textures.at(aiTextureType_OPACITY).size() > 0)
    {
        program.set_int("material.has_roughness_map", 1);
        textures.at(aiTextureType_OPACITY).at(0)->bind(3);
    }
    else
    {
        program.set_int("material.has_roughness_map", 0);
        liminal::texture::unbind(3);
    }

    if (textures.at(aiTextureType_AMBIENT).size() > 0)
    {
        program.set_int("material.has_occlusion_map", 1);
        textures.at(aiTextureType_AMBIENT).at(0)->bind(4);
    }
    else
    {
        program.set_int("material.has_occlusion_map", 0);
        liminal::texture::unbind(4);
    }

    if (textures.at(aiTextureType_HEIGHT).size() > 0)
    {
        program.set_int("material.has_height_map", 1);
        textures.at(aiTextureType_HEIGHT).at(0)->bind(5);
    }
    else
    {
        program.set_int("material.has_height_map", 0);
        liminal::texture::unbind(5);
    }

    if (indices_size)
    {
        glBindVertexArray(vao_id);
        glDrawElements(GL_TRIANGLES, indices_size, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
    else
    {
        glBindVertexArray(vao_id);
        glDrawArrays(GL_TRIANGLES, 0, vertices_size);
        glBindVertexArray(0);
    }

    liminal::texture::unbind(0);
    liminal::texture::unbind(1);
    liminal::texture::unbind(2);
    liminal::texture::unbind(3);
    liminal::texture::unbind(4);
    liminal::texture::unbind(5);
}
