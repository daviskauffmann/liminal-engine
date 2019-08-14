#include <game/game.hpp>

int renderer_init(int render_width, int render_height, float render_scale, int shadow_width, int shadow_height)
{
    Renderer.render_width = render_width;
    Renderer.render_height = render_height;
    Renderer.render_scale = render_scale;
    Renderer.shadow_width = shadow_width;
    Renderer.shadow_height = shadow_height;
    Renderer.reflection_width = render_width;
    Renderer.reflection_height = render_height;
    Renderer.refraction_width = render_width;
    Renderer.refraction_height = render_height;
    Renderer.render_mode = RENDER_MODE_FORWARD;

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

    // setup opengl
    glViewport(0, 0, render_width, render_height);
    glClearColor(1.0f, 0.0f, 1.0f, 0.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_STENCIL_TEST);

    // create programs
    Renderer.depth_program = program_create(
        "assets/shaders/depth.vert",
        "assets/shaders/depth.frag");

    if (!Renderer.depth_program)
    {
        printf("Error: Couldn't create depth program\n");

        return 1;
    }

    Renderer.forward_color_program = program_create(
        "assets/shaders/forward.vert",
        "assets/shaders/forward_color.frag");

    if (!Renderer.forward_color_program)
    {
        printf("Error: Couldn't create forward color program\n");

        return 1;
    }

    Renderer.forward_sun_program = program_create(
        "assets/shaders/forward.vert",
        "assets/shaders/forward_sun.frag");

    if (!Renderer.forward_sun_program)
    {
        printf("Error: Couldn't create forward sun program\n");

        return 1;
    }

    Renderer.forward_directional_program = program_create(
        "assets/shaders/forward.vert",
        "assets/shaders/forward_directional.frag");

    if (!Renderer.forward_directional_program)
    {
        printf("Error: Couldn't create forward directional program\n");

        return 1;
    }

    Renderer.forward_point_program = program_create(
        "assets/shaders/forward.vert",
        "assets/shaders/forward_point.frag");

    if (!Renderer.forward_point_program)
    {
        printf("Error: Couldn't create forward point program\n");

        return 1;
    }

    Renderer.forward_spot_program = program_create(
        "assets/shaders/forward.vert",
        "assets/shaders/forward_spot.frag");

    if (!Renderer.forward_spot_program)
    {
        printf("Error: Couldn't create forward spot program\n");

        return 1;
    }

    Renderer.forward_reflection_program = program_create(
        "assets/shaders/forward.vert",
        "assets/shaders/forward_reflection.frag");

    if (!Renderer.forward_reflection_program)
    {
        printf("Error: Couldn't create forward reflection program\n");

        return 1;
    }

    Renderer.geometry_program = program_create(
        "assets/shaders/geometry.vert",
        "assets/shaders/geometry.frag");

    if (!Renderer.geometry_program)
    {
        printf("Error: Couldn't create geometry program\n");

        return 1;
    }

    Renderer.deferred_sun_program = program_create(
        "assets/shaders/deferred.vert",
        "assets/shaders/deferred_sun.frag");

    if (!Renderer.deferred_sun_program)
    {
        printf("Error: Couldn't create deferred sun program\n");

        return 1;
    }

    Renderer.deferred_directional_program = program_create(
        "assets/shaders/deferred.vert",
        "assets/shaders/deferred_directional.frag");

    if (!Renderer.deferred_directional_program)
    {
        printf("Error: Couldn't create deferred directional program\n");

        return 1;
    }

    Renderer.deferred_point_program = program_create(
        "assets/shaders/deferred.vert",
        "assets/shaders/deferred_point.frag");

    if (!Renderer.deferred_point_program)
    {
        printf("Error: Couldn't create deferred point program\n");

        return 1;
    }

    Renderer.deferred_spot_program = program_create(
        "assets/shaders/deferred.vert",
        "assets/shaders/deferred_spot.frag");

    if (!Renderer.deferred_spot_program)
    {
        printf("Error: Couldn't create deferred spot program\n");

        return 1;
    }

    Renderer.skybox_program = program_create(
        "assets/shaders/skybox.vert",
        "assets/shaders/skybox.frag");

    if (!Renderer.skybox_program)
    {
        printf("Error: Couldn't create skybox program\n");

        return 1;
    }

    Renderer.water_program = program_create(
        "assets/shaders/water.vert",
        "assets/shaders/water.frag");

    if (!Renderer.water_program)
    {
        printf("Error: Couldn't create water program\n");

        return 1;
    }

    Renderer.sprite_program = program_create(
        "assets/shaders/sprite.vert",
        "assets/shaders/sprite.frag");

    if (!Renderer.sprite_program)
    {
        printf("Error: Couldn't create sprite program\n");

        return 1;
    }

    Renderer.screen_program = program_create(
        "assets/shaders/screen.vert",
        "assets/shaders/screen.frag");

    if (!Renderer.screen_program)
    {
        printf("Error: Couldn't create screen program\n");

        return 1;
    }

    // setup shader samplers
    program_bind(Renderer.forward_sun_program);
    program_set_int(Renderer.forward_sun_program, "material.diffuse_map", 0);
    program_set_int(Renderer.forward_sun_program, "material.specular_map", 1);
    program_set_int(Renderer.forward_sun_program, "material.normal_map", 2);
    program_set_int(Renderer.forward_sun_program, "material.emission_map", 3);
    program_set_int(Renderer.forward_sun_program, "sun.depth_map", 4);
    program_unbind();

    program_bind(Renderer.forward_directional_program);
    program_set_int(Renderer.forward_directional_program, "material.diffuse_map", 0);
    program_set_int(Renderer.forward_directional_program, "material.specular_map", 1);
    program_set_int(Renderer.forward_directional_program, "material.normal_map", 2);
    program_set_int(Renderer.forward_directional_program, "material.emission_map", 3);
    program_unbind();

    program_bind(Renderer.forward_point_program);
    program_set_int(Renderer.forward_point_program, "material.diffuse_map", 0);
    program_set_int(Renderer.forward_point_program, "material.specular_map", 1);
    program_set_int(Renderer.forward_point_program, "material.normal_map", 2);
    program_set_int(Renderer.forward_point_program, "material.emission_map", 3);
    program_unbind();

    program_bind(Renderer.forward_spot_program);
    program_set_int(Renderer.forward_spot_program, "material.diffuse_map", 0);
    program_set_int(Renderer.forward_spot_program, "material.specular_map", 1);
    program_set_int(Renderer.forward_spot_program, "material.normal_map", 2);
    program_set_int(Renderer.forward_spot_program, "material.emission_map", 3);
    program_unbind();

    program_bind(Renderer.forward_reflection_program);
    program_set_int(Renderer.forward_reflection_program, "material.reflectivity_map", 0);
    program_set_int(Renderer.forward_reflection_program, "skybox.color_map", 1);
    program_unbind();

    program_bind(Renderer.geometry_program);
    program_set_int(Renderer.geometry_program, "material.diffuse_map", 0);
    program_set_int(Renderer.geometry_program, "material.specular_map", 1);
    program_set_int(Renderer.geometry_program, "material.normal_map", 2);
    program_set_int(Renderer.geometry_program, "material.emission_map", 3);
    program_unbind();

    program_bind(Renderer.deferred_sun_program);
    program_set_int(Renderer.deferred_sun_program, "geometry.position_map", 0);
    program_set_int(Renderer.deferred_sun_program, "geometry.normal_map", 1);
    program_set_int(Renderer.deferred_sun_program, "geometry.albedo_specular_map", 2);
    program_set_int(Renderer.deferred_sun_program, "sun.depth_map", 3);
    program_unbind();

    program_bind(Renderer.deferred_directional_program);
    program_set_int(Renderer.deferred_directional_program, "geometry.position_map", 0);
    program_set_int(Renderer.deferred_directional_program, "geometry.normal_map", 1);
    program_set_int(Renderer.deferred_directional_program, "geometry.albedo_specular_map", 2);
    program_unbind();

    program_bind(Renderer.deferred_point_program);
    program_set_int(Renderer.deferred_point_program, "geometry.position_map", 0);
    program_set_int(Renderer.deferred_point_program, "geometry.normal_map", 1);
    program_set_int(Renderer.deferred_point_program, "geometry.albedo_specular_map", 2);
    program_unbind();

    program_bind(Renderer.deferred_spot_program);
    program_set_int(Renderer.deferred_spot_program, "geometry.position_map", 0);
    program_set_int(Renderer.deferred_spot_program, "geometry.normal_map", 1);
    program_set_int(Renderer.deferred_spot_program, "geometry.albedo_specular_map", 2);
    program_unbind();

    program_bind(Renderer.skybox_program);
    program_set_int(Renderer.skybox_program, "skybox.color_map", 0);
    program_unbind();

    program_bind(Renderer.water_program);
    program_set_int(Renderer.water_program, "water.reflection_map", 0);
    program_set_int(Renderer.water_program, "water.refraction_map", 1);
    program_set_int(Renderer.water_program, "water.dudv_map", 2);
    program_set_int(Renderer.water_program, "water.normal_map", 3);
    program_set_int(Renderer.water_program, "sun.depth_map", 4);
    program_unbind();

    program_bind(Renderer.sprite_program);
    program_set_int(Renderer.sprite_program, "sprite.color_map", 0);
    program_unbind();

    program_bind(Renderer.screen_program);
    program_set_int(Renderer.screen_program, "screen.color_map", 0);
    program_unbind();

    // setup screen fbo
    glGenFramebuffers(1, &Renderer.screen_fbo_id);

    glGenTextures(1, &Renderer.screen_texture_id);
    glBindTexture(GL_TEXTURE_2D, Renderer.screen_texture_id);

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

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_2D, 0);

    glGenRenderbuffers(1, &Renderer.screen_rbo_id);
    glBindRenderbuffer(GL_RENDERBUFFER, Renderer.screen_rbo_id);

    glRenderbufferStorage(
        GL_RENDERBUFFER,
        GL_DEPTH24_STENCIL8,
        render_width,
        render_height);

    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, Renderer.screen_fbo_id);

    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D,
        Renderer.screen_texture_id,
        0);
    glFramebufferRenderbuffer(
        GL_FRAMEBUFFER,
        GL_DEPTH_STENCIL_ATTACHMENT,
        GL_RENDERBUFFER,
        Renderer.screen_rbo_id);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        printf("Error: Couldn't complete screen framebuffer\n");

        return 1;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // setup depthmap fbo
    glGenFramebuffers(1, &Renderer.depthmap_fbo_id);
    glBindFramebuffer(GL_FRAMEBUFFER, Renderer.depthmap_fbo_id);

    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    glGenTextures(1, &Renderer.depthmap_texture_id);
    glBindTexture(GL_TEXTURE_2D, Renderer.depthmap_texture_id);

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
        Renderer.depthmap_texture_id,
        0);

    glBindTexture(GL_TEXTURE_2D, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        printf("Error: Couldn't complete depthmap framebuffer\n");

        return 1;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // setup geometry fbo
    glGenFramebuffers(1, &Renderer.geometry_fbo_id);
    glBindFramebuffer(GL_FRAMEBUFFER, Renderer.geometry_fbo_id);

    glGenTextures(1, &Renderer.geometry_position_texture_id);
    glBindTexture(GL_TEXTURE_2D, Renderer.geometry_position_texture_id);

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
        Renderer.geometry_position_texture_id,
        0);

    glBindTexture(GL_TEXTURE_2D, 0);

    glGenTextures(1, &Renderer.geometry_normal_texture_id);
    glBindTexture(GL_TEXTURE_2D, Renderer.geometry_normal_texture_id);

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
        Renderer.geometry_normal_texture_id,
        0);

    glBindTexture(GL_TEXTURE_2D, 0);

    glGenTextures(1, &Renderer.geometry_albedo_specular_texture_id);
    glBindTexture(GL_TEXTURE_2D, Renderer.geometry_albedo_specular_texture_id);

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
        Renderer.geometry_albedo_specular_texture_id,
        0);

    glBindTexture(GL_TEXTURE_2D, 0);

    GLenum geometry_fbo_attachments[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    glDrawBuffers(sizeof(geometry_fbo_attachments) / sizeof(GLenum), geometry_fbo_attachments);

    glGenRenderbuffers(1, &Renderer.geometry_rbo_id);
    glBindRenderbuffer(GL_RENDERBUFFER, Renderer.geometry_rbo_id);

    glRenderbufferStorage(
        GL_RENDERBUFFER,
        GL_DEPTH_COMPONENT,
        (GLsizei)(render_width * render_scale),
        (GLsizei)(render_height * render_scale));

    glFramebufferRenderbuffer(
        GL_FRAMEBUFFER,
        GL_DEPTH_ATTACHMENT,
        GL_RENDERBUFFER,
        Renderer.geometry_rbo_id);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        printf("Error: Couldn't complete geometry framebuffer\n");

        return 1;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // setup water reflection fbo
    glGenFramebuffers(1, &Renderer.water_reflection_fbo_id);
    glBindFramebuffer(GL_FRAMEBUFFER, Renderer.water_reflection_fbo_id);

    glGenTextures(1, &Renderer.water_reflection_color_texture_id);
    glBindTexture(GL_TEXTURE_2D, Renderer.water_reflection_color_texture_id);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGB8,
        Renderer.reflection_width,
        Renderer.reflection_height,
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
        Renderer.water_reflection_color_texture_id,
        0);

    glBindTexture(GL_TEXTURE_2D, 0);

    GLenum water_reflection_fbo_attachments[] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(sizeof(water_reflection_fbo_attachments) / sizeof(GLenum), water_reflection_fbo_attachments);

    glGenTextures(1, &Renderer.water_reflection_depth_texture_id);
    glBindTexture(GL_TEXTURE_2D, Renderer.water_reflection_depth_texture_id);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_DEPTH_COMPONENT32,
        Renderer.reflection_width,
        Renderer.reflection_height,
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
        Renderer.water_reflection_depth_texture_id,
        0);

    glBindTexture(GL_TEXTURE_2D, 0);

    glGenRenderbuffers(1, &Renderer.water_reflection_depth_rbo_id);
    glBindRenderbuffer(GL_RENDERBUFFER, Renderer.water_reflection_depth_rbo_id);

    glRenderbufferStorage(
        GL_RENDERBUFFER,
        GL_DEPTH_COMPONENT,
        Renderer.reflection_width,
        Renderer.reflection_height);

    glFramebufferRenderbuffer(
        GL_FRAMEBUFFER,
        GL_DEPTH_ATTACHMENT,
        GL_RENDERBUFFER,
        Renderer.water_reflection_depth_rbo_id);

    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        printf("Error: Couldn't complete water reflection framebuffer\n");

        return 1;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // setup water refraction fbo
    glGenFramebuffers(1, &Renderer.water_refraction_fbo_id);
    glBindFramebuffer(GL_FRAMEBUFFER, Renderer.water_refraction_fbo_id);

    glGenTextures(1, &Renderer.water_refraction_color_texture_id);
    glBindTexture(GL_TEXTURE_2D, Renderer.water_refraction_color_texture_id);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGB8,
        Renderer.refraction_width,
        Renderer.refraction_height,
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
        Renderer.water_refraction_color_texture_id,
        0);

    glBindTexture(GL_TEXTURE_2D, 0);

    GLenum water_refraction_fbo_attachments[] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(sizeof(water_refraction_fbo_attachments) / sizeof(GLenum), water_refraction_fbo_attachments);

    glGenTextures(1, &Renderer.water_refraction_depth_texture_id);
    glBindTexture(GL_TEXTURE_2D, Renderer.water_refraction_depth_texture_id);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_DEPTH_COMPONENT32,
        Renderer.refraction_width,
        Renderer.refraction_height,
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
        Renderer.water_refraction_depth_texture_id,
        0);

    glBindTexture(GL_TEXTURE_2D, 0);

    glGenRenderbuffers(1, &Renderer.water_refraction_depth_rbo_id);
    glBindRenderbuffer(GL_RENDERBUFFER, Renderer.water_refraction_depth_rbo_id);

    glRenderbufferStorage(
        GL_RENDERBUFFER,
        GL_DEPTH_COMPONENT,
        Renderer.refraction_width,
        Renderer.refraction_height);

    glFramebufferRenderbuffer(
        GL_FRAMEBUFFER,
        GL_DEPTH_ATTACHMENT,
        GL_RENDERBUFFER,
        Renderer.water_refraction_depth_rbo_id);

    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        printf("Error: Couldn't complete water refraction framebuffer\n");

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
    Renderer.num_water_vertices = sizeof(water_vertices) / sizeof(float) / 2;

    glGenVertexArrays(1, &Renderer.water_vao_id);
    glGenBuffers(1, &Renderer.water_vbo_id);

    glBindVertexArray(Renderer.water_vao_id);

    glBindBuffer(GL_ARRAY_BUFFER, Renderer.water_vbo_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(water_vertices), &water_vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid *)(0 * sizeof(GLfloat))); // position
    glEnableVertexAttribArray(0);

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
    Renderer.num_skybox_vertices = sizeof(skybox_vertices) / sizeof(float) / 3;

    glGenVertexArrays(1, &Renderer.skybox_vao_id);
    glGenBuffers(1, &Renderer.skybox_vbo_id);

    glBindVertexArray(Renderer.skybox_vao_id);

    glBindBuffer(GL_ARRAY_BUFFER, Renderer.skybox_vbo_id);
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
    Renderer.num_sprite_vertices = sizeof(sprite_vertices) / sizeof(float) / 4;

    glGenVertexArrays(1, &Renderer.sprite_vao_id);
    glGenBuffers(1, &Renderer.sprite_vbo_id);

    glBindVertexArray(Renderer.sprite_vao_id);

    glBindBuffer(GL_ARRAY_BUFFER, Renderer.sprite_vbo_id);
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
    Renderer.num_screen_vertices = sizeof(screen_vertices) / sizeof(float) / 4;

    glGenVertexArrays(1, &Renderer.screen_vao_id);
    glGenBuffers(1, &Renderer.screen_vbo_id);

    glBindVertexArray(Renderer.screen_vao_id);

    glBindBuffer(GL_ARRAY_BUFFER, Renderer.screen_vbo_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(screen_vertices), &screen_vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid *)(0 * sizeof(GLfloat))); // position
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid *)(2 * sizeof(GLfloat))); // uv
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    // create water dudv texture
    Renderer.water_dudv_texture = texture_create("assets/images/water_dudv.png");

    if (!Renderer.water_dudv_texture)
    {
        printf("Error: Couldn't load water dudv texture\n");

        return 1;
    }

    // create water normal texture
    Renderer.water_normal_texture = texture_create("assets/images/water_normal.png");

    if (!Renderer.water_normal_texture)
    {
        printf("Error: Couldn't load water normal texture\n");

        return 1;
    }

    return 0;
}

