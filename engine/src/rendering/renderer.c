#include <engine/engine.h>

// TODO: move standard assets?

// TODO: create a way to specify paths for engine assets and game assets separately

// TODO: support resizing
// we need to resize the viewport, as well as recreate all the framebuffers

// TODO: render a sun texture in the sky
// calculate its position based on the direction

// standard assets
struct texture *default_texture;

struct material *default_material;

struct mesh *quad_mesh;
struct mesh *cube_mesh;

static int render_width;
static int render_height;
static float render_scale;
static int shadow_width;
static int shadow_height;
static enum render_mode render_mode;

// programs
static struct program *depth_program;
static struct program *forward_color_program;
static struct program *forward_sun_program;
static struct program *forward_directional_program;
static struct program *forward_point_program;
static struct program *forward_spot_program;
static struct program *geometry_program;
static struct program *deferred_sun_program;
static struct program *deferred_directional_program;
static struct program *deferred_point_program;
static struct program *deferred_spot_program;
static struct program *skybox_program;
static struct program *water_program;
static struct program *sprite_program;
static struct program *post_program;

// framebuffers
static GLuint screen_fbo_id;
static GLuint screen_texture_id;
static GLuint screen_rbo_id;

static GLuint depthmap_fbo_id;
static GLuint depthmap_texture_id;

static GLuint geometry_fbo_id;
static GLuint geometry_position_texture_id;
static GLuint geometry_normal_texture_id;
static GLuint geometry_albedo_specular_texture_id;
static GLuint geometry_rbo_id;

static GLuint water_fbo_id;
static GLuint water_color_texture_id;
static GLuint water_depth_texture_id;

// water
static unsigned int num_water_vertices;
static GLuint water_vao_id;
static GLuint water_vbo_id;

// skybox
static unsigned int num_skybox_vertices;
static GLuint skybox_vao_id;
static GLuint skybox_vbo_id;

// sprite
static unsigned int num_sprite_vertices;
static GLuint sprite_vao_id;
static GLuint sprite_vbo_id;

// screen
static unsigned int num_screen_vertices;
static GLuint screen_vao_id;
static GLuint screen_vbo_id;

// renderables
static struct camera *camera;

static struct object **objects;
static unsigned int num_objects;

static struct sun *sun;

static struct directional_light **directional_lights;
static unsigned int num_directional_lights;

static struct point_light **point_lights;
static unsigned int num_point_lights;

static struct spot_light **spot_lights;
static unsigned int num_spot_lights;

static struct cubemap *skybox;

static struct water **waters;
static unsigned int num_waters;

static struct sprite **sprites;
static unsigned int num_sprites;

