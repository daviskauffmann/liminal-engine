#ifndef LIMINAL_CORE_ENGINE_HPP
#define LIMINAL_CORE_ENGINE_HPP

namespace liminal
{
    class assets;
    class platform;
    class renderer;

    class engine
    {
    public:
        static engine *instance;

        liminal::assets *assets;
        liminal::platform *platform;
        liminal::renderer *renderer;

        engine();
        ~engine();

        void run(int argc, char *argv[]);
        void stop();

    private:
        bool running = false;
    };
}

#endif
