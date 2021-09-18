#ifndef APP_HPP
#define APP_HPP

namespace liminal
{
    class app
    {
    public:
        virtual ~app() {}

        virtual void update(float delta_time) {}
    };

    liminal::app *create_app(int argc, char *argv[]);
}

#endif
