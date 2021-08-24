#ifndef TERRAIN_HPP
#define TERRAIN_HPP

#include <bullet/btBulletDynamicsCommon.h>
#include <glm/vec3.hpp>
#include <glm/matrix.hpp>
#include <SDL2/SDL.h>

#include "mesh.hpp"

namespace liminal
{
    struct terrain
    {
        glm::vec3 position;
        float size;
        liminal::mesh *mesh;
        btRigidBody *rigidbody;

        terrain(const std::string &heightmap_filename, glm::vec3 position, float size, float height_scale);
        ~terrain();

        glm::mat4 calc_model() const;

    private:
        float height_scale;

        std::vector<float> heightfield;
        btDefaultMotionState *motion_state;
        btCollisionShape *collision_shape;

        float get_height(SDL_Surface *surface, int x, int z) const;
    };
}

#endif