void renderer_set_mode(enum RenderMode _render_mode)
{
    Renderer.render_mode = _render_mode;
}

void renderer_add_object(struct object *object)
{
    struct object **objects = realloc(Renderer.objects, (Renderer.num_objects + 1) * sizeof(struct object *));

    if (!objects)
    {
        printf("Error: Couldn't reallocate object array\n");

        return;
    }

    Renderer.objects = objects;
    Renderer.objects[Renderer.num_objects++] = object;
}

void renderer_set_sun(struct sun *_sun)
{
    Renderer.sun = _sun;
}

void renderer_add_directional_light(struct directional_light *directional_light)
{
    struct directional_light **directional_lights = realloc(Renderer.directional_lights, (Renderer.num_directional_lights + 1) * sizeof(struct directional_light *));

    if (!directional_lights)
    {
        printf("Error: Couldn't reallocate directional light array\n");

        return;
    }

    Renderer.directional_lights = directional_lights;
    Renderer.directional_lights[Renderer.num_directional_lights++] = directional_light;
}

void renderer_add_point_light(struct point_light *point_light)
{
    struct point_light **point_lights = realloc(Renderer.point_lights, (Renderer.num_point_lights + 1) * sizeof(struct point_light *));

    if (!point_lights)
    {
        printf("Error: Couldn't reallocate point light array\n");

        return;
    }

    Renderer.point_lights = point_lights;
    Renderer.point_lights[Renderer.num_point_lights++] = point_light;
}

