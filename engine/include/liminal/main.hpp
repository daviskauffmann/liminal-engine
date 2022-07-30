#ifndef LIMINAL_MAIN_HPP
#define LIMINAL_MAIN_HPP

#include <liminal/core/app.hpp>

int main(int argc, char *argv[])
{
    auto app = liminal::create_app(argc, argv);
    app->run();
    delete app;

    return 0;
}

#endif
