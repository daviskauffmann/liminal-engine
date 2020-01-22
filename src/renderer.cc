#include "renderer.hh"

#include <iostream>

#include <glm/matrix.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <SDL2/SDL_image.h>

#include "stb_image.h"

#define LIGHTING_COLOR 1
#define LIGHTING_TEXTURE 2
#define LIGHTING_FORWARD 3
#define LIGHTING_DEFERRED 4
#define LIGHTING_MODE LIGHTING_FORWARD

namespace pk
{
renderer::renderer(
    int display_width, int display_height, float render_scale,
    int reflection_width, int reflection_height,
    int refraction_width, int refraction_height)
{
    IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);
    const SDL_version *version = IMG_Linked_Version();
    std::cout << "SDL_image " << std::to_string(version->major) << "." << std::to_string(version->minor) << "." << std::to_string(version->patch) << std::endl;

    GLenum error = glewInit();
    if (error != GLEW_OK)
    {
        std::cout << "Error: Couldn't initialize GLEW\n"
                  << glewGetErrorString(error) << std::endl;
    }
    std::cout << "GLEW " << glewGetString(GLEW_VERSION) << std::endl;
    std::cout << "OpenGL " << glGetString(GL_VERSION) << std::endl;
    std::cout << "Vendor " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "Renderer " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "GLSL " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