void renderer_add_spot_light(struct spot_light *spot_light)
{
    struct spot_light **spot_lights = realloc(Renderer.spot_lights, (Renderer.num_spot_lights + 1) * sizeof(struct spot_light *));

    if (!spot_lights)
    {
        printf("Error: Couldn't reallocate spot light array\n");

        return;
    }

    Renderer.spot_lights = spot_lights;
    Renderer.spot_lights[Renderer.num_spot_lights++] = spot_light;
}

void renderer_set_skybox(struct cubemap *_skybox)
{
    Renderer.skybox = _skybox;
}

void renderer_add_water(struct water *water)
{
    struct water **waters = realloc(Renderer.waters, (Renderer.num_waters + 1) * sizeof(struct water *));

    if (!waters)
    {
        printf("Error: Couldn't reallocate water array\n");

        return;
    }

    Renderer.waters = waters;
    Renderer.waters[Renderer.num_waters++] = water;
}

void renderer_add_sprite(struct sprite *sprite)
{
    struct sprite **new_sprites = realloc(Renderer.sprites, (Renderer.num_sprites + 1) * sizeof(struct sprite *));

    if (!new_sprites)
    {
        printf("Error: Couldn't reallocate sprite array\n");

        return;
    }

    Renderer.sprites = new_sprites;
    Renderer.sprites[Renderer.num_sprites++] = sprite;
}

