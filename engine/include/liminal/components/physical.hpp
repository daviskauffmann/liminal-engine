#ifndef LIMINAL_COMPONENTS_PHYSICAL_HPP
#define LIMINAL_COMPONENTS_PHYSICAL_HPP

class btDiscreteDynamicsWorld;
class btRigidBody;

namespace liminal
{
    class physical
    {
    public:
        btDiscreteDynamicsWorld *world;
        float mass;
        btRigidBody *rigidbody;

        physical(btDiscreteDynamicsWorld *const world, const float mass = 1.0f);
        ~physical();
    };
}

#endif
