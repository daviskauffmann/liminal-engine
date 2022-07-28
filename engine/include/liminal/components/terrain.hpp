#ifndef LIMINAL_COMPONENTS_TERRAIN_HPP
#define LIMINAL_COMPONENTS_TERRAIN_HPP

#include <SDL2/SDL.h>
#include <bullet/btBulletDynamicsCommon.h>
#include <glm/matrix.hpp>
#include <glm/vec3.hpp>
#include <liminal/graphics/mesh.hpp>

namespace liminal
{
    class terrain
    {
    public:
        btDiscreteDynamicsWorld *world;
        glm::vec3 position;
        float size;
        liminal::mesh *mesh;
        btRigidBody *rigidbody;

        terrain(btDiscreteDynamicsWorld *const world, const std::string &filename, const glm::vec3 &position, const float size, const float height_scale);
        ~terrain();

        glm::mat4 get_model_matrix() const;

    private:
        float height_scale;
        std::vector<float> heightfield;

        float get_height(const SDL_Surface *const surface, const int x, const int z) const;
    };
}

#endif
