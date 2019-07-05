#include <engine/engine.h>

// TODO: create a way to specify paths for engine assets and game assets separately

// TODO: support resizing
// we need to resize the viewport, as well as recreate all the framebuffers

// TODO: render a sun texture in the sky
// calculate its position based on the direction

// TODO: deferred reflections
// new gbuffer texture (2 bytes)
// byte 1: reflective map (just the red value)
// byte 2: material reflectivity

// TODO: user programmable part of the rendering pipeline

struct renderer
{
    // settings
    int render_width;
    int render_height;
    float render_scale;
    int shadow_width;
    int shadow_height;
    int reflection_width;
    int reflection_height;
    int refraction_width;
    int refraction_height;
    enum render_mode render_mode;

    // shader programs
    struct program *depth_program;
    struct program *forward_color_program;
    struct program *forward_sun_program;
    struct program *forward_directional_program;
    struct program *forward_point_program;
    struct program *forward_spot_program;
    struct program *forward_reflection_program;
    struct program *geometry_program;
    struct program *deferred_sun_program;
    struct program *deferred_directional_program;
    struct program *deferred_point_program;
    struct program *deferred_spot_program;
    struct program *skybox_program;
    struct program *water_program;
    struct program *sprite_program;
    struct program *post_program;

    // framebuffers
    GLuint screen_fbo_id;
    GLuint screen_texture_id;
    GLuint screen_rbo_id;

    GLuint depthmap_fbo_id;
    GLuint depthmap_texture_id;

    GLuint geometry_fbo_id;
    GLuint geometry_position_texture_id;
    GLuint geometry_normal_texture_id;
    GLuint geometry_albedo_specular_texture_id;
    GLuint geometry_rbo_id;

    GLuint water_reflection_fbo_id;
    GLuint water_reflection_color_texture_id;
    GLuint water_reflection_depth_texture_id;
    GLuint water_reflection_depth_rbo_id;

    GLuint water_refraction_fbo_id;
    GLuint water_refraction_color_texture_id;
    GLuint water_refraction_depth_texture_id;
    GLuint water_refraction_depth_rbo_id;

    // water mesh
    unsigned int num_water_vertices;
    GLuint water_vao_id;
    GLuint water_vbo_id;

    // skybox mesh
    unsigned int num_skybox_vertices;
    GLuint skybox_vao_id;
    GLuint skybox_vbo_id;

    // sprite mesh
    unsigned int num_sprite_vertices;
    GLuint sprite_vao_id;
    GLuint sprite_vbo_id;

    // screen mesh
    unsigned int num_screen_vertices;
    GLuint screen_vao_id;
    GLuint screen_vbo_id;

    // textures
    struct texture *water_dudv_texture;

    // renderables
    struct object **objects;
    unsigned long long num_objects;

    struct sun *sun;

    struct directional_light **directional_lights;
    unsigned long long num_directional_lights;

    struct point_light **point_lights;
    unsigned long long num_point_lights;

    struct spot_light **spot_lights;
    unsigned long long num_spot_lights;

    struct cubemap *skybox;

    struct water **waters;
    unsigned long long num_waters;

    struct sprite **sprites;
    unsigned long long num_sprites;
};

struct renderer renderer;

