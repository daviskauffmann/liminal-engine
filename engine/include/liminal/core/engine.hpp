#ifndef LIMINAL_CORE_ENGINE_HPP
#define LIMINAL_CORE_ENGINE_HPP

namespace liminal
{
    class platform;
    class renderer;

    class engine
    {
    public:
        static engine *instance;

        liminal::platform *platform;
        liminal::renderer *renderer;

        engine();
        ~engine();

        void run(int argc, char *argv[]);
    };
}

#endif
