#ifndef MAIN_HPP
#define MAIN_HPP

#include <liminal/core/engine.hpp>

int main(int argc, char *argv[])
{
    liminal::engine::get_instance().run(argc, argv);

    return 0;
}

#endif