int renderer_init(int render_width, int render_height, float render_scale, int shadow_width, int shadow_height, struct texture *water_dudv_texture)
{
    renderer.render_width = render_width;
    renderer.render_height = render_height;
    renderer.render_scale = render_scale;
    renderer.shadow_width = shadow_width;
    renderer.shadow_height = shadow_height;
    renderer.reflection_width = render_width; // TODO: paramterize
    renderer.reflection_height = render_height;
    renderer.refraction_width = render_width;
    renderer.refraction_height = render_height;
    renderer.water_dudv_texture = water_dudv_texture;
    renderer.render_mode = RENDER_MODE_FORWARD;

    // setup opengl
    // TODO: face culling
    glViewport(0, 0, render_width, render_height);
    glClearColor(1.0f, 0.0f, 1.0f, 0.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_STENCIL_TEST);

    // create programs
    renderer.depth_program = program_create(
        "../engine/assets/shaders/depth.vert",
        "../engine/assets/shaders/depth.frag");

    if (!renderer.depth_program)
    {
        printf("Error: Couldn't create depth program\n");

        return 1;
    }

    renderer.forward_color_program = program_create(
        "../engine/assets/shaders/forward.vert",
        "../engine/assets/shaders/forward_color.frag");

    if (!renderer.forward_color_program)
    {
        printf("Error: Couldn't create forward color program\n");

        return 1;
    }

    renderer.forward_sun_program = program_create(
        "../engine/assets/shaders/forward.vert",
        "../engine/assets/shaders/forward_sun.frag");

    if (!renderer.forward_sun_program)
    {
        printf("Error: Couldn't create forward sun program\n");

        return 1;
    }

    renderer.forward_directional_program = program_create(
        "../engine/assets/shaders/forward.vert",
        "../engine/assets/shaders/forward_directional.frag");

    if (!renderer.forward_directional_program)
    {
        printf("Error: Couldn't create forward directional program\n");

        return 1;
    }

    renderer.forward_point_program = program_create(
        "../engine/assets/shaders/forward.vert",
        "../engine/assets/shaders/forward_point.frag");

    if (!renderer.forward_point_program)
    {
        printf("Error: Couldn't create forward point program\n");

        return 1;
    }

    renderer.forward_spot_program = program_create(
        "../engine/assets/shaders/forward.vert",
        "../engine/assets/shaders/forward_spot.frag");

    if (!renderer.forward_spot_program)
    {
        printf("Error: Couldn't create forward spot program\n");

        return 1;
    }

    renderer.forward_reflection_program = program_create(
        "../engine/assets/shaders/forward.vert",
        "../engine/assets/shaders/forward_reflection.frag");

    if (!renderer.forward_reflection_program)
    {
        printf("Error: Couldn't create forward reflection program\n");

        return 1;
    }

    renderer.geometry_program = program_create(
        "../engine/assets/shaders/geometry.vert",
        "../engine/assets/shaders/geometry.frag");

    if (!renderer.geometry_program)
    {
        printf("Error: Couldn't create geometry program\n");

        return 1;
    }

    renderer.deferred_sun_program = program_create(
        "../engine/assets/shaders/deferred.vert",
        "../engine/assets/shaders/deferred_sun.frag");

    if (!renderer.deferred_sun_program)
    {
        printf("Error: Couldn't create deferred sun program\n");

        return 1;
    }

    renderer.deferred_directional_program = program_create(
        "../engine/assets/shaders/deferred.vert",
        "../engine/assets/shaders/deferred_directional.frag");

    if (!renderer.deferred_directional_program)
    {
        printf("Error: Couldn't create deferred directional program\n");

        return 1;
    }

    renderer.deferred_point_program = program_create(
        "../engine/assets/shaders/deferred.vert",
        "../engine/assets/shaders/deferred_point.frag");

    if (!renderer.deferred_point_program)
    {
        printf("Error: Couldn't create deferred point program\n");

        return 1;
    }

    renderer.deferred_spot_program = program_create(
        "../engine/assets/shaders/deferred.vert",
        "../engine/assets/shaders/deferred_spot.frag");

    if (!renderer.deferred_spot_program)
    {
        printf("Error: Couldn't create deferred spot program\n");

        return 1;
    }

    renderer.skybox_program = program_create(
        "../engine/assets/shaders/skybox.vert",
        "../engine/assets/shaders/skybox.frag");

    if (!renderer.skybox_program)
    {
        printf("Error: Couldn't create skybox program\n");

        return 1;
    }

    renderer.water_program = program_create(
        "../engine/assets/shaders/water.vert",
        "../engine/assets/shaders/water.frag");

    if (!renderer.water_program)
    {
        printf("Error: Couldn't create water program\n");

        return 1;
    }

    renderer.sprite_program = program_create(
        "../engine/assets/shaders/sprite.vert",
        "../engine/assets/shaders/sprite.frag");

    if (!renderer.sprite_program)
    {
        printf("Error: Couldn't create sprite program\n");

        return 1;
    }

    renderer.post_program = program_create(
        "../engine/assets/shaders/post.vert",
        "../engine/assets/shaders/post.frag");

    if (!renderer.post_program)
    {
        printf("Error: Couldn't create post program\n");

        return 1;
    }

    // setup shader samplers
    program_bind(renderer.forward_sun_program);
    program_set_int(renderer.forward_sun_program, "material.diffuse", 0);
    program_set_int(renderer.forward_sun_program, "material.specular", 1);
    program_set_int(renderer.forward_sun_program, "material.normal", 2);
    program_set_int(renderer.forward_sun_program, "material.emission", 3);
    program_set_int(renderer.forward_sun_program, "depthmap.texture", 4);
    program_unbind();

    program_bind(renderer.forward_directional_program);
    program_set_int(renderer.forward_directional_program, "material.diffuse", 0);
    program_set_int(renderer.forward_directional_program, "material.specular", 1);
    program_set_int(renderer.forward_directional_program, "material.normal", 2);
    program_set_int(renderer.forward_directional_program, "material.emission", 3);
    program_unbind();

    program_bind(renderer.forward_point_program);
    program_set_int(renderer.forward_point_program, "material.diffuse", 0);
    program_set_int(renderer.forward_point_program, "material.specular", 1);
    program_set_int(renderer.forward_point_program, "material.normal", 2);
    program_set_int(renderer.forward_point_program, "material.emission", 3);
    program_unbind();

    program_bind(renderer.forward_spot_program);
    program_set_int(renderer.forward_spot_program, "material.diffuse", 0);
    program_set_int(renderer.forward_spot_program, "material.specular", 1);
    program_set_int(renderer.forward_spot_program, "material.normal", 2);
    program_set_int(renderer.forward_spot_program, "material.emission", 3);
    program_unbind();

    program_bind(renderer.forward_reflection_program);
    program_set_int(renderer.forward_reflection_program, "material.reflective", 0);
    program_set_int(renderer.forward_reflection_program, "skybox.texture", 1);
    program_unbind();

    program_bind(renderer.geometry_program);
    program_set_int(renderer.geometry_program, "material.diffuse", 0);
    program_set_int(renderer.geometry_program, "material.specular", 1);
    program_set_int(renderer.geometry_program, "material.normal", 2);
    program_set_int(renderer.geometry_program, "material.emission", 3);
    program_unbind();

    program_bind(renderer.deferred_sun_program);
    program_set_int(renderer.deferred_sun_program, "geometry.position", 0);
    program_set_int(renderer.deferred_sun_program, "geometry.normal", 1);
    program_set_int(renderer.deferred_sun_program, "geometry.albedo_specular", 2);
    program_set_int(renderer.deferred_sun_program, "depthmap.texture", 4);
    program_unbind();

    program_bind(renderer.deferred_directional_program);
    program_set_int(renderer.deferred_directional_program, "geometry.position", 0);
    program_set_int(renderer.deferred_directional_program, "geometry.normal", 1);
    program_set_int(renderer.deferred_directional_program, "geometry.albedo_specular", 2);
    program_unbind();

    program_bind(renderer.deferred_point_program);
    program_set_int(renderer.deferred_point_program, "geometry.position", 0);
    program_set_int(renderer.deferred_point_program, "geometry.normal", 1);
    program_set_int(renderer.deferred_point_program, "geometry.albedo_specular", 2);
    program_unbind();

    program_bind(renderer.deferred_spot_program);
    program_set_int(renderer.deferred_spot_program, "geometry.position", 0);
    program_set_int(renderer.deferred_spot_program, "geometry.normal", 1);
    program_set_int(renderer.deferred_spot_program, "geometry.albedo_specular", 2);
    program_unbind();

    program_bind(renderer.skybox_program);
    program_set_int(renderer.skybox_program, "skybox.texture", 0);
    program_unbind();

    program_bind(renderer.water_program);
    program_set_int(renderer.water_program, "water.reflection", 0);
    program_set_int(renderer.water_program, "water.refraction", 1);
    program_set_int(renderer.water_program, "water.dudv", 2);
    program_unbind();

    program_bind(renderer.sprite_program);
    program_set_int(renderer.sprite_program, "sprite.image", 0);
    program_unbind();

    program_bind(renderer.post_program);
    program_set_int(renderer.post_program, "screen.texture", 0);
    program_unbind();

    // setup screen fbo
    glGenFramebuffers(1, &renderer.screen_fbo_id);
    glBindFramebuffer(GL_FRAMEBUFFER, renderer.screen_fbo_id);

    glGenTextures(1, &renderer.screen_texture_id);
    glBindTexture(GL_TEXTURE_2D, renderer.screen_texture_id);

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
        renderer.screen_texture_id,
        0);

    glBindTexture(GL_TEXTURE_2D, 0);

    glGenRenderbuffers(1, &renderer.screen_rbo_id);
    glBindRenderbuffer(GL_RENDERBUFFER, renderer.screen_rbo_id);

    glRenderbufferStorage(
        GL_RENDERBUFFER,
        GL_DEPTH24_STENCIL8,
        render_width,
        render_height);

    glFramebufferRenderbuffer(
        GL_FRAMEBUFFER,
        GL_DEPTH_STENCIL_ATTACHMENT,
        GL_RENDERBUFFER,
        renderer.screen_rbo_id);

    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        printf("Error: Couldn't complete screen framebuffer\n");

        return 1;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // setup depthmap fbo
    glGenFramebuffers(1, &renderer.depthmap_fbo_id);
    glBindFramebuffer(GL_FRAMEBUFFER, renderer.depthmap_fbo_id);

    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    glGenTextures(1, &renderer.depthmap_texture_id);
    glBindTexture(GL_TEXTURE_2D, renderer.depthmap_texture_id);

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
        renderer.depthmap_texture_id,
        0);

    glBindTexture(GL_TEXTURE_2D, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        printf("Error: Couldn't complete depthmap framebuffer\n");

        return 1;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // setup geometry fbo
    glGenFramebuffers(1, &renderer.geometry_fbo_id);
    glBindFramebuffer(GL_FRAMEBUFFER, renderer.geometry_fbo_id);

    // TODO: dont store position because it can be recreated in the fragment shader from depth
    glGenTextures(1, &renderer.geometry_position_texture_id);
    glBindTexture(GL_TEXTURE_2D, renderer.geometry_position_texture_id);

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
        renderer.geometry_position_texture_id,
        0);

    glBindTexture(GL_TEXTURE_2D, 0);

    // TODO: find a way to optimize this
    glGenTextures(1, &renderer.geometry_normal_texture_id);
    glBindTexture(GL_TEXTURE_2D, renderer.geometry_normal_texture_id);

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
        renderer.geometry_normal_texture_id,
        0);

    glBindTexture(GL_TEXTURE_2D, 0);

    glGenTextures(1, &renderer.geometry_albedo_specular_texture_id);
    glBindTexture(GL_TEXTURE_2D, renderer.geometry_albedo_specular_texture_id);

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
        renderer.geometry_albedo_specular_texture_id,
        0);

    glBindTexture(GL_TEXTURE_2D, 0);

    GLenum geometry_fbo_attachments[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    glDrawBuffers(sizeof(geometry_fbo_attachments) / sizeof(GLenum), geometry_fbo_attachments);

    glGenRenderbuffers(1, &renderer.geometry_rbo_id);
    glBindRenderbuffer(GL_RENDERBUFFER, renderer.geometry_rbo_id);

    glRenderbufferStorage(
        GL_RENDERBUFFER,
        GL_DEPTH_COMPONENT,
        (GLsizei)(render_width * render_scale),
        (GLsizei)(render_height * render_scale));

    glFramebufferRenderbuffer(
        GL_FRAMEBUFFER,
        GL_DEPTH_ATTACHMENT,
        GL_RENDERBUFFER,
        renderer.geometry_rbo_id);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        printf("Error: Couldn't complete geometry framebuffer\n");

        return 1;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // setup water reflection fbo
    glGenFramebuffers(1, &renderer.water_reflection_fbo_id);
    glBindFramebuffer(GL_FRAMEBUFFER, renderer.water_reflection_fbo_id);

    glGenTextures(1, &renderer.water_reflection_color_texture_id);
    glBindTexture(GL_TEXTURE_2D, renderer.water_reflection_color_texture_id);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGB8,
        renderer.reflection_width,
        renderer.reflection_height,
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
        renderer.water_reflection_color_texture_id,
        0);

    glBindTexture(GL_TEXTURE_2D, 0);

    GLenum water_reflection_fbo_attachments[] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(sizeof(water_reflection_fbo_attachments) / sizeof(GLenum), water_reflection_fbo_attachments);

    glGenTextures(1, &renderer.water_reflection_depth_texture_id);
    glBindTexture(GL_TEXTURE_2D, renderer.water_reflection_depth_texture_id);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_DEPTH_COMPONENT32,
        renderer.reflection_width,
        renderer.reflection_height,
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
        renderer.water_reflection_depth_texture_id,
        0);

    glBindTexture(GL_TEXTURE_2D, 0);

    glGenRenderbuffers(1, &renderer.water_reflection_depth_rbo_id);
    glBindRenderbuffer(GL_RENDERBUFFER, renderer.water_reflection_depth_rbo_id);

    glRenderbufferStorage(
        GL_RENDERBUFFER,
        GL_DEPTH_COMPONENT,
        renderer.reflection_width,
        renderer.reflection_height);

    glFramebufferRenderbuffer(
        GL_FRAMEBUFFER,
        GL_DEPTH_ATTACHMENT,
        GL_RENDERBUFFER,
        renderer.water_reflection_depth_rbo_id);

    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        printf("Error: Couldn't complete water reflection framebuffer\n");

        return 1;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // setup water refraction fbo
    glGenFramebuffers(1, &renderer.water_refraction_fbo_id);
    glBindFramebuffer(GL_FRAMEBUFFER, renderer.water_refraction_fbo_id);

    glGenTextures(1, &renderer.water_refraction_color_texture_id);
    glBindTexture(GL_TEXTURE_2D, renderer.water_refraction_color_texture_id);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGB8,
        renderer.refraction_width,
        renderer.refraction_height,
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
        renderer.water_refraction_color_texture_id,
        0);

    glBindTexture(GL_TEXTURE_2D, 0);

    GLenum water_refraction_fbo_attachments[] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(sizeof(water_refraction_fbo_attachments) / sizeof(GLenum), water_refraction_fbo_attachments);

    glGenTextures(1, &renderer.water_refraction_depth_texture_id);
    glBindTexture(GL_TEXTURE_2D, renderer.water_refraction_depth_texture_id);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_DEPTH_COMPONENT32,
        renderer.refraction_width,
        renderer.refraction_height,
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
        renderer.water_refraction_depth_texture_id,
        0);

    glBindTexture(GL_TEXTURE_2D, 0);

    glGenRenderbuffers(1, &renderer.water_refraction_depth_rbo_id);
    glBindRenderbuffer(GL_RENDERBUFFER, renderer.water_refraction_depth_rbo_id);

    glRenderbufferStorage(
        GL_RENDERBUFFER,
        GL_DEPTH_COMPONENT,
        renderer.refraction_width,
        renderer.refraction_height);

    glFramebufferRenderbuffer(
        GL_FRAMEBUFFER,
        GL_DEPTH_ATTACHMENT,
        GL_RENDERBUFFER,
        renderer.water_refraction_depth_rbo_id);

    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        printf("Error: Couldn't complete water refraction framebuffer\n");

        return 1;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // create water mesh
    float water_vertices[] = {
        // position   // uv
        -1.0f, -1.0f, 0.0f, 0.0f,
        -1.0f,  1.0f, 0.0f, 1.0f,
         1.0f, -1.0f, 1.0f, 0.0f,
         1.0f, -1.0f, 1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f, 1.0f,
         1.0f,  1.0f, 1.0f, 1.0f
    };
    renderer.num_water_vertices = sizeof(water_vertices) / sizeof(float) / 4;

    glGenVertexArrays(1, &renderer.water_vao_id);
    glGenBuffers(1, &renderer.water_vbo_id);

    glBindVertexArray(renderer.water_vao_id);

    glBindBuffer(GL_ARRAY_BUFFER, renderer.water_vbo_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(water_vertices), &water_vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid *)(0 * sizeof(GLfloat))); // position
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid *)(2 * sizeof(GLfloat))); // uv
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

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
    renderer.num_skybox_vertices = sizeof(skybox_vertices) / sizeof(float) / 3;

    glGenVertexArrays(1, &renderer.skybox_vao_id);
    glGenBuffers(1, &renderer.skybox_vbo_id);

    glBindVertexArray(renderer.skybox_vao_id);

    glBindBuffer(GL_ARRAY_BUFFER, renderer.skybox_vbo_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skybox_vertices), &skybox_vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid *)(0 * sizeof(GLfloat))); // position
    glEnableVertexAttribArray(0);

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
    renderer.num_sprite_vertices = sizeof(sprite_vertices) / sizeof(float) / 4;

    glGenVertexArrays(1, &renderer.sprite_vao_id);
    glGenBuffers(1, &renderer.sprite_vbo_id);

    glBindVertexArray(renderer.sprite_vao_id);

    glBindBuffer(GL_ARRAY_BUFFER, renderer.sprite_vbo_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(sprite_vertices), &sprite_vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid *)(0 * sizeof(GLfloat))); // position
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid *)(2 * sizeof(GLfloat))); // uv
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    // create screen mesh
    float screen_vertices[] = {
        // position   // uv
        -1.0f, -1.0f, 0.0f, 0.0f,
        -1.0f,  1.0f, 0.0f, 1.0f,
         1.0f, -1.0f, 1.0f, 0.0f,
         1.0f, -1.0f, 1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f, 1.0f,
         1.0f,  1.0f, 1.0f, 1.0f
    };
    renderer.num_screen_vertices = sizeof(screen_vertices) / sizeof(float) / 4;

    glGenVertexArrays(1, &renderer.screen_vao_id);
    glGenBuffers(1, &renderer.screen_vbo_id);

    glBindVertexArray(renderer.screen_vao_id);

    glBindBuffer(GL_ARRAY_BUFFER, renderer.screen_vbo_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(screen_vertices), &screen_vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid *)(0 * sizeof(GLfloat))); // position
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid *)(2 * sizeof(GLfloat))); // uv
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    // create water dudv texture

    return 0;
}

