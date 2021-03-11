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
        static float size;
        static float height_scale;

        glm::vec3 position;
        liminal::mesh *mesh;
        btRigidBody *rigidbody;

        terrain(glm::vec3 position, const std::string &heightmap_filename);
        ~terrain();

        glm::mat4 calc_model() const;

    private:
        std::vector<float> heightfield;
        btDefaultMotionState *motion_state;
        btCollisionShape *collision_shape;

        float get_height(SDL_Surface *surface, int x, int z) const;
    };
} // namespace liminal

#endif
