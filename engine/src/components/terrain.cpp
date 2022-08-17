#include <liminal/components/terrain.hpp>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <assimp/scene.h>
#include <bullet/BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h>
#include <glm/gtc/matrix_transform.hpp>
#include <liminal/core/assets.hpp>
#include <liminal/graphics/mesh.hpp>
#include <liminal/graphics/texture.hpp>
#include <stdexcept>

// TODO: multiple textures w/ blend map

// TODO: 3d terrain (caves and whatnot)
// this will probably be a different class

liminal::terrain::terrain(btDiscreteDynamicsWorld *const world, const std::string &filename, const glm::vec3 &position, const float size, const float height_scale, std::shared_ptr<liminal::assets> assets)
    : world(world),
      position(position),
      size(size),
      height_scale(height_scale)
{
    const auto surface = IMG_Load(filename.c_str());
    if (!surface)
    {
        throw std::runtime_error(IMG_GetError());
    }

    std::vector<liminal::mesh::vertex> vertices;
    for (int z = 0; z < surface->h; z++)
    {
        for (int x = 0; x < surface->w; x++)
        {
            liminal::mesh::vertex vertex;
            vertex.position = {
                -(float)x / ((float)surface->w - 1) * size,
                get_height(surface, x, z),
                -(float)z / ((float)surface->h - 1) * size};
            vertex.normal = glm::normalize(glm::vec3(
                get_height(surface, x + 1, z) - get_height(surface, x - 1, z),
                2,
                get_height(surface, x, z + 1) - get_height(surface, x, z - 1)));
            vertex.uv = {
                (float)x / ((float)surface->w - 1),
                (float)z / ((float)surface->h - 1)};
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

    std::array<std::vector<std::shared_ptr<liminal::texture>>, liminal::mesh::num_textures> textures;

    textures.at(aiTextureType_DIFFUSE).push_back(assets->load_texture("assets/images/grass1-albedo3.png"));
    // textures.at(aiTextureType_DIFFUSE).push_back(assets->load_texture(""));
    // textures.at(aiTextureType_DIFFUSE).push_back(assets->load_texture(""));
    // textures.at(aiTextureType_DIFFUSE).push_back(assets->load_texture(""));

    textures.at(aiTextureType_NORMALS).push_back(assets->load_texture("assets/images/grass1-normal1-ogl.png"));
    // textures.at(aiTextureType_NORMALS).push_back(assets->load_texture(""));
    // textures.at(aiTextureType_NORMALS).push_back(assets->load_texture(""));
    // textures.at(aiTextureType_NORMALS).push_back(assets->load_texture(""));

    textures.at(aiTextureType_SHININESS).push_back(assets->load_texture("assets/images/grass1-metal.png"));
    // textures.at(aiTextureType_SHININESS).push_back(assets->load_texture(""));
    // textures.at(aiTextureType_SHININESS).push_back(assets->load_texture(""));
    // textures.at(aiTextureType_SHININESS).push_back(assets->load_texture(""));

    textures.at(aiTextureType_OPACITY).push_back(assets->load_texture("assets/images/grass1-rough.png"));
    // textures.at(aiTextureType_OPACITY).push_back(assets->load_texture(""));
    // textures.at(aiTextureType_OPACITY).push_back(assets->load_texture(""));
    // textures.at(aiTextureType_OPACITY).push_back(assets->load_texture(""));

    textures.at(aiTextureType_AMBIENT).push_back(assets->load_texture("assets/images/grass1-ao.png"));
    // textures.at(aiTextureType_AMBIENT).push_back(assets->load_texture(""));
    // textures.at(aiTextureType_AMBIENT).push_back(assets->load_texture(""));
    // textures.at(aiTextureType_AMBIENT).push_back(assets->load_texture(""));

    textures.at(aiTextureType_HEIGHT).push_back(assets->load_texture("assets/images/grass1-height.png"));
    // textures.at(aiTextureType_HEIGHT).push_back(assets->load_texture(""));
    // textures.at(aiTextureType_HEIGHT).push_back(assets->load_texture(""));
    // textures.at(aiTextureType_HEIGHT).push_back(assets->load_texture(""));

    mesh = std::make_unique<liminal::mesh>(vertices, indices, textures);

    btTransform transform;
    transform.setIdentity();
    transform.setOrigin(btVector3(position.x, position.y, position.z));
    const auto motion_state = new btDefaultMotionState(transform);
    const auto collision_shape = new btHeightfieldTerrainShape((int)size, (int)size, heightfield.data(), 1, 0, 100, 1, PHY_FLOAT, true);
    const auto construction_info = btRigidBody::btRigidBodyConstructionInfo(0, motion_state, collision_shape);
    rigidbody = std::make_unique<btRigidBody>(construction_info);
    world->addRigidBody(rigidbody.get());
}

liminal::terrain::~terrain()
{
    delete rigidbody->getMotionState();
    delete rigidbody->getCollisionShape();
    world->removeRigidBody(rigidbody.get());
}

glm::mat4 liminal::terrain::get_model_matrix() const
{
    // TODO: remove and use transform component
    return glm::translate(glm::identity<glm::mat4>(), position);
}

float liminal::terrain::get_height(const SDL_Surface *const surface, const int x, const int z) const
{
    if (x < 0 || x >= surface->w || z < 0 || z >= surface->h)
    {
        return 0;
    }

    const union pixel_t
    {
        // unsigned int pixel;
        struct color_t
        {
            unsigned char r;
            unsigned char g;
            unsigned char b;
            // unsigned char a;
        } color;
    } pixel = *reinterpret_cast<pixel_t *>(reinterpret_cast<unsigned char *>(surface->pixels) + z * surface->pitch + x * surface->format->BytesPerPixel);

    auto height = (float)((pixel.color.r << 16) | (pixel.color.g << 8) | (pixel.color.b));
    height -= 0xffffff / 2;
    height /= 0xffffff / 2;
    height *= height_scale;

    return height;
}
