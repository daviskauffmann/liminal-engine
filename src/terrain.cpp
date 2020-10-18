#include "terrain.hpp"

#include <assimp/scene.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <SDL2/SDL_image.h>

#include "texture.hpp"

float pk::terrain::size = 800;
float pk::terrain::height_scale = 100;

// TODO: read from heightmap image file

// TODO: multiple textures w/ blend map

// TODO: 3d terrain (caves and whatnot)
// this will probably be a different class

pk::terrain::terrain(glm::vec3 position, const std::string &heightmap_filename)
    : position(position)
{
    SDL_Surface *heightmap_surface = IMG_Load(heightmap_filename.c_str());
    if (!heightmap_surface)
    {
        std::cout << "Error: " << IMG_GetError() << std::endl;
    }
    std::cout << "Loaded: " << heightmap_filename << std::endl;

    std::vector<pk::vertex> vertices;
    for (int z = 0; z < heightmap_surface->h; z++)
    {
        for (int x = 0; x < heightmap_surface->w; x++)
        {
            glm::vec3 position(
                -(float)x / ((float)heightmap_surface->w - 1) * size,
                get_height(heightmap_surface, x, z),
                -(float)z / ((float)heightmap_surface->h - 1) * size);
            glm::vec3 normal = glm::normalize(glm::vec3(
                get_height(heightmap_surface, x + 1, z) - get_height(heightmap_surface, x - 1, z),
                2.0f,
                get_height(heightmap_surface, x, z + 1) - get_height(heightmap_surface, x, z - 1)));
            glm::vec2 uv(
                (float)x / ((float)heightmap_surface->w - 1),
                (float)z / ((float)heightmap_surface->h - 1));

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
    for (int z = 0; z < heightmap_surface->h - 1; z++)
    {
        for (int x = 0; x < heightmap_surface->w - 1; x++)
        {
            unsigned int top_left = (z * heightmap_surface->h) + x;
            unsigned int top_right = top_left + 1;
            unsigned int bottom_left = ((z + 1) * heightmap_surface->h) + x;
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
    for (aiTextureType type = aiTextureType_NONE; type <= AI_TEXTURE_TYPE_MAX; type = (aiTextureType)(type + 1))
    {
        std::vector<pk::texture *> textures_for_type;
        textures.push_back(textures_for_type);
    }
    // TODO: cleanup textures in destructor
    textures[aiTextureType_DIFFUSE].push_back(new pk::texture("assets/images/grass1-albedo3.png"));
    // textures[aiTextureType_DIFFUSE].push_back(new pk::texture(""));
    // textures[aiTextureType_DIFFUSE].push_back(new pk::texture(""));
    // textures[aiTextureType_DIFFUSE].push_back(new pk::texture(""));

    textures[aiTextureType_NORMALS].push_back(new pk::texture("assets/images/grass1-normal1-ogl.png"));
    // textures[aiTextureType_NORMALS].push_back(new pk::texture(""));
    // textures[aiTextureType_NORMALS].push_back(new pk::texture(""));
    // textures[aiTextureType_NORMALS].push_back(new pk::texture(""));

    textures[aiTextureType_SHININESS].push_back(new pk::texture("assets/images/grass1-metal.png"));
    // textures[aiTextureType_SHININESS].push_back(new pk::texture(""));
    // textures[aiTextureType_SHININESS].push_back(new pk::texture(""));
    // textures[aiTextureType_SHININESS].push_back(new pk::texture(""));

    textures[aiTextureType_OPACITY].push_back(new pk::texture("assets/images/grass1-rough.png"));
    // textures[aiTextureType_OPACITY].push_back(new pk::texture(""));
    // textures[aiTextureType_OPACITY].push_back(new pk::texture(""));
    // textures[aiTextureType_OPACITY].push_back(new pk::texture(""));

    textures[aiTextureType_AMBIENT].push_back(new pk::texture("assets/images/grass1-ao.png"));
    // textures[aiTextureType_AMBIENT].push_back(new pk::texture(""));
    // textures[aiTextureType_AMBIENT].push_back(new pk::texture(""));
    // textures[aiTextureType_AMBIENT].push_back(new pk::texture(""));

    textures[aiTextureType_HEIGHT].push_back(new pk::texture("assets/images/grass1-height.png"));
    // textures[aiTextureType_HEIGHT].push_back(new pk::texture(""));
    // textures[aiTextureType_HEIGHT].push_back(new pk::texture(""));
    // textures[aiTextureType_HEIGHT].push_back(new pk::texture(""));

    mesh = new pk::mesh(vertices, indices, textures);

    SDL_FreeSurface(heightmap_surface);
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

float pk::terrain::get_height(SDL_Surface *surface, int x, int z) const
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
    } pixel = *(pixel_t *)((unsigned char *)surface->pixels + z * surface->pitch + x * surface->format->BytesPerPixel);

    float height = (pixel.color.r << 16) | (pixel.color.g << 8) | (pixel.color.b);
    height -= 0xffffff / 2;
    height /= 0xffffff / 2;
    height *= height_scale;

    return height;
}
