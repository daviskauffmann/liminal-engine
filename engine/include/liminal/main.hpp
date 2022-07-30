#ifndef LIMINAL_MAIN_HPP
#define LIMINAL_MAIN_HPP

#include <liminal/core/app.hpp>

static size_t leak = 0;

void *operator new(size_t size)
{
    leak += size;

    return malloc(size);
}

void operator delete(void *memory, size_t size)
{
    leak -= size;

    free(memory);
}

int main(int argc, char *argv[])
{
    auto app = liminal::create_app(argc, argv);
    app->run();
    delete app;

    std::cout << "Bytes leaked: " << leak << std::endl;

    return 0;
}

#endif
