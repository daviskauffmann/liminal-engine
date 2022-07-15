#ifndef LIMINAL_COMPONENTS_PHYSICAL_HPP
#define LIMINAL_COMPONENTS_PHYSICAL_HPP

class btRigidBody;

namespace liminal
{
    class physical
    {
    public:
        float mass;
        btRigidBody *rigidbody;

        physical(float mass);
        ~physical();
    };
}

#endif