void render_scene(GLuint fbo_id, struct camera *camera, float aspect, unsigned int elapsed_time, vec4 clipping_plane)
{
    // clear the framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_id);

    glViewport(0, 0, Renderer.render_width, Renderer.render_height);
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
    if (Renderer.sun)
    {
        // calculate sun projection matrix
        glm_ortho(-10.0f, 10.0f, -10.0f, 10.0f, -10.0f, 10.0f, sun_projection);

        // calculate sun "position"
        vec3 sun_position;
        glm_vec_sub(GLM_VEC3_ZERO, Renderer.sun->direction, sun_position);

        // calculate sun view matrix
        glm_lookat(sun_position, GLM_VEC3_ZERO, GLM_YUP, sun_view);

        // render sun shadows to depthmap
        glBindFramebuffer(GL_FRAMEBUFFER, Renderer.depthmap_fbo_id);

        glViewport(0, 0, Renderer.shadow_width, Renderer.shadow_height);
        glClear(GL_DEPTH_BUFFER_BIT);

        program_bind(Renderer.depth_program);

        program_set_mat4(Renderer.depth_program, "sun.projection", sun_projection);
        program_set_mat4(Renderer.depth_program, "sun.view", sun_view);

        for (unsigned int i = 0; i < Renderer.num_objects; i++)
        {
            struct object *object = Renderer.objects[i];

            mat4 object_model = GLM_MAT4_IDENTITY_INIT;
            object_calc_model(object, object_model);

            program_set_mat4(Renderer.depth_program, "object.model", object_model);

            mesh_draw(object->mesh);
        }

        program_unbind();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    switch (Renderer.render_mode)
    {
    case RENDER_MODE_FORWARD:
    {
        // draw objects to the specified framebuffer
        if (Renderer.num_objects > 0)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, fbo_id);

            glViewport(0, 0, Renderer.render_width, Renderer.render_height);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glEnable(GL_CLIP_DISTANCE0);

            for (unsigned int i = 0; i < Renderer.num_objects; i++)
            {
                struct object *object = Renderer.objects[i];

                mat4 object_model = GLM_MAT4_IDENTITY_INIT;
                object_calc_model(object, object_model);

                if (Renderer.sun)
                {
                    program_bind(Renderer.forward_sun_program);

                    program_set_mat4(Renderer.forward_sun_program, "camera.projection", camera_projection);
                    program_set_mat4(Renderer.forward_sun_program, "camera.view", camera_view);
                    program_set_vec3(Renderer.forward_sun_program, "camera.position", camera->position);

                    program_set_mat4(Renderer.forward_sun_program, "object.model", object_model);

                    program_set_vec4(Renderer.forward_sun_program, "clipping_plane", clipping_plane);

                    program_set_vec3(Renderer.forward_sun_program, "material.color", object->material->color);
                    program_set_float(Renderer.forward_sun_program, "material.shininess", object->material->shininess);
                    program_set_float(Renderer.forward_sun_program, "material.glow", object->material->glow);

                    program_set_vec3(Renderer.forward_sun_program, "sun.direction", Renderer.sun->direction);
                    program_set_vec3(Renderer.forward_sun_program, "sun.ambient_color", Renderer.sun->ambient_color);
                    program_set_vec3(Renderer.forward_sun_program, "sun.diffuse_color", Renderer.sun->diffuse_color);
                    program_set_vec3(Renderer.forward_sun_program, "sun.specular_color", Renderer.sun->specular_color);
                    program_set_mat4(Renderer.forward_sun_program, "sun.projection", sun_projection);
                    program_set_mat4(Renderer.forward_sun_program, "sun.view", sun_view);

                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, object->material->diffuse_map ? object->material->diffuse_map->texture_id : 0);
                    glActiveTexture(GL_TEXTURE1);
                    glBindTexture(GL_TEXTURE_2D, object->material->specular_map ? object->material->specular_map->texture_id : 0);
                    glActiveTexture(GL_TEXTURE2);
                    glBindTexture(GL_TEXTURE_2D, object->material->normal_map ? object->material->normal_map->texture_id : 0);
                    glActiveTexture(GL_TEXTURE3);
                    glBindTexture(GL_TEXTURE_2D, object->material->emission_map ? object->material->emission_map->texture_id : 0);
                    glActiveTexture(GL_TEXTURE4);
                    glBindTexture(GL_TEXTURE_2D, Renderer.depthmap_texture_id);

                    mesh_draw(object->mesh);

                    program_unbind();
                }

                glEnable(GL_BLEND);
                glBlendFunc(GL_ONE, GL_ONE);
                glDepthMask(GL_FALSE);
                glDepthFunc(GL_EQUAL);

                if (Renderer.num_directional_lights > 0)
                {
                    program_bind(Renderer.forward_directional_program);

                    program_set_mat4(Renderer.forward_directional_program, "camera.projection", camera_projection);
                    program_set_mat4(Renderer.forward_directional_program, "camera.view", camera_view);
                    program_set_vec3(Renderer.forward_directional_program, "camera.position", camera->position);

                    program_set_mat4(Renderer.forward_directional_program, "object.model", object_model);

                    program_set_vec4(Renderer.forward_directional_program, "clipping_plane", clipping_plane);

                    program_set_vec3(Renderer.forward_directional_program, "material.color", object->material->color);
                    program_set_float(Renderer.forward_directional_program, "material.shininess", object->material->shininess);
                    program_set_float(Renderer.forward_directional_program, "material.glow", object->material->glow);

                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, object->material->diffuse_map ? object->material->diffuse_map->texture_id : 0);
                    glActiveTexture(GL_TEXTURE1);
                    glBindTexture(GL_TEXTURE_2D, object->material->specular_map ? object->material->specular_map->texture_id : 0);
                    glActiveTexture(GL_TEXTURE2);
                    glBindTexture(GL_TEXTURE_2D, object->material->normal_map ? object->material->normal_map->texture_id : 0);
                    glActiveTexture(GL_TEXTURE3);
                    glBindTexture(GL_TEXTURE_2D, object->material->emission_map ? object->material->emission_map->texture_id : 0);

                    for (unsigned int j = 0; j < Renderer.num_directional_lights; j++)
                    {
                        struct directional_light *directional_light = Renderer.directional_lights[j];

                        program_set_vec3(Renderer.forward_directional_program, "directional_light.direction", directional_light->direction);
                        program_set_vec3(Renderer.forward_directional_program, "directional_light.ambient", directional_light->ambient_color);
                        program_set_vec3(Renderer.forward_directional_program, "directional_light.diffuse_color", directional_light->diffuse_color);
                        program_set_vec3(Renderer.forward_directional_program, "directional_light.specular_color", directional_light->specular_color);

                        mesh_draw(object->mesh);
                    }

                    program_unbind();
                }

                if (Renderer.num_point_lights > 0)
                {
                    program_bind(Renderer.forward_point_program);

                    program_set_mat4(Renderer.forward_point_program, "camera.projection", camera_projection);
                    program_set_mat4(Renderer.forward_point_program, "camera.view", camera_view);
                    program_set_vec3(Renderer.forward_point_program, "camera.position", camera->position);

                    program_set_mat4(Renderer.forward_point_program, "object.model", object_model);

                    program_set_vec4(Renderer.forward_point_program, "clipping_plane", clipping_plane);

                    program_set_vec3(Renderer.forward_point_program, "material.color", object->material->color);
                    program_set_float(Renderer.forward_point_program, "material.shininess", object->material->shininess);
                    program_set_float(Renderer.forward_point_program, "material.glow", object->material->glow);

                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, object->material->diffuse_map ? object->material->diffuse_map->texture_id : 0);
                    glActiveTexture(GL_TEXTURE1);
                    glBindTexture(GL_TEXTURE_2D, object->material->specular_map ? object->material->specular_map->texture_id : 0);
                    glActiveTexture(GL_TEXTURE2);
                    glBindTexture(GL_TEXTURE_2D, object->material->normal_map ? object->material->normal_map->texture_id : 0);
                    glActiveTexture(GL_TEXTURE3);
                    glBindTexture(GL_TEXTURE_2D, object->material->emission_map ? object->material->emission_map->texture_id : 0);

                    for (unsigned int j = 0; j < Renderer.num_point_lights; j++)
                    {
                        struct point_light *point_light = Renderer.point_lights[j];

                        program_set_vec3(Renderer.forward_point_program, "point_light.position", point_light->position);
                        program_set_vec3(Renderer.forward_point_program, "point_light.ambient_color", point_light->ambient_color);
                        program_set_vec3(Renderer.forward_point_program, "point_light.diffuse_color", point_light->diffuse_color);
                        program_set_vec3(Renderer.forward_point_program, "point_light.specular_color", point_light->specular_color);
                        program_set_vec3(Renderer.forward_point_program, "point_light.attenuation", point_light->attenuation);

                        mesh_draw(object->mesh);
                    }

                    program_unbind();
                }

                if (Renderer.num_spot_lights > 0)
                {
                    program_bind(Renderer.forward_spot_program);

                    program_set_mat4(Renderer.forward_spot_program, "camera.projection", camera_projection);
                    program_set_mat4(Renderer.forward_spot_program, "camera.view", camera_view);
                    program_set_vec3(Renderer.forward_spot_program, "camera.position", camera->position);

                    program_set_mat4(Renderer.forward_spot_program, "object.model", object_model);

                    program_set_vec4(Renderer.forward_spot_program, "clipping_plane", clipping_plane);

                    program_set_vec3(Renderer.forward_spot_program, "material.color", object->material->color);
                    program_set_float(Renderer.forward_spot_program, "material.shininess", object->material->shininess);
                    program_set_float(Renderer.forward_spot_program, "material.glow", object->material->glow);

                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, object->material->diffuse_map ? object->material->diffuse_map->texture_id : 0);
                    glActiveTexture(GL_TEXTURE1);
                    glBindTexture(GL_TEXTURE_2D, object->material->specular_map ? object->material->specular_map->texture_id : 0);
                    glActiveTexture(GL_TEXTURE2);
                    glBindTexture(GL_TEXTURE_2D, object->material->normal_map ? object->material->normal_map->texture_id : 0);
                    glActiveTexture(GL_TEXTURE3);
                    glBindTexture(GL_TEXTURE_2D, object->material->emission_map ? object->material->emission_map->texture_id : 0);

                    for (unsigned int j = 0; j < Renderer.num_spot_lights; j++)
                    {
                        struct spot_light *spot_light = Renderer.spot_lights[j];

                        program_set_vec3(Renderer.forward_spot_program, "spot_light.position", spot_light->position);
                        program_set_vec3(Renderer.forward_spot_program, "spot_light.direction", spot_light->direction);
                        program_set_vec3(Renderer.forward_spot_program, "spot_light.ambient_color", spot_light->ambient_color);
                        program_set_vec3(Renderer.forward_spot_program, "spot_light.diffuse_color", spot_light->diffuse_color);
                        program_set_vec3(Renderer.forward_spot_program, "spot_light.specular_color", spot_light->specular_color);
                        program_set_vec3(Renderer.forward_spot_program, "spot_light.attenuation", spot_light->attenuation);
                        program_set_float(Renderer.forward_spot_program, "spot_light.inner_cutoff", spot_light->inner_cutoff);
                        program_set_float(Renderer.forward_spot_program, "spot_light.outer_cutoff", spot_light->outer_cutoff);

                        mesh_draw(object->mesh);
                    }

                    program_unbind();
                }

                if (Renderer.skybox)
                {
                    program_bind(Renderer.forward_reflection_program);

                    program_set_mat4(Renderer.forward_reflection_program, "camera.projection", camera_projection);
                    program_set_mat4(Renderer.forward_reflection_program, "camera.view", camera_view);
                    program_set_vec3(Renderer.forward_reflection_program, "camera.position", camera->position);

                    program_set_mat4(Renderer.forward_reflection_program, "object.model", object_model);

                    program_set_vec4(Renderer.forward_reflection_program, "clipping_plane", clipping_plane);

                    program_set_float(Renderer.forward_reflection_program, "material.reflectivity", 0.0f);

                    // TEMPORARY: use specular texture for reflectivity
                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, object->material->specular_map ? object->material->specular_map->texture_id : 0);
                    glActiveTexture(GL_TEXTURE1);
                    glBindTexture(GL_TEXTURE_CUBE_MAP, Renderer.skybox->texture_id);

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
        if (Renderer.num_objects > 0)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, Renderer.geometry_fbo_id);

            glViewport(0, 0, (GLsizei)(Renderer.render_width * Renderer.render_scale), (GLsizei)(Renderer.render_height * Renderer.render_scale));
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glEnable(GL_CLIP_DISTANCE0);

            program_bind(Renderer.geometry_program);

            program_set_mat4(Renderer.geometry_program, "camera.projection", camera_projection);
            program_set_mat4(Renderer.geometry_program, "camera.view", camera_view);
            program_set_vec3(Renderer.geometry_program, "camera.position", camera->position);

            for (unsigned int i = 0; i < Renderer.num_objects; i++)
            {
                struct object *object = Renderer.objects[i];

                mat4 object_model = GLM_MAT4_IDENTITY_INIT;
                object_calc_model(object, object_model);

                program_set_mat4(Renderer.geometry_program, "object.model", object_model);

                program_set_vec4(Renderer.geometry_program, "clipping_plane", clipping_plane);

                program_set_vec3(Renderer.geometry_program, "material.color", object->material->color);
                program_set_float(Renderer.geometry_program, "material.shininess", object->material->shininess);
                program_set_float(Renderer.geometry_program, "material.glow", object->material->glow);

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, object->material->diffuse_map ? object->material->diffuse_map->texture_id : 0);
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, object->material->specular_map ? object->material->specular_map->texture_id : 0);
                glActiveTexture(GL_TEXTURE2);
                glBindTexture(GL_TEXTURE_2D, object->material->normal_map ? object->material->normal_map->texture_id : 0);
                glActiveTexture(GL_TEXTURE3);
                glBindTexture(GL_TEXTURE_2D, object->material->emission_map ? object->material->emission_map->texture_id : 0);

                mesh_draw(object->mesh);
            }

            program_unbind();

            glDisable(GL_CLIP_DISTANCE0);

            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            // draw the gbuffer to the specified framebuffer
            glBindFramebuffer(GL_FRAMEBUFFER, fbo_id);

            glViewport(0, 0, Renderer.render_width, Renderer.render_height);
            glClear(GL_COLOR_BUFFER_BIT);
            glDisable(GL_DEPTH_TEST);

            if (Renderer.sun)
            {
                program_bind(Renderer.deferred_sun_program);

                program_set_vec3(Renderer.deferred_sun_program, "camera.position", camera->position);

                program_set_vec3(Renderer.deferred_sun_program, "sun.direction", Renderer.sun->direction);
                program_set_vec3(Renderer.deferred_sun_program, "sun.ambient_color", Renderer.sun->ambient_color);
                program_set_vec3(Renderer.deferred_sun_program, "sun.diffuse_color", Renderer.sun->diffuse_color);
                program_set_vec3(Renderer.deferred_sun_program, "sun.specular_color", Renderer.sun->specular_color);
                program_set_mat4(Renderer.deferred_sun_program, "sun.projection", sun_projection);
                program_set_mat4(Renderer.deferred_sun_program, "sun.view", sun_view);

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, Renderer.geometry_position_texture_id);
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, Renderer.geometry_normal_texture_id);
                glActiveTexture(GL_TEXTURE2);
                glBindTexture(GL_TEXTURE_2D, Renderer.geometry_albedo_specular_texture_id);
                glActiveTexture(GL_TEXTURE3);
                glBindTexture(GL_TEXTURE_2D, Renderer.depthmap_texture_id);

                glBindVertexArray(Renderer.screen_vao_id);
                glDrawArrays(GL_TRIANGLES, 0, Renderer.num_screen_vertices);
                glBindVertexArray(0);

                program_unbind();
            }

            glEnable(GL_BLEND);
            glBlendFunc(GL_ONE, GL_ONE);
            glDepthMask(GL_FALSE);
            glDepthFunc(GL_EQUAL);

            if (Renderer.num_directional_lights > 0)
            {
                program_bind(Renderer.deferred_directional_program);

                program_set_vec3(Renderer.deferred_directional_program, "camera.position", camera->position);

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, Renderer.geometry_position_texture_id);
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, Renderer.geometry_normal_texture_id);
                glActiveTexture(GL_TEXTURE2);
                glBindTexture(GL_TEXTURE_2D, Renderer.geometry_albedo_specular_texture_id);

                for (unsigned int i = 0; i < Renderer.num_directional_lights; i++)
                {
                    struct directional_light *directional_light = Renderer.directional_lights[i];

                    program_set_vec3(Renderer.deferred_directional_program, "directional_light.direction", directional_light->direction);
                    program_set_vec3(Renderer.deferred_directional_program, "directional_light.ambient_color", directional_light->ambient_color);
                    program_set_vec3(Renderer.deferred_directional_program, "directional_light.diffuse_color", directional_light->diffuse_color);
                    program_set_vec3(Renderer.deferred_directional_program, "directional_light.specular_color", directional_light->specular_color);

                    glBindVertexArray(Renderer.screen_vao_id);
                    glDrawArrays(GL_TRIANGLES, 0, Renderer.num_screen_vertices);
                    glBindVertexArray(0);
                }

                program_unbind();
            }

            if (Renderer.num_point_lights > 0)
            {
                program_bind(Renderer.deferred_point_program);

                program_set_vec3(Renderer.deferred_point_program, "camera.position", camera->position);

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, Renderer.geometry_position_texture_id);
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, Renderer.geometry_normal_texture_id);
                glActiveTexture(GL_TEXTURE2);
                glBindTexture(GL_TEXTURE_2D, Renderer.geometry_albedo_specular_texture_id);

                for (unsigned int i = 0; i < Renderer.num_point_lights; i++)
                {
                    struct point_light *point_light = Renderer.point_lights[i];

                    program_set_vec3(Renderer.deferred_point_program, "point_light.position", point_light->position);
                    program_set_vec3(Renderer.deferred_point_program, "point_light.ambient_color", point_light->ambient_color);
                    program_set_vec3(Renderer.deferred_point_program, "point_light.diffuse_color", point_light->diffuse_color);
                    program_set_vec3(Renderer.deferred_point_program, "point_light.specular_color", point_light->specular_color);
                    program_set_vec3(Renderer.deferred_point_program, "point_light.attenuation", point_light->attenuation);

                    glBindVertexArray(Renderer.screen_vao_id);
                    glDrawArrays(GL_TRIANGLES, 0, Renderer.num_screen_vertices);
                    glBindVertexArray(0);
                }

                program_unbind();
            }

            if (Renderer.num_spot_lights > 0)
            {
                program_bind(Renderer.deferred_spot_program);

                program_set_vec3(Renderer.deferred_spot_program, "camera.position", camera->position);

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, Renderer.geometry_position_texture_id);
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, Renderer.geometry_normal_texture_id);
                glActiveTexture(GL_TEXTURE2);
                glBindTexture(GL_TEXTURE_2D, Renderer.geometry_albedo_specular_texture_id);

                for (unsigned int i = 0; i < Renderer.num_spot_lights; i++)
                {
                    struct spot_light *spot_light = Renderer.spot_lights[i];

                    program_set_vec3(Renderer.deferred_spot_program, "spot_light.position", spot_light->position);
                    program_set_vec3(Renderer.deferred_spot_program, "spot_light.direction", spot_light->direction);
                    program_set_vec3(Renderer.deferred_spot_program, "spot_light.ambient_color", spot_light->ambient_color);
                    program_set_vec3(Renderer.deferred_spot_program, "spot_light.diffuse_color", spot_light->diffuse_color);
                    program_set_vec3(Renderer.deferred_spot_program, "spot_light.specular_color", spot_light->specular_color);
                    program_set_vec3(Renderer.deferred_spot_program, "spot_light.attenuation", spot_light->attenuation);
                    program_set_float(Renderer.deferred_spot_program, "spot_light.inner_cutoff", spot_light->inner_cutoff);
                    program_set_float(Renderer.deferred_spot_program, "spot_light.outer_cutoff", spot_light->outer_cutoff);

                    glBindVertexArray(Renderer.screen_vao_id);
                    glDrawArrays(GL_TRIANGLES, 0, Renderer.num_screen_vertices);
                    glBindVertexArray(0);
                }

                program_unbind();
            }

            glDepthFunc(GL_LESS);
            glDepthMask(GL_TRUE);
            glDisable(GL_BLEND);

            glEnable(GL_DEPTH_TEST);

            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            // copy depth information to specified framebuffer
            glBindFramebuffer(GL_READ_FRAMEBUFFER, Renderer.geometry_fbo_id);
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo_id);
            glBlitFramebuffer(0, 0, Renderer.render_width, Renderer.render_height, 0, 0, Renderer.render_width, Renderer.render_height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }
    }
    break;
    }

    // draw other things that need to be drawn after the deferred rendering pass
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_id);

    glViewport(0, 0, Renderer.render_width, Renderer.render_height);

    // render skybox
    if (Renderer.skybox)
    {
        glDepthFunc(GL_LEQUAL);

        mat4 camera_view_no_translate;
        glm_mat4_copy(camera_view, camera_view_no_translate);
        // glm_rotate(camera_view_no_translate, glm_rad(elapsed_time) * 0.001, GLM_YUP);
        camera_view_no_translate[3][0] = 0.0f;
        camera_view_no_translate[3][1] = 0.0f;
        camera_view_no_translate[3][2] = 0.0f;

        program_bind(Renderer.skybox_program);

        program_set_mat4(Renderer.skybox_program, "camera.projection", camera_projection);
        program_set_mat4(Renderer.skybox_program, "camera.view", camera_view_no_translate);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, Renderer.skybox->texture_id);

        glBindVertexArray(Renderer.skybox_vao_id);
        glDrawArrays(GL_TRIANGLES, 0, Renderer.num_skybox_vertices);
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

    mat4 sun_projection;
    mat4 sun_view;
    if (Renderer.sun)
    {
        // calculate sun projection matrix
        glm_ortho(-10.0f, 10.0f, -10.0f, 10.0f, -10.0f, 10.0f, sun_projection);

        // calculate sun "position"
        vec3 sun_position;
        glm_vec_sub(GLM_VEC3_ZERO, Renderer.sun->direction, sun_position);

        // calculate sun view matrix
        glm_lookat(sun_position, GLM_VEC3_ZERO, GLM_YUP, sun_view);
    }

    // send camera transformations to water program
    program_bind(Renderer.water_program);

    program_set_mat4(Renderer.water_program, "camera.projection", camera_projection);
    program_set_mat4(Renderer.water_program, "camera.view", camera_view);
    program_set_vec3(Renderer.water_program, "camera.position", camera->position);

    if (Renderer.sun)
    {
        program_set_vec3(Renderer.water_program, "sun.direction", Renderer.sun->direction);
        program_set_vec3(Renderer.water_program, "sun.ambient_color", Renderer.sun->ambient_color);
        program_set_vec3(Renderer.water_program, "sun.diffuse_color", Renderer.sun->diffuse_color);
        program_set_vec3(Renderer.water_program, "sun.specular_color", Renderer.sun->specular_color);
        program_set_mat4(Renderer.water_program, "sun.projection", sun_projection);
        program_set_mat4(Renderer.water_program, "sun.view", sun_view);
    }

    program_set_unsigned_int(Renderer.water_program, "elapsed_time", elapsed_time);

    program_unbind();

    for (unsigned int i = 0; i < Renderer.num_waters; i++)
    {
        struct water *water = Renderer.waters[i];

        // render water reflection
        vec4 reflection_clipping_plane = { 0.0f, 1.0f, 0.0f, -water->position[1] };
        bool reflect = false;

        if (camera->position[1] > water->position[1]) // don't draw reflections if under the water
        {
            reflect = true;

            float old_camera_y = camera->position[1];
            float old_camera_pitch = camera->pitch;

            camera->position[1] -= 2 * (camera->position[1] - water->position[1]);
            camera->pitch = -camera->pitch;

            render_scene(Renderer.water_reflection_fbo_id, camera, aspect, elapsed_time, reflection_clipping_plane);

            camera->position[1] = old_camera_y;
            camera->pitch = old_camera_pitch;
        }

        // render water refraction
        vec4 refraction_clipping_plane = { 0.0f, -1.0f, 0.0f, water->position[1] };

        if (camera->position[1] < water->position[1]) // flip refraction clipping plane if under the water
        {
            refraction_clipping_plane[1] = 1.0f;
            refraction_clipping_plane[3] = -water->position[1];
        }

        render_scene(Renderer.water_refraction_fbo_id, camera, aspect, elapsed_time, refraction_clipping_plane);

        glBindFramebuffer(GL_FRAMEBUFFER, fbo_id);

        program_bind(Renderer.water_program);

        mat4 water_model = GLM_MAT4_IDENTITY_INIT;
        glm_translate(water_model, water->position);
        vec3 water_scale;
        water_scale[0] = water->scale[0];
        water_scale[1] = 1.0f;
        water_scale[2] = water->scale[1];
        glm_scale(water_model, water_scale);

        program_set_mat4(Renderer.water_program, "water.model", water_model);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, reflect ? Renderer.water_reflection_color_texture_id : 0);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, Renderer.water_refraction_color_texture_id);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, Renderer.water_dudv_texture ? Renderer.water_dudv_texture->texture_id : 0);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, Renderer.water_normal_texture ? Renderer.water_normal_texture->texture_id : 0);
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, Renderer.depthmap_texture_id);

        glBindVertexArray(Renderer.water_vao_id);
        glDrawArrays(GL_TRIANGLES, 0, Renderer.num_water_vertices);
        glBindVertexArray(0);

        program_unbind();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}

