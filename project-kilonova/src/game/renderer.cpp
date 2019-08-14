#include <game/game.hpp>

namespace pk
{
    renderer::renderer(int render_width, int render_height, float render_scale, int shadow_width, int shadow_height)
    {
        this->render_width = render_width;
        this->render_height = render_height;
        this->render_scale = render_scale;
        this->shadow_width = shadow_width;
        this->shadow_height = shadow_height;
        this->reflection_width = render_width;
        this->reflection_height = render_height;
        this->refraction_width = render_width;
        this->refraction_height = render_height;
        this->render_mode = RENDER_MODE_FORWARD;

        // init GLEW
        {
            GLenum glewError = glewInit();

            if (glewError != GLEW_OK)
            {
                std::cout << glewGetErrorString(glewError) << std::endl;

                throw std::exception("Couldn't initialize glew... see log");
            }
        }

        std::cout << "GLEW " << glewGetString(GLEW_VERSION) << std::endl;
        std::cout << "OpenGL " << glGetString(GL_VERSION) << std::endl;
        std::cout << "Vendor " << glGetString(GL_VENDOR) << std::endl;
        std::cout << "Renderer " << glGetString(GL_RENDERER) << std::endl;
        std::cout << "GLSL " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

        // setup opengl
        glViewport(0, 0, render_width, render_height);
        glClearColor(1.0f, 0.0f, 1.0f, 0.0f);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glEnable(GL_STENCIL_TEST);

        // create programs
        this->depth_program = new pk::program(
            "assets/shaders/depth.vert",
            "assets/shaders/depth.frag");
        this->forward_color_program = new pk::program(
            "assets/shaders/forward.vert",
            "assets/shaders/forward_color.frag");
        this->forward_sun_program = new pk::program(
            "assets/shaders/forward.vert",
            "assets/shaders/forward_sun.frag");
        this->forward_directional_program = new pk::program(
            "assets/shaders/forward.vert",
            "assets/shaders/forward_directional.frag");
        this->forward_point_program = new pk::program(
            "assets/shaders/forward.vert",
            "assets/shaders/forward_point.frag");
        this->forward_spot_program = new pk::program(
            "assets/shaders/forward.vert",
            "assets/shaders/forward_spot.frag");
        this->forward_reflection_program = new pk::program(
            "assets/shaders/forward.vert",
            "assets/shaders/forward_reflection.frag");
        this->geometry_program = new pk::program(
            "assets/shaders/geometry.vert",
            "assets/shaders/geometry.frag");
        this->deferred_sun_program = new pk::program(
            "assets/shaders/deferred.vert",
            "assets/shaders/deferred_sun.frag");
        this->deferred_directional_program = new pk::program(
            "assets/shaders/deferred.vert",
            "assets/shaders/deferred_directional.frag");
        this->deferred_point_program = new pk::program(
            "assets/shaders/deferred.vert",
            "assets/shaders/deferred_point.frag");
        this->deferred_spot_program = new pk::program(
            "assets/shaders/deferred.vert",
            "assets/shaders/deferred_spot.frag");
        this->skybox_program = new pk::program(
            "assets/shaders/skybox.vert",
            "assets/shaders/skybox.frag");
        this->water_program = new pk::program(
            "assets/shaders/water.vert",
            "assets/shaders/water.frag");
        this->sprite_program = new pk::program(
            "assets/shaders/sprite.vert",
            "assets/shaders/sprite.frag");
        this->screen_program = new pk::program(
            "assets/shaders/screen.vert",
            "assets/shaders/screen.frag");

        // setup shader samplers
        this->forward_sun_program->bind();
        this->forward_sun_program->set_int("material.diffuse_map", 0);
        this->forward_sun_program->set_int("material.specular_map", 1);
        this->forward_sun_program->set_int("material.normal_map", 2);
        this->forward_sun_program->set_int("material.emission_map", 3);
        this->forward_sun_program->set_int("sun.depth_map", 4);
        this->forward_sun_program->unbind();

        this->forward_directional_program->bind();
        this->forward_directional_program->set_int("material.diffuse_map", 0);
        this->forward_directional_program->set_int("material.specular_map", 1);
        this->forward_directional_program->set_int("material.normal_map", 2);
        this->forward_directional_program->set_int("material.emission_map", 3);
        this->forward_directional_program->unbind();

        this->forward_point_program->bind();
        this->forward_point_program->set_int("material.diffuse_map", 0);
        this->forward_point_program->set_int("material.specular_map", 1);
        this->forward_point_program->set_int("material.normal_map", 2);
        this->forward_point_program->set_int("material.emission_map", 3);
        this->forward_point_program->unbind();

        this->forward_spot_program->bind();
        this->forward_spot_program->set_int("material.diffuse_map", 0);
        this->forward_spot_program->set_int("material.specular_map", 1);
        this->forward_spot_program->set_int("material.normal_map", 2);
        this->forward_spot_program->set_int("material.emission_map", 3);
        this->forward_spot_program->unbind();

        this->forward_reflection_program->bind();
        this->forward_reflection_program->set_int("material.reflectivity_map", 0);
        this->forward_reflection_program->set_int("skybox.color_map", 1);
        this->forward_reflection_program->unbind();

        this->geometry_program->bind();
        this->geometry_program->set_int("material.diffuse_map", 0);
        this->geometry_program->set_int("material.specular_map", 1);
        this->geometry_program->set_int("material.normal_map", 2);
        this->geometry_program->set_int("material.emission_map", 3);
        this->geometry_program->unbind();

        this->deferred_sun_program->bind();
        this->deferred_sun_program->set_int("geometry.position_map", 0);
        this->deferred_sun_program->set_int("geometry.normal_map", 1);
        this->deferred_sun_program->set_int("geometry.albedo_specular_map", 2);
        this->deferred_sun_program->set_int("sun.depth_map", 3);
        this->deferred_sun_program->unbind();

        this->deferred_directional_program->bind();
        this->deferred_directional_program->set_int("geometry.position_map", 0);
        this->deferred_directional_program->set_int("geometry.normal_map", 1);
        this->deferred_directional_program->set_int("geometry.albedo_specular_map", 2);
        this->deferred_directional_program->unbind();

        this->deferred_point_program->bind();
        this->deferred_point_program->set_int("geometry.position_map", 0);
        this->deferred_point_program->set_int("geometry.normal_map", 1);
        this->deferred_point_program->set_int("geometry.albedo_specular_map", 2);
        this->deferred_point_program->unbind();

        this->deferred_spot_program->bind();
        this->deferred_spot_program->set_int("geometry.position_map", 0);
        this->deferred_spot_program->set_int("geometry.normal_map", 1);
        this->deferred_spot_program->set_int("geometry.albedo_specular_map", 2);
        this->deferred_spot_program->unbind();

        this->skybox_program->bind();
        this->skybox_program->set_int("skybox.color_map", 0);
        this->skybox_program->unbind();

        this->water_program->bind();
        this->water_program->set_int("water.reflection_map", 0);
        this->water_program->set_int("water.refraction_map", 1);
        this->water_program->set_int("water.dudv_map", 2);
        this->water_program->set_int("water.normal_map", 3);
        this->water_program->set_int("sun.depth_map", 4);
        this->water_program->unbind();

        this->sprite_program->bind();
        this->sprite_program->set_int("sprite.color_map", 0);
        this->sprite_program->unbind();

        this->screen_program->bind();
        this->screen_program->set_int("screen.color_map", 0);
        this->screen_program->unbind();

        // setup screen fbo
        glGenFramebuffers(1, &this->screen_fbo_id);

        glGenTextures(1, &this->screen_texture_id);
        glBindTexture(GL_TEXTURE_2D, this->screen_texture_id);

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

        glGenRenderbuffers(1, &this->screen_rbo_id);
        glBindRenderbuffer(GL_RENDERBUFFER, this->screen_rbo_id);

        glRenderbufferStorage(
            GL_RENDERBUFFER,
            GL_DEPTH24_STENCIL8,
            render_width,
            render_height);

        glBindRenderbuffer(GL_RENDERBUFFER, 0);

        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, this->screen_fbo_id);

