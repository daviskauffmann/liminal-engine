#ifndef LIMINAL_PHYSICS_WORLD_HPP
#define LIMINAL_PHYSICS_WORLD_HPP

#include <memory>

class btDiscreteDynamicsWorld;

namespace liminal
{
    class rigidbody;

    class world
    {
    public:
        world();
        world(world &&other) = delete;
        world(const world &other) = delete;
        world &operator=(world &&other) = delete;
        world &operator=(const world &other) = delete;
        ~world();

        void add_rigidbody(std::shared_ptr<liminal::rigidbody> rigidbody);
        void remove_rigidbody(std::shared_ptr<liminal::rigidbody> rigidbody);

        void update(float delta_time);

    private:
        btDiscreteDynamicsWorld *bt_world;
    };
}

#endif