int renderer_init(int _render_width, int _render_height, float _render_scale, int _shadow_width, int _shadow_height)
{
    // init GLEW
    {
        GLenum glewError = glewInit();

        if (glewError != GLEW_OK)
        {
            printf("Error: %s\n", glewGetErrorString(glewError));

            return 1;
        }
    }

    printf("GLEW %s\n", glewGetString(GLEW_VERSION));
    printf("OpenGL %s\n", glGetString(GL_VERSION));
    printf("Vendor %s\n", glGetString(GL_VENDOR));
    printf("Renderer %s\n", glGetString(GL_RENDERER));
    printf("GLSL %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

    // create default textures
    int default_texture_width = 256;
    int default_texture_height = 256;
    unsigned int *default_texture_pixels = malloc(default_texture_width * default_texture_height * sizeof(unsigned int));
    memset(default_texture_pixels, -1, default_texture_width * default_texture_height * sizeof(unsigned int));

    default_texture = texture_create(default_texture_width, default_texture_height, 4, default_texture_pixels);

    if (!default_texture)
    {
        printf("Error: Couldn't create default texture\n");

        return 1;
    }

    free(default_texture_pixels);

    // create default materials
    vec3 default_material_color = { 1.0f, 1.0f, 1.0f };

    default_material = material_create(
        default_material_color,
        default_texture,
        default_texture,
        16.0f,
        NULL,
        NULL,
        1.0f);

    if (!default_material)
    {
        printf("Error: Couldn't create default material\n");

        return 1;
    }

    // create default meshes
    float quad_vertices[] = {
        // position           // normal            // uv
         1.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f, // top right
         1.0f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f, // bottom right
        -1.0f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f, // bottom left
        -1.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f  // top left
    };

    unsigned int quad_indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

    quad_mesh = mesh_create(
        quad_vertices,
        sizeof(quad_vertices),
        quad_indices,
        sizeof(quad_indices));

    if (!quad_mesh)
    {
        printf("Error: Couldn't create quad mesh\n");

        return 1;
    }

    float cube_vertices[] = {
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

    cube_mesh = mesh_create(
        cube_vertices,
        sizeof(cube_vertices),
        cube_indices,
        sizeof(cube_indices));

    if (!cube_mesh)
    {
        printf("Error: Couldn't create cube mesh\n");

        return 1;
    }

    render_width = _render_width;
    render_height = _render_height;
    render_scale = _render_scale;
    shadow_width = _shadow_width;
    shadow_height = _shadow_height;
    render_mode = RENDER_MODE_FORWARD;

    // setup opengl
    // TODO: face culling
    glViewport(0, 0, render_width, render_height);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_STENCIL_TEST);

    // create programs
    depth_program = program_create(
        "../engine/assets/shaders/depth.vert",
        "../engine/assets/shaders/depth.frag");

    if (!depth_program)
    {
        printf("Error: Couldn't create depth program\n");

        return 1;
    }

    forward_color_program = program_create(
        "../engine/assets/shaders/forward.vert",
        "../engine/assets/shaders/forward_color.frag");

    if (!forward_color_program)
    {
        printf("Error: Couldn't create forward color program\n");

        return 1;
    }

    forward_sun_program = program_create(
        "../engine/assets/shaders/forward.vert",
        "../engine/assets/shaders/forward_sun.frag");

    if (!forward_sun_program)
    {
        printf("Error: Couldn't create forward sun program\n");

        return 1;
    }

    forward_directional_program = program_create(
        "../engine/assets/shaders/forward.vert",
        "../engine/assets/shaders/forward_directional.frag");

    if (!forward_directional_program)
    {
        printf("Error: Couldn't create forward directional program\n");

        return 1;
    }

    forward_point_program = program_create(
        "../engine/assets/shaders/forward.vert",
        "../engine/assets/shaders/forward_point.frag");

    if (!forward_point_program)
    {
        printf("Error: Couldn't create forward point program\n");

        return 1;
    }

    forward_spot_program = program_create(
        "../engine/assets/shaders/forward.vert",
        "../engine/assets/shaders/forward_spot.frag");

    if (!forward_spot_program)
    {
        printf("Error: Couldn't create forward spot program\n");

        return 1;
    }

    geometry_program = program_create(
        "../engine/assets/shaders/geometry.vert",
        "../engine/assets/shaders/geometry.frag");

    if (!geometry_program)
    {
        printf("Error: Couldn't create geometry program\n");

        return 1;
    }

    deferred_sun_program = program_create(
        "../engine/assets/shaders/deferred.vert",
        "../engine/assets/shaders/deferred_sun.frag");

    if (!deferred_sun_program)
    {
        printf("Error: Couldn't create deferred sun program\n");

        return 1;
    }

    deferred_directional_program = program_create(
        "../engine/assets/shaders/deferred.vert",
        "../engine/assets/shaders/deferred_directional.frag");

    if (!deferred_directional_program)
    {
        printf("Error: Couldn't create deferred directional program\n");

        return 1;
    }

    deferred_point_program = program_create(
        "../engine/assets/shaders/deferred.vert",
        "../engine/assets/shaders/deferred_point.frag");

    if (!deferred_point_program)
    {
        printf("Error: Couldn't create deferred point program\n");

        return 1;
    }

    deferred_spot_program = program_create(
        "../engine/assets/shaders/deferred.vert",
        "../engine/assets/shaders/deferred_spot.frag");

    if (!deferred_spot_program)
    {
        printf("Error: Couldn't create deferred spot program\n");

        return 1;
    }

    skybox_program = program_create(
        "../engine/assets/shaders/skybox.vert",
        "../engine/assets/shaders/skybox.frag");

    if (!skybox_program)
    {
        printf("Error: Couldn't create skybox program\n");

        return 1;
    }

    water_program = program_create(
        "../engine/assets/shaders/water.vert",
        "../engine/assets/shaders/water.frag");

    if (!water_program)
    {
        printf("Error: Couldn't create water program\n");

        return 1;
    }

    sprite_program = program_create(
        "../engine/assets/shaders/sprite.vert",
        "../engine/assets/shaders/sprite.frag");

    if (!sprite_program)
    {
        printf("Error: Couldn't create sprite program\n");

        return 1;
    }

    post_program = program_create(
        "../engine/assets/shaders/post.vert",
        "../engine/assets/shaders/post.frag");

    if (!post_program)
    {
        printf("Error: Couldn't create post program\n");

        return 1;
    }

    // setup shader samplers
    program_bind(forward_sun_program);
    program_set_int(forward_sun_program, "material.diffuse", 0);
    program_set_int(forward_sun_program, "material.specular", 1);
    program_set_int(forward_sun_program, "material.normal", 2);
    program_set_int(forward_sun_program, "material.emission", 3);
    program_set_int(forward_sun_program, "depthmap.texture", 4);
    program_unbind();

    program_bind(forward_directional_program);
    program_set_int(forward_directional_program, "material.diffuse", 0);
    program_set_int(forward_directional_program, "material.specular", 1);
    program_set_int(forward_directional_program, "material.normal", 2);
    program_set_int(forward_directional_program, "material.emission", 3);
    program_unbind();

    program_bind(forward_point_program);
    program_set_int(forward_point_program, "material.diffuse", 0);
    program_set_int(forward_point_program, "material.specular", 1);
    program_set_int(forward_point_program, "material.normal", 2);
    program_set_int(forward_point_program, "material.emission", 3);
    program_unbind();

    program_bind(forward_spot_program);
    program_set_int(forward_spot_program, "material.diffuse", 0);
    program_set_int(forward_spot_program, "material.specular", 1);
    program_set_int(forward_spot_program, "material.normal", 2);
    program_set_int(forward_spot_program, "material.emission", 3);
    program_unbind();

    program_bind(geometry_program);
    program_set_int(geometry_program, "material.diffuse", 0);
    program_set_int(geometry_program, "material.specular", 1);
    program_set_int(geometry_program, "material.normal", 2);
    program_set_int(geometry_program, "material.emission", 3);
    program_unbind();

    program_bind(deferred_sun_program);
    program_set_int(deferred_sun_program, "geometry.position", 0);
    program_set_int(deferred_sun_program, "geometry.normal", 1);
    program_set_int(deferred_sun_program, "geometry.albedo_specular", 2);
    program_set_int(deferred_sun_program, "depthmap.texture", 4);
    program_unbind();

    program_bind(deferred_directional_program);
    program_set_int(deferred_directional_program, "geometry.position", 0);
    program_set_int(deferred_directional_program, "geometry.normal", 1);
    program_set_int(deferred_directional_program, "geometry.albedo_specular", 2);
    program_unbind();

    program_bind(deferred_point_program);
    program_set_int(deferred_point_program, "geometry.position", 0);
    program_set_int(deferred_point_program, "geometry.normal", 1);
    program_set_int(deferred_point_program, "geometry.albedo_specular", 2);
    program_unbind();

    program_bind(deferred_spot_program);
    program_set_int(deferred_spot_program, "geometry.position", 0);
    program_set_int(deferred_spot_program, "geometry.normal", 1);
    program_set_int(deferred_spot_program, "geometry.albedo_specular", 2);
    program_unbind();

    program_bind(skybox_program);
    program_set_int(skybox_program, "skybox.texture", 0);
    program_unbind();

    program_bind(water_program);
    program_set_int(water_program, "skybox.texture", 0);
    program_unbind();

    program_bind(sprite_program);
    program_set_int(sprite_program, "sprite.image", 0);
    program_unbind();

    program_bind(post_program);
    program_set_int(post_program, "screen.texture", 0);
    program_unbind();

    // setup screen fbo
    glGenFramebuffers(1, &screen_fbo_id);
    glBindFramebuffer(GL_FRAMEBUFFER, screen_fbo_id);

    glGenTextures(1, &screen_texture_id);
    glBindTexture(GL_TEXTURE_2D, screen_texture_id);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGB,
        render_width,
        render_height,
        0,
        GL_RGB,
        GL_UNSIGNED_BYTE,
        NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D,
        screen_texture_id,
        0);

    glBindTexture(GL_TEXTURE_2D, 0);

    glGenRenderbuffers(1, &screen_rbo_id);
    glBindRenderbuffer(GL_RENDERBUFFER, screen_rbo_id);

    glRenderbufferStorage(
        GL_RENDERBUFFER,
        GL_DEPTH24_STENCIL8,
        render_width,
        render_height);

    glFramebufferRenderbuffer(
        GL_FRAMEBUFFER,
        GL_DEPTH_STENCIL_ATTACHMENT,
        GL_RENDERBUFFER,
        screen_rbo_id);

    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        printf("Error: Couldn't complete screen framebuffer\n");

        return 1;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // setup depthmap fbo
    glGenFramebuffers(1, &depthmap_fbo_id);
    glBindFramebuffer(GL_FRAMEBUFFER, depthmap_fbo_id);

    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    glGenTextures(1, &depthmap_texture_id);
    glBindTexture(GL_TEXTURE_2D, depthmap_texture_id);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_DEPTH_COMPONENT,
        shadow_width,
        shadow_height,
        0,
        GL_DEPTH_COMPONENT,
        GL_FLOAT,
        NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    GLfloat border_color[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_color);

    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_DEPTH_ATTACHMENT,
        GL_TEXTURE_2D,
        depthmap_texture_id,
        0);

    glBindTexture(GL_TEXTURE_2D, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        printf("Error: Couldn't complete depthmap framebuffer\n");

        return 1;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // setup geometry fbo
    glGenFramebuffers(1, &geometry_fbo_id);
    glBindFramebuffer(GL_FRAMEBUFFER, geometry_fbo_id);

    // TODO: dont store position because it can be recreated in the fragment shader from depth
    glGenTextures(1, &geometry_position_texture_id);
    glBindTexture(GL_TEXTURE_2D, geometry_position_texture_id);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGB16F,
        (GLsizei)(render_width * render_scale),
        (GLsizei)(render_height * render_scale),
        0,
        GL_RGB,
        GL_FLOAT,
        NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D,
        geometry_position_texture_id,
        0);

    glBindTexture(GL_TEXTURE_2D, 0);

    // TODO: find a way to optimize this
    glGenTextures(1, &geometry_normal_texture_id);
    glBindTexture(GL_TEXTURE_2D, geometry_normal_texture_id);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGB16F,
        (GLsizei)(render_width * render_scale),
        (GLsizei)(render_height * render_scale),
        0,
        GL_RGB,
        GL_FLOAT,
        NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT1,
        GL_TEXTURE_2D,
        geometry_normal_texture_id,
        0);

    glBindTexture(GL_TEXTURE_2D, 0);

    glGenTextures(1, &geometry_albedo_specular_texture_id);
    glBindTexture(GL_TEXTURE_2D, geometry_albedo_specular_texture_id);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA8,
        (GLsizei)(render_width * render_scale),
        (GLsizei)(render_height * render_scale),
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT2,
        GL_TEXTURE_2D,
        geometry_albedo_specular_texture_id,
        0);

    glBindTexture(GL_TEXTURE_2D, 0);

    GLenum geometry_fbo_attachments[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    glDrawBuffers(sizeof(geometry_fbo_attachments) / sizeof(GLenum), geometry_fbo_attachments);

    glGenRenderbuffers(1, &geometry_rbo_id);
    glBindRenderbuffer(GL_RENDERBUFFER, geometry_rbo_id);

    glRenderbufferStorage(
        GL_RENDERBUFFER,
        GL_DEPTH_COMPONENT,
        (GLsizei)(render_width * render_scale),
        (GLsizei)(render_height * render_scale));

    glFramebufferRenderbuffer(
        GL_FRAMEBUFFER,
        GL_DEPTH_ATTACHMENT,
        GL_RENDERBUFFER,
        geometry_rbo_id);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        printf("Error: Couldn't complete geometry framebuffer\n");

        return 1;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // setup water fbo
    glGenFramebuffers(1, &water_fbo_id);
    glBindFramebuffer(GL_FRAMEBUFFER, water_fbo_id);

    glGenTextures(1, &water_color_texture_id);
    glBindTexture(GL_TEXTURE_2D, water_color_texture_id);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGB8,
        render_width, // TODO: specific water texture sizes?
        render_height,
        0,
        GL_RGB,
        GL_UNSIGNED_BYTE,
        NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D,
        water_color_texture_id,
        0);

    glBindTexture(GL_TEXTURE_2D, 0);

    GLenum water_fbo_attachments[] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(sizeof(water_fbo_attachments) / sizeof(GLenum), water_fbo_attachments);

    glGenTextures(1, &water_depth_texture_id);
    glBindTexture(GL_TEXTURE_2D, water_depth_texture_id);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_DEPTH_COMPONENT32,
        render_width,
        render_height,
        0,
        GL_DEPTH_COMPONENT,
        GL_FLOAT,
        NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_DEPTH_ATTACHMENT,
        GL_TEXTURE_2D,
        water_depth_texture_id,
        0);

    glBindTexture(GL_TEXTURE_2D, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        printf("Error: Couldn't complete water framebuffer\n");

        return 1;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // create water mesh
    float water_vertices[] = {
        // position 
        -1.0f, -1.0f,
        -1.0f,  1.0f,
         1.0f, -1.0f,
         1.0f, -1.0f,
        -1.0f,  1.0f,
         1.0f,  1.0f
    };
    num_water_vertices = sizeof(water_vertices) / sizeof(float) / 2;

    glGenVertexArrays(1, &water_vao_id);
    glBindVertexArray(water_vao_id);

    glGenBuffers(1, &water_vbo_id);
    glBindBuffer(GL_ARRAY_BUFFER, water_vbo_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(water_vertices), &water_vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid *)0); // position
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

    // create skybox mesh
    float skybox_vertices[] = {
        // position
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };
    num_skybox_vertices = sizeof(skybox_vertices) / sizeof(float) / 3;

    glGenVertexArrays(1, &skybox_vao_id);
    glBindVertexArray(skybox_vao_id);

    glGenBuffers(1, &skybox_vbo_id);
    glBindBuffer(GL_ARRAY_BUFFER, skybox_vbo_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skybox_vertices), &skybox_vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid *)0); // position
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

    // create sprite mesh
    float sprite_vertices[] = {
        // position // uv
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,

        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f
    };
    num_sprite_vertices = sizeof(sprite_vertices) / sizeof(float) / 4;

    glGenVertexArrays(1, &sprite_vao_id);
    glBindVertexArray(sprite_vao_id);

    glGenBuffers(1, &sprite_vbo_id);
    glBindBuffer(GL_ARRAY_BUFFER, sprite_vbo_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(sprite_vertices), &sprite_vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid *)0);                     // position
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid *)(2 * sizeof(GLfloat))); // uv
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

    // create screen mesh
    float screen_vertices[] = {
        // position    // uv
        -1.0f, -1.0f,  0.0f,  0.0f,
        -1.0f,  1.0f,  0.0f,  1.0f,
         1.0f, -1.0f,  1.0f,  0.0f,
         1.0f, -1.0f,  1.0f,  0.0f,
        -1.0f,  1.0f,  0.0f,  1.0f,
         1.0f,  1.0f,  1.0f,  1.0f,
    };
    num_screen_vertices = sizeof(screen_vertices) / sizeof(float) / 4;

    glGenVertexArrays(1, &screen_vao_id);
    glBindVertexArray(screen_vao_id);

    glGenBuffers(1, &screen_vbo_id);
    glBindBuffer(GL_ARRAY_BUFFER, screen_vbo_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(screen_vertices), &screen_vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid *)0); // position
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid *)(2 * sizeof(GLfloat))); // uv
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

    return 0;
}

