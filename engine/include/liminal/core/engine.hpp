#ifndef LIMINAL_CORE_ENGINE_HPP
#define LIMINAL_CORE_ENGINE_HPP

namespace liminal
{
    class platform;
    class renderer;

    class engine
    {
    public:
        static engine &get_instance();

        liminal::platform *platform;
        liminal::renderer *renderer;

        ~engine();

        void run(int argc, char *argv[]);

    private:
        engine();
    };
}

#endif
