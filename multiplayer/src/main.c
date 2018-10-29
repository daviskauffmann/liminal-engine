#include <engine/engine.h>
#include <shared/client.h>
#include <shared/server.h>

// TODO: rethink project structure

// perhaps separate networking from the core engine
// that way, we can build and run the server without needing to link with opengl stuff

// we also need a way to get the shared files in both the client and server
// does it make sense to make a .dll just for that?

int main(int argc, char *argv[])
{
    printf("1: Server\n2: Client\n");
    char c;
    scanf_s("%c", &c);

    switch (c)
    {
    case '1':
        return server_start();
    case '2':
        return client_start();
    }

    return 0;
}