void render_sprites(GLuint fbo_id, float aspect)
{
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_id);

    program_bind(Renderer.sprite_program);

    // calculate ortho projection
    mat4 camera_projection_ortho;
    glm_ortho_default(aspect, camera_projection_ortho);

    program_set_mat4(Renderer.sprite_program, "camera.projection", camera_projection_ortho);

    for (unsigned int i = 0; i < Renderer.num_sprites; i++)
    {
        struct sprite *sprite = Renderer.sprites[i];

        mat4 sprite_model = GLM_MAT4_IDENTITY_INIT;
        sprite_calc_model(sprite, sprite_model);

        program_set_mat4(Renderer.sprite_program, "sprite.model", sprite_model);
        program_set_vec3(Renderer.sprite_program, "sprite.color", sprite->color);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, sprite->color_map->texture_id);

        glBindVertexArray(Renderer.sprite_vao_id);
        glDrawArrays(GL_TRIANGLES, 0, Renderer.num_sprite_vertices);
        glBindVertexArray(0);
    }

    program_unbind();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void DEBUG_render_point_lights(GLuint fbo_id, struct camera *camera, float aspect)
{
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_id);

    // draw point lights as a cube
    if (Renderer.num_point_lights > 0)
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

        program_bind(Renderer.forward_color_program);

        program_set_mat4(Renderer.forward_color_program, "camera.projection", camera_projection);
        program_set_mat4(Renderer.forward_color_program, "camera.view", camera_view);

        for (unsigned int i = 0; i < Renderer.num_point_lights; i++)
        {
            struct point_light *point_light = Renderer.point_lights[i];

            mat4 object_model = GLM_MAT4_IDENTITY_INIT;
            glm_translate(object_model, point_light->position);
            vec3 scale = { 0.125f, 0.125f, 0.125f };
            glm_scale(object_model, scale);

            program_set_mat4(Renderer.forward_color_program, "object.model", object_model);

            program_set_vec3(Renderer.forward_color_program, "material.color", point_light->specular_color);

            mesh_draw(assets.cube_mesh);
        }

        program_unbind();
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void DEBUG_render_geometry_fbos(GLuint fbo_id, struct camera *camera, float aspect)
{
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_id);

    // draw gbuffers
    program_bind(Renderer.sprite_program);

    // calculate ortho projection
    mat4 camera_projection_ortho;
    glm_ortho_default(aspect, camera_projection_ortho);

    program_set_mat4(Renderer.sprite_program, "camera.projection", camera_projection_ortho);

    vec3 position = GLM_VEC3_ZERO_INIT;
    vec3 color = GLM_VEC3_ONE_INIT;

    position[0] = -1.75f;
    position[1] = 0.0f;

    {
        mat4 sprite_model = GLM_MAT4_IDENTITY_INIT;
        glm_translate(sprite_model, position);

        program_set_mat4(Renderer.sprite_program, "sprite.model", sprite_model);
        program_set_vec3(Renderer.sprite_program, "sprite.color", color);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, Renderer.geometry_position_texture_id);

        glBindVertexArray(Renderer.sprite_vao_id);
        glDrawArrays(GL_TRIANGLES, 0, Renderer.num_sprite_vertices);
        glBindVertexArray(0);
    }

    position[0] += 1.0f;

    {
        mat4 sprite_model = GLM_MAT4_IDENTITY_INIT;
        glm_translate(sprite_model, position);

        program_set_mat4(Renderer.sprite_program, "sprite.model", sprite_model);
        program_set_vec3(Renderer.sprite_program, "sprite.color", color);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, Renderer.geometry_normal_texture_id);

        glBindVertexArray(Renderer.sprite_vao_id);
        glDrawArrays(GL_TRIANGLES, 0, Renderer.num_sprite_vertices);
        glBindVertexArray(0);
    }

    position[0] += 1.0f;

    {
        mat4 sprite_model = GLM_MAT4_IDENTITY_INIT;
        glm_translate(sprite_model, position);

        program_set_mat4(Renderer.sprite_program, "sprite.model", sprite_model);
        program_set_vec3(Renderer.sprite_program, "sprite.color", color);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, Renderer.geometry_albedo_specular_texture_id);

        glBindVertexArray(Renderer.sprite_vao_id);
        glDrawArrays(GL_TRIANGLES, 0, Renderer.num_sprite_vertices);
        glBindVertexArray(0);
    }

    program_unbind();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void DEBUG_render_depthmap_fbo(GLuint fbo_id, struct camera *camera, float aspect)
{
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_id);

    // draw depthmap
    program_bind(Renderer.sprite_program);

    // calculate ortho projection
    mat4 camera_projection_ortho;
    glm_ortho_default(aspect, camera_projection_ortho);

    program_set_mat4(Renderer.sprite_program, "camera.projection", camera_projection_ortho);

    vec3 position = GLM_VEC3_ZERO_INIT;
    vec3 color = GLM_VEC3_ONE_INIT;

    position[0] = -1.75f;
    position[1] = -1.0f;

    {
        mat4 sprite_model = GLM_MAT4_IDENTITY_INIT;
        glm_translate(sprite_model, position);

        program_set_mat4(Renderer.sprite_program, "sprite.model", sprite_model);
        program_set_vec3(Renderer.sprite_program, "sprite.color", color);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, Renderer.depthmap_texture_id);

        glBindVertexArray(Renderer.sprite_vao_id);
        glDrawArrays(GL_TRIANGLES, 0, Renderer.num_sprite_vertices);
        glBindVertexArray(0);
    }

    program_unbind();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void DEBUG_render_water_fbos(GLuint fbo_id, struct camera *camera, float aspect)
{
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_id);

    // draw water reflection and refraction textures
    // NOTE: this will draw the textures for whatever water tile was rendered last, since each water tile calculates its own reflection/refraction textures
    program_bind(Renderer.sprite_program);

    // calculate ortho projection
    mat4 camera_projection_ortho;
    glm_ortho_default(aspect, camera_projection_ortho);

    program_set_mat4(Renderer.sprite_program, "camera.projection", camera_projection_ortho);

    vec3 position = GLM_VEC3_ZERO_INIT;
    vec3 color = GLM_VEC3_ONE_INIT;

    position[0] = -0.75f;
    position[1] = -1.0f;

    {
        mat4 sprite_model = GLM_MAT4_IDENTITY_INIT;
        glm_translate(sprite_model, position);

        program_set_mat4(Renderer.sprite_program, "sprite.model", sprite_model);
        program_set_vec3(Renderer.sprite_program, "sprite.color", color);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, Renderer.water_reflection_color_texture_id);

        glBindVertexArray(Renderer.sprite_vao_id);
        glDrawArrays(GL_TRIANGLES, 0, Renderer.num_sprite_vertices);
        glBindVertexArray(0);
    }

    position[0] += 1.0f;

    {
        mat4 sprite_model = GLM_MAT4_IDENTITY_INIT;
        glm_translate(sprite_model, position);

        program_set_mat4(Renderer.sprite_program, "sprite.model", sprite_model);
        program_set_vec3(Renderer.sprite_program, "sprite.color", color);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, Renderer.water_refraction_color_texture_id);

        glBindVertexArray(Renderer.sprite_vao_id);
        glDrawArrays(GL_TRIANGLES, 0, Renderer.num_sprite_vertices);
        glBindVertexArray(0);
    }

    program_unbind();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void render_screen(GLuint fbo_id)
{
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_id);

    glViewport(0, 0, Renderer.render_width, Renderer.render_height);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);

    program_bind(Renderer.screen_program);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, Renderer.screen_texture_id);

    glBindVertexArray(Renderer.screen_vao_id);
    glDrawArrays(GL_TRIANGLES, 0, Renderer.num_screen_vertices);
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

    render_scene(Renderer.screen_fbo_id, camera, aspect, elapsed_time, clipping_plane);

    // DEBUG_render_point_lights(renderer.screen_fbo_id, camera, aspect);
    if (Renderer.render_mode == RENDER_MODE_DEFERRED)
    {
        // DEBUG_render_geometry_fbos(renderer.screen_fbo_id, camera, aspect);
    }
    // DEBUG_render_depthmap_fbo(renderer.screen_fbo_id, camera, aspect);

    // render water
    if (Renderer.num_waters > 0)
    {
        render_waters(Renderer.screen_fbo_id, camera, aspect, elapsed_time);
    }

    // DEBUG_render_water_fbos(renderer.screen_fbo_id, camera, aspect);

    // render sprites
    if (Renderer.num_sprites > 0)
    {
        render_sprites(Renderer.screen_fbo_id, aspect);
    }

    // render the screen framebuffer to the default framebuffer and apply post-processing
    render_screen(0);

    // clear renderables
    Renderer.num_objects = 0;
    Renderer.num_directional_lights = 0;
    Renderer.num_point_lights = 0;
    Renderer.num_spot_lights = 0;
    Renderer.num_waters = 0;
    Renderer.num_sprites = 0;
}