void renderer_set_mode(enum render_mode _render_mode)
{
    renderer.render_mode = _render_mode;
}

void renderer_add_object(struct object *object)
{
    struct object **objects = realloc(renderer.objects, (renderer.num_objects + 1) * sizeof(struct object *));

    if (!objects)
    {
        printf("Error: Couldn't reallocate object array\n");

        return;
    }

    renderer.objects = objects;
    renderer.objects[renderer.num_objects++] = object;
}

void renderer_set_sun(struct sun *_sun)
{
    renderer.sun = _sun;
}

void renderer_add_directional_light(struct directional_light *directional_light)
{
    struct directional_light **directional_lights = realloc(renderer.directional_lights, (renderer.num_directional_lights + 1) * sizeof(struct directional_light *));

    if (!directional_lights)
    {
        printf("Error: Couldn't reallocate directional light array\n");

        return;
    }

    renderer.directional_lights = directional_lights;
    renderer.directional_lights[renderer.num_directional_lights++] = directional_light;
}

void renderer_add_point_light(struct point_light *point_light)
{
    struct point_light **point_lights = realloc(renderer.point_lights, (renderer.num_point_lights + 1) * sizeof(struct point_light *));

    if (!point_lights)
    {
        printf("Error: Couldn't reallocate point light array\n");

        return;
    }

    renderer.point_lights = point_lights;
    renderer.point_lights[renderer.num_point_lights++] = point_light;
}

