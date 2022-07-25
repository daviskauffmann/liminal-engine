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

        virtual void update(const unsigned int current_time, const float delta_time);
        virtual void resize(int width, int height);

    private:
        int window_width;
        int window_height;
        float render_scale;
        bool running = true;
        float time_scale = 1;
        bool console_open = false;
    };

    liminal::app *create_app(int argc, char *argv[]);
}

#endif
