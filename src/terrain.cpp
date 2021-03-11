#include "terrain.hpp"

#include <assimp/scene.h>
#include <bullet/BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <SDL2/SDL_image.h>

#include "texture.hpp"

float liminal::terrain::size = 800;
float liminal::terrain::height_scale = 100;

// TODO: read from heightmap image file

// TODO: multiple textures w/ blend map

// TODO: 3d terrain (caves and whatnot)
// this will probably be a different class

liminal::terrain::terrain(glm::vec3 position, const std::string &heightmap_filename)
    : position(position)
{
    SDL_Surface *heightmap_surface = IMG_Load(heightmap_filename.c_str());
    if (!heightmap_surface)
    {
        std::cerr << "Error: Failed to load terrain heightmap texture: " << IMG_GetError() << std::endl;
        return;
    }

    std::vector<liminal::vertex> vertices;
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

            liminal::vertex vertex;
            vertex.position = position;
            vertex.normal = normal;
            vertex.uv = uv;
            vertex.tangent = glm::vec3(0.0f, 0.0f, 0.0f);
            vertex.bitangent = glm::vec3(0.0f, 0.0f, 0.0f);

            vertices.push_back(vertex);

            heightfield.push_back(position.x);
            heightfield.push_back(position.y);
            heightfield.push_back(position.z);
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

    SDL_FreeSurface(heightmap_surface);

    std::vector<std::vector<liminal::texture *>> textures;
    for (aiTextureType type = aiTextureType_NONE; type <= AI_TEXTURE_TYPE_MAX; type = (aiTextureType)(type + 1))
    {
        std::vector<liminal::texture *> textures_for_type;
        textures.push_back(textures_for_type);
    }
    // TODO: cleanup textures in destructor
    textures[aiTextureType_DIFFUSE].push_back(new liminal::texture("assets/images/grass1-albedo3.png"));
    // textures[aiTextureType_DIFFUSE].push_back(new liminal::texture(""));
    // textures[aiTextureType_DIFFUSE].push_back(new liminal::texture(""));
    // textures[aiTextureType_DIFFUSE].push_back(new liminal::texture(""));

    textures[aiTextureType_NORMALS].push_back(new liminal::texture("assets/images/grass1-normal1-ogl.png"));
    // textures[aiTextureType_NORMALS].push_back(new liminal::texture(""));
    // textures[aiTextureType_NORMALS].push_back(new liminal::texture(""));
    // textures[aiTextureType_NORMALS].push_back(new liminal::texture(""));

    textures[aiTextureType_SHININESS].push_back(new liminal::texture("assets/images/grass1-metal.png"));
    // textures[aiTextureType_SHININESS].push_back(new liminal::texture(""));
    // textures[aiTextureType_SHININESS].push_back(new liminal::texture(""));
    // textures[aiTextureType_SHININESS].push_back(new liminal::texture(""));

    textures[aiTextureType_OPACITY].push_back(new liminal::texture("assets/images/grass1-rough.png"));
    // textures[aiTextureType_OPACITY].push_back(new liminal::texture(""));
    // textures[aiTextureType_OPACITY].push_back(new liminal::texture(""));
    // textures[aiTextureType_OPACITY].push_back(new liminal::texture(""));

    textures[aiTextureType_AMBIENT].push_back(new liminal::texture("assets/images/grass1-ao.png"));
    // textures[aiTextureType_AMBIENT].push_back(new liminal::texture(""));
    // textures[aiTextureType_AMBIENT].push_back(new liminal::texture(""));
    // textures[aiTextureType_AMBIENT].push_back(new liminal::texture(""));

    textures[aiTextureType_HEIGHT].push_back(new liminal::texture("assets/images/grass1-height.png"));
    // textures[aiTextureType_HEIGHT].push_back(new liminal::texture(""));
    // textures[aiTextureType_HEIGHT].push_back(new liminal::texture(""));
    // textures[aiTextureType_HEIGHT].push_back(new liminal::texture(""));

    mesh = new liminal::mesh(vertices, indices, textures);

    btTransform transform;
    transform.setIdentity();
    transform.setOrigin(btVector3(0.0f, -50.0f, 0.0f));

    motion_state = new btDefaultMotionState(transform);

    collision_shape = new btHeightfieldTerrainShape(size, size, heightfield.data(), 1, 0, 100, 1, PHY_FLOAT, true);
    rigidbody = new btRigidBody(btRigidBody::btRigidBodyConstructionInfo(0.0f, motion_state, collision_shape));
}

liminal::terrain::~terrain()
{
    delete motion_state;
    delete collision_shape;

    delete mesh;
}

glm::mat4 liminal::terrain::calc_model() const
{
    glm::mat4 model = glm::identity<glm::mat4>();
    model = glm::translate(model, position);
    return model;
}

float liminal::terrain::get_height(SDL_Surface *surface, int x, int z) const
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
