#ifndef LIMINAL_CORE_ENGINE_HPP
#define LIMINAL_CORE_ENGINE_HPP

namespace liminal
{
    class engine
    {
    public:
        static engine *instance;

        engine();
        ~engine();

        void run(int argc, char *argv[]);
        void stop();

    private:
        bool running = false;
    };
}

#endif