void renderer_set_mode(enum render_mode _render_mode)
{
    render_mode = _render_mode;
}

void renderer_set_camera(struct camera *_camera)
{
    camera = _camera;
}

void renderer_add_object(struct object *object)
{
    objects = realloc(objects, sizeof(struct object *) * (num_objects + 1));
    objects[num_objects++] = object;
}

void renderer_set_sun(struct sun *_sun)
{
    sun = _sun;
}

void renderer_add_directional_light(struct directional_light *directional_light)
{
    directional_lights = realloc(directional_lights, sizeof(struct directional_light *) * (num_directional_lights + 1));
    directional_lights[num_directional_lights++] = directional_light;
}

void renderer_add_point_light(struct point_light *point_light)
{
    point_lights = realloc(point_lights, sizeof(struct point_light *) * (num_point_lights + 1));
    point_lights[num_point_lights++] = point_light;
}

void renderer_add_spot_light(struct spot_light *spot_light)
{
    spot_lights = realloc(spot_lights, sizeof(struct spot_light *) * (num_spot_lights + 1));
    spot_lights[num_spot_lights++] = spot_light;
}

void renderer_set_skybox(struct cubemap *_skybox)
{
    skybox = _skybox;
}

void renderer_add_water(struct water *water)
{
    waters = realloc(waters, sizeof(struct water *) * (num_waters + 1));
    waters[num_waters++] = water;
}