        glFramebufferTexture2D(
            GL_FRAMEBUFFER,
            GL_COLOR_ATTACHMENT0,
            GL_TEXTURE_2D,
            this->screen_texture_id,
            0);
        glFramebufferRenderbuffer(
            GL_FRAMEBUFFER,
            GL_DEPTH_STENCIL_ATTACHMENT,
            GL_RENDERBUFFER,
            this->screen_rbo_id);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            throw std::exception("Couldn't complete screen framebuffer");
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // setup depthmap fbo
        glGenFramebuffers(1, &this->depthmap_fbo_id);
        glBindFramebuffer(GL_FRAMEBUFFER, this->depthmap_fbo_id);

        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);

        glGenTextures(1, &this->depthmap_texture_id);
        glBindTexture(GL_TEXTURE_2D, this->depthmap_texture_id);

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
            this->depthmap_texture_id,
            0);

        glBindTexture(GL_TEXTURE_2D, 0);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            throw std::exception("Couldn't complete depthmap framebuffer");
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // setup geometry fbo
        glGenFramebuffers(1, &this->geometry_fbo_id);
        glBindFramebuffer(GL_FRAMEBUFFER, this->geometry_fbo_id);

        glGenTextures(1, &this->geometry_position_texture_id);
        glBindTexture(GL_TEXTURE_2D, this->geometry_position_texture_id);

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
            this->geometry_position_texture_id,
            0);

        glBindTexture(GL_TEXTURE_2D, 0);

        glGenTextures(1, &this->geometry_normal_texture_id);
        glBindTexture(GL_TEXTURE_2D, this->geometry_normal_texture_id);

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
            this->geometry_normal_texture_id,
            0);

        glBindTexture(GL_TEXTURE_2D, 0);

        glGenTextures(1, &this->geometry_albedo_specular_texture_id);
        glBindTexture(GL_TEXTURE_2D, this->geometry_albedo_specular_texture_id);

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
            this->geometry_albedo_specular_texture_id,
            0);

        glBindTexture(GL_TEXTURE_2D, 0);

        GLenum geometry_fbo_attachments[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
        glDrawBuffers(sizeof(geometry_fbo_attachments) / sizeof(GLenum), geometry_fbo_attachments);

        glGenRenderbuffers(1, &this->geometry_rbo_id);
        glBindRenderbuffer(GL_RENDERBUFFER, this->geometry_rbo_id);

        glRenderbufferStorage(
            GL_RENDERBUFFER,
            GL_DEPTH_COMPONENT,
            (GLsizei)(render_width * render_scale),
            (GLsizei)(render_height * render_scale));

        glFramebufferRenderbuffer(
            GL_FRAMEBUFFER,
            GL_DEPTH_ATTACHMENT,
            GL_RENDERBUFFER,
            this->geometry_rbo_id);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            throw std::exception("Couldn't complete geometry framebuffer");
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // setup water reflection fbo
        glGenFramebuffers(1, &this->water_reflection_fbo_id);
        glBindFramebuffer(GL_FRAMEBUFFER, this->water_reflection_fbo_id);

        glGenTextures(1, &this->water_reflection_color_texture_id);
        glBindTexture(GL_TEXTURE_2D, this->water_reflection_color_texture_id);

        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RGB8,
            this->reflection_width,
            this->reflection_height,
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
            this->water_reflection_color_texture_id,
            0);

        glBindTexture(GL_TEXTURE_2D, 0);

        GLenum water_reflection_fbo_attachments[] = { GL_COLOR_ATTACHMENT0 };
        glDrawBuffers(sizeof(water_reflection_fbo_attachments) / sizeof(GLenum), water_reflection_fbo_attachments);

        glGenTextures(1, &this->water_reflection_depth_texture_id);
        glBindTexture(GL_TEXTURE_2D, this->water_reflection_depth_texture_id);

        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_DEPTH_COMPONENT32,
            this->reflection_width,
            this->reflection_height,
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
            this->water_reflection_depth_texture_id,
            0);

        glBindTexture(GL_TEXTURE_2D, 0);

        glGenRenderbuffers(1, &this->water_reflection_depth_rbo_id);
        glBindRenderbuffer(GL_RENDERBUFFER, this->water_reflection_depth_rbo_id);

        glRenderbufferStorage(
            GL_RENDERBUFFER,
            GL_DEPTH_COMPONENT,
            this->reflection_width,
            this->reflection_height);

        glFramebufferRenderbuffer(
            GL_FRAMEBUFFER,
            GL_DEPTH_ATTACHMENT,
            GL_RENDERBUFFER,
            this->water_reflection_depth_rbo_id);

        glBindRenderbuffer(GL_RENDERBUFFER, 0);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            throw std::exception("Couldn't complete water reflection framebuffer");
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // setup water refraction fbo
        glGenFramebuffers(1, &this->water_refraction_fbo_id);
        glBindFramebuffer(GL_FRAMEBUFFER, this->water_refraction_fbo_id);

        glGenTextures(1, &this->water_refraction_color_texture_id);
        glBindTexture(GL_TEXTURE_2D, this->water_refraction_color_texture_id);

        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RGB8,
            this->refraction_width,
            this->refraction_height,
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
            this->water_refraction_color_texture_id,
            0);

        glBindTexture(GL_TEXTURE_2D, 0);

        GLenum water_refraction_fbo_attachments[] = { GL_COLOR_ATTACHMENT0 };
        glDrawBuffers(sizeof(water_refraction_fbo_attachments) / sizeof(GLenum), water_refraction_fbo_attachments);

        glGenTextures(1, &this->water_refraction_depth_texture_id);
        glBindTexture(GL_TEXTURE_2D, this->water_refraction_depth_texture_id);

        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_DEPTH_COMPONENT32,
            this->refraction_width,
            this->refraction_height,
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
            this->water_refraction_depth_texture_id,
            0);

        glBindTexture(GL_TEXTURE_2D, 0);

        glGenRenderbuffers(1, &this->water_refraction_depth_rbo_id);
        glBindRenderbuffer(GL_RENDERBUFFER, this->water_refraction_depth_rbo_id);

        glRenderbufferStorage(
            GL_RENDERBUFFER,
            GL_DEPTH_COMPONENT,
            this->refraction_width,
            this->refraction_height);

        glFramebufferRenderbuffer(
            GL_FRAMEBUFFER,
            GL_DEPTH_ATTACHMENT,
            GL_RENDERBUFFER,
            this->water_refraction_depth_rbo_id);

        glBindRenderbuffer(GL_RENDERBUFFER, 0);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            throw std::exception("Couldn't complete water refraction framebuffer");
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
        this->num_water_vertices = sizeof(water_vertices) / sizeof(float) / 2;

        glGenVertexArrays(1, &this->water_vao_id);
        glGenBuffers(1, &this->water_vbo_id);

        glBindVertexArray(this->water_vao_id);

        glBindBuffer(GL_ARRAY_BUFFER, this->water_vbo_id);
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
        this->num_skybox_vertices = sizeof(skybox_vertices) / sizeof(float) / 3;

        glGenVertexArrays(1, &this->skybox_vao_id);
        glGenBuffers(1, &this->skybox_vbo_id);

        glBindVertexArray(this->skybox_vao_id);

        glBindBuffer(GL_ARRAY_BUFFER, this->skybox_vbo_id);
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
        this->num_sprite_vertices = sizeof(sprite_vertices) / sizeof(float) / 4;

        glGenVertexArrays(1, &this->sprite_vao_id);
        glGenBuffers(1, &this->sprite_vbo_id);

        glBindVertexArray(this->sprite_vao_id);

        glBindBuffer(GL_ARRAY_BUFFER, this->sprite_vbo_id);
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
        this->num_screen_vertices = sizeof(screen_vertices) / sizeof(float) / 4;

        glGenVertexArrays(1, &this->screen_vao_id);
        glGenBuffers(1, &this->screen_vbo_id);

        glBindVertexArray(this->screen_vao_id);

        glBindBuffer(GL_ARRAY_BUFFER, this->screen_vbo_id);
        glBufferData(GL_ARRAY_BUFFER, sizeof(screen_vertices), &screen_vertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid *)(0 * sizeof(GLfloat))); // position
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid *)(2 * sizeof(GLfloat))); // uv
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);

        // create water dudv texture
        this->water_dudv_texture = new pk::texture("assets/images/water_dudv.png");

        if (!this->water_dudv_texture)
        {
            throw std::exception("Couldn't load water dudv texture");
        }

        // create water normal texture
        this->water_normal_texture = new pk::texture("assets/images/water_normal.png");

        if (!this->water_normal_texture)
        {
            throw std::exception("Couldn't load water normal texture");
        }
    }

    renderer::~renderer()
    {

    }

    void renderer::set_mode(pk::render_mode render_mode)
    {
        this->render_mode = render_mode;
    }

    void renderer::add_object(pk::object *object)
    {
        this->objects.push_back(object);
    }

    void renderer::set_sun(pk::sun *sun)
    {
        this->sun = sun;
    }

    void renderer::add_directional_light(pk::directional_light *directional_light)
    {
        this->directional_lights.push_back(directional_light);
    }

    void renderer::add_point_light(pk::point_light *point_light)
    {
        this->point_lights.push_back(point_light);
    }

    void renderer::add_spot_light(pk::spot_light *spot_light)
    {
        this->spot_lights.push_back(spot_light);
    }

    void renderer::set_skybox(pk::cubemap *skybox)
    {
        this->skybox = skybox;
    }

    void renderer::add_water(pk::water *water)
    {
        this->waters.push_back(water);
    }

    void renderer::add_sprite(pk::sprite *sprite)
    {
        this->sprites.push_back(sprite);
    }

    void renderer::render_scene(GLuint fbo_id, struct camera *camera, float aspect, unsigned int elapsed_time, vec4 clipping_plane)
    {
        // clear the framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, fbo_id);

        glViewport(0, 0, this->render_width, this->render_height);
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
        camera->calc_front(&camera_front);

        vec3 camera_target;
        glm_vec_add(camera->position, camera_front, camera_target);

        vec3 camera_up;
        camera->calc_up(&camera_up);

        mat4 camera_view;
        glm_lookat(
            camera->position,
            camera_target,
            camera_up,
            camera_view);

        mat4 sun_projection;
        mat4 sun_view;
        if (this->sun)
        {
            // calculate sun projection matrix
            glm_ortho(-10.0f, 10.0f, -10.0f, 10.0f, -10.0f, 10.0f, sun_projection);

            // calculate sun "position"
            vec3 sun_position;
            glm_vec_sub(vec3{ 0.0f, 0.0f, 0.0f }, this->sun->direction, sun_position);

            // calculate sun view matrix
            glm_lookat(sun_position, vec3{ 0.0f, 0.0f, 0.0f }, vec3{ 0.0f, 1.0f, 0.0f }, sun_view);

            // render sun shadows to depthmap
            glBindFramebuffer(GL_FRAMEBUFFER, this->depthmap_fbo_id);

            glViewport(0, 0, this->shadow_width, this->shadow_height);
            glClear(GL_DEPTH_BUFFER_BIT);

            this->depth_program->bind();

            this->depth_program->set_mat4("sun.projection", sun_projection);
            this->depth_program->set_mat4("sun.view", sun_view);

            for (auto &object : this->objects)
            {
                mat4 object_model = GLM_MAT4_IDENTITY_INIT;
                object->calc_model(object_model);

                this->depth_program->set_mat4("object.model", object_model);

                object->mesh->draw();
            }

            this->depth_program->unbind();

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        switch (this->render_mode)
        {
        case RENDER_MODE_FORWARD:
        {
            // draw objects to the specified framebuffer
            if (this->num_objects > 0)
            {
                glBindFramebuffer(GL_FRAMEBUFFER, fbo_id);

                glViewport(0, 0, this->render_width, this->render_height);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                glEnable(GL_CLIP_DISTANCE0);

                for (unsigned int i = 0; i < this->num_objects; i++)
                {
                    struct object *object = this->objects[i];

                    mat4 object_model = GLM_MAT4_IDENTITY_INIT;
                    object_calc_model(object, object_model);

                    if (this->sun)
                    {
                        program_bind(this->forward_sun_program);

                        program_set_mat4(this->forward_sun_program, "camera.projection", camera_projection);
                        program_set_mat4(this->forward_sun_program, "camera.view", camera_view);
                        program_set_vec3(this->forward_sun_program, "camera.position", camera->position);

                        program_set_mat4(this->forward_sun_program, "object.model", object_model);

                        program_set_vec4(this->forward_sun_program, "clipping_plane", clipping_plane);

                        program_set_vec3(this->forward_sun_program, "material.color", object->material->color);
                        program_set_float(this->forward_sun_program, "material.shininess", object->material->shininess);
                        program_set_float(this->forward_sun_program, "material.glow", object->material->glow);

                        program_set_vec3(this->forward_sun_program, "sun.direction", this->sun->direction);
                        program_set_vec3(this->forward_sun_program, "sun.ambient_color", this->sun->ambient_color);
                        program_set_vec3(this->forward_sun_program, "sun.diffuse_color", this->sun->diffuse_color);
                        program_set_vec3(this->forward_sun_program, "sun.specular_color", this->sun->specular_color);
                        program_set_mat4(this->forward_sun_program, "sun.projection", sun_projection);
                        program_set_mat4(this->forward_sun_program, "sun.view", sun_view);

                        glActiveTexture(GL_TEXTURE0);
                        glBindTexture(GL_TEXTURE_2D, object->material->diffuse_map ? object->material->diffuse_map->texture_id : 0);
                        glActiveTexture(GL_TEXTURE1);
                        glBindTexture(GL_TEXTURE_2D, object->material->specular_map ? object->material->specular_map->texture_id : 0);
                        glActiveTexture(GL_TEXTURE2);
                        glBindTexture(GL_TEXTURE_2D, object->material->normal_map ? object->material->normal_map->texture_id : 0);
                        glActiveTexture(GL_TEXTURE3);
                        glBindTexture(GL_TEXTURE_2D, object->material->emission_map ? object->material->emission_map->texture_id : 0);
                        glActiveTexture(GL_TEXTURE4);
                        glBindTexture(GL_TEXTURE_2D, this->depthmap_texture_id);

                        mesh_draw(object->mesh);

                        program_unbind();
                    }

                    glEnable(GL_BLEND);
                    glBlendFunc(GL_ONE, GL_ONE);
                    glDepthMask(GL_FALSE);
                    glDepthFunc(GL_EQUAL);

                    if (this->num_directional_lights > 0)
                    {
                        program_bind(this->forward_directional_program);

                        program_set_mat4(this->forward_directional_program, "camera.projection", camera_projection);
                        program_set_mat4(this->forward_directional_program, "camera.view", camera_view);
                        program_set_vec3(this->forward_directional_program, "camera.position", camera->position);

                        program_set_mat4(this->forward_directional_program, "object.model", object_model);

                        program_set_vec4(this->forward_directional_program, "clipping_plane", clipping_plane);

                        program_set_vec3(this->forward_directional_program, "material.color", object->material->color);
                        program_set_float(this->forward_directional_program, "material.shininess", object->material->shininess);
                        program_set_float(this->forward_directional_program, "material.glow", object->material->glow);

                        glActiveTexture(GL_TEXTURE0);
                        glBindTexture(GL_TEXTURE_2D, object->material->diffuse_map ? object->material->diffuse_map->texture_id : 0);
                        glActiveTexture(GL_TEXTURE1);
                        glBindTexture(GL_TEXTURE_2D, object->material->specular_map ? object->material->specular_map->texture_id : 0);
                        glActiveTexture(GL_TEXTURE2);
                        glBindTexture(GL_TEXTURE_2D, object->material->normal_map ? object->material->normal_map->texture_id : 0);
                        glActiveTexture(GL_TEXTURE3);
                        glBindTexture(GL_TEXTURE_2D, object->material->emission_map ? object->material->emission_map->texture_id : 0);

                        for (unsigned int j = 0; j < this->num_directional_lights; j++)
                        {
                            struct directional_light *directional_light = this->directional_lights[j];

                            program_set_vec3(this->forward_directional_program, "directional_light.direction", directional_light->direction);
                            program_set_vec3(this->forward_directional_program, "directional_light.ambient", directional_light->ambient_color);
                            program_set_vec3(this->forward_directional_program, "directional_light.diffuse_color", directional_light->diffuse_color);
                            program_set_vec3(this->forward_directional_program, "directional_light.specular_color", directional_light->specular_color);

                            mesh_draw(object->mesh);
                        }

                        program_unbind();
                    }

                    if (this->num_point_lights > 0)
                    {
                        program_bind(this->forward_point_program);

                        program_set_mat4(this->forward_point_program, "camera.projection", camera_projection);
                        program_set_mat4(this->forward_point_program, "camera.view", camera_view);
                        program_set_vec3(this->forward_point_program, "camera.position", camera->position);

                        program_set_mat4(this->forward_point_program, "object.model", object_model);

                        program_set_vec4(this->forward_point_program, "clipping_plane", clipping_plane);

                        program_set_vec3(this->forward_point_program, "material.color", object->material->color);
                        program_set_float(this->forward_point_program, "material.shininess", object->material->shininess);
                        program_set_float(this->forward_point_program, "material.glow", object->material->glow);

                        glActiveTexture(GL_TEXTURE0);
                        glBindTexture(GL_TEXTURE_2D, object->material->diffuse_map ? object->material->diffuse_map->texture_id : 0);
                        glActiveTexture(GL_TEXTURE1);
                        glBindTexture(GL_TEXTURE_2D, object->material->specular_map ? object->material->specular_map->texture_id : 0);
                        glActiveTexture(GL_TEXTURE2);
                        glBindTexture(GL_TEXTURE_2D, object->material->normal_map ? object->material->normal_map->texture_id : 0);
                        glActiveTexture(GL_TEXTURE3);
                        glBindTexture(GL_TEXTURE_2D, object->material->emission_map ? object->material->emission_map->texture_id : 0);

                        for (unsigned int j = 0; j < this->num_point_lights; j++)
                        {
                            struct point_light *point_light = this->point_lights[j];

                            program_set_vec3(this->forward_point_program, "point_light.position", point_light->position);
                            program_set_vec3(this->forward_point_program, "point_light.ambient_color", point_light->ambient_color);
                            program_set_vec3(this->forward_point_program, "point_light.diffuse_color", point_light->diffuse_color);
                            program_set_vec3(this->forward_point_program, "point_light.specular_color", point_light->specular_color);
                            program_set_vec3(this->forward_point_program, "point_light.attenuation", point_light->attenuation);

                            mesh_draw(object->mesh);
                        }

                        program_unbind();
                    }

                    if (this->num_spot_lights > 0)
                    {
                        program_bind(this->forward_spot_program);

                        program_set_mat4(this->forward_spot_program, "camera.projection", camera_projection);
                        program_set_mat4(this->forward_spot_program, "camera.view", camera_view);
                        program_set_vec3(this->forward_spot_program, "camera.position", camera->position);

                        program_set_mat4(this->forward_spot_program, "object.model", object_model);

                        program_set_vec4(this->forward_spot_program, "clipping_plane", clipping_plane);

                        program_set_vec3(this->forward_spot_program, "material.color", object->material->color);
                        program_set_float(this->forward_spot_program, "material.shininess", object->material->shininess);
                        program_set_float(this->forward_spot_program, "material.glow", object->material->glow);

                        glActiveTexture(GL_TEXTURE0);
                        glBindTexture(GL_TEXTURE_2D, object->material->diffuse_map ? object->material->diffuse_map->texture_id : 0);
                        glActiveTexture(GL_TEXTURE1);
                        glBindTexture(GL_TEXTURE_2D, object->material->specular_map ? object->material->specular_map->texture_id : 0);
                        glActiveTexture(GL_TEXTURE2);
                        glBindTexture(GL_TEXTURE_2D, object->material->normal_map ? object->material->normal_map->texture_id : 0);
                        glActiveTexture(GL_TEXTURE3);
                        glBindTexture(GL_TEXTURE_2D, object->material->emission_map ? object->material->emission_map->texture_id : 0);

                        for (unsigned int j = 0; j < this->num_spot_lights; j++)
                        {
                            struct spot_light *spot_light = this->spot_lights[j];

                            program_set_vec3(this->forward_spot_program, "spot_light.position", spot_light->position);
                            program_set_vec3(this->forward_spot_program, "spot_light.direction", spot_light->direction);
                            program_set_vec3(this->forward_spot_program, "spot_light.ambient_color", spot_light->ambient_color);
                            program_set_vec3(this->forward_spot_program, "spot_light.diffuse_color", spot_light->diffuse_color);
                            program_set_vec3(this->forward_spot_program, "spot_light.specular_color", spot_light->specular_color);
                            program_set_vec3(this->forward_spot_program, "spot_light.attenuation", spot_light->attenuation);
                            program_set_float(this->forward_spot_program, "spot_light.inner_cutoff", spot_light->inner_cutoff);
                            program_set_float(this->forward_spot_program, "spot_light.outer_cutoff", spot_light->outer_cutoff);

                            mesh_draw(object->mesh);
                        }

                        program_unbind();
                    }

                    if (this->skybox)
                    {
                        program_bind(this->forward_reflection_program);

                        program_set_mat4(this->forward_reflection_program, "camera.projection", camera_projection);
                        program_set_mat4(this->forward_reflection_program, "camera.view", camera_view);
                        program_set_vec3(this->forward_reflection_program, "camera.position", camera->position);

                        program_set_mat4(this->forward_reflection_program, "object.model", object_model);

                        program_set_vec4(this->forward_reflection_program, "clipping_plane", clipping_plane);

                        program_set_float(this->forward_reflection_program, "material.reflectivity", 0.0f);

                        // TEMPORARY: use specular texture for reflectivity
                        glActiveTexture(GL_TEXTURE0);
                        glBindTexture(GL_TEXTURE_2D, object->material->specular_map ? object->material->specular_map->texture_id : 0);
                        glActiveTexture(GL_TEXTURE1);
                        glBindTexture(GL_TEXTURE_CUBE_MAP, this->skybox->texture_id);

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
            if (this->num_objects > 0)
            {
                glBindFramebuffer(GL_FRAMEBUFFER, this->geometry_fbo_id);

                glViewport(0, 0, (GLsizei)(this->render_width * this->render_scale), (GLsizei)(this->render_height * this->render_scale));
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                glEnable(GL_CLIP_DISTANCE0);

                program_bind(this->geometry_program);

                program_set_mat4(this->geometry_program, "camera.projection", camera_projection);
                program_set_mat4(this->geometry_program, "camera.view", camera_view);
                program_set_vec3(this->geometry_program, "camera.position", camera->position);

                for (unsigned int i = 0; i < this->num_objects; i++)
                {
                    struct object *object = this->objects[i];

                    mat4 object_model = GLM_MAT4_IDENTITY_INIT;
                    object_calc_model(object, object_model);

                    program_set_mat4(this->geometry_program, "object.model", object_model);

                    program_set_vec4(this->geometry_program, "clipping_plane", clipping_plane);

                    program_set_vec3(this->geometry_program, "material.color", object->material->color);
                    program_set_float(this->geometry_program, "material.shininess", object->material->shininess);
                    program_set_float(this->geometry_program, "material.glow", object->material->glow);

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

                glViewport(0, 0, this->render_width, this->render_height);
                glClear(GL_COLOR_BUFFER_BIT);
                glDisable(GL_DEPTH_TEST);

                if (this->sun)
                {
                    program_bind(this->deferred_sun_program);

                    program_set_vec3(this->deferred_sun_program, "camera.position", camera->position);

                    program_set_vec3(this->deferred_sun_program, "sun.direction", this->sun->direction);
                    program_set_vec3(this->deferred_sun_program, "sun.ambient_color", this->sun->ambient_color);
                    program_set_vec3(this->deferred_sun_program, "sun.diffuse_color", this->sun->diffuse_color);
                    program_set_vec3(this->deferred_sun_program, "sun.specular_color", this->sun->specular_color);
                    program_set_mat4(this->deferred_sun_program, "sun.projection", sun_projection);
                    program_set_mat4(this->deferred_sun_program, "sun.view", sun_view);

                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, this->geometry_position_texture_id);
                    glActiveTexture(GL_TEXTURE1);
                    glBindTexture(GL_TEXTURE_2D, this->geometry_normal_texture_id);
                    glActiveTexture(GL_TEXTURE2);
                    glBindTexture(GL_TEXTURE_2D, this->geometry_albedo_specular_texture_id);
                    glActiveTexture(GL_TEXTURE3);
                    glBindTexture(GL_TEXTURE_2D, this->depthmap_texture_id);

                    glBindVertexArray(this->screen_vao_id);
                    glDrawArrays(GL_TRIANGLES, 0, this->num_screen_vertices);
                    glBindVertexArray(0);

                    program_unbind();
                }

                glEnable(GL_BLEND);
                glBlendFunc(GL_ONE, GL_ONE);
                glDepthMask(GL_FALSE);
                glDepthFunc(GL_EQUAL);

                if (this->num_directional_lights > 0)
                {
                    program_bind(this->deferred_directional_program);

                    program_set_vec3(this->deferred_directional_program, "camera.position", camera->position);

                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, this->geometry_position_texture_id);
                    glActiveTexture(GL_TEXTURE1);
                    glBindTexture(GL_TEXTURE_2D, this->geometry_normal_texture_id);
                    glActiveTexture(GL_TEXTURE2);
                    glBindTexture(GL_TEXTURE_2D, this->geometry_albedo_specular_texture_id);

                    for (unsigned int i = 0; i < this->num_directional_lights; i++)
                    {
                        struct directional_light *directional_light = this->directional_lights[i];

                        program_set_vec3(this->deferred_directional_program, "directional_light.direction", directional_light->direction);
                        program_set_vec3(this->deferred_directional_program, "directional_light.ambient_color", directional_light->ambient_color);
                        program_set_vec3(this->deferred_directional_program, "directional_light.diffuse_color", directional_light->diffuse_color);
                        program_set_vec3(this->deferred_directional_program, "directional_light.specular_color", directional_light->specular_color);

                        glBindVertexArray(this->screen_vao_id);
                        glDrawArrays(GL_TRIANGLES, 0, this->num_screen_vertices);
                        glBindVertexArray(0);
                    }

                    program_unbind();
                }

                if (this->num_point_lights > 0)
                {
                    program_bind(this->deferred_point_program);

                    program_set_vec3(this->deferred_point_program, "camera.position", camera->position);

                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, this->geometry_position_texture_id);
                    glActiveTexture(GL_TEXTURE1);
                    glBindTexture(GL_TEXTURE_2D, this->geometry_normal_texture_id);
                    glActiveTexture(GL_TEXTURE2);
                    glBindTexture(GL_TEXTURE_2D, this->geometry_albedo_specular_texture_id);

                    for (unsigned int i = 0; i < this->num_point_lights; i++)
                    {
                        struct point_light *point_light = this->point_lights[i];

                        program_set_vec3(this->deferred_point_program, "point_light.position", point_light->position);
                        program_set_vec3(this->deferred_point_program, "point_light.ambient_color", point_light->ambient_color);
                        program_set_vec3(this->deferred_point_program, "point_light.diffuse_color", point_light->diffuse_color);
                        program_set_vec3(this->deferred_point_program, "point_light.specular_color", point_light->specular_color);
                        program_set_vec3(this->deferred_point_program, "point_light.attenuation", point_light->attenuation);

                        glBindVertexArray(this->screen_vao_id);
                        glDrawArrays(GL_TRIANGLES, 0, this->num_screen_vertices);
                        glBindVertexArray(0);
                    }

                    program_unbind();
                }

                if (this->num_spot_lights > 0)
                {
                    program_bind(this->deferred_spot_program);

                    program_set_vec3(this->deferred_spot_program, "camera.position", camera->position);

                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, this->geometry_position_texture_id);
                    glActiveTexture(GL_TEXTURE1);
                    glBindTexture(GL_TEXTURE_2D, this->geometry_normal_texture_id);
                    glActiveTexture(GL_TEXTURE2);
                    glBindTexture(GL_TEXTURE_2D, this->geometry_albedo_specular_texture_id);

                    for (unsigned int i = 0; i < this->num_spot_lights; i++)
                    {
                        struct spot_light *spot_light = this->spot_lights[i];

                        program_set_vec3(this->deferred_spot_program, "spot_light.position", spot_light->position);
                        program_set_vec3(this->deferred_spot_program, "spot_light.direction", spot_light->direction);
                        program_set_vec3(this->deferred_spot_program, "spot_light.ambient_color", spot_light->ambient_color);
                        program_set_vec3(this->deferred_spot_program, "spot_light.diffuse_color", spot_light->diffuse_color);
                        program_set_vec3(this->deferred_spot_program, "spot_light.specular_color", spot_light->specular_color);
                        program_set_vec3(this->deferred_spot_program, "spot_light.attenuation", spot_light->attenuation);
                        program_set_float(this->deferred_spot_program, "spot_light.inner_cutoff", spot_light->inner_cutoff);
                        program_set_float(this->deferred_spot_program, "spot_light.outer_cutoff", spot_light->outer_cutoff);

                        glBindVertexArray(this->screen_vao_id);
                        glDrawArrays(GL_TRIANGLES, 0, this->num_screen_vertices);
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
                glBindFramebuffer(GL_READ_FRAMEBUFFER, this->geometry_fbo_id);
                glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo_id);
                glBlitFramebuffer(0, 0, this->render_width, this->render_height, 0, 0, this->render_width, this->render_height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
            }
        }
        break;
        }

        // draw other things that need to be drawn after the deferred rendering pass
        glBindFramebuffer(GL_FRAMEBUFFER, fbo_id);

        glViewport(0, 0, this->render_width, this->render_height);

        // render skybox
        if (this->skybox)
        {
            glDepthFunc(GL_LEQUAL);

            mat4 camera_view_no_translate;
            glm_mat4_copy(camera_view, camera_view_no_translate);
            // glm_rotate(camera_view_no_translate, glm_rad(elapsed_time) * 0.001, GLM_YUP);
            camera_view_no_translate[3][0] = 0.0f;
            camera_view_no_translate[3][1] = 0.0f;
            camera_view_no_translate[3][2] = 0.0f;

            program_bind(this->skybox_program);

            program_set_mat4(this->skybox_program, "camera.projection", camera_projection);
            program_set_mat4(this->skybox_program, "camera.view", camera_view_no_translate);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, this->skybox->texture_id);

            glBindVertexArray(this->skybox_vao_id);
            glDrawArrays(GL_TRIANGLES, 0, this->num_skybox_vertices);
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
        if (this->sun)
        {
            // calculate sun projection matrix
            glm_ortho(-10.0f, 10.0f, -10.0f, 10.0f, -10.0f, 10.0f, sun_projection);

            // calculate sun "position"
            vec3 sun_position;
            glm_vec_sub(GLM_VEC3_ZERO, this->sun->direction, sun_position);

            // calculate sun view matrix
            glm_lookat(sun_position, GLM_VEC3_ZERO, GLM_YUP, sun_view);
        }

        // send camera transformations to water program
        program_bind(this->water_program);

        program_set_mat4(this->water_program, "camera.projection", camera_projection);
        program_set_mat4(this->water_program, "camera.view", camera_view);
        program_set_vec3(this->water_program, "camera.position", camera->position);

        if (this->sun)
        {
            program_set_vec3(this->water_program, "sun.direction", this->sun->direction);
            program_set_vec3(this->water_program, "sun.ambient_color", this->sun->ambient_color);
            program_set_vec3(this->water_program, "sun.diffuse_color", this->sun->diffuse_color);
            program_set_vec3(this->water_program, "sun.specular_color", this->sun->specular_color);
            program_set_mat4(this->water_program, "sun.projection", sun_projection);
            program_set_mat4(this->water_program, "sun.view", sun_view);
        }

        program_set_unsigned_int(this->water_program, "elapsed_time", elapsed_time);

        program_unbind();

        for (unsigned int i = 0; i < this->num_waters; i++)
        {
            struct water *water = this->waters[i];

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

                render_scene(this->water_reflection_fbo_id, camera, aspect, elapsed_time, reflection_clipping_plane);

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

            render_scene(this->water_refraction_fbo_id, camera, aspect, elapsed_time, refraction_clipping_plane);

            glBindFramebuffer(GL_FRAMEBUFFER, fbo_id);

            program_bind(this->water_program);

            mat4 water_model = GLM_MAT4_IDENTITY_INIT;
            glm_translate(water_model, water->position);
            vec3 water_scale;
            water_scale[0] = water->scale[0];
            water_scale[1] = 1.0f;
            water_scale[2] = water->scale[1];
            glm_scale(water_model, water_scale);

            program_set_mat4(this->water_program, "water.model", water_model);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, reflect ? this->water_reflection_color_texture_id : 0);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, this->water_refraction_color_texture_id);
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, this->water_dudv_texture ? this->water_dudv_texture->texture_id : 0);
            glActiveTexture(GL_TEXTURE3);
            glBindTexture(GL_TEXTURE_2D, this->water_normal_texture ? this->water_normal_texture->texture_id : 0);
            glActiveTexture(GL_TEXTURE4);
            glBindTexture(GL_TEXTURE_2D, this->depthmap_texture_id);

            glBindVertexArray(this->water_vao_id);
            glDrawArrays(GL_TRIANGLES, 0, this->num_water_vertices);
            glBindVertexArray(0);

            program_unbind();

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }
    }

    void render_sprites(GLuint fbo_id, float aspect)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, fbo_id);

        program_bind(this->sprite_program);

        // calculate ortho projection
        mat4 camera_projection_ortho;
        glm_ortho_default(aspect, camera_projection_ortho);

        program_set_mat4(this->sprite_program, "camera.projection", camera_projection_ortho);

        for (unsigned int i = 0; i < this->num_sprites; i++)
        {
            struct sprite *sprite = this->sprites[i];

            mat4 sprite_model = GLM_MAT4_IDENTITY_INIT;
            sprite_calc_model(sprite, sprite_model);

            program_set_mat4(this->sprite_program, "sprite.model", sprite_model);
            program_set_vec3(this->sprite_program, "sprite.color", sprite->color);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, sprite->color_map->texture_id);

            glBindVertexArray(this->sprite_vao_id);
            glDrawArrays(GL_TRIANGLES, 0, this->num_sprite_vertices);
            glBindVertexArray(0);
        }

        program_unbind();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void DEBUG_render_point_lights(GLuint fbo_id, struct camera *camera, float aspect)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, fbo_id);

        // draw point lights as a cube
        if (this->num_point_lights > 0)
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

            program_bind(this->forward_color_program);

            program_set_mat4(this->forward_color_program, "camera.projection", camera_projection);
            program_set_mat4(this->forward_color_program, "camera.view", camera_view);

            for (unsigned int i = 0; i < this->num_point_lights; i++)
            {
                struct point_light *point_light = this->point_lights[i];

                mat4 object_model = GLM_MAT4_IDENTITY_INIT;
                glm_translate(object_model, point_light->position);
                vec3 scale = { 0.125f, 0.125f, 0.125f };
                glm_scale(object_model, scale);

                program_set_mat4(this->forward_color_program, "object.model", object_model);

                program_set_vec3(this->forward_color_program, "material.color", point_light->specular_color);

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
        program_bind(this->sprite_program);

        // calculate ortho projection
        mat4 camera_projection_ortho;
        glm_ortho_default(aspect, camera_projection_ortho);

        program_set_mat4(this->sprite_program, "camera.projection", camera_projection_ortho);

        vec3 position = GLM_VEC3_ZERO_INIT;
        vec3 color = GLM_VEC3_ONE_INIT;

        position[0] = -1.75f;
        position[1] = 0.0f;

        {
            mat4 sprite_model = GLM_MAT4_IDENTITY_INIT;
            glm_translate(sprite_model, position);

            program_set_mat4(this->sprite_program, "sprite.model", sprite_model);
            program_set_vec3(this->sprite_program, "sprite.color", color);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, this->geometry_position_texture_id);

            glBindVertexArray(this->sprite_vao_id);
            glDrawArrays(GL_TRIANGLES, 0, this->num_sprite_vertices);
            glBindVertexArray(0);
        }

        position[0] += 1.0f;

        {
            mat4 sprite_model = GLM_MAT4_IDENTITY_INIT;
            glm_translate(sprite_model, position);

            program_set_mat4(this->sprite_program, "sprite.model", sprite_model);
            program_set_vec3(this->sprite_program, "sprite.color", color);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, this->geometry_normal_texture_id);

            glBindVertexArray(this->sprite_vao_id);
            glDrawArrays(GL_TRIANGLES, 0, this->num_sprite_vertices);
            glBindVertexArray(0);
        }

        position[0] += 1.0f;

        {
            mat4 sprite_model = GLM_MAT4_IDENTITY_INIT;
            glm_translate(sprite_model, position);

            program_set_mat4(this->sprite_program, "sprite.model", sprite_model);
            program_set_vec3(this->sprite_program, "sprite.color", color);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, this->geometry_albedo_specular_texture_id);

            glBindVertexArray(this->sprite_vao_id);
            glDrawArrays(GL_TRIANGLES, 0, this->num_sprite_vertices);
            glBindVertexArray(0);
        }

        program_unbind();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void DEBUG_render_depthmap_fbo(GLuint fbo_id, struct camera *camera, float aspect)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, fbo_id);

        // draw depthmap
        program_bind(this->sprite_program);

        // calculate ortho projection
        mat4 camera_projection_ortho;
        glm_ortho_default(aspect, camera_projection_ortho);

        program_set_mat4(this->sprite_program, "camera.projection", camera_projection_ortho);

        vec3 position = GLM_VEC3_ZERO_INIT;
        vec3 color = GLM_VEC3_ONE_INIT;

        position[0] = -1.75f;
        position[1] = -1.0f;

        {
            mat4 sprite_model = GLM_MAT4_IDENTITY_INIT;
            glm_translate(sprite_model, position);

            program_set_mat4(this->sprite_program, "sprite.model", sprite_model);
            program_set_vec3(this->sprite_program, "sprite.color", color);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, this->depthmap_texture_id);

            glBindVertexArray(this->sprite_vao_id);
            glDrawArrays(GL_TRIANGLES, 0, this->num_sprite_vertices);
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
        program_bind(this->sprite_program);

        // calculate ortho projection
        mat4 camera_projection_ortho;
        glm_ortho_default(aspect, camera_projection_ortho);

        program_set_mat4(this->sprite_program, "camera.projection", camera_projection_ortho);

        vec3 position = GLM_VEC3_ZERO_INIT;
        vec3 color = GLM_VEC3_ONE_INIT;

        position[0] = -0.75f;
        position[1] = -1.0f;

        {
            mat4 sprite_model = GLM_MAT4_IDENTITY_INIT;
            glm_translate(sprite_model, position);

            program_set_mat4(this->sprite_program, "sprite.model", sprite_model);
            program_set_vec3(this->sprite_program, "sprite.color", color);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, this->water_reflection_color_texture_id);

            glBindVertexArray(this->sprite_vao_id);
            glDrawArrays(GL_TRIANGLES, 0, this->num_sprite_vertices);
            glBindVertexArray(0);
        }

        position[0] += 1.0f;

        {
            mat4 sprite_model = GLM_MAT4_IDENTITY_INIT;
            glm_translate(sprite_model, position);

            program_set_mat4(this->sprite_program, "sprite.model", sprite_model);
            program_set_vec3(this->sprite_program, "sprite.color", color);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, this->water_refraction_color_texture_id);

            glBindVertexArray(this->sprite_vao_id);
            glDrawArrays(GL_TRIANGLES, 0, this->num_sprite_vertices);
            glBindVertexArray(0);
        }

        program_unbind();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void render_screen(GLuint fbo_id)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, fbo_id);

        glViewport(0, 0, this->render_width, this->render_height);
        glClear(GL_COLOR_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST);

        program_bind(this->screen_program);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, this->screen_texture_id);

        glBindVertexArray(this->screen_vao_id);
        glDrawArrays(GL_TRIANGLES, 0, this->num_screen_vertices);
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

        render_scene(this->screen_fbo_id, camera, aspect, elapsed_time, clipping_plane);

        // DEBUG_render_point_lights(this->screen_fbo_id, camera, aspect);
        if (this->render_mode == RENDER_MODE_DEFERRED)
        {
            // DEBUG_render_geometry_fbos(this->screen_fbo_id, camera, aspect);
        }
        // DEBUG_render_depthmap_fbo(this->screen_fbo_id, camera, aspect);

        // render water
        if (this->num_waters > 0)
        {
            render_waters(this->screen_fbo_id, camera, aspect, elapsed_time);
        }

        // DEBUG_render_water_fbos(this->screen_fbo_id, camera, aspect);

        // render sprites
        if (this->num_sprites > 0)
        {
            render_sprites(this->screen_fbo_id, aspect);
        }

        // render the screen framebuffer to the default framebuffer and apply post-processing
        render_screen(0);

        // clear renderables
        this->num_objects = 0;
        this->num_directional_lights = 0;
        this->num_point_lights = 0;
        this->num_spot_lights = 0;
        this->num_waters = 0;
        this->num_sprites = 0;
    }
}
