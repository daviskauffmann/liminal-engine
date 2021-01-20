#include "client.hpp"

#include <bullet/btBulletDynamicsCommon.h>
#include <iostream>
#include <imgui.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_opengl3.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_net.h>
#include <spdlog/spdlog.h>

#include "audio.hpp"
#include "directional_light.hpp"
#include "camera.hpp"
#include "message.hpp"
#include "model.hpp"
#include "object.hpp"
#include "point_light.hpp"
#include "renderer.hpp"
#include "server.hpp"
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

#define WINDOW_TITLE "Project Kilonova"

struct player
{
    int client_id;
    pk::object *avatar;
};

int pk::client_main(cxxopts::ParseResult result)
{
    if (SDL_Init(SDL_FLAGS) != 0)
    {
        spdlog::error("Failed to initialize SDL: {}", SDL_GetError());
        return 1;
    }

    if (IMG_Init(IMG_FLAGS) != IMG_FLAGS)
    {
        spdlog::error("Failed to initialize SDL_image: {}", IMG_GetError());
        return 1;
    }

    if (Mix_Init(MIX_FLAGS) != MIX_FLAGS)
    {
        spdlog::error("Failed to initialize SDL_mixer: {}", Mix_GetError());
        return 1;
    }

    if (SDLNet_Init() != 0)
    {
        spdlog::error("Failed to initialize SDL_net: {}", SDLNet_GetError());
        return 1;
    }

    int window_width = result["width"].as<int>();
    int window_height = result["height"].as<int>();
    int render_scale = glm::clamp(result["scale"].as<float>(), 0.1f, 1.0f);

    SDL_Window *window = SDL_CreateWindow(
        WINDOW_TITLE,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        window_width,
        window_height,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if (!window)
    {
        spdlog::error("Failed to create window: {}", SDL_GetError());
        return 1;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetSwapInterval(0);
    SDL_GLContext context = SDL_GL_CreateContext(window);
    if (!context)
    {
        spdlog::error("Failed to create OpenGL context: {}", SDL_GetError());
        return 1;
    }

    GLenum error = glewInit();
    if (error != GLEW_OK)
    {
        spdlog::error("Failed to initialize GLEW: {}", glewGetErrorString(error));
        return 1;
    }

    ImGui::CreateContext();
    ImGui_ImplSDL2_InitForOpenGL(window, context);
    ImGui_ImplOpenGL3_Init("#version 460");
    ImGuiIO &io = ImGui::GetIO();
    io.IniFilename = "assets/imgui.ini";

    if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 1024) != 0)
    {
        spdlog::error("Failed to initialize the mixer API: {}", Mix_GetError());
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

    pk::skybox *skybox = nullptr;
    // pk::skybox *skybox = new pk::skybox("assets/images/Circus_Backstage_8k.jpg");
    // pk::skybox *skybox = new pk::skybox("assets/images/GCanyon_C_YumaPoint_8k.jpg");

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

    bool online = true;

    IPaddress server_address;
    std::string server_host = result["host"].as<std::string>();
    unsigned short server_port = result["port"].as<unsigned short>();
    if (SDLNet_ResolveHost(&server_address, server_host.c_str(), server_port))
    {
        spdlog::error("Failed to resolve host: {}", SDLNet_GetError());
        online = false;
    }

    TCPsocket tcp_socket = SDLNet_TCP_Open(&server_address);
    if (!tcp_socket)
    {
        spdlog::error("Failed to open TCP socket: {}", SDLNet_GetError());
        online = false;
    }

    UDPsocket udp_socket = SDLNet_UDP_Open(0);
    if (!udp_socket)
    {
        spdlog::error("Failed to open UDP socket: {}", SDLNet_GetError());
        return 1;
    }

    SDLNet_SocketSet socket_set = SDLNet_AllocSocketSet(2);
    if (!socket_set)
    {
        spdlog::error("Failed to allocate socket set: {}", SDLNet_GetError());
        return 1;
    }
    SDLNet_TCP_AddSocket(socket_set, tcp_socket);
    SDLNet_UDP_AddSocket(socket_set, udp_socket);

    player players[MAX_CLIENTS];
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        players[i].client_id = -1;
    }

    int client_id = -1;

    if (online)
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

                for (int i = 0; i < MAX_CLIENTS; i++)
                {
                    players[i].client_id = connect_ok_message->clients[i].id;

                    if (players[i].client_id != -1 && players[i].client_id != client_id)
                    {
                        players[connect_ok_message->clients[i].id].avatar = new pk::object(
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
                spdlog::error("Server is full");
                online = false;
            }
            break;
            default:
            {
                spdlog::error("Unknown server response");
                online = false;
            }
            break;
            }
        }
    }

    if (online)
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
            spdlog::error("Failed to make UDP connection");
            online = false;
        }
        SDLNet_FreePacket(packet);
    }

    if (!online)
    {
        spdlog::info("Starting in offline mode");
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

        if (online)
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

                            players[connect_broadcast_message->id].client_id = connect_broadcast_message->id;
                            players[connect_broadcast_message->id].avatar = new pk::object(
                                cube_model,
                                glm::vec3(connect_broadcast_message->client.x, connect_broadcast_message->client.y, connect_broadcast_message->client.z),
                                glm::vec3(0.0f, 0.0f, 0.0f),
                                glm::vec3(1.0f, 1.0f, 1.0f),
                                1.0f);

                            spdlog::info("Client with ID {} has joined", connect_broadcast_message->id);
                        }
                        break;
                        case pk::message_type::MESSAGE_DISCONNECT_BROADCAST:
                        {
                            pk::id_message *id_message = (pk::id_message *)message;

                            players[id_message->id].client_id = -1;
                            delete players[id_message->id].avatar;
                            players[id_message->id].avatar = nullptr;

                            spdlog::info("Client with ID {} has disconnected", id_message->id);
                        }
                        break;
                        case pk::message_type::MESSAGE_CHAT_BROADCAST:
                        {
                            pk::chat_message *chat_message = (pk::chat_message *)message;

                            spdlog::info("Client {}: {}", chat_message->id, chat_message->str);
                        }
                        break;
                        default:
                        {
                            spdlog::error("Unknown TCP packet type: {}", message->type);
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

                            if (players[position_message->id].client_id != -1)
                            {
                                btTransform transform;
                                transform.setIdentity();
                                transform.setOrigin(btVector3(position_message->x, position_message->y, position_message->z));
                                transform.setRotation(btQuaternion(0, 0, 0));
                                players[position_message->id].avatar->rigidbody->setWorldTransform(transform);
                            }
                        }
                        break;
                        default:
                        {
                            spdlog::error("Unknown UDP packet type: {}", message->type);
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
                    spdlog::info("Window resized to {}x{}", window_width, window_height);
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
                        spdlog::info("Render scale changed to {}", render_scale);
                    }
                    break;
                    case SDLK_EQUALS:
                    {
                        if (render_scale < 1.0f)
                        {
                            render_scale += 0.1f;
                        }
                        renderer.set_screen_size(window_width, window_height, render_scale);
                        spdlog::info("Render scale changed to {}", render_scale);
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
                spdlog::error("Failed to send UDP packet");
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
            if (players[i].client_id != -1 && players[i].client_id != client_id)
            {
                renderer.objects.push_back(players[i].avatar);
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
        if (players[i].client_id != -1 && players[i].client_id != client_id)
        {
            delete players[i].avatar;
        }
    }

    if (online)
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