void renderer_add_sprite(struct sprite *sprite)
{
    sprites = realloc(sprites, sizeof(struct sprite *) * (num_sprites + 1));
    sprites[num_sprites++] = sprite;
}

// TODO: validation
//       for example, if there is no camera set, dont render anything
//       but if there is no skybox set, still render the scene
// TODO: split this up into sub functions
void renderer_draw(bool ortho, float aspect)
{
    // calculate camera projection matrix
    mat4 camera_projection;
    if (ortho)
    {
        glm_ortho_default(aspect, camera_projection);
    }
    else
    {
        glm_perspective(
            glm_rad(camera->fov),
            aspect,
            0.01f,
            100.0f,
            camera_projection);
    }

    // calculate camera view matrix
    mat4 camera_view;
    vec3 camera_target;
    glm_vec_add(camera->position, camera->front, camera_target);
    glm_lookat(
        camera->position,
        camera_target,
        camera->up,
        camera_view);

    // calculate sun projection matrix
    mat4 sun_projection;
    glm_ortho(-10.0f, 10.0f, -10.0f, 10.0f, -10.0f, 10.0f, sun_projection);

    // calculate sun view matrix
    mat4 sun_view;
    vec3 sun_position;
    glm_vec_sub(GLM_VEC3_ZERO, sun->direction, sun_position);
    glm_lookat(sun_position, GLM_VEC3_ZERO, GLM_YUP, sun_view);

    // bind depthmap fbo
    glBindFramebuffer(GL_FRAMEBUFFER, depthmap_fbo_id);

    // start shadow pass
    glViewport(0, 0, shadow_width, shadow_height);
    glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    program_bind(depth_program);

    program_set_mat4(depth_program, "sun.projection", sun_projection);
    program_set_mat4(depth_program, "sun.view", sun_view);

    for (unsigned int i = 0; i < num_objects; i++)
    {
        // calculate model matrix
        mat4 object_model = GLM_MAT4_IDENTITY_INIT;
        object_calc_model(objects[i], object_model);

        program_set_mat4(depth_program, "object.model", object_model);

        mesh_draw(objects[i]->mesh);
    }

    program_unbind();

    // end shadow pass

    // unbind depthmap fbo
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    switch (render_mode)
    {
    case RENDER_MODE_FORWARD:
    {
        // bind screen fbo
        glBindFramebuffer(GL_FRAMEBUFFER, screen_fbo_id);

        // forward rendering
        glViewport(0, 0, render_width, render_height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        for (unsigned int i = 0; i < num_objects; i++)
        {
            // calculate model matrix
            mat4 object_model = GLM_MAT4_IDENTITY_INIT;
            object_calc_model(objects[i], object_model);

            // sun
            program_bind(forward_sun_program);

            program_set_mat4(forward_sun_program, "camera.projection", camera_projection);
            program_set_mat4(forward_sun_program, "camera.view", camera_view);
            program_set_vec3(forward_sun_program, "camera.position", camera->position);

            program_set_mat4(forward_sun_program, "object.model", object_model);

            program_set_vec3(forward_sun_program, "material.color", objects[i]->material->color);
            program_set_float(forward_sun_program, "material.shininess", objects[i]->material->shininess);
            program_set_float(forward_sun_program, "material.glow", objects[i]->material->glow);

            program_set_vec3(forward_sun_program, "sun.direction", sun->direction);
            program_set_vec3(forward_sun_program, "sun.ambient", sun->ambient);
            program_set_vec3(forward_sun_program, "sun.diffuse", sun->diffuse);
            program_set_vec3(forward_sun_program, "sun.specular", sun->specular);
            program_set_mat4(forward_sun_program, "sun.projection", sun_projection);
            program_set_mat4(forward_sun_program, "sun.view", sun_view);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, objects[i]->material->diffuse ? objects[i]->material->diffuse->texture_id : 0);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, objects[i]->material->specular ? objects[i]->material->specular->texture_id : 0);
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, objects[i]->material->normal ? objects[i]->material->normal->texture_id : 0);
            glActiveTexture(GL_TEXTURE3);
            glBindTexture(GL_TEXTURE_2D, objects[i]->material->emission ? objects[i]->material->emission->texture_id : 0);
            glActiveTexture(GL_TEXTURE4);
            glBindTexture(GL_TEXTURE_2D, depthmap_texture_id);

            mesh_draw(objects[i]->mesh);

            program_unbind();

            // enable blending
            glEnable(GL_BLEND);
            glBlendFunc(GL_ONE, GL_ONE);
            glDepthMask(GL_FALSE);
            glDepthFunc(GL_EQUAL);

            // directional lights
            program_bind(forward_directional_program);

            program_set_mat4(forward_directional_program, "camera.projection", camera_projection);
            program_set_mat4(forward_directional_program, "camera.view", camera_view);
            program_set_vec3(forward_directional_program, "camera.position", camera->position);

            program_set_mat4(forward_directional_program, "object.model", object_model);

            program_set_vec3(forward_directional_program, "material.color", objects[i]->material->color);
            program_set_float(forward_directional_program, "material.shininess", objects[i]->material->shininess);
            program_set_float(forward_directional_program, "material.glow", objects[i]->material->glow);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, objects[i]->material->diffuse ? objects[i]->material->diffuse->texture_id : 0);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, objects[i]->material->specular ? objects[i]->material->specular->texture_id : 0);
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, objects[i]->material->normal ? objects[i]->material->normal->texture_id : 0);
            glActiveTexture(GL_TEXTURE3);
            glBindTexture(GL_TEXTURE_2D, objects[i]->material->emission ? objects[i]->material->emission->texture_id : 0);

            for (unsigned int j = 0; j < num_directional_lights; j++)
            {
                program_set_vec3(forward_directional_program, "directional_light.direction", directional_lights[j]->direction);
                program_set_vec3(forward_directional_program, "directional_light.ambient", directional_lights[j]->ambient);
                program_set_vec3(forward_directional_program, "directional_light.diffuse", directional_lights[j]->diffuse);
                program_set_vec3(forward_directional_program, "directional_light.specular", directional_lights[j]->specular);

                mesh_draw(objects[i]->mesh);
            }

            program_unbind();

            // point lights
            program_bind(forward_point_program);

            program_set_mat4(forward_point_program, "camera.projection", camera_projection);
            program_set_mat4(forward_point_program, "camera.view", camera_view);
            program_set_vec3(forward_point_program, "camera.position", camera->position);

            program_set_mat4(forward_point_program, "object.model", object_model);

            program_set_vec3(forward_point_program, "material.color", objects[i]->material->color);
            program_set_float(forward_point_program, "material.shininess", objects[i]->material->shininess);
            program_set_float(forward_point_program, "material.glow", objects[i]->material->glow);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, objects[i]->material->diffuse ? objects[i]->material->diffuse->texture_id : 0);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, objects[i]->material->specular ? objects[i]->material->specular->texture_id : 0);
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, objects[i]->material->normal ? objects[i]->material->normal->texture_id : 0);
            glActiveTexture(GL_TEXTURE3);
            glBindTexture(GL_TEXTURE_2D, objects[i]->material->emission ? objects[i]->material->emission->texture_id : 0);

            for (unsigned int j = 0; j < num_point_lights; j++)
            {
                program_set_vec3(forward_point_program, "point_light.position", point_lights[j]->position);
                program_set_vec3(forward_point_program, "point_light.ambient", point_lights[j]->ambient);
                program_set_vec3(forward_point_program, "point_light.diffuse", point_lights[j]->diffuse);
                program_set_vec3(forward_point_program, "point_light.specular", point_lights[j]->specular);
                program_set_vec3(forward_point_program, "point_light.attenuation", point_lights[j]->attenuation);

                mesh_draw(objects[i]->mesh);
            }

            program_unbind();

            // spot lights
            program_bind(forward_spot_program);

            program_set_mat4(forward_spot_program, "camera.projection", camera_projection);
            program_set_mat4(forward_spot_program, "camera.view", camera_view);
            program_set_vec3(forward_spot_program, "camera.position", camera->position);

            program_set_mat4(forward_spot_program, "object.model", object_model);

            program_set_vec3(forward_spot_program, "material.color", objects[i]->material->color);
            program_set_float(forward_spot_program, "material.shininess", objects[i]->material->shininess);
            program_set_float(forward_spot_program, "material.glow", objects[i]->material->glow);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, objects[i]->material->diffuse ? objects[i]->material->diffuse->texture_id : 0);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, objects[i]->material->specular ? objects[i]->material->specular->texture_id : 0);
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, objects[i]->material->normal ? objects[i]->material->normal->texture_id : 0);
            glActiveTexture(GL_TEXTURE3);
            glBindTexture(GL_TEXTURE_2D, objects[i]->material->emission ? objects[i]->material->emission->texture_id : 0);

            for (unsigned int j = 0; j < num_spot_lights; j++)
            {
                program_set_vec3(forward_spot_program, "spot_light.position", spot_lights[j]->position);
                program_set_vec3(forward_spot_program, "spot_light.direction", spot_lights[j]->direction);
                program_set_vec3(forward_spot_program, "spot_light.ambient", spot_lights[j]->ambient);
                program_set_vec3(forward_spot_program, "spot_light.diffuse", spot_lights[j]->diffuse);
                program_set_vec3(forward_spot_program, "spot_light.specular", spot_lights[j]->specular);
                program_set_vec3(forward_spot_program, "spot_light.attenuation", spot_lights[j]->attenuation);
                program_set_float(forward_spot_program, "spot_light.inner_cutoff", spot_lights[j]->inner_cutoff);
                program_set_float(forward_spot_program, "spot_light.outer_cutoff", spot_lights[j]->outer_cutoff);

                mesh_draw(objects[i]->mesh);
            }

            program_unbind();

            // disable blending
            glDepthFunc(GL_LESS);
            glDepthMask(GL_TRUE);
            glDisable(GL_BLEND);
        }

        // unbind screen fbo
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    break;
    case RENDER_MODE_DEFERRED:
    {
        // bind geometry fbo
        glBindFramebuffer(GL_FRAMEBUFFER, geometry_fbo_id);

        // geometry pass
        glViewport(0, 0, (GLsizei)(render_width * render_scale), (GLsizei)(render_height * render_scale));
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        program_bind(geometry_program);

        program_set_mat4(geometry_program, "camera.projection", camera_projection);
        program_set_mat4(geometry_program, "camera.view", camera_view);
        program_set_vec3(geometry_program, "camera.position", camera->position);

        for (unsigned int i = 0; i < num_objects; i++)
        {
            // calculate model matrix
            mat4 object_model = GLM_MAT4_IDENTITY_INIT;
            object_calc_model(objects[i], object_model);

            program_set_mat4(geometry_program, "object.model", object_model);

            program_set_vec3(geometry_program, "material.color", objects[i]->material->color);
            program_set_float(geometry_program, "material.shininess", objects[i]->material->shininess);
            program_set_float(geometry_program, "material.glow", objects[i]->material->glow);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, objects[i]->material->diffuse ? objects[i]->material->diffuse->texture_id : 0);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, objects[i]->material->specular ? objects[i]->material->specular->texture_id : 0);
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, objects[i]->material->normal ? objects[i]->material->normal->texture_id : 0);
            glActiveTexture(GL_TEXTURE3);
            glBindTexture(GL_TEXTURE_2D, objects[i]->material->emission ? objects[i]->material->emission->texture_id : 0);

            mesh_draw(objects[i]->mesh);
        }

        program_unbind();

        // unbind geometry fbo
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // bind screen fbo
        glBindFramebuffer(GL_FRAMEBUFFER, screen_fbo_id);

        // lighting pass
        glViewport(0, 0, render_width, render_height);
        glClear(GL_COLOR_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST);

        // sun
        program_bind(deferred_sun_program);

        program_set_vec3(deferred_sun_program, "camera.position", camera->position);

        program_set_vec3(deferred_sun_program, "sun.direction", sun->direction);
        program_set_vec3(deferred_sun_program, "sun.ambient", sun->ambient);
        program_set_vec3(deferred_sun_program, "sun.diffuse", sun->diffuse);
        program_set_vec3(deferred_sun_program, "sun.specular", sun->specular);
        program_set_mat4(deferred_sun_program, "sun.projection", sun_projection);
        program_set_mat4(deferred_sun_program, "sun.view", sun_view);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, geometry_position_texture_id);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, geometry_normal_texture_id);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, geometry_albedo_specular_texture_id);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, depthmap_texture_id);

        // draw screen mesh
        glBindVertexArray(screen_vao_id);
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, screen_vbo_id);
        glDrawArrays(GL_TRIANGLES, 0, num_screen_vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glBindVertexArray(0);

        program_unbind();

        // enable blending
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE);
        glDepthMask(GL_FALSE);
        glDepthFunc(GL_EQUAL);

        // directional lights
        program_bind(deferred_directional_program);

        program_set_vec3(deferred_directional_program, "camera.position", camera->position);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, geometry_position_texture_id);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, geometry_normal_texture_id);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, geometry_albedo_specular_texture_id);

        for (unsigned int i = 0; i < num_directional_lights; i++)
        {
            program_set_vec3(deferred_directional_program, "directional_light.direction", directional_lights[i]->direction);
            program_set_vec3(deferred_directional_program, "directional_light.ambient", directional_lights[i]->ambient);
            program_set_vec3(deferred_directional_program, "directional_light.diffuse", directional_lights[i]->diffuse);
            program_set_vec3(deferred_directional_program, "directional_light.specular", directional_lights[i]->specular);

            // draw screen mesh
            glBindVertexArray(screen_vao_id);
            glEnableVertexAttribArray(0);
            glEnableVertexAttribArray(1);
            glBindBuffer(GL_ARRAY_BUFFER, screen_vbo_id);
            glDrawArrays(GL_TRIANGLES, 0, num_screen_vertices);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glDisableVertexAttribArray(0);
            glDisableVertexAttribArray(1);
            glBindVertexArray(0);
        }

        program_unbind();

        // point lights
        program_bind(deferred_point_program);

        program_set_vec3(deferred_point_program, "camera.position", camera->position);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, geometry_position_texture_id);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, geometry_normal_texture_id);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, geometry_albedo_specular_texture_id);

        for (unsigned int i = 0; i < num_point_lights; i++)
        {
            program_set_vec3(deferred_point_program, "point_light.position", point_lights[i]->position);
            program_set_vec3(deferred_point_program, "point_light.ambient", point_lights[i]->ambient);
            program_set_vec3(deferred_point_program, "point_light.diffuse", point_lights[i]->diffuse);
            program_set_vec3(deferred_point_program, "point_light.specular", point_lights[i]->specular);
            program_set_vec3(deferred_point_program, "point_light.attenuation", point_lights[i]->attenuation);

            // draw screen mesh
            glBindVertexArray(screen_vao_id);
            glEnableVertexAttribArray(0);
            glEnableVertexAttribArray(1);
            glBindBuffer(GL_ARRAY_BUFFER, screen_vbo_id);
            glDrawArrays(GL_TRIANGLES, 0, num_screen_vertices);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glDisableVertexAttribArray(0);
            glDisableVertexAttribArray(1);
            glBindVertexArray(0);
        }

        program_unbind();

        // spot lights
        program_bind(deferred_spot_program);

        program_set_vec3(deferred_spot_program, "camera.position", camera->position);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, geometry_position_texture_id);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, geometry_normal_texture_id);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, geometry_albedo_specular_texture_id);

        for (unsigned int i = 0; i < num_spot_lights; i++)
        {
            program_set_vec3(deferred_spot_program, "spot_light.position", spot_lights[i]->position);
            program_set_vec3(deferred_spot_program, "spot_light.direction", spot_lights[i]->direction);
            program_set_vec3(deferred_spot_program, "spot_light.ambient", spot_lights[i]->ambient);
            program_set_vec3(deferred_spot_program, "spot_light.diffuse", spot_lights[i]->diffuse);
            program_set_vec3(deferred_spot_program, "spot_light.specular", spot_lights[i]->specular);
            program_set_vec3(deferred_spot_program, "spot_light.attenuation", spot_lights[i]->attenuation);
            program_set_float(deferred_spot_program, "spot_light.inner_cutoff", spot_lights[i]->inner_cutoff);
            program_set_float(deferred_spot_program, "spot_light.outer_cutoff", spot_lights[i]->outer_cutoff);

            // draw screen mesh
            glBindVertexArray(screen_vao_id);
            glEnableVertexAttribArray(0);
            glEnableVertexAttribArray(1);
            glBindBuffer(GL_ARRAY_BUFFER, screen_vbo_id);
            glDrawArrays(GL_TRIANGLES, 0, num_screen_vertices);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glDisableVertexAttribArray(0);
            glDisableVertexAttribArray(1);
            glBindVertexArray(0);
        }

        program_unbind();

        // TODO: emissive lighting pass

        // disable blending
        glDepthFunc(GL_LESS);
        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);

        // end lighting pass
        glEnable(GL_DEPTH_TEST);

        // unbind screen fbo
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // copy depth information to screen fbo
        glBindFramebuffer(GL_READ_FRAMEBUFFER, geometry_fbo_id);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, screen_fbo_id);
        glBlitFramebuffer(0, 0, render_width, render_height, 0, 0, render_width, render_height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    break;
    }

    // bind screen fbo
    glBindFramebuffer(GL_FRAMEBUFFER, screen_fbo_id);

    // TEST: draw lights as a cube
    glViewport(0, 0, render_width, render_height);

    program_bind(forward_color_program);

    program_set_mat4(forward_color_program, "camera.projection", camera_projection);
    program_set_mat4(forward_color_program, "camera.view", camera_view);

    for (unsigned int i = 0; i < num_point_lights; i++)
    {
        // calculate model matrix
        mat4 object_model = GLM_MAT4_IDENTITY_INIT;
        glm_translate(object_model, point_lights[i]->position);
        vec3 scale = { 0.125f, 0.125f, 0.125f };
        glm_scale(object_model, scale);

        program_set_mat4(forward_color_program, "object.model", object_model);

        program_set_vec3(forward_color_program, "material.color", point_lights[i]->specular);

        mesh_draw(cube_mesh);
    }

    program_unbind();

    // start water pass
    glViewport(0, 0, render_width, render_height);

    program_bind(water_program);

    program_set_mat4(water_program, "camera.projection", camera_projection);
    program_set_mat4(water_program, "camera.view", camera_view);
    program_set_vec3(water_program, "camera.position", camera->position);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skybox->texture_id);

    for (unsigned int i = 0; i < num_waters; i++)
    {
        // calculate model matrix
        mat4 water_model = GLM_MAT4_IDENTITY_INIT;
        glm_translate(water_model, waters[i]->position);
        vec3 water_scale;
        water_scale[0] = waters[i]->scale[0];
        water_scale[1] = 1.0f;
        water_scale[2] = waters[i]->scale[1];
        glm_scale(water_model, water_scale);

        program_set_mat4(water_program, "water.model", water_model);

        // draw water mesh
        glBindVertexArray(water_vao_id);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, water_vbo_id);
        glDrawArrays(GL_TRIANGLES, 0, num_water_vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDisableVertexAttribArray(0);
        glBindVertexArray(0);
    }

    program_unbind();

    // end water pass

    // start skybox pass
    glViewport(0, 0, render_width, render_height);
    glDepthFunc(GL_LEQUAL);

    mat4 camera_view_no_translate;
    glm_mat4_copy(camera_view, camera_view_no_translate);
    camera_view_no_translate[3][0] = 0.0f;
    camera_view_no_translate[3][1] = 0.0f;
    camera_view_no_translate[3][2] = 0.0f;
    camera_view_no_translate[3][3] = 0.0f;

    program_bind(skybox_program);

    program_set_mat4(skybox_program, "camera.projection", camera_projection);
    program_set_mat4(skybox_program, "camera.view", camera_view_no_translate);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skybox->texture_id);

    // draw sprite mesh
    glBindVertexArray(skybox_vao_id);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, skybox_vbo_id);
    glDrawArrays(GL_TRIANGLES, 0, num_skybox_vertices);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDisableVertexAttribArray(0);
    glBindVertexArray(0);

    program_unbind();

    // end skybox pass
    glDepthFunc(GL_LESS);

    // start sprite pass
    glViewport(0, 0, render_width, render_height);

    program_bind(sprite_program);

    program_set_mat4(sprite_program, "camera.projection", camera_projection);

    for (unsigned int i = 0; i < num_sprites; i++)
    {
        // calculate model matrix
        mat4 sprite_model = GLM_MAT4_IDENTITY_INIT;
        sprite_calc_model(sprites[i], sprite_model);

        program_set_mat4(sprite_program, "sprite.model", sprite_model);
        program_set_vec3(sprite_program, "sprite.color", sprites[i]->color);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, sprites[i]->image->texture_id);

        glBindVertexArray(sprite_vao_id);
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, sprite_vbo_id);
        glDrawArrays(GL_TRIANGLES, 0, num_sprite_vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glBindVertexArray(0);
    }

    program_unbind();

    // end sprite pass

    // unbind screen fbo
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // post processing pass
    glViewport(0, 0, render_width, render_height);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);

    program_bind(post_program);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, screen_texture_id);

    // draw screen mesh
    glBindVertexArray(screen_vao_id);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, screen_vbo_id);
    glDrawArrays(GL_TRIANGLES, 0, num_screen_vertices);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glBindVertexArray(0);

    program_unbind();

    num_objects = 0;
    num_directional_lights = 0;
    num_point_lights = 0;
    num_spot_lights = 0;
}