void renderer_quit(void)
{
    glDeleteBuffers(1, &Renderer.sprite_vbo_id);
    glDeleteVertexArrays(1, &Renderer.sprite_vao_id);

    free(Renderer.spot_lights);
    free(Renderer.point_lights);
    free(Renderer.objects);

    glDeleteRenderbuffers(1, &Renderer.geometry_rbo_id);
    glDeleteTextures(1, &Renderer.geometry_albedo_specular_texture_id);
    glDeleteTextures(1, &Renderer.geometry_normal_texture_id);
    glDeleteTextures(1, &Renderer.geometry_position_texture_id);
    glDeleteFramebuffers(1, &Renderer.geometry_fbo_id);

    glDeleteTextures(1, &Renderer.depthmap_texture_id);
    glDeleteFramebuffers(1, &Renderer.depthmap_fbo_id);

    glDeleteRenderbuffers(1, &Renderer.screen_rbo_id);
    glDeleteTextures(1, &Renderer.screen_texture_id);
    glDeleteFramebuffers(1, &Renderer.screen_fbo_id);

    program_destroy(Renderer.depth_program);
    program_destroy(Renderer.screen_program);
    program_destroy(Renderer.geometry_program);
    program_destroy(Renderer.deferred_spot_program);
    program_destroy(Renderer.deferred_point_program);
    program_destroy(Renderer.deferred_directional_program);
    program_destroy(Renderer.deferred_sun_program);
    program_destroy(Renderer.forward_spot_program);
    program_destroy(Renderer.forward_point_program);
    program_destroy(Renderer.forward_directional_program);
    program_destroy(Renderer.forward_sun_program);
    program_destroy(Renderer.forward_color_program);

    assets_unload();
}