void renderer_add_spot_light(struct spot_light *spot_light)
{
    struct spot_light **spot_lights = realloc(renderer.spot_lights, (renderer.num_spot_lights + 1) * sizeof(struct spot_light *));

    if (!spot_lights)
    {
        printf("Error: Couldn't reallocate spot light array\n");

        return;
    }

    renderer.spot_lights = spot_lights;
    renderer.spot_lights[renderer.num_spot_lights++] = spot_light;
}

void renderer_set_skybox(struct cubemap *_skybox)
{
    renderer.skybox = _skybox;
}

void renderer_add_water(struct water *water)
{
    struct water **waters = realloc(renderer.waters, (renderer.num_waters + 1) * sizeof(struct water *));

    if (!waters)
    {
        printf("Error: Couldn't reallocate water array\n");

        return;
    }

    renderer.waters = waters;
    renderer.waters[renderer.num_waters++] = water;
}

void renderer_add_sprite(struct sprite *sprite)
{
    struct sprite **new_sprites = realloc(renderer.sprites, (renderer.num_sprites + 1) * sizeof(struct sprite *));

    if (!new_sprites)
    {
        printf("Error: Couldn't reallocate sprite array\n");

        return;
    }

    renderer.sprites = new_sprites;
    renderer.sprites[renderer.num_sprites++] = sprite;
}

