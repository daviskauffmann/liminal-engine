#include "terrain.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include "texture.hpp"

constexpr float size = 500;
constexpr int vertex_count = 128;

// TODO: multiple textures w/ blend map

// TODO: erosion simulation?

namespace pk
{
    terrain::terrain(int grid_x, int grid_z)
        : position(glm::vec3(grid_x * size, 0, grid_z * size))
    {
        std::vector<pk::vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<std::vector<pk::texture *>> textures;
        for (int y = 0; y < vertex_count; y++)
        {
            for (int x = 0; x < vertex_count; x++)
            {
                glm::vec3 position(
                    -(float)x / ((float)vertex_count - 1) * size,
                    sinf((float)(rand() % RAND_MAX)) - 2.0f,
                    -(float)y / ((float)vertex_count - 1) * size);
                glm::vec3 normal(0.0f, 1.0f, 0.0f);
                glm::vec2 uv(
                    (float)x / ((float)vertex_count - 1),
                    (float)y / ((float)vertex_count - 1));
                pk::vertex vertex;
                vertex.position = position;
                vertex.normal = normal;
                vertex.uv = uv;
                vertex.tangent = glm::vec3(0.0f, 0.0f, 0.0f);
                vertex.bitangent = glm::vec3(0.0f, 0.0f, 0.0f);
                vertices.push_back(vertex);
            }
        }
        for (unsigned int z = 0; z < vertex_count - 1; z++)
        {
            for (unsigned int x = 0; x < vertex_count - 1; x++)
            {
                unsigned int top_left = (z * vertex_count) + x;
                unsigned int top_right = top_left + 1;
                unsigned int bottom_left = ((z + 1) * vertex_count) + x;
                unsigned int bottom_right = bottom_left + 1;
                indices.push_back(top_left);
                indices.push_back(bottom_left);
                indices.push_back(top_right);
                indices.push_back(top_right);
                indices.push_back(bottom_left);
                indices.push_back(bottom_right);
            }
        }
        mesh = new pk::mesh(vertices, indices, textures);
    }

    terrain::~terrain()
    {
        delete mesh;
    }

    glm::mat4 terrain::calc_model() const
    {
        glm::mat4 model = glm::identity<glm::mat4>();
        model = glm::translate(model, position);
        return model;
    }
} // namespace pk
