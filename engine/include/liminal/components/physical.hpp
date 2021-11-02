#ifndef LIMINAL_COMPONENTS_PHYSICAL_HPP
#define LIMINAL_COMPONENTS_PHYSICAL_HPP

class btRigidBody;

namespace liminal
{
    struct physical
    {
        float mass;
        btRigidBody *rigidbody;

        physical(float mass);
    };
}

#endif