    glClearColor(1.0f, 0.0f, 1.0f, 0.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    // glEnable(GL_FRAMEBUFFER_SRGB);

    std::vector<float> water_vertices =
        {-1.0f, -1.0f,
         -1.0f, +1.0f,
         +1.0f, -1.0f,

         +1.0f, -1.0f,
         -1.0f, +1.0f,
         +1.0f, +1.0f};
    water_vertices_size = (GLsizei)(water_vertices.size() * sizeof(float));
    glGenVertexArrays(1, &water_vao_id);
    glBindVertexArray(water_vao_id);
    glGenBuffers(1, &water_vbo_id);
    glBindBuffer(GL_ARRAY_BUFFER, water_vbo_id);
    glBufferData(GL_ARRAY_BUFFER, water_vertices_size, water_vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid *)(0 * sizeof(GLfloat)));
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    std::vector<float> skybox_vertices =
        {-1.0f, +1.0f, -1.0f,
         -1.0f, -1.0f, -1.0f,
         +1.0f, -1.0f, -1.0f,
         +1.0f, -1.0f, -1.0f,
         +1.0f, +1.0f, -1.0f,
         -1.0f, +1.0f, -1.0f,

         -1.0f, -1.0f, +1.0f,
         -1.0f, -1.0f, -1.0f,
         -1.0f, +1.0f, -1.0f,
         -1.0f, +1.0f, -1.0f,
         -1.0f, +1.0f, +1.0f,
         -1.0f, -1.0f, +1.0f,

         +1.0f, -1.0f, -1.0f,
         +1.0f, -1.0f, +1.0f,
         +1.0f, +1.0f, +1.0f,
         +1.0f, +1.0f, +1.0f,
         +1.0f, +1.0f, -1.0f,
         +1.0f, -1.0f, -1.0f,

         -1.0f, -1.0f, +1.0f,
         -1.0f, +1.0f, +1.0f,
         +1.0f, +1.0f, +1.0f,
         +1.0f, +1.0f, +1.0f,
         +1.0f, -1.0f, +1.0f,
         -1.0f, -1.0f, +1.0f,

         -1.0f, +1.0f, -1.0f,
         +1.0f, +1.0f, -1.0f,
         +1.0f, +1.0f, +1.0f,
         +1.0f, +1.0f, +1.0f,
         -1.0f, +1.0f, +1.0f,
         -1.0f, +1.0f, -1.0f,

         -1.0f, -1.0f, -1.0f,
         -1.0f, -1.0f, +1.0f,
         +1.0f, -1.0f, -1.0f,
         +1.0f, -1.0f, -1.0f,
         -1.0f, -1.0f, +1.0f,
         +1.0f, -1.0f, +1.0f};
    skybox_vertices_size = (GLsizei)(skybox_vertices.size() * sizeof(float));
    glGenVertexArrays(1, &skybox_vao_id);
    glBindVertexArray(skybox_vao_id);
    glGenBuffers(1, &skybox_vbo_id);
    glBindBuffer(GL_ARRAY_BUFFER, skybox_vbo_id);
    glBufferData(GL_ARRAY_BUFFER, skybox_vertices_size, skybox_vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid *)(0 * sizeof(GLfloat)));
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    std::vector<float> sprite_vertices =
        {+0.0f, +1.0f, +0.0f, +1.0f,
         +1.0f, +0.0f, +1.0f, +0.0f,
         +0.0f, +0.0f, +0.0f, +0.0f,

         +0.0f, +1.0f, +0.0f, +1.0f,
         +1.0f, +1.0f, +1.0f, +1.0f,
         +1.0f, +0.0f, +1.0f, +0.0f};
    sprite_vertices_size = (GLsizei)(sprite_vertices.size() * sizeof(float));
    glGenVertexArrays(1, &sprite_vao_id);
    glBindVertexArray(sprite_vao_id);
    glGenBuffers(1, &sprite_vbo_id);
    glBindBuffer(GL_ARRAY_BUFFER, sprite_vbo_id);
    glBufferData(GL_ARRAY_BUFFER, sprite_vertices_size, sprite_vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid *)(0 * sizeof(GLfloat)));
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid *)(2 * sizeof(GLfloat)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    std::vector<float> screen_vertices =
        {-1.0f, -1.0f, +0.0f, +0.0f,
         -1.0f, +1.0f, +0.0f, +1.0f,
         +1.0f, -1.0f, +1.0f, +0.0f,
         +1.0f, -1.0f, +1.0f, +0.0f,
         -1.0f, +1.0f, +0.0f, +1.0f,
         +1.0f, +1.0f, +1.0f, +1.0f};
    screen_vertices_size = (GLsizei)(screen_vertices.size() * sizeof(float));
    glGenVertexArrays(1, &screen_vao_id);
    glBindVertexArray(screen_vao_id);
    glGenBuffers(1, &screen_vbo_id);
    glBindBuffer(GL_ARRAY_BUFFER, screen_vbo_id);
    glBufferData(GL_ARRAY_BUFFER, screen_vertices_size, screen_vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid *)(0 * sizeof(GLfloat)));
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid *)(2 * sizeof(GLfloat)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    water_dudv_texture = new pk::texture("assets/images/water_dudv.png");
    water_normal_texture = new pk::texture("assets/images/water_normal.png");

    depth_program = new pk::program(
        "assets/shaders/depth.vs",
        "assets/shaders/depth.fs");
    depth_cube_program = new pk::program(
        "assets/shaders/depth_cube.vs",
        "assets/shaders/depth_cube.gs",
        "assets/shaders/depth_cube.fs");
    color_program = new pk::program(
        "assets/shaders/color.vs",
        "assets/shaders/color.fs");
    texture_program = new pk::program(
        "assets/shaders/texture.vs",
        "assets/shaders/texture.fs");
    forward_ambient_program = new pk::program(
        "assets/shaders/forward.vs",
        "assets/shaders/forward_ambient.fs");
    forward_directional_program = new pk::program(
        "assets/shaders/forward.vs",
        "assets/shaders/forward_directional.fs");
    forward_point_program = new pk::program(
        "assets/shaders/forward.vs",
        "assets/shaders/forward_point.fs");
    forward_spot_program = new pk::program(
        "assets/shaders/forward.vs",
        "assets/shaders/forward_spot.fs");
    geometry_program = new pk::program(
        "assets/shaders/geometry.vs",
        "assets/shaders/geometry.fs");
    background_program = new pk::program(
        "assets/shaders/background.vs",
        "assets/shaders/background.fs");
    water_program = new pk::program(
        "assets/shaders/water.vs",
        "assets/shaders/water.fs");
    terrain_program = new pk::program(
        "assets/shaders/terrain.vs",
        "assets/shaders/terrain.fs");
    sprite_program = new pk::program(
        "assets/shaders/sprite.vs",
        "assets/shaders/sprite.fs");
    screen_program = new pk::program(
        "assets/shaders/screen.vs",
        "assets/shaders/screen.fs");

    texture_program->bind();
    texture_program->set_int("color_map", 0);
    texture_program->unbind();

    forward_ambient_program->bind();
    forward_ambient_program->set_int("material.albedo_map", 0);
    forward_ambient_program->set_int("material.normal_map", 1);
    forward_ambient_program->set_int("material.metallic_map", 2);
    forward_ambient_program->set_int("material.roughness_map", 3);
    forward_ambient_program->set_int("material.occlusion_map", 4);
    forward_ambient_program->set_int("material.height_map", 5);
    forward_ambient_program->set_int("irradiance_cubemap", 6);
    forward_ambient_program->set_int("prefilter_cubemap", 7);
    forward_ambient_program->set_int("brdf_map", 8);
    forward_ambient_program->unbind();

    forward_directional_program->bind();
    forward_directional_program->set_int("material.albedo_map", 0);
    forward_directional_program->set_int("material.normal_map", 1);
    forward_directional_program->set_int("material.metallic_map", 2);
    forward_directional_program->set_int("material.roughness_map", 3);
    forward_directional_program->set_int("material.occlusion_map", 4);
    forward_directional_program->set_int("material.height_map", 5);
    forward_directional_program->set_int("light.depth_map", 6);
    forward_directional_program->unbind();

    forward_point_program->bind();
    forward_point_program->set_int("material.albedo_map", 0);
    forward_point_program->set_int("material.normal_map", 1);
    forward_point_program->set_int("material.metallic_map", 2);
    forward_point_program->set_int("material.roughness_map", 3);
    forward_point_program->set_int("material.occlusion_map", 4);
    forward_point_program->set_int("material.height_map", 5);
    forward_point_program->set_int("light.depth_cubemap", 6);
    forward_point_program->unbind();

    forward_spot_program->bind();
    forward_spot_program->set_int("material.albedo_map", 0);
    forward_spot_program->set_int("material.normal_map", 1);
    forward_spot_program->set_int("material.metallic_map", 2);
    forward_spot_program->set_int("material.roughness_map", 3);
    forward_spot_program->set_int("material.occlusion_map", 4);
    forward_spot_program->set_int("material.height_map", 5);
    forward_spot_program->set_int("light.depth_map", 6);
    forward_spot_program->unbind();

    geometry_program->bind();
    geometry_program->set_int("material.albedo_map", 0);
    geometry_program->set_int("material.normal_map", 1);
    geometry_program->set_int("material.metallic_map", 2);
    geometry_program->set_int("material.roughness_map", 3);
    geometry_program->set_int("material.occlusion_map", 4);
    geometry_program->set_int("material.height_map", 5);
    geometry_program->unbind();

    background_program->bind();
    background_program->set_int("environment_cubemap", 0);
    background_program->unbind();

    water_program->bind();
    water_program->set_int("water.reflection_map", 0);
    water_program->set_int("water.refraction_map", 1);
    water_program->set_int("water.depth_map", 2);
    water_program->set_int("water.dudv_map", 3);
    water_program->set_int("water.normal_map", 4);
    water_program->unbind();

    terrain_program->bind();
    terrain_program->set_int("material.albedo_map", 0);
    terrain_program->unbind();

    sprite_program->bind();
    sprite_program->set_int("sprite.texture", 0);
    sprite_program->unbind();

    screen_program->bind();
    screen_program->set_int("screen.texture", 0);
    screen_program->unbind();

    set_screen_size(display_width, display_height, render_scale);
    set_reflection_size(reflection_width, reflection_height);
    set_refraction_size(refraction_width, refraction_height);

    // TODO: move below function so this can be recalculated during runtime
    pk::program *equirectangular_to_cubemap_program = new pk::program(
        "assets/shaders/cubemap.vs",
        "assets/shaders/equirectangular_to_cubemap.fs");
    pk::program *irradiance_convolution_program = new pk::program(
        "assets/shaders/cubemap.vs",
        "assets/shaders/irradiance_convolution.fs");
    pk::program *prefilter_convolution_program = new pk::program(
        "assets/shaders/cubemap.vs",
        "assets/shaders/prefilter_convolution.fs");
    pk::program *brdf_program = new pk::program(
        "assets/shaders/brdf.vs",
        "assets/shaders/brdf.fs");

    equirectangular_to_cubemap_program->bind();
    equirectangular_to_cubemap_program->set_int("equirectangular_map", 0);
    equirectangular_to_cubemap_program->unbind();

    irradiance_convolution_program->bind();
    irradiance_convolution_program->set_int("environment_cubemap", 0);
    irradiance_convolution_program->unbind();

    prefilter_convolution_program->bind();
    prefilter_convolution_program->set_int("environment_cubemap", 0);
    prefilter_convolution_program->unbind();

    glm::mat4 capture_projection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    glm::mat4 capture_views[] =
        {glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
         glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
         glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
         glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
         glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
         glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f))};

    GLuint equirectangular_texture_id;
    glGenTextures(1, &equirectangular_texture_id);
    glBindTexture(GL_TEXTURE_2D, equirectangular_texture_id);
    stbi_set_flip_vertically_on_load(true);
    int width, height, num_components;
    float *image = stbi_loadf("assets/images/GCanyon_C_YumaPoint_8k.jpg", &width, &height, &num_components, 0);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGB16F,
        width,
        height,
        0,
        GL_RGB,
        GL_FLOAT,
        image);
    stbi_image_free(image);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenTextures(1, &environment_cubemap_id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, environment_cubemap_id);
    for (unsigned int i = 0; i < 6; i++)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 4096, 4096, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    GLuint capture_rbo_id;
    glGenRenderbuffers(1, &capture_rbo_id);
    glBindRenderbuffer(GL_RENDERBUFFER, capture_rbo_id);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 4096, 4096);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    GLuint capture_fbo_id;
    glGenFramebuffers(1, &capture_fbo_id);
    glBindFramebuffer(GL_FRAMEBUFFER, capture_fbo_id);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, capture_rbo_id);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    equirectangular_to_cubemap_program->bind();
    equirectangular_to_cubemap_program->set_mat4("capture.projection", capture_projection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, equirectangular_texture_id);
    glViewport(0, 0, 4096, 4096);
    glBindFramebuffer(GL_FRAMEBUFFER, capture_fbo_id);
    for (unsigned int i = 0; i < 6; i++)
    {
        equirectangular_to_cubemap_program->set_mat4("capture.view", capture_views[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, environment_cubemap_id, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glBindVertexArray(skybox_vao_id);
        glDrawArrays(GL_TRIANGLES, 0, skybox_vertices_size);
        glBindVertexArray(0);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    equirectangular_to_cubemap_program->unbind();

    glBindTexture(GL_TEXTURE_CUBE_MAP, environment_cubemap_id);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    glDeleteTextures(1, &equirectangular_texture_id);

    glGenTextures(1, &irradiance_cubemap_id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, irradiance_cubemap_id);
    for (unsigned int i = 0; i < 6; i++)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 32, 32, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, capture_fbo_id);
    glBindRenderbuffer(GL_RENDERBUFFER, capture_rbo_id);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    irradiance_convolution_program->bind();
    irradiance_convolution_program->set_mat4("capture.projection", capture_projection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, environment_cubemap_id);
    glViewport(0, 0, 32, 32);
    glBindFramebuffer(GL_FRAMEBUFFER, capture_fbo_id);
    for (unsigned int i = 0; i < 6; i++)
    {
        irradiance_convolution_program->set_mat4("capture.view", capture_views[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradiance_cubemap_id, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glBindVertexArray(skybox_vao_id);
        glDrawArrays(GL_TRIANGLES, 0, skybox_vertices_size);
        glBindVertexArray(0);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    irradiance_convolution_program->unbind();

    glGenTextures(1, &prefilter_cubemap_id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, prefilter_cubemap_id);
    for (unsigned int i = 0; i < 6; i++)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 128, 128, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    prefilter_convolution_program->bind();
    prefilter_convolution_program->set_mat4("capture.projection", capture_projection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, environment_cubemap_id);
    glBindFramebuffer(GL_FRAMEBUFFER, capture_fbo_id);
    const unsigned int max_mip_levels = 5;
    for (unsigned int mip_level = 0; mip_level < max_mip_levels; mip_level++)
    {
        unsigned int mip_width = (unsigned int)(128 * std::pow(0.5f, (float)mip_level));
        unsigned int mip_height = (unsigned int)(128 * std::pow(0.5f, (float)mip_level));
        glBindRenderbuffer(GL_RENDERBUFFER, capture_rbo_id);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mip_width, mip_height);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        glViewport(0, 0, mip_width, mip_height);
        float roughness = (float)mip_level / (float)(max_mip_levels - 1);
        prefilter_convolution_program->set_float("roughness", roughness);
        for (unsigned int i = 0; i < 6; ++i)
        {
            prefilter_convolution_program->set_mat4("capture.view", capture_views[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilter_cubemap_id, mip_level);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glBindVertexArray(skybox_vao_id);
            glDrawArrays(GL_TRIANGLES, 0, skybox_vertices_size);
            glBindVertexArray(0);
        }
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    prefilter_convolution_program->unbind();

    glGenTextures(1, &brdf_map_id);
    glBindTexture(GL_TEXTURE_2D, brdf_map_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindFramebuffer(GL_FRAMEBUFFER, capture_fbo_id);
    glBindRenderbuffer(GL_RENDERBUFFER, capture_rbo_id);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdf_map_id, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, capture_fbo_id);
    glViewport(0, 0, 512, 512);
    brdf_program->bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBindVertexArray(screen_vao_id);
    glDrawArrays(GL_TRIANGLES, 0, screen_vertices_size);
    glBindVertexArray(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glDeleteRenderbuffers(1, &capture_rbo_id);
    glDeleteFramebuffers(1, &capture_fbo_id);

    delete equirectangular_to_cubemap_program;
    delete irradiance_convolution_program;
    delete prefilter_convolution_program;
    delete brdf_program;
}

renderer::~renderer()
{
    delete water_dudv_texture;
    delete water_normal_texture;

    delete depth_program;
    delete depth_cube_program;
    delete color_program;
    delete texture_program;
    delete forward_ambient_program;
    delete forward_directional_program;
    delete forward_point_program;
    delete forward_spot_program;
    delete geometry_program;
    delete background_program;
    delete water_program;
    delete terrain_program;
    delete sprite_program;
    delete screen_program;

    glDeleteRenderbuffers(1, &screen_rbo_id);
    glDeleteTextures(1, &screen_texture_id);
    glDeleteFramebuffers(1, &screen_fbo_id);

    glDeleteRenderbuffers(1, &geometry_rbo_id);
    glDeleteTextures(1, &geometry_normal_texture_id);
    glDeleteTextures(1, &geometry_position_texture_id);
    glDeleteFramebuffers(1, &geometry_fbo_id);

    glDeleteRenderbuffers(1, &water_reflection_rbo_id);
    glDeleteTextures(1, &water_reflection_color_texture_id);
    glDeleteFramebuffers(1, &water_reflection_fbo_id);

    glDeleteRenderbuffers(1, &water_refraction_rbo_id);
    glDeleteTextures(1, &water_refraction_depth_texture_id);
    glDeleteTextures(1, &water_refraction_color_texture_id);
    glDeleteFramebuffers(1, &water_refraction_fbo_id);

    glDeleteBuffers(1, &water_vbo_id);
    glDeleteVertexArrays(1, &water_vao_id);

    glDeleteBuffers(1, &skybox_vbo_id);
    glDeleteVertexArrays(1, &skybox_vao_id);

    glDeleteBuffers(1, &sprite_vbo_id);
    glDeleteVertexArrays(1, &sprite_vao_id);

    glDeleteBuffers(1, &screen_vbo_id);
    glDeleteVertexArrays(1, &screen_vao_id);

    IMG_Quit();
}

void renderer::set_screen_size(int display_width, int display_height, float render_scale)
{
    this->display_width = display_width;
    this->display_height = display_height;
    this->render_width = (int)(display_width * render_scale);
    this->render_height = (int)(display_height * render_scale);

    glDeleteRenderbuffers(1, &screen_rbo_id);
    glDeleteTextures(1, &screen_texture_id);
    glDeleteFramebuffers(1, &screen_fbo_id);

    glDeleteRenderbuffers(1, &geometry_rbo_id);
    glDeleteTextures(1, &geometry_normal_texture_id);
    glDeleteTextures(1, &geometry_position_texture_id);
    glDeleteFramebuffers(1, &geometry_fbo_id);

    // setup screen fbo
    glGenTextures(1, &screen_texture_id);
    glBindTexture(GL_TEXTURE_2D, screen_texture_id);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA16F,
        render_width,
        render_height,
        0,
        GL_RGBA,
        GL_FLOAT,
        nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenRenderbuffers(1, &screen_rbo_id);
    glBindRenderbuffer(GL_RENDERBUFFER, screen_rbo_id);
    glRenderbufferStorage(
        GL_RENDERBUFFER,
        GL_DEPTH_STENCIL,
        display_width,
        display_height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glGenFramebuffers(1, &screen_fbo_id);
    glBindFramebuffer(GL_FRAMEBUFFER, screen_fbo_id);
    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D,
        screen_texture_id,
        0);
    glFramebufferRenderbuffer(
        GL_FRAMEBUFFER,
        GL_DEPTH_ATTACHMENT,
        GL_RENDERBUFFER,
        screen_rbo_id);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "Error: Couldn't complete screen framebuffer" << std::endl;
    }

    // setup geometry fbo
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glGenTextures(1, &geometry_position_texture_id);
    glBindTexture(GL_TEXTURE_2D, geometry_position_texture_id);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGB16F,
        render_width,
        render_height,
        0,
        GL_RGB,
        GL_FLOAT,
        nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenTextures(1, &geometry_normal_texture_id);
    glBindTexture(GL_TEXTURE_2D, geometry_normal_texture_id);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGB16F,
        render_width,
        render_height,
        0,
        GL_RGB,
        GL_FLOAT,
        nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenTextures(1, &geometry_albedo_texture_id);
    glBindTexture(GL_TEXTURE_2D, geometry_albedo_texture_id);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGB16F,
        render_width,
        render_height,
        0,
        GL_RGB,
        GL_FLOAT,
        nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenTextures(1, &geometry_material_texture_id);
    glBindTexture(GL_TEXTURE_2D, geometry_material_texture_id);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA16F,
        render_width,
        render_height,
        0,
        GL_RGBA,
        GL_FLOAT,
        nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenRenderbuffers(1, &geometry_rbo_id);
    glBindRenderbuffer(GL_RENDERBUFFER, geometry_rbo_id);
    glRenderbufferStorage(
        GL_RENDERBUFFER,
        GL_DEPTH_COMPONENT,
        render_width,
        render_height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glGenFramebuffers(1, &geometry_fbo_id);
    glBindFramebuffer(GL_FRAMEBUFFER, geometry_fbo_id);
    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D,
        geometry_position_texture_id,
        0);
    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT1,
        GL_TEXTURE_2D,
        geometry_normal_texture_id,
        0);
    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT2,
        GL_TEXTURE_2D,
        geometry_albedo_texture_id,
        0);
    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT3,
        GL_TEXTURE_2D,
        geometry_material_texture_id,
        0);
    glFramebufferRenderbuffer(
        GL_FRAMEBUFFER,
        GL_DEPTH_ATTACHMENT,
        GL_RENDERBUFFER,
        geometry_rbo_id);
    GLenum geometry_color_attachments[] = {
        GL_COLOR_ATTACHMENT0,
        GL_COLOR_ATTACHMENT1,
        GL_COLOR_ATTACHMENT2,
        GL_COLOR_ATTACHMENT3};
    glDrawBuffers(sizeof(geometry_color_attachments) / sizeof(GLenum), geometry_color_attachments);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "Error: Couldn't complete geometry framebuffer" << std::endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void renderer::set_reflection_size(int reflection_width, int reflection_height)
{
    this->reflection_width = reflection_width;
    this->reflection_height = reflection_height;

    glDeleteRenderbuffers(1, &water_reflection_rbo_id);
    glDeleteTextures(1, &water_reflection_color_texture_id);
    glDeleteFramebuffers(1, &water_reflection_fbo_id);

    // setup water reflection fbo
    glGenTextures(1, &water_reflection_color_texture_id);
    glBindTexture(GL_TEXTURE_2D, water_reflection_color_texture_id);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA,
        reflection_width,
        reflection_height,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenRenderbuffers(1, &water_reflection_rbo_id);
    glBindRenderbuffer(GL_RENDERBUFFER, water_reflection_rbo_id);
    glRenderbufferStorage(
        GL_RENDERBUFFER,
        GL_DEPTH_COMPONENT,
        reflection_width,
        reflection_height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glGenFramebuffers(1, &water_reflection_fbo_id);
    glBindFramebuffer(GL_FRAMEBUFFER, water_reflection_fbo_id);
    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D,
        water_reflection_color_texture_id,
        0);
    glFramebufferRenderbuffer(
        GL_FRAMEBUFFER,
        GL_DEPTH_ATTACHMENT,
        GL_RENDERBUFFER,
        water_reflection_rbo_id);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "Error: Couldn't complete water reflection framebuffer" << std::endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void renderer::set_refraction_size(int refraction_width, int refraction_height)
{
    this->refraction_width = refraction_width;
    this->refraction_height = refraction_height;

    glDeleteRenderbuffers(1, &water_refraction_rbo_id);
    glDeleteTextures(1, &water_refraction_depth_texture_id);
    glDeleteTextures(1, &water_refraction_color_texture_id);
    glDeleteFramebuffers(1, &water_refraction_fbo_id);

    // setup water refraction fbo
    glGenTextures(1, &water_refraction_color_texture_id);
    glBindTexture(GL_TEXTURE_2D, water_refraction_color_texture_id);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA,
        refraction_width,
        refraction_height,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenTextures(1, &water_refraction_depth_texture_id);
    glBindTexture(GL_TEXTURE_2D, water_refraction_depth_texture_id);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_DEPTH_COMPONENT,
        refraction_width,
        refraction_height,
        0,
        GL_DEPTH_COMPONENT,
        GL_FLOAT,
        nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenRenderbuffers(1, &water_refraction_rbo_id);
    glBindRenderbuffer(GL_RENDERBUFFER, water_refraction_rbo_id);
    glRenderbufferStorage(
        GL_RENDERBUFFER,
        GL_DEPTH_COMPONENT,
        refraction_width,
        refraction_height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glGenFramebuffers(1, &water_refraction_fbo_id);
    glBindFramebuffer(GL_FRAMEBUFFER, water_refraction_fbo_id);
    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D,
        water_refraction_color_texture_id,
        0);
    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_DEPTH_ATTACHMENT,
        GL_TEXTURE_2D,
        water_refraction_depth_texture_id,
        0);
    glFramebufferRenderbuffer(
        GL_FRAMEBUFFER,
        GL_DEPTH_ATTACHMENT,
        GL_RENDERBUFFER,
        water_refraction_rbo_id);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "Error: Couldn't complete water refraction framebuffer" << std::endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void renderer::add_object(pk::object *object)
{
    objects.push_back(object);
}

void renderer::add_directional_light(pk::directional_light *directional_light)
{
    directional_lights.push_back(directional_light);
}

void renderer::add_point_light(pk::point_light *point_light)
{
    point_lights.push_back(point_light);
}

void renderer::add_spot_light(pk::spot_light *spot_light)
{
    spot_lights.push_back(spot_light);
}

void renderer::add_water(pk::water *water)
{
    waters.push_back(water);
}

void renderer::add_terrain(pk::terrain *terrain)
{
    terrains.push_back(terrain);
}

void renderer::add_sprite(pk::sprite *sprite)
{
    sprites.push_back(sprite);
}

void renderer::flush(pk::camera *camera, unsigned int elapsed_time, float delta_time)
{
    if (!camera)
    {
        std::cout << "Error: Camera cannot be null" << std::endl;
        return;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, screen_fbo_id);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    render_scene(screen_fbo_id, render_width, render_height, camera, elapsed_time);
    if (waters.size() > 0)
    {
        render_waters(screen_fbo_id, camera, elapsed_time);
    }
    if (sprites.size() > 0)
    {
        render_sprites(screen_fbo_id);
    }
    render_screen(0);

    objects.clear();
    directional_lights.clear();
    point_lights.clear();
    spot_lights.clear();
    waters.clear();
    terrains.clear();
    sprites.clear();
}

void renderer::render_scene(GLuint fbo_id, int width, int height, pk::camera *camera, unsigned int elapsed_time, glm::vec4 clipping_plane)
{
#if LIGHTING_MODE == LIGHTING_FORWARD || LIGHTING_MODE == LIGHTING_DEFERRED
    for (auto &directional_light : directional_lights)
    {
        directional_light->projection = directional_light->calc_projection();
        directional_light->view = directional_light->calc_view(camera->position);

        glBindFramebuffer(GL_FRAMEBUFFER, directional_light->depth_map_fbo_id);
        glViewport(0, 0, directional_light->depth_map_width, directional_light->depth_map_height);
        glClear(GL_DEPTH_BUFFER_BIT);
        depth_program->bind();
        depth_program->set_mat4("projection", directional_light->projection);
        depth_program->set_mat4("view", directional_light->view);
        for (auto &object : objects)
        {
            glm::mat4 object_model = object->calc_model();
            depth_program->set_mat4("object.model", object_model);
            object->mesh->draw();
        }
        depth_program->unbind();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    for (auto &point_light : point_lights)
    {
        float far_plane = 25.0f;

        glm::mat4 projection = glm::perspective(glm::radians(90.0f), (float)point_light->depth_cube_width / (float)point_light->depth_cube_height, 1.0f, far_plane);
        std::vector<glm::mat4> shadow_matrices;
        shadow_matrices.push_back(projection * glm::lookAt(point_light->position, point_light->position + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
        shadow_matrices.push_back(projection * glm::lookAt(point_light->position, point_light->position + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
        shadow_matrices.push_back(projection * glm::lookAt(point_light->position, point_light->position + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
        shadow_matrices.push_back(projection * glm::lookAt(point_light->position, point_light->position + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
        shadow_matrices.push_back(projection * glm::lookAt(point_light->position, point_light->position + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
        shadow_matrices.push_back(projection * glm::lookAt(point_light->position, point_light->position + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));

        glBindFramebuffer(GL_FRAMEBUFFER, point_light->depth_cubemap_fbo_id);
        glViewport(0, 0, point_light->depth_cube_width, point_light->depth_cube_height);
        glClear(GL_DEPTH_BUFFER_BIT);
        depth_cube_program->bind();
        for (unsigned int i = 0; i < 6; i++)
        {
            depth_cube_program->set_mat4("shadow_matrices[" + std::to_string(i) + "]", shadow_matrices[i]);
        }
        depth_cube_program->set_vec3("light.position", point_light->position);
        depth_cube_program->set_float("far_plane", far_plane);
        for (auto &object : objects)
        {
            glm::mat4 object_model = object->calc_model();
            depth_cube_program->set_mat4("object.model", object_model);
            object->mesh->draw();
        }
        depth_cube_program->unbind();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    for (auto &spot_light : spot_lights)
    {
        spot_light->projection = spot_light->calc_projection();
        spot_light->view = spot_light->calc_view();

        glBindFramebuffer(GL_FRAMEBUFFER, spot_light->depth_map_fbo_id);
        glViewport(0, 0, spot_light->depth_map_width, spot_light->depth_map_height);
        glClear(GL_DEPTH_BUFFER_BIT);
        depth_program->bind();
        depth_program->set_mat4("projection", spot_light->projection);
        depth_program->set_mat4("view", spot_light->view);
        for (auto &object : objects)
        {
            glm::mat4 object_model = object->calc_model();
            depth_program->set_mat4("object.model", object_model);
            object->mesh->draw();
        }
        depth_program->unbind();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
#endif

    glm::mat4 camera_projection = camera->calc_projection((float)width / (float)height);
    glm::mat4 camera_view = camera->calc_view();

    glViewport(0, 0, width, height);
    if (objects.size() > 0)
    {
#if LIGHTING_MODE == LIGHTING_COLOR
        glBindFramebuffer(GL_FRAMEBUFFER, fbo_id);
        glEnable(GL_CLIP_DISTANCE0);
        for (auto &object : objects)
        {
            glm::mat4 object_model = object->calc_model();
            color_program->bind();
            color_program->set_mat4("camera.projection", camera_projection);
            color_program->set_mat4("camera.view", camera_view);
            color_program->set_mat4("object.model", object_model);
            color_program->set_vec3("color", object->material->albedo_color);
            color_program->set_vec4("clipping_plane", clipping_plane);
            object->mesh->draw();
            color_program->unbind();
        }
        glDisable(GL_CLIP_DISTANCE0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
#elif LIGHTING_MODE == LIGHTING_TEXTURE
        glBindFramebuffer(GL_FRAMEBUFFER, fbo_id);
        glEnable(GL_CLIP_DISTANCE0);
        for (auto &object : objects)
        {
            glm::mat4 object_model = object->calc_model();
            texture_program->bind();
            texture_program->set_mat4("camera.projection", camera_projection);
            texture_program->set_mat4("camera.view", camera_view);
            texture_program->set_mat4("object.model", object_model);
            texture_program->set_vec3("color", object->material->albedo_color);
            texture_program->set_vec4("clipping_plane", clipping_plane);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, object->material->albedo_map ? object->material->albedo_map->texture_id : 0);
            object->mesh->draw();
            texture_program->unbind();
        }
        glDisable(GL_CLIP_DISTANCE0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
#elif LIGHTING_MODE == LIGHTING_FORWARD
        glBindFramebuffer(GL_FRAMEBUFFER, fbo_id);
        glEnable(GL_CLIP_DISTANCE0);
        for (auto &object : objects)
        {
            glm::mat4 object_model = object->calc_model();
            forward_ambient_program->bind();
            forward_ambient_program->set_mat4("camera.projection", camera_projection);
            forward_ambient_program->set_mat4("camera.view", camera_view);
            forward_ambient_program->set_vec3("camera.position", camera->position);
            forward_ambient_program->set_mat4("object.model", object_model);
            forward_ambient_program->set_vec3("material.albedo_color", object->material->albedo_color);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, object->material->albedo_map ? object->material->albedo_map->texture_id : 0);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, object->material->normal_map ? object->material->normal_map->texture_id : 0);
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, object->material->metallic_map ? object->material->metallic_map->texture_id : 0);
            glActiveTexture(GL_TEXTURE3);
            glBindTexture(GL_TEXTURE_2D, object->material->roughness_map ? object->material->roughness_map->texture_id : 0);
            glActiveTexture(GL_TEXTURE4);
            glBindTexture(GL_TEXTURE_2D, object->material->occlusion_map ? object->material->occlusion_map->texture_id : 0);
            glActiveTexture(GL_TEXTURE5);
            glBindTexture(GL_TEXTURE_2D, object->material->height_map ? object->material->height_map->texture_id : 0);
            glActiveTexture(GL_TEXTURE6);
            glBindTexture(GL_TEXTURE_CUBE_MAP, irradiance_cubemap_id);
            glActiveTexture(GL_TEXTURE7);
            glBindTexture(GL_TEXTURE_CUBE_MAP, prefilter_cubemap_id);
            glActiveTexture(GL_TEXTURE8);
            glBindTexture(GL_TEXTURE_2D, brdf_map_id);
            forward_ambient_program->set_vec4("clipping_plane", clipping_plane);
            object->mesh->draw();
            forward_ambient_program->unbind();

            glEnable(GL_BLEND);
            glBlendFunc(GL_ONE, GL_ONE);
            glDepthMask(GL_FALSE);
            glDepthFunc(GL_EQUAL);
            if (directional_lights.size() > 0)
            {
                forward_directional_program->bind();
                forward_directional_program->set_mat4("camera.projection", camera_projection);
                forward_directional_program->set_mat4("camera.view", camera_view);
                forward_directional_program->set_vec3("camera.position", camera->position);
                forward_directional_program->set_mat4("object.model", object_model);
                forward_directional_program->set_vec3("material.albedo_color", object->material->albedo_color);
                forward_directional_program->set_vec4("clipping_plane", clipping_plane);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, object->material->albedo_map ? object->material->albedo_map->texture_id : 0);
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, object->material->normal_map ? object->material->normal_map->texture_id : 0);
                glActiveTexture(GL_TEXTURE2);
                glBindTexture(GL_TEXTURE_2D, object->material->metallic_map ? object->material->metallic_map->texture_id : 0);
                glActiveTexture(GL_TEXTURE3);
                glBindTexture(GL_TEXTURE_2D, object->material->roughness_map ? object->material->roughness_map->texture_id : 0);
                glActiveTexture(GL_TEXTURE4);
                glBindTexture(GL_TEXTURE_2D, object->material->occlusion_map ? object->material->occlusion_map->texture_id : 0);
                glActiveTexture(GL_TEXTURE5);
                glBindTexture(GL_TEXTURE_2D, object->material->height_map ? object->material->height_map->texture_id : 0);
                for (auto &directional_light : directional_lights)
                {
                    forward_directional_program->set_vec3("light.direction", directional_light->direction);
                    forward_directional_program->set_vec3("light.color", directional_light->color);
                    forward_directional_program->set_mat4("light.projection", directional_light->projection);
                    forward_directional_program->set_mat4("light.view", directional_light->view);
                    glActiveTexture(GL_TEXTURE6);
                    glBindTexture(GL_TEXTURE_2D, directional_light->depth_map_texture_id);
                    object->mesh->draw();
                }
                forward_directional_program->unbind();
            }
            if (point_lights.size() > 0)
            {
                forward_point_program->bind();
                forward_point_program->set_mat4("camera.projection", camera_projection);
                forward_point_program->set_mat4("camera.view", camera_view);
                forward_point_program->set_vec3("camera.position", camera->position);
                forward_point_program->set_mat4("object.model", object_model);
                forward_point_program->set_vec3("material.albedo_color", object->material->albedo_color);
                forward_point_program->set_vec4("clipping_plane", clipping_plane);
                forward_point_program->set_float("far_plane", 25.0f);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, object->material->albedo_map ? object->material->albedo_map->texture_id : 0);
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, object->material->normal_map ? object->material->normal_map->texture_id : 0);
                glActiveTexture(GL_TEXTURE2);
                glBindTexture(GL_TEXTURE_2D, object->material->metallic_map ? object->material->metallic_map->texture_id : 0);
                glActiveTexture(GL_TEXTURE3);
                glBindTexture(GL_TEXTURE_2D, object->material->roughness_map ? object->material->roughness_map->texture_id : 0);
                glActiveTexture(GL_TEXTURE4);
                glBindTexture(GL_TEXTURE_2D, object->material->occlusion_map ? object->material->occlusion_map->texture_id : 0);
                glActiveTexture(GL_TEXTURE5);
                glBindTexture(GL_TEXTURE_2D, object->material->height_map ? object->material->height_map->texture_id : 0);
                for (auto &point_light : point_lights)
                {
                    forward_point_program->set_vec3("light.position", point_light->position);
                    forward_point_program->set_vec3("light.color", point_light->color);
                    glActiveTexture(GL_TEXTURE6);
                    glBindTexture(GL_TEXTURE_CUBE_MAP, point_light->depth_cubemap_texture_id);
                    object->mesh->draw();
                }
                forward_point_program->unbind();
            }
            if (spot_lights.size() > 0)
            {
                forward_spot_program->bind();
                forward_spot_program->set_mat4("camera.projection", camera_projection);
                forward_spot_program->set_mat4("camera.view", camera_view);
                forward_spot_program->set_vec3("camera.position", camera->position);
                forward_spot_program->set_mat4("object.model", object_model);
                forward_spot_program->set_vec3("material.albedo_color", object->material->albedo_color);
                forward_spot_program->set_vec4("clipping_plane", clipping_plane);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, object->material->albedo_map ? object->material->albedo_map->texture_id : 0);
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, object->material->normal_map ? object->material->normal_map->texture_id : 0);
                glActiveTexture(GL_TEXTURE2);
                glBindTexture(GL_TEXTURE_2D, object->material->metallic_map ? object->material->metallic_map->texture_id : 0);
                glActiveTexture(GL_TEXTURE3);
                glBindTexture(GL_TEXTURE_2D, object->material->roughness_map ? object->material->roughness_map->texture_id : 0);
                glActiveTexture(GL_TEXTURE4);
                glBindTexture(GL_TEXTURE_2D, object->material->occlusion_map ? object->material->occlusion_map->texture_id : 0);
                glActiveTexture(GL_TEXTURE5);
                glBindTexture(GL_TEXTURE_2D, object->material->height_map ? object->material->height_map->texture_id : 0);
                for (auto &spot_light : spot_lights)
                {
                    forward_spot_program->set_vec3("light.position", spot_light->position);
                    forward_spot_program->set_vec3("light.direction", spot_light->direction);
                    forward_spot_program->set_vec3("light.color", spot_light->color);
                    forward_spot_program->set_float("light.inner_cutoff", spot_light->inner_cutoff);
                    forward_spot_program->set_float("light.outer_cutoff", spot_light->outer_cutoff);
                    forward_spot_program->set_mat4("light.projection", spot_light->projection);
                    forward_spot_program->set_mat4("light.view", spot_light->view);
                    glActiveTexture(GL_TEXTURE6);
                    glBindTexture(GL_TEXTURE_2D, spot_light->depth_map_texture_id);
                    object->mesh->draw();
                }
                forward_spot_program->unbind();
            }
            glDepthFunc(GL_LESS);
            glDepthMask(GL_TRUE);
            glDisable(GL_BLEND);
        }
        glDisable(GL_CLIP_DISTANCE0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
#elif LIGHTING_MODE == LIGHTING_DEFERRED
        glBindFramebuffer(GL_FRAMEBUFFER, geometry_fbo_id);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_CLIP_DISTANCE0);
        geometry_program->bind();
        geometry_program->set_mat4("camera.projection", camera_projection);
        geometry_program->set_mat4("camera.view", camera_view);
        geometry_program->set_vec3("camera.position", camera->position);
        for (auto &object : objects)
        {
            glm::mat4 object_model = object->calc_model();
            geometry_program->set_mat4("object.model", object_model);
            geometry_program->set_vec3("material.albedo_color", object->material->albedo_color);
            geometry_program->set_vec4("clipping_plane", clipping_plane);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, object->material->albedo_map ? object->material->albedo_map->texture_id : 0);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, object->material->normal_map ? object->material->normal_map->texture_id : 0);
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, object->material->metallic_map ? object->material->metallic_map->texture_id : 0);
            glActiveTexture(GL_TEXTURE3);
            glBindTexture(GL_TEXTURE_2D, object->material->roughness_map ? object->material->roughness_map->texture_id : 0);
            glActiveTexture(GL_TEXTURE4);
            glBindTexture(GL_TEXTURE_2D, object->material->occlusion_map ? object->material->occlusion_map->texture_id : 0);
            glActiveTexture(GL_TEXTURE5);
            glBindTexture(GL_TEXTURE_2D, object->material->height_map ? object->material->height_map->texture_id : 0);
            object->mesh->draw();
        }
        geometry_program->unbind();
        glDisable(GL_CLIP_DISTANCE0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glBindFramebuffer(GL_FRAMEBUFFER, fbo_id);
        glDisable(GL_DEPTH_TEST);
        for (auto &object : objects)
        {
            glm::mat4 object_model = object->calc_model();
            color_program->bind();
            color_program->set_mat4("camera.projection", camera_projection);
            color_program->set_mat4("camera.view", camera_view);
            color_program->set_mat4("object.model", object_model);
            color_program->set_vec4("clipping_plane", clipping_plane);
            color_program->set_vec3("color", glm::vec3(0.0f));
            object->mesh->draw();
        }
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE);
        glDepthMask(GL_FALSE);
        glDepthFunc(GL_EQUAL);
        // TODO: deferred pbr lighting
        glDepthFunc(GL_LESS);
        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glBindFramebuffer(GL_READ_FRAMEBUFFER, geometry_fbo_id);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo_id);
        glBlitFramebuffer(0, 0, render_width, render_height, 0, 0, render_width, render_height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
#endif
    }

    glBindFramebuffer(GL_FRAMEBUFFER, fbo_id);

    if (terrains.size() > 0)
    {
        glEnable(GL_CLIP_DISTANCE0);
        terrain_program->bind();
        terrain_program->set_mat4("camera.projection", camera_projection);
        terrain_program->set_mat4("camera.view", camera_view);
        terrain_program->set_vec3("camera.position", camera->position);
        terrain_program->set_vec4("clipping_plane", clipping_plane);
        for (auto &terrain : terrains)
        {
            glm::mat4 terrain_model = terrain->calc_model();
            terrain_program->set_mat4("terrain.model", terrain_model);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, terrain->material->albedo_map ? terrain->material->albedo_map->texture_id : 0);
            terrain->mesh->draw();
        }
        terrain_program->unbind();
        glDisable(GL_CLIP_DISTANCE0);
    }

    glDepthFunc(GL_LEQUAL);
    glm::mat4 camera_view_no_translate = camera_view;
    // camera_view_no_translate = glm::rotate(camera_view_no_translate, glm::radians((float)elapsed_time) * 0.001f, glm::vec3(0.0f, 1.0f, 0.0f));
    camera_view_no_translate[3][0] = 0.0f;
    camera_view_no_translate[3][1] = 0.0f;
    camera_view_no_translate[3][2] = 0.0f;
    background_program->bind();
    background_program->set_mat4("camera.projection", camera_projection);
    background_program->set_mat4("camera.view", camera_view_no_translate);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, environment_cubemap_id);
    glBindVertexArray(skybox_vao_id);
    glDrawArrays(GL_TRIANGLES, 0, skybox_vertices_size);
    glBindVertexArray(0);
    background_program->unbind();
    glDepthFunc(GL_LESS);

    if (point_lights.size() > 0)
    {
        glEnable(GL_CLIP_DISTANCE0);
        for (auto &point_light : point_lights)
        {
            glm::mat4 model(1.0f);
            model = glm::translate(model, point_light->position);
            model = glm::scale(model, {0.25f, 0.25f, 0.25f});
            color_program->bind();
            color_program->set_mat4("camera.projection", camera_projection);
            color_program->set_mat4("camera.view", camera_view);
            color_program->set_mat4("object.model", model);
            color_program->set_vec3("color", point_light->color);
            color_program->set_vec4("clipping_plane", clipping_plane);
            glBindVertexArray(skybox_vao_id);
            glDrawArrays(GL_TRIANGLES, 0, skybox_vertices_size);
            glBindVertexArray(0);
            color_program->unbind();
        }
        glDisable(GL_CLIP_DISTANCE0);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void renderer::render_waters(GLuint fbo_id, pk::camera *camera, unsigned int elapsed_time)
{
    glm::mat4 camera_projection = camera->calc_projection((float)render_width / (float)render_height);
    glm::mat4 camera_view = camera->calc_view();

    water_program->bind();
    water_program->set_mat4("camera.projection", camera_projection);
    water_program->set_mat4("camera.view", camera_view);
    water_program->set_vec3("camera.position", camera->position);
    water_program->set_unsigned_int("elapsed_time", elapsed_time);
    water_program->unbind();
    for (auto &water : waters)
    {
        glm::mat4 water_model(1.0f);
        water_model = glm::translate(water_model, water->position);
        water_model = glm::scale(water_model, {water->scale.x, 1.0f, water->scale.y});

        bool reflect = false;
        if (camera->position.y > water->position.y) // don't draw reflections if under the water
        {
            reflect = true;
            float old_camera_y = camera->position.y;
            float old_camera_pitch = camera->pitch;
            camera->position.y -= 2 * (camera->position.y - water->position.y);
            camera->pitch = -camera->pitch;
            glm::vec4 reflection_clipping_plane = {0.0f, 1.0f, 0.0f, -water->position.y};
            glBindFramebuffer(GL_FRAMEBUFFER, water_reflection_fbo_id);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            render_scene(water_reflection_fbo_id, reflection_width, reflection_height, camera, elapsed_time, reflection_clipping_plane);
            camera->position.y = old_camera_y;
            camera->pitch = old_camera_pitch;
        }

        glm::vec4 refraction_clipping_plane = {0.0f, -1.0f, 0.0f, water->position.y};
        if (camera->position.y < water->position.y) // flip refraction clipping plane if under the water
        {
            refraction_clipping_plane.y = 1.0f;
            refraction_clipping_plane.w = -water->position.y;
        }
        glBindFramebuffer(GL_FRAMEBUFFER, water_refraction_fbo_id);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        render_scene(water_refraction_fbo_id, refraction_width, refraction_height, camera, elapsed_time, refraction_clipping_plane);

        glViewport(0, 0, render_width, render_height);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo_id);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        water_program->bind();
        water_program->set_mat4("water.model", water_model);
        water_program->set_float("near_plane", 0.1f);
        water_program->set_float("far_plane", 1000.0f);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, reflect ? water_reflection_color_texture_id : 0);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, water_refraction_color_texture_id);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, water_refraction_depth_texture_id);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, water_dudv_texture ? water_dudv_texture->texture_id : 0);
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, water_normal_texture ? water_normal_texture->texture_id : 0);
        glBindVertexArray(water_vao_id);
        glDrawArrays(GL_TRIANGLES, 0, water_vertices_size);
        glBindVertexArray(0);
        water_program->unbind();
        glDisable(GL_BLEND);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}

void renderer::render_sprites(GLuint fbo_id)
{
    glViewport(0, 0, display_width, display_height);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_id);
    sprite_program->bind();
    glm::mat4 camera_projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 100.0f);
    sprite_program->set_mat4("camera.projection", camera_projection);
    for (auto &sprite : sprites)
    {
        glm::mat4 sprite_model = sprite->calc_model();
        sprite_program->set_mat4("sprite.model", sprite_model);
        sprite_program->set_vec3("sprite.color", sprite->color);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, sprite->texture->texture_id);
        glBindVertexArray(sprite_vao_id);
        glDrawArrays(GL_TRIANGLES, 0, sprite_vertices_size);
        glBindVertexArray(0);
    }
    sprite_program->unbind();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void renderer::render_screen(GLuint fbo_id)
{
    glViewport(0, 0, display_width, display_height);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_id);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    screen_program->bind();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, screen_texture_id);
    glBindVertexArray(screen_vao_id);
    glDrawArrays(GL_TRIANGLES, 0, screen_vertices_size);
    glBindVertexArray(0);
    screen_program->unbind();
    glEnable(GL_DEPTH_TEST);
}
} // namespace pk