void render_scene(GLuint fbo_id, struct camera *camera, float aspect, vec4 clipping_plane)
{
    // clear the framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_id);

    glViewport(0, 0, renderer.render_width, renderer.render_height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // calculate camera projection matrix
    mat4 camera_projection;
    glm_perspective(
        glm_rad(camera->fov),
        aspect,
        0.01f,
        100.0f,
        camera_projection);

    // calculate camera view matrix
    vec3 camera_front;
    camera_calc_front(camera, &camera_front);

    vec3 camera_target;
    glm_vec_add(camera->position, camera_front, camera_target);

    vec3 camera_up;
    camera_calc_up(camera, &camera_up);

    mat4 camera_view;
    glm_lookat(
        camera->position,
        camera_target,
        camera_up,
        camera_view);

    mat4 sun_projection;
    mat4 sun_view;
    if (renderer.sun)
    {
        // calculate sun projection matrix
        glm_ortho(-10.0f, 10.0f, -10.0f, 10.0f, -10.0f, 10.0f, sun_projection);

        // calculate sun "position"
        vec3 sun_position;
        glm_vec_sub(GLM_VEC3_ZERO, renderer.sun->direction, sun_position);

        // calculate sun view matrix
        glm_lookat(sun_position, GLM_VEC3_ZERO, GLM_YUP, sun_view);

        // render sun shadows to depthmap
        glBindFramebuffer(GL_FRAMEBUFFER, renderer.depthmap_fbo_id);

        glViewport(0, 0, renderer.shadow_width, renderer.shadow_height);
        glClear(GL_DEPTH_BUFFER_BIT);

        program_bind(renderer.depth_program);

        program_set_mat4(renderer.depth_program, "sun.projection", sun_projection);
        program_set_mat4(renderer.depth_program, "sun.view", sun_view);

        for (unsigned int i = 0; i < renderer.num_objects; i++)
        {
            struct object *object = renderer.objects[i];

            mat4 object_model = GLM_MAT4_IDENTITY_INIT;
            object_calc_model(object, object_model);

            program_set_mat4(renderer.depth_program, "object.model", object_model);

            mesh_draw(object->mesh);
        }

        program_unbind();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    switch (renderer.render_mode)
    {
    case RENDER_MODE_FORWARD:
    {
        // draw objects to the specified framebuffer
        if (renderer.num_objects > 0)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, fbo_id);

            glViewport(0, 0, renderer.render_width, renderer.render_height);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glEnable(GL_CLIP_DISTANCE0);

            for (unsigned int i = 0; i < renderer.num_objects; i++)
            {
                struct object *object = renderer.objects[i];

                mat4 object_model = GLM_MAT4_IDENTITY_INIT;
                object_calc_model(object, object_model);

                if (renderer.sun)
                {
                    program_bind(renderer.forward_sun_program);

                    program_set_mat4(renderer.forward_sun_program, "camera.projection", camera_projection);
                    program_set_mat4(renderer.forward_sun_program, "camera.view", camera_view);
                    program_set_vec3(renderer.forward_sun_program, "camera.position", camera->position);

                    program_set_mat4(renderer.forward_sun_program, "object.model", object_model);

                    program_set_vec4(renderer.forward_sun_program, "clipping_plane", clipping_plane);

                    program_set_vec3(renderer.forward_sun_program, "material.color", object->material->color);
                    program_set_float(renderer.forward_sun_program, "material.shininess", object->material->shininess);
                    program_set_float(renderer.forward_sun_program, "material.glow", object->material->glow);

                    program_set_vec3(renderer.forward_sun_program, "sun.direction", renderer.sun->direction);
                    program_set_vec3(renderer.forward_sun_program, "sun.ambient", renderer.sun->ambient);
                    program_set_vec3(renderer.forward_sun_program, "sun.diffuse", renderer.sun->diffuse);
                    program_set_vec3(renderer.forward_sun_program, "sun.specular", renderer.sun->specular);
                    program_set_mat4(renderer.forward_sun_program, "sun.projection", sun_projection);
                    program_set_mat4(renderer.forward_sun_program, "sun.view", sun_view);

                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, object->material->diffuse ? object->material->diffuse->texture_id : 0);
                    glActiveTexture(GL_TEXTURE1);
                    glBindTexture(GL_TEXTURE_2D, object->material->specular ? object->material->specular->texture_id : 0);
                    glActiveTexture(GL_TEXTURE2);
                    glBindTexture(GL_TEXTURE_2D, object->material->normal ? object->material->normal->texture_id : 0);
                    glActiveTexture(GL_TEXTURE3);
                    glBindTexture(GL_TEXTURE_2D, object->material->emission ? object->material->emission->texture_id : 0);
                    glActiveTexture(GL_TEXTURE4);
                    glBindTexture(GL_TEXTURE_2D, renderer.depthmap_texture_id);

                    mesh_draw(object->mesh);

                    program_unbind();
                }

                glEnable(GL_BLEND);
                glBlendFunc(GL_ONE, GL_ONE);
                glDepthMask(GL_FALSE);
                glDepthFunc(GL_EQUAL);

                if (renderer.num_directional_lights > 0)
                {
                    program_bind(renderer.forward_directional_program);

                    program_set_mat4(renderer.forward_directional_program, "camera.projection", camera_projection);
                    program_set_mat4(renderer.forward_directional_program, "camera.view", camera_view);
                    program_set_vec3(renderer.forward_directional_program, "camera.position", camera->position);

                    program_set_mat4(renderer.forward_directional_program, "object.model", object_model);

                    program_set_vec4(renderer.forward_directional_program, "clipping_plane", clipping_plane);

                    program_set_vec3(renderer.forward_directional_program, "material.color", object->material->color);
                    program_set_float(renderer.forward_directional_program, "material.shininess", object->material->shininess);
                    program_set_float(renderer.forward_directional_program, "material.glow", object->material->glow);

                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, object->material->diffuse ? object->material->diffuse->texture_id : 0);
                    glActiveTexture(GL_TEXTURE1);
                    glBindTexture(GL_TEXTURE_2D, object->material->specular ? object->material->specular->texture_id : 0);
                    glActiveTexture(GL_TEXTURE2);
                    glBindTexture(GL_TEXTURE_2D, object->material->normal ? object->material->normal->texture_id : 0);
                    glActiveTexture(GL_TEXTURE3);
                    glBindTexture(GL_TEXTURE_2D, object->material->emission ? object->material->emission->texture_id : 0);

                    for (unsigned int j = 0; j < renderer.num_directional_lights; j++)
                    {
                        struct directional_light *directional_light = renderer.directional_lights[j];

                        program_set_vec3(renderer.forward_directional_program, "directional_light.direction", directional_light->direction);
                        program_set_vec3(renderer.forward_directional_program, "directional_light.ambient", directional_light->ambient);
                        program_set_vec3(renderer.forward_directional_program, "directional_light.diffuse", directional_light->diffuse);
                        program_set_vec3(renderer.forward_directional_program, "directional_light.specular", directional_light->specular);

                        mesh_draw(object->mesh);
                    }

                    program_unbind();
                }

                if (renderer.num_point_lights > 0)
                {
                    program_bind(renderer.forward_point_program);

                    program_set_mat4(renderer.forward_point_program, "camera.projection", camera_projection);
                    program_set_mat4(renderer.forward_point_program, "camera.view", camera_view);
                    program_set_vec3(renderer.forward_point_program, "camera.position", camera->position);

                    program_set_mat4(renderer.forward_point_program, "object.model", object_model);

                    program_set_vec4(renderer.forward_point_program, "clipping_plane", clipping_plane);

                    program_set_vec3(renderer.forward_point_program, "material.color", object->material->color);
                    program_set_float(renderer.forward_point_program, "material.shininess", object->material->shininess);
                    program_set_float(renderer.forward_point_program, "material.glow", object->material->glow);

                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, object->material->diffuse ? object->material->diffuse->texture_id : 0);
                    glActiveTexture(GL_TEXTURE1);
                    glBindTexture(GL_TEXTURE_2D, object->material->specular ? object->material->specular->texture_id : 0);
                    glActiveTexture(GL_TEXTURE2);
                    glBindTexture(GL_TEXTURE_2D, object->material->normal ? object->material->normal->texture_id : 0);
                    glActiveTexture(GL_TEXTURE3);
                    glBindTexture(GL_TEXTURE_2D, object->material->emission ? object->material->emission->texture_id : 0);

                    for (unsigned int j = 0; j < renderer.num_point_lights; j++)
                    {
                        struct point_light *point_light = renderer.point_lights[j];

                        program_set_vec3(renderer.forward_point_program, "point_light.position", point_light->position);
                        program_set_vec3(renderer.forward_point_program, "point_light.ambient", point_light->ambient);
                        program_set_vec3(renderer.forward_point_program, "point_light.diffuse", point_light->diffuse);
                        program_set_vec3(renderer.forward_point_program, "point_light.specular", point_light->specular);
                        program_set_vec3(renderer.forward_point_program, "point_light.attenuation", point_light->attenuation);

                        mesh_draw(object->mesh);
                    }

                    program_unbind();
                }

                if (renderer.num_spot_lights > 0)
                {
                    program_bind(renderer.forward_spot_program);

                    program_set_mat4(renderer.forward_spot_program, "camera.projection", camera_projection);
                    program_set_mat4(renderer.forward_spot_program, "camera.view", camera_view);
                    program_set_vec3(renderer.forward_spot_program, "camera.position", camera->position);

                    program_set_mat4(renderer.forward_spot_program, "object.model", object_model);

                    program_set_vec4(renderer.forward_spot_program, "clipping_plane", clipping_plane);

                    program_set_vec3(renderer.forward_spot_program, "material.color", object->material->color);
                    program_set_float(renderer.forward_spot_program, "material.shininess", object->material->shininess);
                    program_set_float(renderer.forward_spot_program, "material.glow", object->material->glow);

                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, object->material->diffuse ? object->material->diffuse->texture_id : 0);
                    glActiveTexture(GL_TEXTURE1);
                    glBindTexture(GL_TEXTURE_2D, object->material->specular ? object->material->specular->texture_id : 0);
                    glActiveTexture(GL_TEXTURE2);
                    glBindTexture(GL_TEXTURE_2D, object->material->normal ? object->material->normal->texture_id : 0);
                    glActiveTexture(GL_TEXTURE3);
                    glBindTexture(GL_TEXTURE_2D, object->material->emission ? object->material->emission->texture_id : 0);

                    for (unsigned int j = 0; j < renderer.num_spot_lights; j++)
                    {
                        struct spot_light *spot_light = renderer.spot_lights[j];

                        program_set_vec3(renderer.forward_spot_program, "spot_light.position", spot_light->position);
                        program_set_vec3(renderer.forward_spot_program, "spot_light.direction", spot_light->direction);
                        program_set_vec3(renderer.forward_spot_program, "spot_light.ambient", spot_light->ambient);
                        program_set_vec3(renderer.forward_spot_program, "spot_light.diffuse", spot_light->diffuse);
                        program_set_vec3(renderer.forward_spot_program, "spot_light.specular", spot_light->specular);
                        program_set_vec3(renderer.forward_spot_program, "spot_light.attenuation", spot_light->attenuation);
                        program_set_float(renderer.forward_spot_program, "spot_light.inner_cutoff", spot_light->inner_cutoff);
                        program_set_float(renderer.forward_spot_program, "spot_light.outer_cutoff", spot_light->outer_cutoff);

                        mesh_draw(object->mesh);
                    }

                    program_unbind();
                }

                // TODO: material reflection parameters
                if (renderer.skybox)
                {
                    program_bind(renderer.forward_reflection_program);

                    program_set_mat4(renderer.forward_reflection_program, "camera.projection", camera_projection);
                    program_set_mat4(renderer.forward_reflection_program, "camera.view", camera_view);
                    program_set_vec3(renderer.forward_reflection_program, "camera.position", camera->position);

                    program_set_mat4(renderer.forward_reflection_program, "object.model", object_model);

                    program_set_vec4(renderer.forward_reflection_program, "clipping_plane", clipping_plane);

                    program_set_float(renderer.forward_reflection_program, "material.reflectivity", 0.0f);

                    // TEMPORARY: use specular texture for reflectivity
                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, object->material->specular ? object->material->specular->texture_id : 0);
                    glActiveTexture(GL_TEXTURE1);
                    glBindTexture(GL_TEXTURE_CUBE_MAP, renderer.skybox->texture_id);

                    mesh_draw(object->mesh);

                    program_unbind();
                }

                glDepthFunc(GL_LESS);
                glDepthMask(GL_TRUE);
                glDisable(GL_BLEND);
            }

            glDisable(GL_CLIP_DISTANCE0);

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }
    }
    break;
    case RENDER_MODE_DEFERRED:
    {
        // draw objects to the gbuffer
        if (renderer.num_objects > 0)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, renderer.geometry_fbo_id);

            glViewport(0, 0, (GLsizei)(renderer.render_width * renderer.render_scale), (GLsizei)(renderer.render_height * renderer.render_scale));
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glEnable(GL_CLIP_DISTANCE0);

            program_bind(renderer.geometry_program);

            program_set_mat4(renderer.geometry_program, "camera.projection", camera_projection);
            program_set_mat4(renderer.geometry_program, "camera.view", camera_view);
            program_set_vec3(renderer.geometry_program, "camera.position", camera->position);

            for (unsigned int i = 0; i < renderer.num_objects; i++)
            {
                struct object *object = renderer.objects[i];

                mat4 object_model = GLM_MAT4_IDENTITY_INIT;
                object_calc_model(object, object_model);

                program_set_mat4(renderer.geometry_program, "object.model", object_model);

                program_set_vec4(renderer.geometry_program, "clipping_plane", clipping_plane);

                program_set_vec3(renderer.geometry_program, "material.color", object->material->color);
                program_set_float(renderer.geometry_program, "material.shininess", object->material->shininess);
                program_set_float(renderer.geometry_program, "material.glow", object->material->glow);

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, object->material->diffuse ? object->material->diffuse->texture_id : 0);
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, object->material->specular ? object->material->specular->texture_id : 0);
                glActiveTexture(GL_TEXTURE2);
                glBindTexture(GL_TEXTURE_2D, object->material->normal ? object->material->normal->texture_id : 0);
                glActiveTexture(GL_TEXTURE3);
                glBindTexture(GL_TEXTURE_2D, object->material->emission ? object->material->emission->texture_id : 0);

                mesh_draw(object->mesh);
            }

            program_unbind();

            glDisable(GL_CLIP_DISTANCE0);

            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            // draw the gbuffer to the specified framebuffer
            glBindFramebuffer(GL_FRAMEBUFFER, fbo_id);

            glViewport(0, 0, renderer.render_width, renderer.render_height);
            glClear(GL_COLOR_BUFFER_BIT);
            glDisable(GL_DEPTH_TEST);

            if (renderer.sun)
            {
                program_bind(renderer.deferred_sun_program);

                program_set_vec3(renderer.deferred_sun_program, "camera.position", camera->position);

                program_set_vec3(renderer.deferred_sun_program, "sun.direction", renderer.sun->direction);
                program_set_vec3(renderer.deferred_sun_program, "sun.ambient", renderer.sun->ambient);
                program_set_vec3(renderer.deferred_sun_program, "sun.diffuse", renderer.sun->diffuse);
                program_set_vec3(renderer.deferred_sun_program, "sun.specular", renderer.sun->specular);
                program_set_mat4(renderer.deferred_sun_program, "sun.projection", sun_projection);
                program_set_mat4(renderer.deferred_sun_program, "sun.view", sun_view);

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, renderer.geometry_position_texture_id);
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, renderer.geometry_normal_texture_id);
                glActiveTexture(GL_TEXTURE2);
                glBindTexture(GL_TEXTURE_2D, renderer.geometry_albedo_specular_texture_id);
                glActiveTexture(GL_TEXTURE3);
                glBindTexture(GL_TEXTURE_2D, renderer.depthmap_texture_id);

                glBindVertexArray(renderer.screen_vao_id);
                glDrawArrays(GL_TRIANGLES, 0, renderer.num_screen_vertices);
                glBindVertexArray(0);

                program_unbind();
            }

            glEnable(GL_BLEND);
            glBlendFunc(GL_ONE, GL_ONE);
            glDepthMask(GL_FALSE);
            glDepthFunc(GL_EQUAL);

            if (renderer.num_directional_lights > 0)
            {
                program_bind(renderer.deferred_directional_program);

                program_set_vec3(renderer.deferred_directional_program, "camera.position", camera->position);

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, renderer.geometry_position_texture_id);
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, renderer.geometry_normal_texture_id);
                glActiveTexture(GL_TEXTURE2);
                glBindTexture(GL_TEXTURE_2D, renderer.geometry_albedo_specular_texture_id);

                for (unsigned int i = 0; i < renderer.num_directional_lights; i++)
                {
                    struct directional_light *directional_light = renderer.directional_lights[i];

                    program_set_vec3(renderer.deferred_directional_program, "directional_light.direction", directional_light->direction);
                    program_set_vec3(renderer.deferred_directional_program, "directional_light.ambient", directional_light->ambient);
                    program_set_vec3(renderer.deferred_directional_program, "directional_light.diffuse", directional_light->diffuse);
                    program_set_vec3(renderer.deferred_directional_program, "directional_light.specular", directional_light->specular);

                    glBindVertexArray(renderer.screen_vao_id);
                    glDrawArrays(GL_TRIANGLES, 0, renderer.num_screen_vertices);
                    glBindVertexArray(0);
                }

                program_unbind();
            }

            if (renderer.num_point_lights > 0)
            {
                program_bind(renderer.deferred_point_program);

                program_set_vec3(renderer.deferred_point_program, "camera.position", camera->position);

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, renderer.geometry_position_texture_id);
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, renderer.geometry_normal_texture_id);
                glActiveTexture(GL_TEXTURE2);
                glBindTexture(GL_TEXTURE_2D, renderer.geometry_albedo_specular_texture_id);

                for (unsigned int i = 0; i < renderer.num_point_lights; i++)
                {
                    struct point_light *point_light = renderer.point_lights[i];

                    program_set_vec3(renderer.deferred_point_program, "point_light.position", point_light->position);
                    program_set_vec3(renderer.deferred_point_program, "point_light.ambient", point_light->ambient);
                    program_set_vec3(renderer.deferred_point_program, "point_light.diffuse", point_light->diffuse);
                    program_set_vec3(renderer.deferred_point_program, "point_light.specular", point_light->specular);
                    program_set_vec3(renderer.deferred_point_program, "point_light.attenuation", point_light->attenuation);

                    glBindVertexArray(renderer.screen_vao_id);
                    glDrawArrays(GL_TRIANGLES, 0, renderer.num_screen_vertices);
                    glBindVertexArray(0);
                }

                program_unbind();
            }

            if (renderer.num_spot_lights > 0)
            {
                program_bind(renderer.deferred_spot_program);

                program_set_vec3(renderer.deferred_spot_program, "camera.position", camera->position);

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, renderer.geometry_position_texture_id);
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, renderer.geometry_normal_texture_id);
                glActiveTexture(GL_TEXTURE2);
                glBindTexture(GL_TEXTURE_2D, renderer.geometry_albedo_specular_texture_id);

                for (unsigned int i = 0; i < renderer.num_spot_lights; i++)
                {
                    struct spot_light *spot_light = renderer.spot_lights[i];

                    program_set_vec3(renderer.deferred_spot_program, "spot_light.position", spot_light->position);
                    program_set_vec3(renderer.deferred_spot_program, "spot_light.direction", spot_light->direction);
                    program_set_vec3(renderer.deferred_spot_program, "spot_light.ambient", spot_light->ambient);
                    program_set_vec3(renderer.deferred_spot_program, "spot_light.diffuse", spot_light->diffuse);
                    program_set_vec3(renderer.deferred_spot_program, "spot_light.specular", spot_light->specular);
                    program_set_vec3(renderer.deferred_spot_program, "spot_light.attenuation", spot_light->attenuation);
                    program_set_float(renderer.deferred_spot_program, "spot_light.inner_cutoff", spot_light->inner_cutoff);
                    program_set_float(renderer.deferred_spot_program, "spot_light.outer_cutoff", spot_light->outer_cutoff);

                    glBindVertexArray(renderer.screen_vao_id);
                    glDrawArrays(GL_TRIANGLES, 0, renderer.num_screen_vertices);
                    glBindVertexArray(0);
                }

                program_unbind();
            }

            // TODO: reflections
            if (renderer.skybox)
            {

            }

            // TODO: emissive lighting

            glDepthFunc(GL_LESS);
            glDepthMask(GL_TRUE);
            glDisable(GL_BLEND);

            glEnable(GL_DEPTH_TEST);

            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            // copy depth information to specified framebuffer
            glBindFramebuffer(GL_READ_FRAMEBUFFER, renderer.geometry_fbo_id);
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo_id);
            glBlitFramebuffer(0, 0, renderer.render_width, renderer.render_height, 0, 0, renderer.render_width, renderer.render_height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }
    }
    break;
    }

    // draw other things that need to be drawn after the deferred rendering pass
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_id);

    glViewport(0, 0, renderer.render_width, renderer.render_height);

    // render skybox
    if (renderer.skybox)
    {
        glDepthFunc(GL_LEQUAL);

        mat4 camera_view_no_translate;
        glm_mat4_copy(camera_view, camera_view_no_translate);
        camera_view_no_translate[3][0] = 0.0f;
        camera_view_no_translate[3][1] = 0.0f;
        camera_view_no_translate[3][2] = 0.0f;
        camera_view_no_translate[3][3] = 0.0f;

        program_bind(renderer.skybox_program);

        program_set_mat4(renderer.skybox_program, "camera.projection", camera_projection);
        program_set_mat4(renderer.skybox_program, "camera.view", camera_view_no_translate);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, renderer.skybox->texture_id);

        glBindVertexArray(renderer.skybox_vao_id);
        glDrawArrays(GL_TRIANGLES, 0, renderer.num_skybox_vertices);
        glBindVertexArray(0);

        program_unbind();

        glDepthFunc(GL_LESS);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void render_waters(GLuint fbo_id, struct camera *camera, float aspect, unsigned int elapsed_time)
{
    // calculate camera projection matrix
    mat4 camera_projection;
    glm_perspective(
        glm_rad(camera->fov),
        aspect,
        0.01f,
        100.0f,
        camera_projection);

    // calculate camera view matrix
    vec3 camera_front;
    camera_calc_front(camera, &camera_front);

    vec3 camera_target;
    glm_vec_add(camera->position, camera_front, camera_target);

    vec3 camera_up;
    camera_calc_up(camera, &camera_up);

    mat4 camera_view;
    glm_lookat(
        camera->position,
        camera_target,
        camera_up,
        camera_view);

    // send camera transformations to water program
    program_bind(renderer.water_program);

    program_set_mat4(renderer.water_program, "camera.projection", camera_projection);
    program_set_mat4(renderer.water_program, "camera.view", camera_view);

    program_set_unsigned_int(renderer.water_program, "elapsed_time", elapsed_time);

    program_unbind();

    for (unsigned int i = 0; i < renderer.num_waters; i++)
    {
        struct water *water = renderer.waters[i];

        // render water reflection
        vec4 reflection_clipping_plane = { 0.0f, 1.0f, 0.0f, -water->position[1] };

        float old_camera_y = camera->position[1];
        float old_camera_pitch = camera->pitch;

        camera->position[1] -= 2 * (camera->position[1] - water->position[1]);
        camera->pitch = -camera->pitch;

        render_scene(renderer.water_reflection_fbo_id, camera, aspect, reflection_clipping_plane);

        camera->position[1] = old_camera_y;
        camera->pitch = old_camera_pitch;

        // render water refraction
        vec4 refraction_clipping_plane = { 0.0f, -1.0f, 0.0f, water->position[1] };

        render_scene(renderer.water_refraction_fbo_id, camera, aspect, refraction_clipping_plane);

        glBindFramebuffer(GL_FRAMEBUFFER, fbo_id);

        program_bind(renderer.water_program);

        mat4 water_model = GLM_MAT4_IDENTITY_INIT;
        glm_translate(water_model, water->position);
        vec3 water_scale;
        water_scale[0] = water->scale[0];
        water_scale[1] = 1.0f;
        water_scale[2] = water->scale[1];
        glm_scale(water_model, water_scale);

        program_set_mat4(renderer.water_program, "water.model", water_model);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, renderer.water_reflection_color_texture_id);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, renderer.water_refraction_color_texture_id);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, renderer.water_dudv_texture ? renderer.water_dudv_texture->texture_id : 0);

        glBindVertexArray(renderer.water_vao_id);
        glDrawArrays(GL_TRIANGLES, 0, renderer.num_water_vertices);
        glBindVertexArray(0);

        program_unbind();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}

