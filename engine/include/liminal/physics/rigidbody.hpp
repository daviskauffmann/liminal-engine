#ifndef LIMINAL_PHYSICS_RIGIDBODY_HPP
#define LIMINAL_PHYSICS_RIGIDBODY_HPP

#include <glm/vec3.hpp>
#include <tuple>

class btCollisionShape;
class btMotionState;
class btRigidBody;

namespace liminal
{
    namespace physics
    {
        class rigidbody
        {
        public:
            rigidbody(float mass);
            ~rigidbody();
            rigidbody(const rigidbody &other) = delete;
            rigidbody &operator=(const rigidbody &other) = delete;
            rigidbody(rigidbody &&other) = delete;
            rigidbody &operator=(rigidbody &&other) = delete;

            btRigidBody *get_bt_rigidbody() const;

            std::tuple<glm::vec3, glm::vec3> get_world_transform() const;
            void set_world_transform(
                const glm::vec3 &position,
                const glm::vec3 &rotation,
                const glm::vec3 &scale,
                float mass) const;

        private:
            btMotionState *bt_motion_state;
            btCollisionShape *bt_collision_shape;
            btRigidBody *bt_rigidbody;
        };
    }
}

#endif