// TODO: delete all resources
void renderer_quit(void)
{
    glDeleteBuffers(1, &sprite_vbo_id);
    glDeleteVertexArrays(1, &sprite_vao_id);

    free(spot_lights);
    free(point_lights);
    free(objects);

    glDeleteRenderbuffers(1, &geometry_rbo_id);
    glDeleteTextures(1, &geometry_albedo_specular_texture_id);
    glDeleteTextures(1, &geometry_normal_texture_id);
    glDeleteTextures(1, &geometry_position_texture_id);
    glDeleteFramebuffers(1, &geometry_fbo_id);

    glDeleteTextures(1, &depthmap_texture_id);
    glDeleteFramebuffers(1, &depthmap_fbo_id);

    glDeleteRenderbuffers(1, &screen_rbo_id);
    glDeleteTextures(1, &screen_texture_id);
    glDeleteFramebuffers(1, &screen_fbo_id);

    program_destroy(depth_program);
    program_destroy(post_program);
    program_destroy(geometry_program);
    program_destroy(deferred_spot_program);
    program_destroy(deferred_point_program);
    program_destroy(deferred_directional_program);
    program_destroy(deferred_sun_program);
    program_destroy(forward_spot_program);
    program_destroy(forward_point_program);
    program_destroy(forward_directional_program);
    program_destroy(forward_sun_program);
    program_destroy(forward_color_program);

    mesh_destroy(cube_mesh);
    mesh_destroy(quad_mesh);

    material_destroy(default_material);

    texture_destroy(default_texture);
}
