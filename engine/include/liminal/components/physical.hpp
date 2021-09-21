#ifndef PHYSICAL_HPP
#define PHYSICAL_HPP

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
