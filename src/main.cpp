#include <bullet/btBulletDynamicsCommon.h>
#include <cxxopts.hpp>
#include <iostream>
#include <imgui.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_opengl3.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_net.h>

#include "audio.hpp"
#include "directional_light.hpp"
#include "camera.hpp"
#include "message.hpp"
#include "model.hpp"
#include "object.hpp"
#include "point_light.hpp"
#include "renderer.hpp"
#include "skybox.hpp"
#include "sound.hpp"
#include "source.hpp"
#include "spot_light.hpp"
#include "sprite.hpp"
#include "terrain.hpp"
#include "water.hpp"

#define SDL_FLAGS (SDL_INIT_AUDIO | SDL_INIT_VIDEO)
#define IMG_FLAGS (IMG_INIT_JPG | IMG_INIT_PNG)
#define MIX_FLAGS (0)

#define VERSION "v0.0.1"

#define WINDOW_TITLE "Project Kilonova"

struct client
{
    int id;
    TCPsocket socket;
    IPaddress udp_address;
    glm::vec3 position;
    pk::object *avatar;
};

int main(int argc, char *argv[])
{
    bool server;
    bool dedicated;
    std::string server_host;
    unsigned short server_port;
    int window_width;
    int window_height;
    int render_scale;

    try
    {
        cxxopts::Options options("pk");

        cxxopts::OptionAdder option_adder = options.add_options();
        option_adder("d,dedicated", "Run as dedicated server (does nothing ATM)");
        option_adder("height", "Set window height", cxxopts::value<int>()->default_value("720"));
        option_adder("h,help", "Print usage");
        option_adder("host", "Set server host (client only)", cxxopts::value<std::string>()->default_value("127.0.0.1"));
        option_adder("j,join", "Join a server");
        option_adder("port", "Set server port", cxxopts::value<unsigned short>()->default_value("3000"));
        option_adder("scale", "Set render scale", cxxopts::value<float>()->default_value("1.0"));
        option_adder("v,version", "Print version");
        option_adder("width", "Set window width", cxxopts::value<int>()->default_value("1280"));

        cxxopts::ParseResult result = options.parse(argc, argv);

        dedicated = result.count("dedicated");

        window_height = result["height"].as<int>();

        if (result.count("help"))
        {
            std::cout << options.help() << std::endl;
            return 0;
        }

        server_host = result["host"].as<std::string>();

        server = !result.count("join");

        server_port = result["port"].as<unsigned short>();

        render_scale = glm::clamp(result["scale"].as<float>(), 0.1f, 1.0f);

        if (result.count("version"))
        {
            std::cout << VERSION << std::endl;
            return 0;
        }

        window_width = result["width"].as<int>();
    }
    catch (std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    if (SDL_Init(SDL_FLAGS) != 0)
    {
        std::cerr << "Error: Failed to initialize SDL: " << SDL_GetError() << std::endl;
        return 1;
    }

    if (IMG_Init(IMG_FLAGS) != IMG_FLAGS)
    {
        std::cerr << "Error: Failed to initialize SDL_image: " << IMG_GetError() << std::endl;
        return 1;
    }

    if (Mix_Init(MIX_FLAGS) != MIX_FLAGS)
    {
        std::cerr << "Error: Failed to initialize SDL_mixer: " << Mix_GetError() << std::endl;
        return 1;
    }

    if (SDLNet_Init() != 0)
    {
        std::cerr << "Error: Failed to initialize SDL_net: " << SDLNet_GetError() << std::endl;
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow(
        (std::string(WINDOW_TITLE) + (server ? " (Server)" : " (Client)")).c_str(),
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        window_width,
        window_height,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if (!window)
    {
        std::cerr << "Error: Failed to create window: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetSwapInterval(0);
    SDL_GLContext context = SDL_GL_CreateContext(window);
    if (!context)
    {
        std::cerr << "Error: Failed to create OpenGL context: " << SDL_GetError() << std::endl;
        return 1;
    }

    GLenum error = glewInit();
    if (error != GLEW_OK)
    {
        std::cerr << "Error: Failed to initialize GLEW: " << glewGetErrorString(error) << std::endl;
        return 1;
    }

    ImGui::CreateContext();
    ImGui_ImplSDL2_InitForOpenGL(window, context);
    ImGui_ImplOpenGL3_Init("#version 460");
    ImGuiIO &io = ImGui::GetIO();
    io.IniFilename = "assets/imgui.ini";

    if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 1024) != 0)
    {
        std::cerr << "Error: Failed to initialize the mixer API: " << Mix_GetError() << std::endl;
        return 1;
    }

    pk::renderer renderer(
        window_width, window_height, render_scale,
        window_width, window_height,
        window_width, window_height);
    pk::audio audio;

    btDefaultCollisionConfiguration *collision_configuration = new btDefaultCollisionConfiguration();
    btCollisionDispatcher *dispatcher = new btCollisionDispatcher(collision_configuration);
    btBroadphaseInterface *overlapping_pair_cache = new btDbvtBroadphase();
    btSequentialImpulseConstraintSolver *solver = new btSequentialImpulseConstraintSolver();
    btDiscreteDynamicsWorld *world = new btDiscreteDynamicsWorld(dispatcher, overlapping_pair_cache, solver, collision_configuration);

    world->setGravity(btVector3(0.0f, -9.8f, 0.0f));

    pk::camera *camera = new pk::camera(
        glm::vec3(0.0f, 0.0f, 3.0f),
        0.0f,
        -90.0f,
        0.0f,
        45.0f);

    // pk::skybox *skybox = nullptr;
    // pk::skybox *skybox = new pk::skybox("assets/images/Circus_Backstage_8k.jpg");
    pk::skybox *skybox = new pk::skybox("assets/images/GCanyon_C_YumaPoint_8k.jpg");

    pk::model *backpack_model = new pk::model("assets/models/backpack/backpack.obj");
    pk::object *backpack = new pk::object(
        backpack_model,
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(1.0f, 1.0f, 1.0f),
        1.0f);
    // world->addRigidBody(backpack->rigidbody);

    pk::model *cube_model = new pk::model("assets/models/cube/cube.obj");
    pk::object *cube = new pk::object(
        cube_model,
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(1.0f, 1.0f, 1.0f),
        1.0f);
    // world->addRigidBody(cube->rigidbody);

    const float sun_intensity = 10.0f;
    pk::directional_light *sun = new pk::directional_light(
        glm::vec3(0.352286f, -0.547564f, -0.758992f),
        glm::vec3(1.0f, 1.0f, 1.0f) * sun_intensity,
        4096);

    const float light_intensity = 10.0f;
    pk::point_light *red_light = new pk::point_light(
        glm::vec3(2.0f, 0.0f, 2.0f),
        glm::vec3(1.0f, 0.0f, 0.0f) * light_intensity,
        512);
    pk::point_light *yellow_light = new pk::point_light(
        glm::vec3(-2.0f, 0.0f, -2.0f),
        glm::vec3(1.0f, 1.0f, 0.0f) * light_intensity,
        512);
    pk::point_light *green_light = new pk::point_light(
        glm::vec3(2.0f, 0.0f, -2.0f),
        glm::vec3(0.0f, 1.0f, 0.0f) * light_intensity,
        512);
    pk::point_light *blue_light = new pk::point_light(
        glm::vec3(-2.0f, 0.0f, 2.0f),
        glm::vec3(0.0f, 0.0f, 1.0f) * light_intensity,
        512);

    const float flashlight_intensity = 20.0f;
    pk::spot_light *flashlight = new pk::spot_light(
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(1.0f, 1.0f, 1.0f) * flashlight_intensity,
        cosf(glm::radians(12.5f)),
        cosf(glm::radians(15.0f)),
        1024);

    pk::water *water = new pk::water(
        glm::vec3(0.0f, -2.0f, 0.0f),
        glm::vec2(100.0f, 100.0f));

    pk::terrain *terrain = new pk::terrain(glm::vec3(400.0f, 0.0f, 400.0f), "assets/images/heightmap.png");
    // world->addRigidBody(terrain->rigidbody);

    pk::sound *ambient_sound = new pk::sound("assets/audio/ambient.wav");
    pk::sound *bounce_sound = new pk::sound("assets/audio/bounce.wav");
    pk::sound *shoot_sound = new pk::sound("assets/audio/shoot.wav");

    pk::source *ambient_source = new pk::source(glm::vec3(0.0f, 0.0f, 0.0f));
    ambient_source->set_loop(true);
    ambient_source->set_gain(0.25f);
    // ambient_source->play(ambient_sound);
    pk::source *bounce_source = new pk::source(glm::vec3(0.0f, 0.0f, 0.0f));
    pk::source *shoot_source = new pk::source(glm::vec3(0.0f, 0.0f, 0.0f));

    IPaddress server_address;
    if (SDLNet_ResolveHost(&server_address, server ? INADDR_ANY : server_host.c_str(), server_port))
    {
        std::cerr << "Error: Failed to resolve host: " << SDLNet_GetError() << std::endl;
        return 1;
    }

    TCPsocket tcp_socket = SDLNet_TCP_Open(&server_address);
    if (!tcp_socket)
    {
        std::cerr << "Error: Failed to open TCP socket: " << SDLNet_GetError() << std::endl;
        return 1;
    }

    UDPsocket udp_socket = SDLNet_UDP_Open(server ? server_port : 0);
    if (!udp_socket)
    {
        std::cerr << "Error: Failed to open UDP socket: " << SDLNet_GetError() << std::endl;
        return 1;
    }

    SDLNet_SocketSet socket_set = SDLNet_AllocSocketSet(server ? MAX_CLIENTS + 2 : 2);
    if (!socket_set)
    {
        std::cerr << "Error: Failed to allocate socket set: " << SDLNet_GetError() << std::endl;
        return 1;
    }
    SDLNet_TCP_AddSocket(socket_set, tcp_socket);
    SDLNet_UDP_AddSocket(socket_set, udp_socket);

    client clients[MAX_CLIENTS];
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        clients[i].id = -1;
        clients[i].position.x = 0;
        clients[i].position.y = 0;
        clients[i].position.z = 0;
        clients[i].avatar = nullptr;
    }

    int client_id = -1;
    if (server)
    {
        client_id = 0;
        clients[0].id = 0;
    }
    else
    {
        char buffer[PACKET_SIZE];
        if (SDLNet_TCP_Recv(tcp_socket, buffer, sizeof(buffer)) > 1)
        {
            pk::message *message = (pk::message *)buffer;
            switch (message->type)
            {
            case pk::message_type::MESSAGE_CONNECT_OK:
            {
                pk::connect_ok_message *connect_ok_message = (pk::connect_ok_message *)message;

                client_id = connect_ok_message->id;
                std::cout << "ID " << client_id << " assigned by server" << std::endl;
                // TODO: server should tell client their own position

                for (int i = 0; i < MAX_CLIENTS; i++)
                {
                    clients[i].id = connect_ok_message->clients[i].id;

                    if (clients[i].id != -1 && clients[i].id != client_id)
                    {
                        clients[connect_ok_message->clients[i].id].avatar = new pk::object(
                            cube_model,
                            glm::vec3(connect_ok_message->clients[i].x, connect_ok_message->clients[i].y, connect_ok_message->clients[i].z),
                            glm::vec3(0.0f, 0.0f, 0.0f),
                            glm::vec3(1.0f, 1.0f, 1.0f),
                            1.0f);
                    }
                }
            }
            break;
            case pk::message_type::MESSAGE_CONNECT_FULL:
            {
                std::cerr << "Error: Server is full" << std::endl;
                return 1;
            }
            break;
            default:
            {
                std::cerr << "Error: Unknown server response" << std::endl;
                return 1;
            }
            break;
            }
        }

        {
            pk::id_message *id_message = (pk::id_message *)malloc(sizeof(*id_message));
            id_message->type = pk::message_type::MESSAGE_UDP_CONNECT_REQUEST;
            id_message->id = client_id;

            UDPpacket *packet = SDLNet_AllocPacket(PACKET_SIZE);
            packet->address = server_address;
            packet->data = (unsigned char *)id_message;
            packet->len = sizeof(*id_message);
            if (!SDLNet_UDP_Send(udp_socket, -1, packet))
            {
                std::cerr << "Error: Failed to make UDP connection" << std::endl;
                return 1;
            }
            SDLNet_FreePacket(packet);
        }
    }

    unsigned int current_time = 0;
    float time_scale = 1.0f;
    bool console_open = false;
    bool wireframe = false;
    bool edit_mode = false;
    bool lock_cursor = true;
    bool flashlight_on = true;
    bool flashlight_follow = true;

    bool quit = false;
    while (!quit)
    {
        unsigned int previous_time = current_time;
        current_time = SDL_GetTicks();
        float delta_time = ((current_time - previous_time) / 1000.0f) * time_scale;

        int num_keys;
        const unsigned char *keys = SDL_GetKeyboardState(&num_keys);
        int mouse_x, mouse_y;
        unsigned int mouse = SDL_GetMouseState(&mouse_x, &mouse_y);

        SDL_SetRelativeMouseMode((SDL_bool)lock_cursor);

        if (server)
        {
            while (SDLNet_CheckSockets(socket_set, 0) > 0)
            {
                if (SDLNet_SocketReady(tcp_socket))
                {
                    TCPsocket socket = SDLNet_TCP_Accept(tcp_socket);
                    if (socket)
                    {
                        int new_client_id = -1;
                        for (int i = 0; i < MAX_CLIENTS; i++)
                        {
                            if (clients[i].id == -1)
                            {
                                new_client_id = i;
                                break;
                            }
                        }
                        if (new_client_id != -1)
                        {
                            std::cout << "Connected to client, assigning ID " << new_client_id << std::endl;

                            clients[new_client_id].id = new_client_id;
                            clients[new_client_id].socket = socket;
                            clients[new_client_id].position.x = 0;
                            clients[new_client_id].position.y = 0;
                            clients[new_client_id].position.z = 0;
                            clients[new_client_id].avatar = new pk::object(
                                cube_model,
                                glm::vec3(clients[new_client_id].position.x, clients[new_client_id].position.y, clients[new_client_id].position.z),
                                glm::vec3(0.0f, 0.0f, 0.0f),
                                glm::vec3(1.0f, 1.0f, 1.0f),
                                1.0f);

                            SDLNet_TCP_AddSocket(socket_set, clients[new_client_id].socket);

                            pk::connect_ok_message connect_ok_message = pk::connect_ok_message(pk::message_type::MESSAGE_CONNECT_OK, clients[new_client_id].id);
                            for (int i = 0; i < MAX_CLIENTS; i++)
                            {
                                connect_ok_message.clients[i].id = clients[i].id;

                                if (clients[i].id != -1)
                                {
                                    connect_ok_message.clients[i].x = clients[i].position.x;
                                    connect_ok_message.clients[i].y = clients[i].position.y;
                                    connect_ok_message.clients[i].z = clients[i].position.z;
                                }
                            }
                            SDLNet_TCP_Send(socket, &connect_ok_message, sizeof(connect_ok_message));

                            pk::client_info client;
                            client.x = clients[new_client_id].position.x;
                            client.y = clients[new_client_id].position.y;
                            client.z = clients[new_client_id].position.z;
                            pk::connect_broadcast_message connect_broadcast_message = pk::connect_broadcast_message(pk::message_type::MESSAGE_CONNECT_BROADCAST, clients[new_client_id].id, client);
                            for (int i = 0; i < MAX_CLIENTS; i++)
                            {
                                if (clients[i].id != -1 && clients[i].id != client_id && clients[i].id != clients[new_client_id].id)
                                {
                                    if (SDLNet_TCP_Send(clients[i].socket, &connect_broadcast_message, sizeof(connect_broadcast_message)) < (int)sizeof(connect_broadcast_message))
                                    {
                                        std::cerr << "Error: Failed to send TCP packet" << std::endl;
                                    }
                                }
                            }
                        }
                        else
                        {
                            std::cout << "A client tried to connect, but the server is full" << std::endl;

                            pk::message message = pk::message(pk::message_type::MESSAGE_CONNECT_FULL);
                            SDLNet_TCP_Send(socket, &message, sizeof(message));
                        }
                    }
                }

                for (int i = 0; i < MAX_CLIENTS; i++)
                {
                    if (clients[i].id != -1 && clients[i].id != client_id)
                    {
                        if (SDLNet_SocketReady(clients[i].socket))
                        {
                            char buffer[PACKET_SIZE];
                            if (SDLNet_TCP_Recv(clients[i].socket, buffer, sizeof(buffer)) > 1)
                            {
                                pk::message *message = (pk::message *)buffer;
                                switch (message->type)
                                {
                                case pk::message_type::MESSAGE_DISCONNECT_REQUEST:
                                {
                                    std::cout << "Client disconnected" << std::endl;

                                    pk::id_message id_message = pk::id_message(pk::message_type::MESSAGE_DISCONNECT_BROADCAST, clients[i].id);
                                    for (int j = 0; j < MAX_CLIENTS; j++)
                                    {
                                        if (clients[j].id != -1 && clients[j].id != client_id && clients[j].id != clients[i].id)
                                        {
                                            if (SDLNet_TCP_Send(clients[j].socket, &id_message, sizeof(id_message)) < (int)sizeof(id_message))
                                            {
                                                std::cerr << "Error: Failed to send TCP packet" << std::endl;
                                            }
                                        }
                                    }

                                    SDLNet_TCP_DelSocket(socket_set, clients[i].socket);
                                    SDLNet_TCP_Close(clients[i].socket);

                                    clients[i].id = -1;
                                    clients[i].socket = nullptr;
                                }
                                break;
                                case pk::message_type::MESSAGE_CHAT_REQUEST:
                                {
                                    pk::chat_message *chat_message = (pk::chat_message *)message;
                                    std::cout << "Client " << chat_message->id << " says: " << chat_message->str << std::endl;

                                    chat_message->type = pk::message_type::MESSAGE_CHAT_BROADCAST;
                                    for (int j = 0; j < MAX_CLIENTS; j++)
                                    {
                                        if (clients[j].id != -1 && clients[j].id != client_id && clients[j].id != clients[i].id)
                                        {
                                            if (SDLNet_TCP_Send(clients[j].socket, &chat_message, sizeof(chat_message)) < (int)sizeof(chat_message))
                                            {
                                                std::cerr << "Error: Failed to send TCP packet" << std::endl;
                                            }
                                        }
                                    }
                                }
                                break;
                                default:
                                {
                                    std::cerr << "Error: Unknown TCP packet type: " << message->type << std::endl;
                                }
                                break;
                                }
                            }
                        }
                    }
                }

                if (SDLNet_SocketReady(udp_socket))
                {
                    UDPpacket *packet = SDLNet_AllocPacket(PACKET_SIZE);
                    if (SDLNet_UDP_Recv(udp_socket, packet) == 1)
                    {
                        pk::message *message = (pk::message *)packet->data;
                        switch (message->type)
                        {
                        case pk::message_type::MESSAGE_UDP_CONNECT_REQUEST:
                        {
                            pk::id_message *id_message = (pk::id_message *)message;
                            clients[id_message->id].udp_address = packet->address;

                            std::cout << "Saving UDP info of client " << id_message->id << std::endl;
                        }
                        break;
                        case pk::message_type::MESSAGE_POSITION_REQUEST:
                        {
                            pk::position_message *position_message = (pk::position_message *)message;

                            // TODO: validate position

                            if (clients[position_message->id].id != -1)
                            {
                                clients[position_message->id].position.x = position_message->x;
                                clients[position_message->id].position.y = position_message->y;
                                clients[position_message->id].position.z = position_message->z;

                                pk::position_message *position_message2 = (pk::position_message *)malloc(sizeof(*position_message2));
                                position_message2->type = pk::message_type::MESSAGE_POSITION_BROADCAST;
                                position_message2->id = position_message->id;
                                position_message2->x = position_message->x;
                                position_message2->y = position_message->y;
                                position_message2->z = position_message->z;

                                UDPpacket *packet = SDLNet_AllocPacket(PACKET_SIZE);
                                for (int i = 0; i < MAX_CLIENTS; i++)
                                {
                                    if (clients[i].id != -1 && clients[i].id != client_id && clients[i].id != position_message2->id)
                                    {
                                        packet->address = clients[i].udp_address;
                                        packet->data = (unsigned char *)position_message2;
                                        packet->len = sizeof(*position_message2);

                                        if (SDLNet_UDP_Send(udp_socket, -1, packet) != 1)
                                        {
                                            std::cerr << "Error: Failed to send UDP packet" << std::endl;
                                        }
                                    }
                                }
                                SDLNet_FreePacket(packet);
                            }
                        }
                        break;
                        default:
                        {
                            std::cerr << "Error: Unknown UDP packet type: " << message->type << std::endl;
                        }
                        break;
                        }
                    }
                    SDLNet_FreePacket(packet);
                }
            }
        }
        else
        {
            while (SDLNet_CheckSockets(socket_set, 0) > 0)
            {
                if (SDLNet_SocketReady(tcp_socket))
                {
                    char buffer[PACKET_SIZE];
                    if (SDLNet_TCP_Recv(tcp_socket, buffer, sizeof(buffer)) > 1)
                    {
                        pk::message *message = (pk::message *)buffer;
                        switch (message->type)
                        {
                        case pk::message_type::MESSAGE_CONNECT_BROADCAST:
                        {
                            pk::connect_broadcast_message *connect_broadcast_message = (pk::connect_broadcast_message *)message;

                            clients[connect_broadcast_message->id].id = connect_broadcast_message->id;
                            clients[connect_broadcast_message->id].avatar = new pk::object(
                                cube_model,
                                glm::vec3(connect_broadcast_message->client.x, connect_broadcast_message->client.y, connect_broadcast_message->client.z),
                                glm::vec3(0.0f, 0.0f, 0.0f),
                                glm::vec3(1.0f, 1.0f, 1.0f),
                                1.0f);

                            std::cout << "Client with ID " << connect_broadcast_message->id << " has joined " << std::endl;
                        }
                        break;
                        case pk::message_type::MESSAGE_DISCONNECT_BROADCAST:
                        {
                            pk::id_message *id_message = (pk::id_message *)message;

                            clients[id_message->id].id = -1;
                            delete clients[id_message->id].avatar;
                            clients[id_message->id].avatar = nullptr;

                            std::cout << "Client with ID " << id_message->id << " has disconnected" << std::endl;
                        }
                        break;
                        case pk::message_type::MESSAGE_CHAT_BROADCAST:
                        {
                            pk::chat_message *chat_message = (pk::chat_message *)message;

                            std::cout << "Client " << chat_message->id << ": " << chat_message->str << std::endl;
                        }
                        break;
                        default:
                        {
                            std::cerr << "Error: Unknown TCP packet type: " << message->type << std::endl;
                        }
                        break;
                        }
                    }
                }

                if (SDLNet_SocketReady(udp_socket))
                {
                    UDPpacket *packet = SDLNet_AllocPacket(PACKET_SIZE);
                    if (SDLNet_UDP_Recv(udp_socket, packet) == 1)
                    {
                        pk::message *message = (pk::message *)packet->data;
                        switch (message->type)
                        {
                        case pk::message_type::MESSAGE_POSITION_BROADCAST:
                        {
                            pk::position_message *position_message = (pk::position_message *)message;

                            if (clients[position_message->id].id != -1)
                            {
                                clients[position_message->id].position.x = position_message->x;
                                clients[position_message->id].position.y = position_message->y;
                                clients[position_message->id].position.z = position_message->z;
                            }
                        }
                        break;
                        default:
                        {
                            std::cerr << "Error: Unknown UDP packet type: " << message->type << std::endl;
                        }
                        break;
                        }
                    }
                    SDLNet_FreePacket(packet);
                }
            }
        }

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);

            switch (event.type)
            {
            case SDL_QUIT:
            {
                quit = true;
            }
            break;
            case SDL_WINDOWEVENT:
            {
                switch (event.window.event)
                {
                case SDL_WINDOWEVENT_RESIZED:
                {
                    window_width = event.window.data1;
                    window_height = event.window.data2;
                    SDL_SetWindowSize(window, window_width, window_height);
                    renderer.set_screen_size(window_width, window_height, render_scale);
                    renderer.set_reflection_size(window_width, window_height);
                    renderer.set_refraction_size(window_width, window_height);
                    std::cout << "Window resized to " << window_width << "x" << window_height << std::endl;
                }
                break;
                }
            }
            break;
            case SDL_KEYDOWN:
            {
                if (!io.WantCaptureKeyboard)
                {
                    switch (event.key.keysym.sym)
                    {
                    case SDLK_TAB:
                    {
                        lock_cursor = !lock_cursor;
                    }
                    break;
                    case SDLK_RETURN:
                    {
                        if (keys[SDL_SCANCODE_LALT])
                        {
                            unsigned int flags = SDL_GetWindowFlags(window);
                            if (flags & SDL_WINDOW_FULLSCREEN_DESKTOP)
                            {
                                SDL_SetWindowFullscreen(window, 0);
                            }
                            else
                            {
                                SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
                            }
                        }
                    }
                    break;
                    case SDLK_MINUS:
                    {
                        if (render_scale > 0.2f)
                        {
                            render_scale -= 0.1f;
                        }
                        renderer.set_screen_size(window_width, window_height, render_scale);
                        std::cout << "Render scale changed to " << render_scale << std::endl;
                    }
                    break;
                    case SDLK_EQUALS:
                    {
                        if (render_scale < 1.0f)
                        {
                            render_scale += 0.1f;
                        }
                        renderer.set_screen_size(window_width, window_height, render_scale);
                        std::cout << "Render scale changed to " << render_scale << std::endl;
                    }
                    break;
                    case SDLK_BACKQUOTE:
                    {
                        console_open = !console_open;
                    }
                    break;
                    case SDLK_e:
                    {
                        edit_mode = !edit_mode;
                    }
                    break;
                    case SDLK_f:
                    {
                        flashlight_on = !flashlight_on;
                    }
                    break;
                    case SDLK_g:
                    {
                        flashlight_follow = !flashlight_follow;
                    }
                    break;
                    case SDLK_r:
                    {
                        renderer.reload_programs();
                    }
                    break;
                    case SDLK_t:
                    {
                        if (time_scale > 0.25f)
                        {
                            time_scale = 0.25f;
                        }
                        else
                        {
                            time_scale = 1.0f;
                        }
                    }
                    break;
                    case SDLK_F4:
                    {
                        if (keys[SDL_SCANCODE_LALT])
                        {
                            quit = true;
                        }
                    }
                    break;
                    }
                }
            }
            break;
            case SDL_MOUSEMOTION:
            {
                if (!io.WantCaptureMouse)
                {
                    if (SDL_GetRelativeMouseMode())
                    {
                        camera->pitch -= event.motion.yrel * 0.1f;
                        camera->yaw += event.motion.xrel * 0.1f;
                        if (camera->pitch > 89.0f)
                        {
                            camera->pitch = 89.0f;
                        }
                        if (camera->pitch < -89.0f)
                        {
                            camera->pitch = -89.0f;
                        }
                    }
                }
            }
            break;
            case SDL_MOUSEWHEEL:
            {
                if (!io.WantCaptureMouse)
                {
                    if (SDL_GetRelativeMouseMode())
                    {
                        camera->fov -= event.wheel.y;
                        if (camera->fov <= 1.0f)
                        {
                            camera->fov = 1.0f;
                        }
                        if (camera->fov >= 120.0f)
                        {
                            camera->fov = 120.0f;
                        }
                    }
                }
            }
            break;
            }
        }

        glm::vec3 camera_front = camera->calc_front();
        glm::vec3 camera_right = camera->calc_right();

        static glm::vec3 velocity(0.0f, 0.0f, 0.0f);
        glm::vec3 acceleration(0.0f, 0.0f, 0.0f);
        const float speed = 50.0f;
        bool sprint = false;
        if (!io.WantCaptureKeyboard)
        {
            if (keys[SDL_SCANCODE_W])
            {
                acceleration += camera_front;
            }
            if (keys[SDL_SCANCODE_A])
            {
                acceleration -= camera_right;
            }
            if (keys[SDL_SCANCODE_S])
            {
                acceleration -= camera_front;
            }
            if (keys[SDL_SCANCODE_D])
            {
                acceleration += camera_right;
            }
            if (keys[SDL_SCANCODE_SPACE])
            {
                acceleration.y = 1.0f;
            }
            if (keys[SDL_SCANCODE_LCTRL])
            {
                acceleration.y = -1.0f;
            }
            if (keys[SDL_SCANCODE_LSHIFT])
            {
                sprint = true;
            }
        }
        float acceleration_length = glm::length(acceleration);
        if (acceleration_length > 1.0f)
        {
            acceleration /= acceleration_length;
        }
        acceleration *= speed * (sprint ? 2.0f : 1.0f);
        acceleration -= velocity * 10.0f;
        camera->position = 0.5f * acceleration * powf(delta_time, 2.0f) + velocity * delta_time + camera->position;
        velocity = acceleration * delta_time + velocity;
        // camera->pitch = -glm::dot(camera_front, velocity);
        camera->roll = glm::dot(camera_right, velocity);

        if (server)
        {
            pk::position_message *position_message = (pk::position_message *)malloc(sizeof(*position_message));
            position_message->type = pk::message_type::MESSAGE_POSITION_BROADCAST;
            position_message->id = client_id;
            position_message->x = camera->position.x;
            position_message->y = camera->position.y;
            position_message->z = camera->position.z;

            UDPpacket *packet = SDLNet_AllocPacket(PACKET_SIZE);
            for (int i = 0; i < MAX_CLIENTS; i++)
            {
                if (clients[i].id != -1 && clients[i].id != client_id)
                {
                    packet->address = clients[i].udp_address;
                    packet->data = (unsigned char *)position_message;
                    packet->len = sizeof(*position_message);

                    if (SDLNet_UDP_Send(udp_socket, -1, packet) != 1)
                    {
                        std::cerr << "Error: Failed to send UDP packet" << std::endl;
                    }
                }
            }
            SDLNet_FreePacket(packet);
        }
        else
        {
            pk::position_message *position_message = (pk::position_message *)malloc(sizeof(*position_message));
            position_message->type = pk::message_type::MESSAGE_POSITION_REQUEST;
            position_message->id = client_id;
            position_message->x = camera->position.x;
            position_message->y = camera->position.y;
            position_message->z = camera->position.z;

            UDPpacket *packet = SDLNet_AllocPacket(PACKET_SIZE);
            packet->address = server_address;
            packet->data = (unsigned char *)position_message;
            packet->len = sizeof(*position_message);

            if (!SDLNet_UDP_Send(udp_socket, -1, packet))
            {
                std::cerr << "Error: Failed to send UDP packet" << std::endl;
                return 1;
            }
            SDLNet_FreePacket(packet);
        }

        static float angle = 0.0f;
        const float pi = 3.14159f;
        const float distance = 6.0f;
        angle += 0.5f * delta_time;
        if (angle > 2 * pi)
        {
            angle = 0;
        }
        red_light->position.x = distance * sinf(angle);
        red_light->position.z = distance * cosf(angle);
        yellow_light->position.x = distance * sinf(angle + pi / 2);
        yellow_light->position.z = distance * cosf(angle + pi / 2);
        green_light->position.x = distance * sinf(angle + pi);
        green_light->position.z = distance * cosf(angle + pi);
        blue_light->position.x = distance * sinf(angle + 3 * pi / 2);
        blue_light->position.z = distance * cosf(angle + 3 * pi / 2);

        if (flashlight_follow)
        {
            flashlight->position = camera->position;
            flashlight->direction = glm::mix(flashlight->direction, camera_front, 30.0f * delta_time);
        }

        audio.set_listener(camera->position, camera_front, glm::vec3(0.0f, 1.0f, 0.0f));
        ambient_source->set_position(camera->position);
        shoot_source->set_position(camera->position);

        if (!io.WantCaptureMouse)
        {
            if (mouse & SDL_BUTTON(SDL_BUTTON_LEFT))
            {
                if (!shoot_source->is_playing())
                {
                    shoot_source->play(shoot_sound);
                }
            }

            if (mouse & SDL_BUTTON(SDL_BUTTON_RIGHT))
            {
                if (!bounce_source->is_playing())
                {
                    bounce_source->play(bounce_sound);
                }
            }
        }

        world->stepSimulation(delta_time);

        SDL_GL_MakeCurrent(window, context);

        renderer.wireframe = wireframe;
        renderer.camera = camera;
        renderer.skybox = skybox;
        renderer.objects.push_back(backpack);
        // renderer->objects.push_back(cube);
        for (int i = 0; i < MAX_CLIENTS; i++)
        {
            if (clients[i].id != -1 && clients[i].id != client_id)
            {
                btTransform transform;
                transform.setIdentity();
                transform.setOrigin(btVector3(clients[i].position.x, clients[i].position.y, clients[i].position.z));
                transform.setRotation(btQuaternion(0, 0, 0));
                clients[i].avatar->rigidbody->setWorldTransform(transform);

                renderer.objects.push_back(clients[i].avatar);
            }
        }
        renderer.directional_lights.push_back(sun);
        renderer.point_lights.push_back(red_light);
        renderer.point_lights.push_back(yellow_light);
        renderer.point_lights.push_back(green_light);
        renderer.point_lights.push_back(blue_light);
        if (flashlight_on)
        {
            renderer.spot_lights.push_back(flashlight);
        }
        renderer.terrains.push_back(terrain);
        renderer.waters.push_back(water);
        renderer.flush(current_time, delta_time);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame(window);
        ImGui::NewFrame();

        if (edit_mode)
        {
            ImGui::ShowDemoWindow();
        }

        if (console_open)
        {
            ImGui::Begin("Console", &console_open);

            static std::vector<std::string> messages;

            // TODO: command arguments
            char command[256] = {};
            if (ImGui::InputText("Input", command, sizeof(command), ImGuiInputTextFlags_EnterReturnsTrue))
            {
                if (strcmp(command, "help") == 0)
                {
                    messages.push_back("TODO: help");
                }
                else if (strcmp(command, "quit") == 0)
                {
                    quit = true;
                }
                else if (strcmp(command, "wireframe") == 0)
                {
                    wireframe = !wireframe;
                    messages.push_back("Wireframe " + wireframe ? "on" : "off");
                }
                else
                {
                    messages.push_back("Unknown command");
                }
            }

            ImGui::BeginChild("");
            for (auto &message : messages)
            {
                ImGui::Text(message.c_str());
            }
            ImGui::EndChild();

            ImGui::End();
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        SDL_GL_SwapWindow(window);
    }

    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (clients[i].id != -1 && clients[i].id != client_id)
        {
            delete clients[i].avatar;
        }
    }

    if (server)
    {
        // TODO: inform clients that server shut down
        for (int i = 0; i < MAX_CLIENTS; i++)
        {
            if (clients[i].id != -1 && clients[i].id != client_id)
            {
                SDLNet_TCP_DelSocket(socket_set, clients[i].socket);
                SDLNet_TCP_Close(clients[i].socket);
            }
        }
    }
    else
    {
        pk::message message = pk::message(pk::message_type::MESSAGE_DISCONNECT_REQUEST);
        SDLNet_TCP_Send(tcp_socket, &message, sizeof(message));
    }

    delete world;
    delete solver;
    delete overlapping_pair_cache;
    delete dispatcher;
    delete collision_configuration;

    delete camera;

    delete skybox;

    delete backpack_model;
    delete backpack;

    delete cube_model;
    delete cube;

    delete sun;

    delete red_light;
    delete yellow_light;
    delete green_light;
    delete blue_light;

    delete flashlight;

    delete water;

    delete terrain;

    delete ambient_source;
    delete bounce_source;
    delete shoot_source;

    delete ambient_sound;
    delete bounce_sound;
    delete shoot_sound;

    SDLNet_UDP_DelSocket(socket_set, udp_socket);
    SDLNet_TCP_DelSocket(socket_set, tcp_socket);

    SDLNet_FreeSocketSet(socket_set);

    SDLNet_UDP_Close(udp_socket);
    SDLNet_TCP_Close(tcp_socket);

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    Mix_CloseAudio();

    SDL_DestroyWindow(window);
    SDL_GL_DeleteContext(context);

    IMG_Quit();
    Mix_Quit();
    SDLNet_Quit();

    SDL_Quit();

    return 0;
}
