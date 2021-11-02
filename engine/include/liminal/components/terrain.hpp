#ifndef LIMINAL_COMPONENTS_TERRAIN_HPP
#define LIMINAL_COMPONENTS_TERRAIN_HPP

#include <bullet/btBulletDynamicsCommon.h>
#include <glm/vec3.hpp>
#include <glm/matrix.hpp>
#include <liminal/graphics/mesh.hpp>
#include <SDL2/SDL.h>

namespace liminal
{
    class terrain
    {
    public:
        glm::vec3 position;
        float size;
        liminal::mesh *mesh;
        btRigidBody *rigidbody;

        terrain(const std::string &filename, glm::vec3 position, float size, float height_scale);

        glm::mat4 get_model_matrix() const;

    private:
        float height_scale;

        std::vector<float> heightfield;
        btDefaultMotionState *motion_state;
        btCollisionShape *collision_shape;

        float get_height(SDL_Surface *surface, int x, int z) const;
    };
}

#endif