void render_sprites(GLuint fbo_id, float aspect)
{
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_id);

    program_bind(renderer.sprite_program);

    // calculate ortho projection
    mat4 camera_projection_ortho;
    glm_ortho_default(aspect, camera_projection_ortho);

    program_set_mat4(renderer.sprite_program, "camera.projection", camera_projection_ortho);

    for (unsigned int i = 0; i < renderer.num_sprites; i++)
    {
        struct sprite *sprite = renderer.sprites[i];

        mat4 sprite_model = GLM_MAT4_IDENTITY_INIT;
        sprite_calc_model(sprite, sprite_model);

        program_set_mat4(renderer.sprite_program, "sprite.model", sprite_model);
        program_set_vec3(renderer.sprite_program, "sprite.color", sprite->color);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, sprite->image->texture_id);

        glBindVertexArray(renderer.sprite_vao_id);
        glDrawArrays(GL_TRIANGLES, 0, renderer.num_sprite_vertices);
        glBindVertexArray(0);
    }

    program_unbind();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void render_debug(GLuint fbo_id, struct camera *camera, float aspect)
{
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_id);

    // calculate camera projection matrix
    mat4 camera_projection;
    glm_perspective(
        glm_rad(camera->fov),
        aspect,
        0.01f,
        100.0f,
        camera_projection);

    // calculate camera view matrix
    vec3 camera_front;
    camera_calc_front(camera, &camera_front);

    vec3 camera_target;
    glm_vec_add(camera->position, camera_front, camera_target);

    vec3 camera_up;
    camera_calc_up(camera, &camera_up);

    mat4 camera_view;
    glm_lookat(
        camera->position,
        camera_target,
        camera_up,
        camera_view);

    // TEST: draw point lights as a cube
    if (renderer.num_point_lights > 0)
    {
        program_bind(renderer.forward_color_program);

        program_set_mat4(renderer.forward_color_program, "camera.projection", camera_projection);
        program_set_mat4(renderer.forward_color_program, "camera.view", camera_view);

        for (unsigned int i = 0; i < renderer.num_point_lights; i++)
        {
            struct point_light *point_light = renderer.point_lights[i];

            mat4 object_model = GLM_MAT4_IDENTITY_INIT;
            glm_translate(object_model, point_light->position);
            vec3 scale = { 0.125f, 0.125f, 0.125f };
            glm_scale(object_model, scale);

            program_set_mat4(renderer.forward_color_program, "object.model", object_model);

            program_set_vec3(renderer.forward_color_program, "material.color", point_light->specular);

            static GLint light_vao_id = -1;
            static GLint light_vbo_id = -1;
            static GLint num_light_vertices = -1;

            if (light_vao_id == -1)
            {
                float light_vertices[] = {
                    -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
                     1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
                     1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
                     1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
                    -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
                    -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
                    -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
                     1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
                     1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
                     1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
                    -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
                    -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
                    -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
                    -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
                    -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
                    -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
                    -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
                    -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
                     1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
                     1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
                     1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
                     1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
                     1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
                     1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
                    -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
                     1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
                     1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
                     1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
                    -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
                    -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
                    -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
                     1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
                     1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
                     1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
                    -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
                    -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f
                };
                num_light_vertices = sizeof(light_vertices) / sizeof(float) / 2;

                glGenVertexArrays(1, &light_vao_id);
                glGenBuffers(1, &light_vbo_id);

                glBindVertexArray(light_vao_id);

                glBindBuffer(GL_ARRAY_BUFFER, light_vbo_id);
                glBufferData(GL_ARRAY_BUFFER, sizeof(light_vertices), &light_vertices, GL_STATIC_DRAW);
                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)(0 * sizeof(GLfloat)));
                glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)(3 * sizeof(GLfloat)));
                glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)(6 * sizeof(GLfloat)));
                glEnableVertexAttribArray(0);
                glEnableVertexAttribArray(1);
                glEnableVertexAttribArray(2);

                glBindVertexArray(0);
            }

            glBindVertexArray(light_vao_id);
            glDrawArrays(GL_TRIANGLES, 0, num_light_vertices);
            glBindVertexArray(0);
        }

        program_unbind();
    }

    // draw debug textures to display fbos
    program_bind(renderer.sprite_program);

    // calculate ortho projection
    mat4 camera_projection_ortho;
    glm_ortho_default(aspect, camera_projection_ortho);

    program_set_mat4(renderer.sprite_program, "camera.projection", camera_projection_ortho);

    vec3 position = GLM_VEC3_ZERO_INIT;
    vec3 color = GLM_VEC3_ONE_INIT;

    position[0] = -1.75f;
    position[1] = 0.0f;

    {
        mat4 sprite_model = GLM_MAT4_IDENTITY_INIT;
        glm_translate(sprite_model, position);

        program_set_mat4(renderer.sprite_program, "sprite.model", sprite_model);
        program_set_vec3(renderer.sprite_program, "sprite.color", color);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, renderer.geometry_position_texture_id);

        glBindVertexArray(renderer.sprite_vao_id);
        glDrawArrays(GL_TRIANGLES, 0, renderer.num_sprite_vertices);
        glBindVertexArray(0);
    }

    position[0] += 1.0f;

    {
        mat4 sprite_model = GLM_MAT4_IDENTITY_INIT;
        glm_translate(sprite_model, position);

        program_set_mat4(renderer.sprite_program, "sprite.model", sprite_model);
        program_set_vec3(renderer.sprite_program, "sprite.color", color);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, renderer.geometry_normal_texture_id);

        glBindVertexArray(renderer.sprite_vao_id);
        glDrawArrays(GL_TRIANGLES, 0, renderer.num_sprite_vertices);
        glBindVertexArray(0);
    }

    position[0] += 1.0f;

    {
        mat4 sprite_model = GLM_MAT4_IDENTITY_INIT;
        glm_translate(sprite_model, position);

        program_set_mat4(renderer.sprite_program, "sprite.model", sprite_model);
        program_set_vec3(renderer.sprite_program, "sprite.color", color);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, renderer.geometry_albedo_specular_texture_id);

        glBindVertexArray(renderer.sprite_vao_id);
        glDrawArrays(GL_TRIANGLES, 0, renderer.num_sprite_vertices);
        glBindVertexArray(0);
    }

    position[0] += 1.0f;

    {
        mat4 sprite_model = GLM_MAT4_IDENTITY_INIT;
        glm_translate(sprite_model, position);

        program_set_mat4(renderer.sprite_program, "sprite.model", sprite_model);
        program_set_vec3(renderer.sprite_program, "sprite.color", color);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, renderer.depthmap_texture_id);

        glBindVertexArray(renderer.sprite_vao_id);
        glDrawArrays(GL_TRIANGLES, 0, renderer.num_sprite_vertices);
        glBindVertexArray(0);
    }

    position[0] = -1.75f;
    position[1] = -1.0f;

    {
        mat4 sprite_model = GLM_MAT4_IDENTITY_INIT;
        glm_translate(sprite_model, position);

        program_set_mat4(renderer.sprite_program, "sprite.model", sprite_model);
        program_set_vec3(renderer.sprite_program, "sprite.color", color);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, renderer.water_reflection_color_texture_id);

        glBindVertexArray(renderer.sprite_vao_id);
        glDrawArrays(GL_TRIANGLES, 0, renderer.num_sprite_vertices);
        glBindVertexArray(0);
    }

    position[0] += 1.0f;

    {
        mat4 sprite_model = GLM_MAT4_IDENTITY_INIT;
        glm_translate(sprite_model, position);

        program_set_mat4(renderer.sprite_program, "sprite.model", sprite_model);
        program_set_vec3(renderer.sprite_program, "sprite.color", color);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, renderer.water_refraction_color_texture_id);

        glBindVertexArray(renderer.sprite_vao_id);
        glDrawArrays(GL_TRIANGLES, 0, renderer.num_sprite_vertices);
        glBindVertexArray(0);
    }

    program_unbind();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void render_screen(GLuint fbo_id)
{
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_id);

    glViewport(0, 0, renderer.render_width, renderer.render_height);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);

    program_bind(renderer.post_program);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, renderer.screen_texture_id);

    glBindVertexArray(renderer.screen_vao_id);
    glDrawArrays(GL_TRIANGLES, 0, renderer.num_screen_vertices);
    glBindVertexArray(0);

    program_unbind();

    glEnable(GL_DEPTH_TEST);
}

