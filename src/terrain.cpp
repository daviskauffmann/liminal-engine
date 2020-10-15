#include "terrain.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "texture.hpp"

constexpr float size = 800;
constexpr float max_height = 40;

// TODO: read from heightmap image file

// TODO: multiple textures w/ blend map

// TODO: 3d terrain (caves and whatnot)
// this will probably be a different class

float get_height(SDL_Surface *surface, int x, int z)
{
    if (x < 0 || x >= surface->w || z < 0 || z >= surface->h)
    {
        return 0.0f;
    }

    union pixel_t
    {
        unsigned int pixel;
        struct color_t
        {
            unsigned char r;
            unsigned char g;
            unsigned char b;
            unsigned char a;
        } color;
    };
    pixel_t pixel;
    pixel.pixel = *(unsigned int *)((unsigned char *)surface->pixels + z * surface->pitch + x * surface->format->BytesPerPixel);

    float height = (pixel.color.r << 16) | (pixel.color.g << 8) | (pixel.color.b);
    const float max_pixel_color = 0xffffff;
    height += max_pixel_color / 2.0f;
    height /= max_pixel_color / 2.0f;
    height *= max_height;

    return height;
}

pk::terrain::terrain(const std::string &filename, int grid_x, int grid_z)
    : position(glm::vec3(grid_x * size, 0, grid_z * size))
{
    SDL_Surface *surface = IMG_Load(filename.c_str());
    if (!surface)
    {
        std::cout << "Error: " << IMG_GetError() << std::endl;
    }
    std::cout << "Loaded: " << filename << std::endl;

    const int vertex_count = surface->h;

    std::vector<pk::vertex> vertices;
    for (int z = 0; z < vertex_count; z++)
    {
        for (int x = 0; x < vertex_count; x++)
        {
            glm::vec3 position(
                -(float)x / ((float)vertex_count - 1) * size,
                get_height(surface, x, z),
                -(float)z / ((float)vertex_count - 1) * size);

            float height_left = get_height(surface, x - 1, z);
            float height_right = get_height(surface, x + 1, z);
            float height_down = get_height(surface, x, z - 1);
            float height_up = get_height(surface, x, z + 1);
            glm::vec3 normal(height_left - height_right, 2.0f, height_down - height_up);
            normal = glm::normalize(normal);

            glm::vec2 uv(
                (float)x / ((float)vertex_count - 1),
                (float)z / ((float)vertex_count - 1));

            pk::vertex vertex;
            vertex.position = position;
            vertex.normal = normal;
            vertex.uv = uv;
            vertex.tangent = glm::vec3(0.0f, 0.0f, 0.0f);
            vertex.bitangent = glm::vec3(0.0f, 0.0f, 0.0f);

            vertices.push_back(vertex);
        }
    }

    std::vector<unsigned int> indices;
    for (int z = 0; z < vertex_count - 1; z++)
    {
        for (int x = 0; x < vertex_count - 1; x++)
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

    std::vector<std::vector<pk::texture *>> textures;

    mesh = new pk::mesh(vertices, indices, textures);

    SDL_FreeSurface(surface);
}

pk::terrain::~terrain()
{
    delete mesh;
}

glm::mat4 pk::terrain::calc_model() const
{
    glm::mat4 model = glm::identity<glm::mat4>();
    model = glm::translate(model, position);
    return model;
}
