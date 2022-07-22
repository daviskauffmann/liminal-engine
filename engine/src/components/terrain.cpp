#include <liminal/components/terrain.hpp>

#include <assimp/scene.h>
#include <bullet/BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <liminal/graphics/texture.hpp>
#include <liminal/core/assets.hpp>
#include <SDL2/SDL_image.h>

// TODO: read from heightmap image file

// TODO: multiple textures w/ blend map

// TODO: 3d terrain (caves and whatnot)
// this will probably be a different class

liminal::terrain::terrain(const std::string &filename, const glm::vec3 &position, float size, float height_scale)
    : position(position),
      size(size),
      height_scale(height_scale)
{
    const auto surface = IMG_Load(filename.c_str());
    if (!surface)
    {
        std::cerr << "Error: Failed to load terrain heightmap texture: " << IMG_GetError() << std::endl;
        return;
    }

    std::vector<liminal::vertex> vertices;
    for (int z = 0; z < surface->h; z++)
    {
        for (int x = 0; x < surface->w; x++)
        {
            liminal::vertex vertex;
            vertex.position = glm::vec3(
                -(float)x / ((float)surface->w - 1) * size,
                get_height(surface, x, z),
                -(float)z / ((float)surface->h - 1) * size);
            vertex.normal = glm::normalize(glm::vec3(
                get_height(surface, x + 1, z) - get_height(surface, x - 1, z),
                2,
                get_height(surface, x, z + 1) - get_height(surface, x, z - 1)));
            vertex.uv = glm::vec2(
                (float)x / ((float)surface->w - 1),
                (float)z / ((float)surface->h - 1));
            vertices.push_back(vertex);

            heightfield.push_back(vertex.position.x);
            heightfield.push_back(vertex.position.y);
            heightfield.push_back(vertex.position.z);
        }
    }

    std::vector<unsigned int> indices;
    for (int z = 0; z < surface->h - 1; z++)
    {
        for (int x = 0; x < surface->w - 1; x++)
        {
            unsigned int top_left = (z * surface->h) + x;
            unsigned int top_right = top_left + 1;
            unsigned int bottom_left = ((z + 1) * surface->h) + x;
            unsigned int bottom_right = bottom_left + 1;
            indices.push_back(top_left);
            indices.push_back(bottom_left);
            indices.push_back(top_right);
            indices.push_back(top_right);
            indices.push_back(bottom_left);
            indices.push_back(bottom_right);
        }
    }

    SDL_FreeSurface(surface);

    std::vector<std::vector<liminal::texture *>> textures;
    for (aiTextureType type = aiTextureType_NONE; type <= AI_TEXTURE_TYPE_MAX; type = (aiTextureType)(type + 1))
    {
        std::vector<liminal::texture *> textures_for_type;
        textures.push_back(textures_for_type);
    }

    textures[aiTextureType_DIFFUSE].push_back(liminal::assets::instance->load_texture("assets/images/grass1-albedo3.png"));
    // textures[aiTextureType_DIFFUSE].push_back(liminal::assets::instance->load_texture(""));
    // textures[aiTextureType_DIFFUSE].push_back(liminal::assets::instance->load_texture(""));
    // textures[aiTextureType_DIFFUSE].push_back(liminal::assets::instance->load_texture(""));

    textures[aiTextureType_NORMALS].push_back(liminal::assets::instance->load_texture("assets/images/grass1-normal1-ogl.png"));
    // textures[aiTextureType_NORMALS].push_back(liminal::assets::instance->load_texture(""));
    // textures[aiTextureType_NORMALS].push_back(liminal::assets::instance->load_texture(""));
    // textures[aiTextureType_NORMALS].push_back(liminal::assets::instance->load_texture(""));

    textures[aiTextureType_SHININESS].push_back(liminal::assets::instance->load_texture("assets/images/grass1-metal.png"));
    // textures[aiTextureType_SHININESS].push_back(liminal::assets::instance->load_texture(""));
    // textures[aiTextureType_SHININESS].push_back(liminal::assets::instance->load_texture(""));
    // textures[aiTextureType_SHININESS].push_back(liminal::assets::instance->load_texture(""));

    textures[aiTextureType_OPACITY].push_back(liminal::assets::instance->load_texture("assets/images/grass1-rough.png"));
    // textures[aiTextureType_OPACITY].push_back(liminal::assets::instance->load_texture(""));
    // textures[aiTextureType_OPACITY].push_back(liminal::assets::instance->load_texture(""));
    // textures[aiTextureType_OPACITY].push_back(liminal::assets::instance->load_texture(""));

    textures[aiTextureType_AMBIENT].push_back(liminal::assets::instance->load_texture("assets/images/grass1-ao.png"));
    // textures[aiTextureType_AMBIENT].push_back(liminal::assets::instance->load_texture(""));
    // textures[aiTextureType_AMBIENT].push_back(liminal::assets::instance->load_texture(""));
    // textures[aiTextureType_AMBIENT].push_back(liminal::assets::instance->load_texture(""));

    textures[aiTextureType_HEIGHT].push_back(liminal::assets::instance->load_texture("assets/images/grass1-height.png"));
    // textures[aiTextureType_HEIGHT].push_back(liminal::assets::instance->load_texture(""));
    // textures[aiTextureType_HEIGHT].push_back(liminal::assets::instance->load_texture(""));
    // textures[aiTextureType_HEIGHT].push_back(liminal::assets::instance->load_texture(""));

    mesh = new liminal::mesh(vertices, indices, textures);

    btTransform transform;
    transform.setIdentity();
    transform.setOrigin(btVector3(position.x, position.y, position.z));
    const auto motion_state = new btDefaultMotionState(transform);
    const auto collision_shape = new btHeightfieldTerrainShape((int)size, (int)size, heightfield.data(), 1, 0, 100, 1, PHY_FLOAT, true);
    const auto construction_info = btRigidBody::btRigidBodyConstructionInfo(0, motion_state, collision_shape);
    rigidbody = new btRigidBody(construction_info);
}

liminal::terrain::~terrain()
{
    delete mesh;
    delete rigidbody;
}

glm::mat4 liminal::terrain::get_model_matrix() const
{
    // TODO: remove and use transform component
    return glm::translate(glm::identity<glm::mat4>(), position);
}

float liminal::terrain::get_height(SDL_Surface *surface, int x, int z) const
{
    if (x < 0 || x >= surface->w || z < 0 || z >= surface->h)
    {
        return 0;
    }

    const union pixel_t
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

    auto height = (float)((pixel.color.r << 16) | (pixel.color.g << 8) | (pixel.color.b));
    height -= 0xffffff / 2;
    height /= 0xffffff / 2;
    height *= height_scale;

    return height;
}
