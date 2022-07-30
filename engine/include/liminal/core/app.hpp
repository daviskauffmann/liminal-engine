#ifndef LIMINAL_CORE_APP_HPP
#define LIMINAL_CORE_APP_HPP

namespace liminal
{
    class app
    {
    public:
        app(int argc, char *argv[]);
        virtual ~app();

        void run();
        void stop();

        virtual void update(unsigned int current_time, float delta_time);
        virtual void resize(int width, int height);

    private:
        int window_width = 1280;
        int window_height = 720;
        float render_scale = 1;
        bool running = true;
        float time_scale = 1;
        bool console_open = false;
    };

    liminal::app *create_app(int argc, char *argv[]);
}

#endif
