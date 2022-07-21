#ifndef LIMINAL_CORE_APP_HPP
#define LIMINAL_CORE_APP_HPP

// TODO: app shouldn't own a scene like this
// probably just wanna make the physics world and entt registry just part of the engine and accessible globally through some handy api

namespace liminal
{
    class scene;

    class app
    {
    public:
        liminal::scene *scene = nullptr;

        app(int argc, char *argv[]);
        virtual ~app();

        void run();
        void stop();

        virtual void update(unsigned int current_time, float delta_time) {}
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
