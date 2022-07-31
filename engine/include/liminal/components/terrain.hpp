#ifndef LIMINAL_COMPONENTS_TERRAIN_HPP
#define LIMINAL_COMPONENTS_TERRAIN_HPP

#include <SDL2/SDL.h>
#include <bullet/btBulletDynamicsCommon.h>
#include <glm/matrix.hpp>
#include <glm/vec3.hpp>
#include <liminal/graphics/mesh.hpp>
#include <memory>

namespace liminal
{
    class terrain
    {
    public:
        btDiscreteDynamicsWorld *world;
        glm::vec3 position;
        float size;
        std::unique_ptr<liminal::mesh> mesh;
        std::unique_ptr<btRigidBody> rigidbody;

        terrain(btDiscreteDynamicsWorld *const world, const std::string &filename, const glm::vec3 &position, float size, float height_scale);
        terrain(terrain &&other) = default;
        terrain(const terrain &other) = delete;
        terrain &operator=(terrain &&other) = default;
        terrain &operator=(const terrain &other) = delete;
        ~terrain();

        glm::mat4 get_model_matrix() const;

    private:
        float height_scale;
        std::vector<float> heightfield;

        float get_height(const SDL_Surface *surface, int x, int z) const;
    };
}

#endif
