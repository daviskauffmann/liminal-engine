#include <game/game.hpp>

constexpr auto windowTitle = "Project Kilonova";
constexpr auto windowWidth = 1280;
constexpr auto windowHeight = 720;

constexpr auto renderScale = 1.0f;

constexpr auto shadowWidth = 4096;
constexpr auto shadowHeight = 4096;

constexpr auto fpsCap = 300;
constexpr auto frameDelay = 1000 / fpsCap;

int main(int argc, char *argv[])
{
    // parse command line arguments
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0)
        {
            std::cout << "Options:" << std::endl;
            std::cout << "  -h, --help\tPrint this message" << std::endl;
        }
    }

    // init SDL
    SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);
    Mix_Init(0);
    TTF_Init();
    SDLNet_Init();

    // create window
    SDL_Window *window = SDL_CreateWindow(
        windowTitle,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        windowWidth,
        windowHeight,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

    // create OpenGL context
    SDL_GLContext context = SDL_GL_CreateContext(window);

    // setup audio
    Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 1024);

    // setup renderer and audio
    Renderer renderer(windowWidth, windowHeight, renderScale, shadowWidth, shadowHeight);
    Audio audio;

    // create meshes
    float quadVertices[] = {
        // position           // normal            // uv
         1.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f, // top-right
         1.0f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f, // bottom-right
        -1.0f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f, // bottom-left
        -1.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f  // top-left
    };
    unsigned int quadIndices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };
    Mesh quadMesh(
        quadVertices,
        sizeof(quadVertices),
        quadIndices,
        sizeof(quadIndices));

    float cubeVertices[] = {
        // position           // normal            // uv
        // back face
        -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f, // bottom-left
         1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f, // top-right
         1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f, // bottom-right         
         1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f, // top-right
        -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f, // bottom-left
        -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f, // top-left
        // front face
        -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f, // bottom-left
         1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f, // bottom-right
         1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f, // top-right
         1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f, // top-right
        -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f, // top-left
        -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f, // bottom-left
        // left face
        -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f, // top-right
        -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f, // top-left
        -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f, // bottom-left
        -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f, // bottom-left
        -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f, // bottom-right
        -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f, // top-right
        // right face
         1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f, // top-left
         1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f, // bottom-right
         1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f, // top-right         
         1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f, // bottom-right
         1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f, // top-left
         1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f, // bottom-left     
        // bottom face
        -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f, // top-right
         1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f, // top-left
         1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f, // bottom-left
         1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f, // bottom-left
        -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f, // bottom-right
        -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f, // top-right
        // top face
        -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f, // top-left
         1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f, // bottom-right
         1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f, // top-right     
         1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f, // bottom-right
        -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f, // top-left
        -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f  // bottom-left   
    };
    unsigned int cubeIndices[] = {
        0, 1, 2,
        3, 4, 5,
        6, 7, 8,
        9, 10, 11,
        12, 13, 14,
        15, 16, 17,
        18, 19, 20,
        21, 22, 23,
        24, 25, 26,
        27, 28, 29,
        30, 31, 32,
        33, 34, 35 };
    Mesh cubeMesh(
        cubeVertices,
        sizeof(cubeVertices),
        cubeIndices,
        sizeof(cubeIndices));

    // create textures
    Texture defaultTexture("assets/images/default.png");
    Texture boxDiffuseTexture("assets/images/box_diffuse.png");
    Texture boxSpecularTexture("assets/images/box_specular.png");
    Texture cobbleDiffuseTexture("assets/images/cobble_diffuse.jpg");
    Texture cobbleSpecularTexture("assets/images/cobble_specular.jpg");
    Texture grassTexture("assets/images/grass.png");

    // create cubemaps
    Cubemap skyboxCubemap({
        "assets/images/sky/right.jpg",
        "assets/images/sky/left.jpg",
        "assets/images/sky/top.jpg",
        "assets/images/sky/bottom.jpg",
        "assets/images/sky/front.jpg",
        "assets/images/sky/back.jpg" });

    // create sounds
    Sound bounceSound("assets/audio/bounce.wav");
    Sound shootSound("assets/audio/shoot.wav");

    // create materials
    Material defaultMaterial(
        vec3{ 1.0f, 1.0f, 1.0f },
        &defaultTexture,
        &defaultTexture,
        16.0f,
        nullptr,
        nullptr,
        1.0f);
    Material boxMaterial(
        vec3{ 1.0f, 1.0f, 1.0f },
        &boxDiffuseTexture,
        &boxSpecularTexture,
        16.0f,
        nullptr,
        nullptr,
        1.0f);
    Material cobbleMaterial(
        vec3{ 1.0f, 1.0f, 1.0f },
        &cobbleDiffuseTexture,
        &cobbleSpecularTexture,
        16.0f,
        nullptr,
        nullptr,
        1.0f);

    // create objects
    Object floorObject(
        &cubeMesh,
        &cobbleMaterial,
        vec3{ 0.0f, -4.0f, 0.0f },
        vec3{ 0.0f, 0.0f, 0.0f },
        vec3{ 10.0f, 1.0f, 10.0f });
    Object box1Object(
        &cubeMesh,
        &defaultMaterial,
        vec3{ 0.0f, 1.0f, 0.0f },
        vec3{ 0.0f, 0.0f, 0.0f },
        vec3{ 1.0f, 1.0f, 1.0f });
    Object box2Object(
        &cubeMesh,
        &boxMaterial,
        vec3{ 2.0f, -0.5f, 0.0f },
        vec3{ 0.0f, 0.0f, 0.0f },
        vec3{ 0.5f, 0.5f, 0.5f });
    Object box3Object(
        &cubeMesh,
        &boxMaterial,
        vec3{ 0.0f, -0.5f, 2.0f },
        vec3{ 0.0f, 0.0f, 0.0f },
        vec3{ 0.5f, 0.5f, 0.5f });
    Object box4Object(
        &cubeMesh,
        &boxMaterial,
        vec3{ -2.0f, -0.5f, 0.0f },
        vec3{ 0.0f, 0.0f, 0.0f },
        vec3{ 0.5f, 0.5f, 0.5f });
    Object box5Object(
        &cubeMesh,
        &boxMaterial,
        vec3{ 0.0f, -0.5f, -2.0f },
        vec3{ 0.0f, 0.0f, 0.0f },
        vec3{ 0.5f, 0.5f, 0.5f });

    // create sun
    Sun sun(
        vec3{ -0.2f, -1.0f, -0.3f },
        vec3{ 0.1f, 0.1f, 0.1f },
        vec3{ 0.8f, 0.8f, 0.8f },
        vec3{ 1.0f, 1.0f, 1.0f });

    // create point lights
    PointLight redPointLight(
        vec3{ 2.0f, 0.0f, 2.0f },
        vec3{ 0.1f, 0.0f, 0.0f },
        vec3{ 0.8f, 0.0f, 0.0f },
        vec3{ 1.0f, 0.0f, 0.0f },
        vec3{ 1.0f, 0.09f, 0.32f });
    PointLight yellowPointLight(
        vec3{ -2.0f, 0.0f, -2.0f },
        vec3{ 0.1f, 0.1f, 0.0f },
        vec3{ 0.8f, 0.8f, 0.0f },
        vec3{ 1.0f, 1.0f, 0.0f },
        vec3{ 1.0f, 0.09f, 0.32f });
    PointLight greenPointLight(
        vec3{ 2.0f, 0.0f, -2.0f },
        vec3{ 0.0f, 0.1f, 0.0f },
        vec3{ 0.0f, 0.8f, 0.0f },
        vec3{ 0.0f, 1.0f, 0.0f },
        vec3{ 1.0f, 0.09f, 0.32f });
    PointLight bluePointLight(
        vec3{ -2.0f, 0.0f, 2.0f },
        vec3{ 0.0f, 0.0f, 0.1f },
        vec3{ 0.0f, 0.0f, 0.8f },
        vec3{ 0.0f, 0.0f, 1.0f },
        vec3{ 1.0f, 0.09f, 0.32f });

    // create spot lights
    SpotLight torchSpotLight(
        vec3{ 0.0f, 0.0f, 0.0f },
        vec3{ 0.0f, 0.0f, 0.0f },
        vec3{ 0.5f, 0.5f, 0.5f },
        vec3{ 1.0f, 1.0f, 1.0f },
        vec3{ 1.0f, 1.0f, 1.0f },
        vec3{ 1.0f, 0.09f, 0.32f },
        cosf(glm_rad(12.5f)),
        cosf(glm_rad(15.0f)));

    // create waters
    Water testWater(
        vec3{ 0.0f, -2.0f, 0.0f },
        vec3{ 100.0f, 100.0f });

    // create sprites
    Sprite grassSprite(
        vec3{ 1.0f, 1.0f, 1.0f },
        &grassTexture,
        vec2{ 0.0f, 0.0f },
        0.0f,
        vec2{ 1.0f, 1.0f });

    // create camera
    Camera mainCamera(
        vec3{ 0.0f, 0.0f, 3.0f },
        0.0f,
        -90.0f,
        0.0f,
        45.0f);

    // create sources
    Source originSource;
    Source cameraSource;

    // game settings
    unsigned int currentTime = 0;
    float fpsUpdateTimer = 0.0f;
    bool torch = true;
    float bounceTimer = 0.0f;
    float shootTimer = 0.0f;

    SDL_SetRelativeMouseMode(SDL_TRUE);
    SDL_GL_SetSwapInterval(0);
    renderer.setMode(RENDER_MODE_DEFERRED);

    // main loop
    bool quit = false;
    while (!quit)
    {
        // timer for fps cap
        unsigned int frameStart = SDL_GetTicks();

        // calculate time passed since last frame
        unsigned int previousTime = currentTime;
        currentTime = frameStart;

        // calculate delta time and fps
        float deltaTime = (currentTime - previousTime) / 1000.0f;
        unsigned int fps = (unsigned int)(1 / deltaTime);

        // update window title
        fpsUpdateTimer += deltaTime;

        if (fpsUpdateTimer > 0.25f)
        {
            fpsUpdateTimer = 0.0f;

            char title[256];
            sprintf_s(title, sizeof(title), "%s - FPS: %d", windowTitle, fps);
            SDL_SetWindowTitle(window, title);
        }

        // get keyboard input
        int numKeys;
        const unsigned char *keys = SDL_GetKeyboardState(&numKeys);

        // get mouse input
        int mouseX, mouseY;
        unsigned int mouse = SDL_GetMouseState(&mouseX, &mouseY);

        // handle events
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_KEYDOWN:
            {
                switch (event.key.keysym.sym)
                {
                case SDLK_F1:
                {
                    renderer.setMode(RENDER_MODE_FORWARD);
                }
                break;
                case SDLK_F2:
                {
                    renderer.setMode(RENDER_MODE_DEFERRED);
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
                case SDLK_f:
                {
                    torch = !torch;
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
                case SDLK_TAB:
                {
                    SDL_SetRelativeMouseMode((SDL_bool)!SDL_GetRelativeMouseMode());
                }
                break;
                }
            }
            break;
            case SDL_MOUSEMOTION:
            {
                if (SDL_GetRelativeMouseMode())
                {
                    // mouselook
                    mainCamera.pitch -= event.motion.yrel * 0.1f;
                    mainCamera.yaw += event.motion.xrel * 0.1f;

                    if (mainCamera.pitch > 89.0f)
                    {
                        mainCamera.pitch = 89.0f;
                    }
                    if (mainCamera.pitch < -89.0f)
                    {
                        mainCamera.pitch = -89.0f;
                    }
                }
            }
            break;
            case SDL_MOUSEWHEEL:
            {
                // zoom
                if (mainCamera.fov >= 1.0f && mainCamera.fov <= 90.0f)
                {
                    mainCamera.fov -= event.wheel.y;
                }
                if (mainCamera.fov <= 1.0f)
                {
                    mainCamera.fov = 1.0f;
                }
                if (mainCamera.fov >= 90.0f)
                {
                    mainCamera.fov = 90.0f;
                }
            }
            break;
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
                    int width = event.window.data1;
                    int height = event.window.data2;

                    SDL_SetWindowSize(window, width, height);

                    printf("Window resized to %dx%d\n", width, height);
                }
                break;
                }
            }
            break;
            }
        }

        // calculate movement speed
        float speed = 5.0f * deltaTime;

        // sprinting
        if (keys[SDL_SCANCODE_LSHIFT])
        {
            speed *= 2.0f;
        }

        // slow movement speed when moving diagonally
        if ((keys[SDL_SCANCODE_W] && keys[SDL_SCANCODE_D]) ||
            (keys[SDL_SCANCODE_W] && keys[SDL_SCANCODE_A]) ||
            (keys[SDL_SCANCODE_S] && keys[SDL_SCANCODE_D]) ||
            (keys[SDL_SCANCODE_S] && keys[SDL_SCANCODE_A]))
        {
            // precomputed 1 / sqrt(2)
            speed *= 0.71f;
        }

        // calculate camera vectors
        vec3 mainCameraFront;
        mainCamera.calcFront(&mainCameraFront);

        vec3 mainCameraUp;
        mainCamera.calcUp(&mainCameraUp);

        // move forward
        if (keys[SDL_SCANCODE_W])
        {
            vec3 movement;
            glm_vec_scale(mainCameraFront, speed, movement);
            glm_vec_add(mainCamera.position, movement, mainCamera.position);
        }

        // strafe left
        if (keys[SDL_SCANCODE_A])
        {
            vec3 direction;
            glm_cross(mainCameraFront, mainCameraUp, direction);
            glm_normalize(direction);

            vec3 movement;
            glm_vec_scale(direction, -speed, movement);
            glm_vec_add(mainCamera.position, movement, mainCamera.position);
        }

        // move backward
        if (keys[SDL_SCANCODE_S])
        {
            vec3 movement;
            glm_vec_scale(mainCameraFront, -speed, movement);
            glm_vec_add(mainCamera.position, movement, mainCamera.position);
        }

        // strafe right
        if (keys[SDL_SCANCODE_D])
        {
            vec3 direction;
            glm_cross(mainCameraFront, mainCameraUp, direction);
            glm_normalize(direction);

            vec3 movement;
            glm_vec_scale(direction, speed, movement);
            glm_vec_add(mainCamera.position, movement, mainCamera.position);
        }

        if (keys[SDL_SCANCODE_SPACE])
        {
            vec3 movement;
            glm_vec_scale(mainCameraUp, speed, movement);
            glm_vec_add(mainCamera.position, movement, mainCamera.position);
        }

        if (keys[SDL_SCANCODE_LCTRL])
        {
            vec3 movement;
            glm_vec_scale(mainCameraUp, -speed, movement);
            glm_vec_add(mainCamera.position, movement, mainCamera.position);
        }

        // calculate angle for rotating stuff
        float angle = currentTime * 0.001f;
        float sinAngle = sinf(angle);
        float cosAngle = cosf(angle);

        // update objects
        box1Object.rotation[0] = sinAngle;
        box1Object.rotation[1] = cosAngle;

        // update lights
        sun.direction[0] = sinAngle;
        sun.direction[2] = cosAngle;

        glm_vec_copy(mainCamera.position, torchSpotLight.position);
        glm_vec_copy(mainCameraFront, torchSpotLight.direction);

        // update audio
        vec3 mainCameraVelocity = GLM_VEC3_ZERO_INIT;
        glm_vec_scale(mainCameraFront, speed, mainCameraVelocity);
        vec3 mainCameraOrientation[2];
        glm_vec_copy(mainCameraFront, mainCameraOrientation[0]);
        glm_vec_copy(mainCameraUp, mainCameraOrientation[1]);

        audio.setListener(mainCamera.position, mainCameraVelocity, mainCameraOrientation);

        // update sources
        cameraSource.setPosition(mainCamera.position);

        // shooting
        shootTimer += deltaTime;

        if (mouse & SDL_BUTTON(SDL_BUTTON_LEFT))
        {
            if (shootTimer >= 0.25f)
            {
                shootTimer = 0.0f;

                cameraSource.play(&shootSound);
            }
        }

        // 3d audio test
        bounceTimer += deltaTime;

        if (mouse & SDL_BUTTON(SDL_BUTTON_RIGHT))
        {
            if (bounceTimer >= 0.25f)
            {
                bounceTimer = 0.0f;

                originSource.play(&bounceSound);
            }
        }

        // setup renderer
        renderer.addObject(&floorObject);
        renderer.addObject(&box1Object);
        renderer.addObject(&box2Object);
        renderer.addObject(&box3Object);
        renderer.addObject(&box4Object);
        renderer.addObject(&box5Object);

        renderer.setSun(&sun);

        renderer.addPointLight(&redPointLight);
        renderer.addPointLight(&yellowPointLight);
        renderer.addPointLight(&greenPointLight);
        renderer.addPointLight(&bluePointLight);

        if (torch)
        {
            renderer.addSpotLight(&torchSpotLight);
        }

        renderer.setSkybox(&skyboxCubemap);

        renderer.addWater(&testWater);

        renderer.addSprite(&grassSprite);

        // render everything
        renderer.draw(&mainCamera, (float)windowWidth / (float)windowHeight, SDL_GetTicks(), deltaTime);

        // display the window
        SDL_GL_SwapWindow(window);

        // apply fps cap
        unsigned int frameEnd = SDL_GetTicks();
        unsigned int frameTime = frameEnd - frameStart;

        if (frameDelay > frameTime)
        {
            SDL_Delay(frameDelay - frameTime);
        }
    }

    // close audio
    Mix_CloseAudio();

    // close window
    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);

    // close SDL
    SDLNet_Quit();
    TTF_Quit();
    Mix_Quit();
    IMG_Quit();
    SDL_Quit();

    return 0;
}