void renderer_draw(struct camera *camera, float aspect, unsigned int elapsed_time, float delta_time)
{
    if (!camera)
    {
        printf("Error: Camera cannot be null\n");

        return;
    }

    // render scene to the screen framebuffer
    vec4 clipping_plane = GLM_VEC4_ZERO_INIT;

    render_scene(renderer.screen_fbo_id, camera, aspect, clipping_plane);

    // render water
    if (renderer.num_waters > 0)
    {
        render_waters(renderer.screen_fbo_id, camera, aspect, elapsed_time);
    }

    // render sprites
    if (renderer.num_sprites > 0)
    {
        render_sprites(renderer.screen_fbo_id, aspect);
    }

    // TEST: render debug info
    // render_debug(renderer.screen_fbo_id, camera, aspect);

    // render the screen framebuffer to the default framebuffer and apply post-processing
    render_screen(0);

    // clear renderables
    renderer.num_objects = 0;
    renderer.num_directional_lights = 0;
    renderer.num_point_lights = 0;
    renderer.num_spot_lights = 0;
    renderer.num_waters = 0;
    renderer.num_sprites = 0;
}

// TODO: delete all resources
void renderer_quit(void)
{
    glDeleteBuffers(1, &renderer.sprite_vbo_id);
    glDeleteVertexArrays(1, &renderer.sprite_vao_id);

    free(renderer.spot_lights);
    free(renderer.point_lights);
    free(renderer.objects);

    glDeleteRenderbuffers(1, &renderer.geometry_rbo_id);
    glDeleteTextures(1, &renderer.geometry_albedo_specular_texture_id);
    glDeleteTextures(1, &renderer.geometry_normal_texture_id);
    glDeleteTextures(1, &renderer.geometry_position_texture_id);
    glDeleteFramebuffers(1, &renderer.geometry_fbo_id);

    glDeleteTextures(1, &renderer.depthmap_texture_id);
    glDeleteFramebuffers(1, &renderer.depthmap_fbo_id);

    glDeleteRenderbuffers(1, &renderer.screen_rbo_id);
    glDeleteTextures(1, &renderer.screen_texture_id);
    glDeleteFramebuffers(1, &renderer.screen_fbo_id);

    program_destroy(renderer.depth_program);
    program_destroy(renderer.post_program);
    program_destroy(renderer.geometry_program);
    program_destroy(renderer.deferred_spot_program);
    program_destroy(renderer.deferred_point_program);
    program_destroy(renderer.deferred_directional_program);
    program_destroy(renderer.deferred_sun_program);
    program_destroy(renderer.forward_spot_program);
    program_destroy(renderer.forward_point_program);
    program_destroy(renderer.forward_directional_program);
    program_destroy(renderer.forward_sun_program);
    program_destroy(renderer.forward_color_program);

    assets_unload();
}
