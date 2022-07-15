#ifndef LIMINAL_MAIN_HPP
#define LIMINAL_MAIN_HPP

#include <liminal/core/engine.hpp>

int main(int argc, char *argv[])
{
    auto engine = new liminal::engine();
    engine->run(argc, argv);
    delete engine;

    return 0;
}

#endif
