#ifndef LIMINAL_PHYSICS_RIGIDBODY_HPP
#define LIMINAL_PHYSICS_RIGIDBODY_HPP

#include <glm/vec3.hpp>
#include <tuple>

class btRigidBody;

namespace liminal
{
    class rigidbody
    {
    public:
        rigidbody(float mass);
        rigidbody(rigidbody &&other) = delete;
        rigidbody(const rigidbody &other) = delete;
        rigidbody &operator=(rigidbody &&other) = delete;
        rigidbody &operator=(const rigidbody &other) = delete;
        ~rigidbody();

        btRigidBody *get_bt_rigidbody() const;

        std::tuple<glm::vec3, glm::vec3> get_world_transform() const;
        void set_world_transform(const glm::vec3 &position, const glm::vec3 &rotation) const;

    private:
        btRigidBody *bt_rigidbody;
    };
}

#endif
