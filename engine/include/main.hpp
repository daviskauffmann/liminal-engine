#ifndef MAIN_HPP
#define MAIN_HPP

#include "core/engine.hpp"

int main(int argc, char *argv[])
{
    liminal::engine engine;
    engine.run(argc, argv);

    return 0;
}

#endif
