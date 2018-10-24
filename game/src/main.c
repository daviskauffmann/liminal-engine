#include <engine/engine.h>

#define WINDOW_TITLE "Example Game"
#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080

#define RENDER_SCALE 0.5f

#define SHADOW_WIDTH 4096
#define SHADOW_HEIGHT 4096

#define FPS_CAP 120

enum lighting
{
    LIGHTING_DIFFUSE,
    LIGHTING_PHONG
};

int main(int argc, char *argv[])
{
    // setup engine
    if (core_init())
    {
        return 1;
    }

    if (window_init(WINDOW_TITLE, WINDOW_WIDTH, WINDOW_HEIGHT))
    {
        return 1;
    }

    if (audio_init(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 1024))
    {
        return 1;
    }

    if (image_init())
    {
        return 1;
    }

    time_cap_fps(FPS_CAP);

    window_set_fullscreen(SDL_WINDOW_FULLSCREEN_DESKTOP);

    SDL_SetRelativeMouseMode(true);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    // create programs
    struct program *diffuse_program = program_create(
        "assets/shaders/diffuse.vert",
        "assets/shaders/diffuse.frag");

    if (!diffuse_program)
    {
        return 1;
    }

    struct program *phong_program = program_create(
        "assets/shaders/phong.vert",
        "assets/shaders/phong.frag");

    if (!phong_program)
    {
        return 1;
    }

    struct program *geometry_program = program_create(
        "assets/shaders/geometry.vert",
        "assets/shaders/geometry.frag");

    if (!geometry_program)
    {
        return 1;
    }

    struct program *post_program = program_create(
        "assets/shaders/post.vert",
        "assets/shaders/post.frag");

    if (!post_program)
    {
        return 1;
    }

    struct program *depth_program = program_create(
        "assets/shaders/depth.vert",
        "assets/shaders/depth.frag");

    if (!depth_program)
    {
        return 1;
    }

    // create meshes
    float quad_vertices[] = {
        // position          // normal            // uv
        +1.0f, +1.0f, +0.0f, +0.0f, +1.0f, +0.0f, 1.0f, 1.0f, // top right
        +1.0f, -1.0f, +0.0f, +0.0f, +1.0f, +0.0f, 1.0f, 0.0f, // bottom right
        -1.0f, -1.0f, +0.0f, +0.0f, +1.0f, +0.0f, 0.0f, 0.0f, // bottom left
        -1.0f, +1.0f, +0.0f, +0.0f, +1.0f, +0.0f, 0.0f, 1.0f  // top left
    };

    unsigned int quad_indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

    struct mesh *quad_mesh = mesh_create(
        quad_vertices,
        sizeof(quad_vertices),
        quad_indices,
        sizeof(quad_indices));

    if (!quad_mesh)
    {
        return 1;
    }

    float cube_vertices[] = {
        // position          // normal            // uv
        // back face
        -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
         1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
         1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
         1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
        -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
        -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
        // front face
        -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
         1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
         1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
         1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
        -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
        -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
        // left face
        -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
        -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
        -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
        -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
        -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
        -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
        // right face
         1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
         1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
         1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
         1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
         1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
         1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
        // bottom face
        -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
         1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
         1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
         1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
        -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
        -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
        // top face
        -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
         1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
         1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
         1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
        -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
        -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left   
    };

    unsigned int cube_indices[] = {
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

    struct mesh *cube_mesh = mesh_create(
        cube_vertices,
        sizeof(cube_vertices),
        cube_indices,
        sizeof(cube_indices));

    if (!cube_mesh)
    {
        return 1;
    }

    // create textures
    struct texture *box_diffuse_texture = texture_create("assets/images/box_diffuse.png");

    if (!box_diffuse_texture)
    {
        return 1;
    }

    struct texture *box_specular_texture = texture_create("assets/images/box_specular.png");

    if (!box_specular_texture)
    {
        return 1;
    }

    struct texture *cobble_diffuse_texture = texture_create("assets/images/cobble_diffuse.jpg");

    if (!cobble_diffuse_texture)
    {
        return 1;
    }

    struct texture *cobble_specular_texture = texture_create("assets/images/cobble_specular.jpg");

    if (!cobble_specular_texture)
    {
        return 1;
    }

    // create materials
    vec3 box_material_color = { 1.0f, 1.0f, 1.0f };

    struct material *box_material = material_create(
        box_diffuse_texture,
        box_specular_texture,
        NULL,
        NULL,
        box_material_color,
        32.0f,
        1.0f);

    if (!box_material)
    {
        return 1;
    }

    vec3 cobble_material_color = { 1.0f, 1.0f, 1.0f };

    struct material *cobble_material = material_create(
        cobble_diffuse_texture,
        cobble_specular_texture,
        NULL,
        NULL,
        cobble_material_color,
        32.0f,
        1.0f);

    if (!cobble_material)
    {
        return 1;
    }

    // create objects
    vec3 floor_object_position = { 0.0f, -2.0f, 0.0f };
    vec3 floor_object_rotation = { 0.0f, 0.0f, 0.0f };
    vec3 floor_object_scale = { 10.0f, 1.0f, 10.0f };

    struct object *floor_object = object_create(
        cube_mesh,
        cobble_material,
        floor_object_position,
        floor_object_rotation,
        floor_object_scale);

    if (!floor_object)
    {
        return 1;
    }

    vec3 box_1_object_position = { 0.0f, 1.0f, 0.0f };
    vec3 box_1_object_rotation = { 0.0f, 0.0f, 0.0f };
    vec3 box_1_object_scale = { 1.0f, 1.0f, 1.0f };

    struct object *box_1_object = object_create(
        cube_mesh,
        box_material,
        box_1_object_position,
        box_1_object_rotation,
        box_1_object_scale);

    if (!box_1_object)
    {
        return 1;
    }

    vec3 box_2_object_position = { 2.0f, -0.5f, 0.0f };
    vec3 box_2_object_rotation = { 0.0f, 0.0f, 0.0f };
    vec3 box_2_object_scale = { 0.5f, 0.5f, 0.5f };

    struct object *box_2_object = object_create(
        cube_mesh,
        box_material,
        box_2_object_position,
        box_2_object_rotation,
        box_2_object_scale);

    if (!box_2_object)
    {
        return 1;
    }

    vec3 box_3_object_position = { 0.0f, -0.5f, 2.0f };
    vec3 box_3_object_rotation = { 0.0f, 0.0f, 0.0f };
    vec3 box_3_object_scale = { 0.5f, 0.5f, 0.5f };

    struct object *box_3_object = object_create(
        cube_mesh,
        box_material,
        box_3_object_position,
        box_3_object_rotation,
        box_3_object_scale);

    if (!box_3_object)
    {
        return 1;
    }

    vec3 box_4_object_position = { -2.0f, -0.5f, 0.0f };
    vec3 box_4_object_rotation = { 0.0f, 0.0f, 0.0f };
    vec3 box_4_object_scale = { 0.5f, 0.5f, 0.5f };

    struct object *box_4_object = object_create(
        cube_mesh,
        box_material,
        box_4_object_position,
        box_4_object_rotation,
        box_4_object_scale);

    if (!box_4_object)
    {
        return 1;
    }

    vec3 box_5_object_position = { 0.0f, -0.5f, -2.0f };
    vec3 box_5_object_rotation = { 0.0f, 0.0f, 0.0f };
    vec3 box_5_object_scale = { 0.5f, 0.5f, 0.5f };

    struct object *box_5_object = object_create(
        cube_mesh,
        box_material,
        box_5_object_position,
        box_5_object_rotation,
        box_5_object_scale);

    if (!box_5_object)
    {
        return 1;
    }

    struct object *objects[] = {
        floor_object,
        box_1_object,
        box_2_object,
        box_3_object,
        box_4_object,
        box_5_object
    };
    const unsigned int num_objects = sizeof(objects) / sizeof(struct object *);

    // create lights
    vec3 scene_ambient = { 0.1f, 0.1f, 0.1f };

    vec3 directional_light_direction = { -0.2f, -1.0f, -0.3f };
    vec3 directional_light_ambient = { 0.05f, 0.05f, 0.05f };
    vec3 directional_light_diffuse = { 0.4f, 0.4f, 0.4f };
    vec3 directional_light_specular = { 1.0f, 1.0f, 1.0f };

    struct directional_light *directional_light = directional_light_create(
        directional_light_direction,
        directional_light_ambient,
        directional_light_diffuse,
        directional_light_specular);

    if (!directional_light)
    {
        return 1;
    }

    vec3 red_point_light_position = { 2.0f, 0.0f, 2.0f };
    vec3 red_point_light_ambient = { 0.1f, 0.0f, 0.0f };
    vec3 red_point_light_diffuse = { 0.8f, 0.0f, 0.0f };
    vec3 red_point_light_specular = { 1.0f, 0.0f, 0.0f };

    struct point_light *red_point_light = point_light_create(
        red_point_light_position,
        red_point_light_ambient,
        red_point_light_diffuse,
        red_point_light_specular,
        1.0f,
        0.09f,
        0.032f);

    if (!red_point_light)
    {
        return 1;
    }

    vec3 yellow_point_light_position = { -2.0f, 0.0f, -2.0f };
    vec3 yellow_point_light_ambient = { 0.1f, 0.1f, 0.0f };
    vec3 yellow_point_light_diffuse = { 0.8f, 0.8f, 0.0f };
    vec3 yellow_point_light_specular = { 1.0f, 1.0f, 0.0f };

    struct point_light *yellow_point_light = point_light_create(
        yellow_point_light_position,
        yellow_point_light_ambient,
        yellow_point_light_diffuse,
        yellow_point_light_specular,
        1.0f,
        0.09f,
        0.032f);
    if (!yellow_point_light)
    {
        return 1;
    }

    vec3 green_point_light_position = { 2.0f, 0.0f, -2.0f };
    vec3 green_point_light_ambient = { 0.0f, 0.1f, 0.0f };
    vec3 green_point_light_diffuse = { 0.0f, 0.8f, 0.0f };
    vec3 green_point_light_specular = { 0.0f, 1.0f, 0.0f };

    struct point_light *green_point_light = point_light_create(
        green_point_light_position,
        green_point_light_ambient,
        green_point_light_diffuse,
        green_point_light_specular,
        1.0f,
        0.09f,
        0.032f);

    if (!green_point_light)
    {
        return 1;
    }

    vec3 blue_point_light_position = { -2.0f, 0.0f, 2.0f };
    vec3 blue_point_light_ambient = { 0.0f, 0.0f, 0.1f };
    vec3 blue_point_light_diffuse = { 0.0f, 0.0f, 0.8f };
    vec3 blue_point_light_specular = { 0.0f, 0.0f, 1.0f };

    struct point_light *blue_point_light = point_light_create(
        blue_point_light_position,
        blue_point_light_ambient,
        blue_point_light_diffuse,
        blue_point_light_specular,
        1.0f,
        0.09f,
        0.032f);

    if (!blue_point_light)
    {
        return 1;
    }

    struct point_light *point_lights[] = {
        red_point_light,
        yellow_point_light,
        green_point_light,
        blue_point_light };
    const unsigned int num_point_lights = sizeof(point_lights) / sizeof(struct point_light *);

    vec3 spot_light_position = { 0.0f, 0.0f, 0.0f };
    vec3 spot_light_direction = { 0.0f, 0.0f, 0.0f };
    vec3 spot_light_ambient = { 0.0f, 0.0f, 0.0f };
    vec3 spot_light_diffuse = { 0.0f, 0.0f, 0.0f };
    vec3 spot_light_specular = { 0.0f, 0.0f, 0.0f };

    struct spot_light *spot_light = spot_light_create(
        spot_light_position,
        spot_light_direction,
        spot_light_ambient,
        spot_light_diffuse,
        spot_light_specular,
        1.0f,
        0.09f,
        0.032f,
        cosf(glm_rad(12.5f)),
        cosf(glm_rad(15.0f)));

    if (!spot_light)
    {
        return 1;
    }

    // create camera
    vec3 camera_position = { 0.0f, 0.0f, 3.0f };
    vec3 camera_front = { 0.0f, 0.0f, -1.0f };
    vec3 camera_up = { 0.0f, 1.0f, 0.0f };

    struct camera *camera = camera_create(
        camera_position,
        camera_front,
        camera_up,
        0.0f,
        -90.0f,
        0.0f,
        45.0f);

    if (!camera)
    {
        return 1;
    }

    // load music
    Mix_Music *background_music = Mix_LoadMUS("assets/audio/background.mp3");

    if (!background_music)
    {
        error(Mix_GetError());

        return 1;
    }

    // load sounds
    Mix_Chunk *shoot_sound = Mix_LoadWAV("assets/audio/shoot.wav");

    if (!shoot_sound)
    {
        error(Mix_GetError());

        return 1;
    }

    // setup screen fbo
    GLuint screen_fbo;
    glGenFramebuffers(1, &screen_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, screen_fbo);

    GLuint screen_texture;
    glGenTextures(1, &screen_texture);
    glBindTexture(GL_TEXTURE_2D, screen_texture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screen_texture, 0);

    glBindTexture(GL_TEXTURE_2D, 0);

    GLuint screen_rbo;
    glGenRenderbuffers(1, &screen_rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, screen_rbo);

    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, WINDOW_WIDTH, WINDOW_HEIGHT);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, screen_rbo);

    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        error("Couldn't complete screen framebuffer");

        return 1;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // setup depthmap fbo
    GLuint depthmap_fbo;
    glGenFramebuffers(1, &depthmap_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, depthmap_fbo);

    GLuint depthmap_texture;
    glGenTextures(1, &depthmap_texture);
    glBindTexture(GL_TEXTURE_2D, depthmap_texture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, (GLfloat[4]) { 1.0f, 1.0f, 1.0f, 1.0f });

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthmap_texture, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    glBindTexture(GL_TEXTURE_2D, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        error("Couldn't complete depthmap framebuffer");

        return 1;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // setup geometry fbo
    GLuint geometry_fbo;
    glGenFramebuffers(1, &geometry_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, geometry_fbo);

    GLuint geometry_position_texture;
    glGenTextures(1, &geometry_position_texture);
    glBindTexture(GL_TEXTURE_2D, geometry_position_texture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, WINDOW_WIDTH * RENDER_SCALE, WINDOW_HEIGHT * RENDER_SCALE, 0, GL_RGB, GL_FLOAT, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, geometry_position_texture, 0);

    glBindTexture(GL_TEXTURE_2D, 0);

    GLuint geometry_normal_texture;
    glGenTextures(1, &geometry_normal_texture);
    glBindTexture(GL_TEXTURE_2D, geometry_normal_texture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, WINDOW_WIDTH * RENDER_SCALE, WINDOW_HEIGHT * RENDER_SCALE, 0, GL_RGB, GL_FLOAT, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, geometry_normal_texture, 0);

    glBindTexture(GL_TEXTURE_2D, 0);

    GLuint geometry_albedo_texture;
    glGenTextures(1, &geometry_albedo_texture);
    glBindTexture(GL_TEXTURE_2D, geometry_albedo_texture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WINDOW_WIDTH * RENDER_SCALE, WINDOW_HEIGHT * RENDER_SCALE, 0, GL_RGB, GL_FLOAT, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, geometry_albedo_texture, 0);

    glBindTexture(GL_TEXTURE_2D, 0);

    GLuint geometry_specular_texture;
    glGenTextures(1, &geometry_specular_texture);
    glBindTexture(GL_TEXTURE_2D, geometry_specular_texture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WINDOW_WIDTH * RENDER_SCALE, WINDOW_HEIGHT * RENDER_SCALE, 0, GL_RGB, GL_FLOAT, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, geometry_specular_texture, 0);

    glBindTexture(GL_TEXTURE_2D, 0);

    GLuint geometry_emission_texture;
    glGenTextures(1, &geometry_emission_texture);
    glBindTexture(GL_TEXTURE_2D, geometry_emission_texture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WINDOW_WIDTH * RENDER_SCALE, WINDOW_HEIGHT * RENDER_SCALE, 0, GL_RGB, GL_FLOAT, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, geometry_emission_texture, 0);

    glDrawBuffers(5, (GLenum[5]) { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4 });

    GLuint geometry_rbo;
    glGenRenderbuffers(1, &geometry_rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, geometry_rbo);

    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, WINDOW_WIDTH * RENDER_SCALE, WINDOW_HEIGHT * RENDER_SCALE);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, geometry_rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        error("Couldn't complete geometry framebuffer");

        return 1;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // used for switching shaders
    enum lighting lighting = LIGHTING_PHONG;

    // main loop
    bool quit = false;
    while (!quit)
    {
        // start of frame activities
        time_frame_start();

        // update window title
        char title[256];
        sprintf_s(title, sizeof(title), "%s - FPS: %d", WINDOW_TITLE, time_fps());
        window_set_title(title);

        // get keyboard input
        int num_keys;
        const unsigned char *keys = SDL_GetKeyboardState(&num_keys);

        // get mouse input
        int mouse_x, mouse_y;
        unsigned int mouse = SDL_GetMouseState(&mouse_x, &mouse_y);

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
                case SDLK_1:
                {
                    if (Mix_PlayingMusic())
                    {
                        Mix_HaltMusic();
                    }
                    else
                    {
                        Mix_PlayMusic(background_music, -1);
                    }
                }
                break;
                case SDLK_2:
                {
                    if (Mix_PlayingMusic())
                    {
                        if (Mix_PausedMusic())
                        {
                            Mix_ResumeMusic();
                        }
                        else
                        {
                            Mix_PauseMusic();
                        }
                    }
                }
                break;
                case SDLK_F1:
                {
                    lighting = LIGHTING_DIFFUSE;
                }
                break;
                case SDLK_F2:
                {
                    lighting = LIGHTING_PHONG;
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
                case SDLK_RETURN:
                {
                    if (keys[SDL_SCANCODE_LALT])
                    {
                        unsigned int flags = window_get_flags();

                        if (flags & SDL_WINDOW_FULLSCREEN_DESKTOP)
                        {
                            window_set_fullscreen(0);
                        }
                        else
                        {
                            window_set_fullscreen(SDL_WINDOW_FULLSCREEN_DESKTOP);
                        }
                    }
                }
                break;
                case SDLK_TAB:
                {
                    SDL_SetRelativeMouseMode(!SDL_GetRelativeMouseMode());
                }
                break;
                }
            }
            break;
            case SDL_MOUSEMOTION:
            {
                // mouselook
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

                vec3 front = {
                    cosf(glm_rad(camera->yaw)) * cosf(glm_rad(camera->pitch)),
                    sinf(glm_rad(camera->pitch)),
                    sinf(glm_rad(camera->yaw)) * cosf(glm_rad(camera->pitch)) };
                glm_normalize(front);
                glm_vec_copy(front, camera->front);
            }
            break;
            case SDL_MOUSEWHEEL:
            {
                // zoom
                if (camera->fov >= 1.0f && camera->fov <= 90.0f)
                {
                    camera->fov -= event.wheel.y;
                }
                if (camera->fov <= 1.0f)
                {
                    camera->fov = 1.0f;
                }
                if (camera->fov >= 90.0f)
                {
                    camera->fov = 90.0f;
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

                    window_resize(width, height);
                }
                break;
                }
            }
            break;
            }
        }

        // calculate movement speed
        float speed = 5.0f * time_delta();

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

        // move forward
        if (keys[SDL_SCANCODE_W])
        {
            vec3 movement;
            glm_vec_scale(camera->front, speed, movement);
            glm_vec_add(camera->position, movement, camera->position);
        }

        // strafe left
        if (keys[SDL_SCANCODE_A])
        {
            vec3 direction;
            glm_cross(camera->front, camera->up, direction);
            glm_normalize(direction);
            vec3 movement;
            glm_vec_scale(direction, -speed, movement);
            glm_vec_add(camera->position, movement, camera->position);
        }

        // move backward
        if (keys[SDL_SCANCODE_S])
        {
            vec3 movement;
            glm_vec_scale(camera->front, -speed, movement);
            glm_vec_add(camera->position, movement, camera->position);
        }

        // strafe right
        if (keys[SDL_SCANCODE_D])
        {
            vec3 direction;
            glm_cross(camera->front, camera->up, direction);
            glm_normalize(direction);
            vec3 movement;
            glm_vec_scale(direction, speed, movement);
            glm_vec_add(camera->position, movement, camera->position);
        }

        // shooting
        static float shoot_timer = 0.0f;
        shoot_timer += time_delta();
        if (mouse & SDL_BUTTON(SDL_BUTTON_LEFT))
        {
            if (shoot_timer >= 0.25f)
            {
                shoot_timer = 0.0f;

                Mix_PlayChannel(-1, shoot_sound, 0);
            }
        }

        // update objects
        float angle = time_current() * 0.001f;
        objects[1]->rotation[0] = angle;
        objects[1]->rotation[1] = angle;
        objects[1]->rotation[2] = angle;

        directional_light->direction[0] = sinf(angle);
        directional_light->direction[2] = cosf(angle);

        // update lights
        glm_vec_copy(camera->position, spot_light->position);
        glm_vec_copy(camera->front, spot_light->direction);

        // calculate camera projection matrix
        mat4 camera_projection;
        camera_calc_perspective(camera, camera_projection);

        // calculate camera view matrix
        mat4 camera_view;
        camera_calc_view(camera, camera_view);

        // calculate sun projection matrix
        mat4 sun_projection;
        glm_ortho(-10.0f, 10.0f, -10.0f, 10.0f, -10.0f, 10.0f, sun_projection);

        // calculate sun view matrix
        // use the directional light to provide position/direction
        vec3 directional_light_position;
        glm_vec_sub((vec3) { 0.0f, 0.0f, 0.0f }, directional_light->direction, directional_light_position);
        mat4 sun_view;
        glm_lookat(directional_light_position, (vec3) { 0.0f, 0.0f, 0.0f }, (vec3) { 0.0f, 1.0f, 0.0f }, sun_view);

        // bind geometry fbo
        glBindFramebuffer(GL_FRAMEBUFFER, geometry_fbo);

        // geometry pass
        glViewport(0, 0, WINDOW_WIDTH * RENDER_SCALE, WINDOW_HEIGHT * RENDER_SCALE);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        program_bind(geometry_program);

        program_set_mat4(geometry_program, "camera.projection", camera_projection);
        program_set_mat4(geometry_program, "camera.view", camera_view);
        program_set_vec3(geometry_program, "camera.position", camera->position);

        for (int i = 0; i < num_objects; i++)
        {
            // calculate model matrix
            mat4 model = GLM_MAT4_IDENTITY_INIT;
            object_calc_model(objects[i], model);

            program_set_mat4(geometry_program, "object.model", model);

            program_set_texture(geometry_program, "material.diffuse", 0, objects[i]->material->diffuse ? objects[i]->material->diffuse->texture : 0);
            program_set_texture(geometry_program, "material.specular", 1, objects[i]->material->specular ? objects[i]->material->specular->texture : 0);
            program_set_texture(geometry_program, "material.normal", 2, objects[i]->material->normal ? objects[i]->material->normal->texture : 0);
            program_set_texture(geometry_program, "material.emission", 3, objects[i]->material->emission ? objects[i]->material->emission->texture : 0);
            program_set_vec3(geometry_program, "material.color", objects[i]->material->color);
            program_set_float(geometry_program, "material.shininess", objects[i]->material->shininess);
            program_set_float(geometry_program, "material.glow", objects[i]->material->glow);

            mesh_draw(objects[i]->mesh);
        }

        program_unbind();

        // unbind geometry fbo
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // bind depthmap fbo
        glBindFramebuffer(GL_FRAMEBUFFER, depthmap_fbo);

        // shadow pass
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glClear(GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        program_bind(depth_program);

        program_set_mat4(depth_program, "sun.projection", sun_projection);
        program_set_mat4(depth_program, "sun.view", sun_view);

        for (int i = 0; i < num_objects; i++)
        {
            // calculate model matrix
            mat4 model = GLM_MAT4_IDENTITY_INIT;
            object_calc_model(objects[i], model);

            program_set_mat4(depth_program, "object.model", model);

            mesh_draw(objects[i]->mesh);
        }

        program_unbind();

        // unbind depthmap fbo
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // bind screen fbo
        glBindFramebuffer(GL_FRAMEBUFFER, screen_fbo);

        // lighting pass
        glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST);

        switch (lighting)
        {
        case LIGHTING_DIFFUSE:
        {
            program_bind(diffuse_program);

            program_set_mat4(diffuse_program, "camera.projection", camera_projection);
            program_set_mat4(diffuse_program, "camera.view", camera_view);
            program_set_vec3(diffuse_program, "camera.position", camera->position);

            program_set_texture(diffuse_program, "geometry.albedo", 0, geometry_albedo_texture);

            mesh_draw(quad_mesh);

            program_unbind();
        }
        break;
        case LIGHTING_PHONG:
        {
            program_bind(phong_program);

            program_set_mat4(phong_program, "camera.projection", camera_projection);
            program_set_mat4(phong_program, "camera.view", camera_view);
            program_set_vec3(phong_program, "camera.position", camera->position);

            program_set_texture(phong_program, "geometry.position", 0, geometry_position_texture);
            program_set_texture(phong_program, "geometry.normal", 1, geometry_normal_texture);
            program_set_texture(phong_program, "geometry.albedo", 2, geometry_albedo_texture);
            program_set_texture(phong_program, "geometry.specular", 3, geometry_specular_texture);
            program_set_texture(phong_program, "geometry.emission", 4, geometry_emission_texture);

            program_set_mat4(phong_program, "sun.projection", sun_projection);
            program_set_mat4(phong_program, "sun.view", sun_view);

            program_set_vec3(phong_program, "scene.ambient", scene_ambient);

            program_set_vec3(phong_program, "directional_light.direction", directional_light->direction);
            program_set_vec3(phong_program, "directional_light.ambient", directional_light->ambient);
            program_set_vec3(phong_program, "directional_light.diffuse", directional_light->diffuse);
            program_set_vec3(phong_program, "directional_light.specular", directional_light->specular);

            program_set_vec3(phong_program, "point_lights[0].position", point_lights[0]->position);
            program_set_vec3(phong_program, "point_lights[0].ambient", point_lights[0]->ambient);
            program_set_vec3(phong_program, "point_lights[0].diffuse", point_lights[0]->diffuse);
            program_set_vec3(phong_program, "point_lights[0].specular", point_lights[0]->specular);
            program_set_float(phong_program, "point_lights[0].constant", point_lights[0]->constant);
            program_set_float(phong_program, "point_lights[0].linear", point_lights[0]->linear);
            program_set_float(phong_program, "point_lights[0].quadratic", point_lights[0]->quadratic);

            program_set_vec3(phong_program, "point_lights[1].position", point_lights[1]->position);
            program_set_vec3(phong_program, "point_lights[1].ambient", point_lights[1]->ambient);
            program_set_vec3(phong_program, "point_lights[1].diffuse", point_lights[1]->diffuse);
            program_set_vec3(phong_program, "point_lights[1].specular", point_lights[1]->specular);
            program_set_float(phong_program, "point_lights[1].constant", point_lights[1]->constant);
            program_set_float(phong_program, "point_lights[1].linear", point_lights[1]->linear);
            program_set_float(phong_program, "point_lights[1].quadratic", point_lights[1]->quadratic);

            program_set_vec3(phong_program, "point_lights[2].position", point_lights[2]->position);
            program_set_vec3(phong_program, "point_lights[2].ambient", point_lights[2]->ambient);
            program_set_vec3(phong_program, "point_lights[2].diffuse", point_lights[2]->diffuse);
            program_set_vec3(phong_program, "point_lights[2].specular", point_lights[2]->specular);
            program_set_float(phong_program, "point_lights[2].constant", point_lights[2]->constant);
            program_set_float(phong_program, "point_lights[2].linear", point_lights[2]->linear);
            program_set_float(phong_program, "point_lights[2].quadratic", point_lights[2]->quadratic);

            program_set_vec3(phong_program, "point_lights[3].position", point_lights[3]->position);
            program_set_vec3(phong_program, "point_lights[3].ambient", point_lights[3]->ambient);
            program_set_vec3(phong_program, "point_lights[3].diffuse", point_lights[3]->diffuse);
            program_set_vec3(phong_program, "point_lights[3].specular", point_lights[3]->specular);
            program_set_float(phong_program, "point_lights[3].constant", point_lights[3]->constant);
            program_set_float(phong_program, "point_lights[3].linear", point_lights[3]->linear);
            program_set_float(phong_program, "point_lights[3].quadratic", point_lights[3]->quadratic);

            program_set_vec3(phong_program, "spot_light.position", spot_light->position);
            program_set_vec3(phong_program, "spot_light.direction", spot_light->direction);
            program_set_vec3(phong_program, "spot_light.ambient", spot_light->ambient);
            program_set_vec3(phong_program, "spot_light.diffuse", spot_light->diffuse);
            program_set_vec3(phong_program, "spot_light.specular", spot_light->specular);
            program_set_float(phong_program, "spot_light.constant", spot_light->constant);
            program_set_float(phong_program, "spot_light.linear", spot_light->linear);
            program_set_float(phong_program, "spot_light.quadratic", spot_light->quadratic);
            program_set_float(phong_program, "spot_light.cutOff", spot_light->cutOff);
            program_set_float(phong_program, "spot_light.outerCutOff", spot_light->outerCutOff);

            program_set_texture(phong_program, "depthmap.texture", 5, depthmap_texture);

            mesh_draw(quad_mesh);

            program_unbind();
        }
        break;
        }

        // unbind fbo
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // post processing pass
        glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST);

        program_bind(post_program);

        program_set_texture(post_program, "screen.texture", 0, screen_texture);

        mesh_draw(quad_mesh);

        program_unbind();

        // display the window
        window_swap();

        // end of frame activities
        time_frame_end();
    }

    glDeleteRenderbuffers(1, &geometry_rbo);
    glDeleteTextures(1, &geometry_specular_texture);
    glDeleteTextures(1, &geometry_albedo_texture);
    glDeleteTextures(1, &geometry_normal_texture);
    glDeleteTextures(1, &geometry_position_texture);
    glDeleteFramebuffers(1, &geometry_fbo);

    glDeleteTextures(1, &depthmap_texture);
    glDeleteFramebuffers(1, &depthmap_fbo);

    glDeleteRenderbuffers(1, &screen_rbo);
    glDeleteTextures(1, &screen_texture);
    glDeleteFramebuffers(1, &screen_fbo);

    // free resources
    Mix_FreeChunk(shoot_sound);
    Mix_FreeMusic(background_music);
    spot_light_destroy(spot_light);
    for (int i = 0; i < num_point_lights; i++)
    {
        point_light_destroy(point_lights[i]);
    }
    directional_light_destroy(directional_light);
    for (int i = 0; i < num_objects; i++)
    {
        object_destroy(objects[i]);
    }
    material_destroy(cobble_material);
    material_destroy(box_material);
    texture_destroy(cobble_specular_texture);
    texture_destroy(cobble_diffuse_texture);
    texture_destroy(box_specular_texture);
    texture_destroy(box_diffuse_texture);
    mesh_destroy(cube_mesh);
    mesh_destroy(quad_mesh);
    program_destroy(depth_program);
    program_destroy(post_program);
    program_destroy(geometry_program);
    program_destroy(phong_program);
    program_destroy(diffuse_program);

    // close engine
    image_quit();
    audio_quit();
    window_quit();
    core_quit();

    return 0;
}
