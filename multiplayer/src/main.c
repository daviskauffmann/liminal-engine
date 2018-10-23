#include <engine/engine.h>
#include <shared/client.h>
#include <shared/server.h>

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
