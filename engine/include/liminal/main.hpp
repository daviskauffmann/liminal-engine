#ifndef LIMINAL_MAIN_HPP
#define LIMINAL_MAIN_HPP

#include <liminal/core/engine.hpp>

int main(int argc, char *argv[])
{
    liminal::engine::get_instance().run(argc, argv);

    return 0;
}

#endif
