#include <liminal/core/renderer.hpp>

#include <entt/entt.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <imgui.h>
#include <liminal/components/mesh_renderer.hpp>
#include <liminal/components/point_light.hpp>
#include <liminal/components/spot_light.hpp>
#include <liminal/components/sprite.hpp>
#include <liminal/components/terrain.hpp>
#include <liminal/components/water.hpp>
#include <liminal/entities/entity.hpp>
#include <liminal/graphics/skybox.hpp>
#include <limits>
#include <stdexcept>

constexpr float directional_light_shadow_map_size = 10;
constexpr float directional_light_near_plane = -10;
constexpr float directional_light_far_plane = 10;

constexpr float point_light_near_plane = 1;
constexpr float point_light_far_plane = 25;

constexpr float spot_light_near_plane = 0.1f;
constexpr float spot_light_far_plane = 10;

liminal::renderer::renderer(
    const GLsizei target_width, const GLsizei target_height, const float render_scale,
    const GLsizei directional_light_depth_map_size,
    const GLsizei point_light_depth_cubemap_size,
    const GLsizei spot_light_depth_map_size,
    const GLsizei water_reflection_width, const GLsizei water_reflection_height,
    const GLsizei water_refraction_width, const GLsizei water_refraction_height)
    : target_width(target_width),
      target_height(target_height),
      render_scale(render_scale)
{
    // setup fbos
    calc_render_size();
    set_directional_light_depth_map_size(directional_light_depth_map_size);
    set_point_light_depth_cubemap_size(point_light_depth_cubemap_size);
    set_spot_light_depth_map_size(spot_light_depth_map_size);
    set_reflection_size(water_reflection_width, water_reflection_height);
    set_refraction_size(water_refraction_width, water_refraction_height);

    // init OpenGL state
    glClearColor(0, 0, 0, 0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    glCullFace(GL_BACK);
    // glEnable(GL_MULTISAMPLE);
    // glEnable(GL_FRAMEBUFFER_SRGB);

    // create water mesh
    glGenVertexArrays(1, &water_vao_id);
    glBindVertexArray(water_vao_id);
    {
        constexpr std::array<GLfloat, 12> water_vertices{
            -1, -1,
            -1, +1,
            +1, -1,

            +1, -1,
            -1, +1,
            +1, +1};
        water_vertices_size = static_cast<GLsizei>(water_vertices.size() * sizeof(GLfloat));

        glGenBuffers(1, &water_vbo_id);
        glBindBuffer(GL_ARRAY_BUFFER, water_vbo_id);
        glBufferData(GL_ARRAY_BUFFER, water_vertices_size, water_vertices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), reinterpret_cast<GLvoid *>(0 * sizeof(GLfloat)));

        glEnableVertexAttribArray(0);
    }
    glBindVertexArray(0);

    // create skybox mesh
    glGenVertexArrays(1, &skybox_vao_id);
    glBindVertexArray(skybox_vao_id);
    {
        constexpr std::array<GLfloat, 108> skybox_vertices{
            -1, +1, -1,
            -1, -1, -1,
            +1, -1, -1,
            +1, -1, -1,
            +1, +1, -1,
            -1, +1, -1,

            -1, -1, +1,
            -1, -1, -1,
            -1, +1, -1,
            -1, +1, -1,
            -1, +1, +1,
            -1, -1, +1,

            +1, -1, -1,
            +1, -1, +1,
            +1, +1, +1,
            +1, +1, +1,
            +1, +1, -1,
            +1, -1, -1,

            -1, -1, +1,
            -1, +1, +1,
            +1, +1, +1,
            +1, +1, +1,
            +1, -1, +1,
            -1, -1, +1,

            -1, +1, -1,
            +1, +1, -1,
            +1, +1, +1,
            +1, +1, +1,
            -1, +1, +1,
            -1, +1, -1,

            -1, -1, -1,
            -1, -1, +1,
            +1, -1, -1,
            +1, -1, -1,
            -1, -1, +1,
            +1, -1, +1};
        skybox_vertices_size = static_cast<GLsizei>(skybox_vertices.size() * sizeof(GLfloat));

        glGenBuffers(1, &skybox_vbo_id);
        glBindBuffer(GL_ARRAY_BUFFER, skybox_vbo_id);
        glBufferData(GL_ARRAY_BUFFER, skybox_vertices_size, skybox_vertices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), reinterpret_cast<GLvoid *>(0 * sizeof(GLfloat)));

        glEnableVertexAttribArray(0);
    }
    glBindVertexArray(0);

    // create sprite mesh
    glGenVertexArrays(1, &sprite_vao_id);
    glBindVertexArray(sprite_vao_id);
    {
        constexpr std::array<GLfloat, 24> sprite_vertices{
            +0, +1, +0, +1,
            +1, +0, +1, +0,
            +0, +0, +0, +0,

            +0, +1, +0, +1,
            +1, +1, +1, +1,
            +1, +0, +1, +0};
        sprite_vertices_size = static_cast<GLsizei>(sprite_vertices.size() * sizeof(GLfloat));

        glGenBuffers(1, &sprite_vbo_id);
        glBindBuffer(GL_ARRAY_BUFFER, sprite_vbo_id);
        glBufferData(GL_ARRAY_BUFFER, sprite_vertices_size, sprite_vertices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), reinterpret_cast<GLvoid *>(0 * sizeof(GLfloat)));
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), reinterpret_cast<GLvoid *>(2 * sizeof(GLfloat)));

        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
    }
    glBindVertexArray(0);

    // create screen mesh
    glGenVertexArrays(1, &screen_vao_id);
    glBindVertexArray(screen_vao_id);
    {
        constexpr std::array<GLfloat, 24> screen_vertices{
            -1, -1, +0, +0,
            -1, +1, +0, +1,
            +1, -1, +1, +0,

            +1, -1, +1, +0,
            -1, +1, +0, +1,
            +1, +1, +1, +1};
        screen_vertices_size = static_cast<GLsizei>(screen_vertices.size() * sizeof(GLfloat));

        glGenBuffers(1, &screen_vbo_id);
        glBindBuffer(GL_ARRAY_BUFFER, screen_vbo_id);
        glBufferData(GL_ARRAY_BUFFER, screen_vertices_size, screen_vertices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), reinterpret_cast<GLvoid *>(0 * sizeof(GLfloat)));
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), reinterpret_cast<GLvoid *>(2 * sizeof(GLfloat)));

        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
    }
    glBindVertexArray(0);

    // create brdf texture
    {
        constexpr GLsizei brdf_size = 512;

        GLuint capture_fbo_id;
        GLuint capture_rbo_id;

        glGenFramebuffers(1, &capture_fbo_id);
        glBindFramebuffer(GL_FRAMEBUFFER, capture_fbo_id);
        {
            brdf_texture = std::make_unique<liminal::texture>(
                GL_RG16F,
                brdf_size,
                brdf_size,
                GL_RG,
                GL_FLOAT,
                liminal::texture_filter::linear,
                liminal::texture_wrap::clamp_to_edge);

            glGenRenderbuffers(1, &capture_rbo_id);
            glBindRenderbuffer(GL_RENDERBUFFER, capture_rbo_id);
            {
                glRenderbufferStorage(
                    GL_RENDERBUFFER,
                    GL_DEPTH_COMPONENT24,
                    brdf_size,
                    brdf_size);
            }
            glBindRenderbuffer(GL_RENDERBUFFER, 0);

            glFramebufferTexture2D(
                GL_FRAMEBUFFER,
                GL_COLOR_ATTACHMENT0,
                GL_TEXTURE_2D,
                brdf_texture->get_texture_id(),
                0);

            glFramebufferRenderbuffer(
                GL_FRAMEBUFFER,
                GL_DEPTH_ATTACHMENT,
                GL_RENDERBUFFER,
                capture_rbo_id);

            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            {
                throw std::runtime_error("BRDF capture framebuffer is not complete");
            }
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        const auto brdf_program = std::make_unique<liminal::program>(
            "assets/shaders/brdf.vs",
            "assets/shaders/brdf.fs");

        glBindFramebuffer(GL_FRAMEBUFFER, capture_fbo_id);
        {
            glViewport(0, 0, brdf_size, brdf_size);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            brdf_program->bind();
            {
                glBindVertexArray(screen_vao_id);
                glDrawArrays(GL_TRIANGLES, 0, screen_vertices_size);
                glBindVertexArray(0);
            }
            liminal::program::unbind();
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glDeleteFramebuffers(1, &capture_fbo_id);
        glDeleteRenderbuffers(1, &capture_rbo_id);
    }

    // create shader programs and setup samplers
    depth_mesh_program = std::make_unique<liminal::program>(
        "assets/shaders/depth_mesh.vs",
        "assets/shaders/depth.fs");

    depth_skinned_mesh_program = std::make_unique<liminal::program>(
        "assets/shaders/depth_skinned_mesh.vs",
        "assets/shaders/depth.fs");

    depth_cube_mesh_program = std::make_unique<liminal::program>(
        "assets/shaders/depth_cube_mesh.vs",
        "assets/shaders/depth_cube.gs",
        "assets/shaders/depth_cube.fs");

    depth_cube_skinned_mesh_program = std::make_unique<liminal::program>(
        "assets/shaders/depth_cube_skinned_mesh.vs",
        "assets/shaders/depth_cube.gs",
        "assets/shaders/depth_cube.fs");

    color_program = std::make_unique<liminal::program>(
        "assets/shaders/color.vs",
        "assets/shaders/color.fs");

    geometry_mesh_program = std::make_unique<liminal::program>(
        "assets/shaders/geometry_mesh.vs",
        "assets/shaders/geometry_mesh.fs");
    geometry_mesh_program->bind();
    geometry_mesh_program->set_samplers(
        {{"material.albedo_map", 0},
         {"material.normal_map", 1},
         {"material.metallic_map", 2},
         {"material.roughness_map", 3},
         {"material.occlusion_map", 4},
         {"material.height_map", 5}});
    liminal::program::unbind();

    geometry_skinned_mesh_program = std::make_unique<liminal::program>(
        "assets/shaders/geometry_skinned_mesh.vs",
        "assets/shaders/geometry_mesh.fs");
    geometry_skinned_mesh_program->bind();
    geometry_skinned_mesh_program->set_samplers(
        {{"material.albedo_map", 0},
         {"material.normal_map", 1},
         {"material.metallic_map", 2},
         {"material.roughness_map", 3},
         {"material.occlusion_map", 4},
         {"material.height_map", 5}});
    liminal::program::unbind();

    geometry_terrain_program = std::make_unique<liminal::program>(
        "assets/shaders/geometry_mesh.vs",
        "assets/shaders/geometry_terrain.fs");
    geometry_terrain_program->bind();
    geometry_terrain_program->set_samplers(
        {{"materials[0].albedo_map", 0},
         {"materials[0].normal_map", 1},
         {"materials[0].metallic_map", 2},
         {"materials[0].roughness_map", 3},
         {"materials[0].occlusion_map", 4},
         {"materials[0].height_map", 5}});
    liminal::program::unbind();

    deferred_ambient_program = std::make_unique<liminal::program>(
        "assets/shaders/deferred.vs",
        "assets/shaders/deferred_ambient.fs");
    deferred_ambient_program->bind();
    deferred_ambient_program->set_samplers(
        {{"geometry.position_map", 0},
         {"geometry.normal_map", 1},
         {"geometry.color_map", 2},
         {"geometry.albedo_map", 3},
         {"geometry.material_map", 4},
         {"skybox.irradiance_cubemap", 5},
         {"skybox.prefilter_cubemap", 6},
         {"brdf_map", 7}});
    liminal::program::unbind();

    deferred_directional_program = std::make_unique<liminal::program>(
        "assets/shaders/deferred.vs",
        "assets/shaders/deferred_directional.fs");
    deferred_directional_program->bind();
    deferred_directional_program->set_samplers(
        {{"geometry.position_map", 0},
         {"geometry.normal_map", 1},
         {"geometry.color_map", 2},
         {"geometry.albedo_map", 3},
         {"geometry.material_map", 4}});
    for (std::size_t cascade_index = 0; cascade_index < liminal::directional_light::num_cascades; cascade_index++)
    {
        deferred_directional_program->set_sampler(("light.depth_map[" + std::to_string(cascade_index) + "]").c_str(), static_cast<GLint>(5 + cascade_index));
    }
    liminal::program::unbind();

    deferred_point_program = std::make_unique<liminal::program>(
        "assets/shaders/deferred.vs",
        "assets/shaders/deferred_point.fs");
    deferred_point_program->bind();
    deferred_point_program->set_samplers(
        {{"geometry.position_map", 0},
         {"geometry.normal_map", 1},
         {"geometry.color_map", 2},
         {"geometry.albedo_map", 3},
         {"geometry.material_map", 4},
         {"light.depth_cubemap", 5}});
    liminal::program::unbind();

    deferred_spot_program = std::make_unique<liminal::program>(
        "assets/shaders/deferred.vs",
        "assets/shaders/deferred_spot.fs");
    deferred_spot_program->bind();
    deferred_spot_program->set_samplers(
        {{"geometry.position_map", 0},
         {"geometry.normal_map", 1},
         {"geometry.color_map", 2},
         {"geometry.albedo_map", 3},
         {"geometry.material_map", 4},
         {"light.depth_map", 5}});
    liminal::program::unbind();

    skybox_program = std::make_unique<liminal::program>(
        "assets/shaders/skybox.vs",
        "assets/shaders/skybox.fs");
    skybox_program->bind();
    skybox_program->set_samplers(
        {{"skybox.environment_cubemap", 0}});
    liminal::program::unbind();

    water_program = std::make_unique<liminal::program>(
        "assets/shaders/water.vs",
        "assets/shaders/water.fs");
    water_program->bind();
    water_program->set_samplers(
        {{"water.reflection_map", 0},
         {"water.refraction_map", 1},
         {"water.depth_map", 2},
         {"water.dudv_map", 3},
         {"water.normal_map", 4}});
    liminal::program::unbind();

    sprite_program = std::make_unique<liminal::program>(
        "assets/shaders/sprite.vs",
        "assets/shaders/sprite.fs");
    sprite_program->bind();
    sprite_program->set_samplers(
        {{"sprite.texture", 0}});
    liminal::program::unbind();

    gaussian_program = std::make_unique<liminal::program>(
        "assets/shaders/gaussian.vs",
        "assets/shaders/gaussian.fs");
    gaussian_program->bind();
    gaussian_program->set_samplers(
        {{"image", 0}});
    liminal::program::unbind();

    postprocess_program = std::make_unique<liminal::program>(
        "assets/shaders/postprocess.vs",
        "assets/shaders/postprocess.fs");
    postprocess_program->bind();
    postprocess_program->set_samplers(
        {{"hdr_map", 0},
         {"bloom_map", 1}});
    liminal::program::unbind();

    // create water textures
    water_dudv_texture = std::make_unique<liminal::texture>("assets/images/water_dudv.png");
    water_normal_texture = std::make_unique<liminal::texture>("assets/images/water_normal.png");

    // DEBUG: create sphere mesh
    // http://www.songho.ca/opengl/gl_sphere.html
    {
        std::vector<liminal::vertex> vertices;
        constexpr auto radius = 1.0f;
        constexpr std::size_t stack_count = 36;
        constexpr std::size_t sector_count = 18;
        constexpr auto length_inv = 1 / radius;
        constexpr auto sector_step = 2 * 3.14f / sector_count;
        constexpr auto stack_step = 3.14f / stack_count;
        for (std::size_t stack = 0; stack <= stack_count; stack++)
        {
            const auto stack_angle = 3.14f / 2 - stack * stack_step;
            const auto xy = radius * glm::cos(stack_angle);
            const auto z = radius * glm::sin(stack_angle);
            for (std::size_t sector = 0; sector <= sector_count; sector++)
            {
                const auto sector_angle = sector * sector_step;

                liminal::vertex vertex;
                vertex.position.x = xy * glm::cos(sector_angle);
                vertex.position.y = xy * glm::sin(sector_angle);
                vertex.position.z = z;
                vertex.normal.x = vertex.position.x * length_inv;
                vertex.normal.y = vertex.position.y * length_inv;
                vertex.normal.z = vertex.position.z * length_inv;
                vertex.uv.s = static_cast<float>(sector / sector_count);
                vertex.uv.t = static_cast<float>(stack / stack_count);
                vertices.push_back(vertex);
            }
        }

        std::vector<unsigned int> indices;
        for (std::size_t stack = 0; stack < stack_count; stack++)
        {
            unsigned int k1 = static_cast<unsigned int>(stack * (sector_count + 1));
            unsigned int k2 = k1 + static_cast<unsigned int>(sector_count + 1);
            for (std::size_t sector = 0; sector < sector_count; sector++, k1++, k2++)
            {
                if (stack != 0)
                {
                    indices.push_back(k1);
                    indices.push_back(k2);
                    indices.push_back(k1 + 1);
                }
                if (stack != stack_count - 1)
                {
                    indices.push_back(k1 + 1);
                    indices.push_back(k2);
                    indices.push_back(k2 + 1);
                }
            }
        }

        std::array<std::vector<std::shared_ptr<liminal::texture>>, liminal::mesh::num_textures> textures;

        DEBUG_sphere_mesh = std::make_unique<liminal::mesh>(vertices, indices, textures);
    }
}

liminal::renderer::~renderer()
{
    glDeleteFramebuffers(1, &geometry_fbo_id);

    glDeleteFramebuffers(1, &hdr_fbo_id);

    glDeleteFramebuffers(1, &final_fbo_id);

    glDeleteFramebuffers(num_directional_light_shadows, directional_light_depth_map_fbo_ids.data());

    glDeleteFramebuffers(num_point_light_shadows, point_light_depth_cubemap_fbo_ids.data());

    glDeleteFramebuffers(num_spot_light_shadows, spot_light_depth_map_fbo_ids.data());

    glDeleteFramebuffers(1, &water_reflection_fbo_id);

    glDeleteFramebuffers(1, &water_refraction_fbo_id);

    glDeleteFramebuffers(2, bloom_fbo_ids.data());

    glDeleteVertexArrays(1, &water_vao_id);
    glDeleteBuffers(1, &water_vbo_id);

    glDeleteVertexArrays(1, &skybox_vao_id);
    glDeleteBuffers(1, &skybox_vbo_id);

    glDeleteVertexArrays(1, &sprite_vao_id);
    glDeleteBuffers(1, &sprite_vbo_id);

    glDeleteVertexArrays(1, &screen_vao_id);
    glDeleteBuffers(1, &screen_vbo_id);
}

float liminal::renderer::get_aspect_ratio() const
{
    return static_cast<float>(render_width) / static_cast<float>(render_height);
}

void liminal::renderer::set_target_size(const GLsizei width, const GLsizei height)
{
    target_width = width;
    target_height = height;

    calc_render_size();
}

void liminal::renderer::set_render_scale(const float scale)
{
    render_scale = scale;

    calc_render_size();
}

void liminal::renderer::calc_render_size()
{
    render_width = static_cast<GLsizei>(target_width * render_scale);
    render_height = static_cast<GLsizei>(target_height * render_scale);

    glDeleteFramebuffers(1, &geometry_fbo_id);

    glDeleteFramebuffers(1, &hdr_fbo_id);

    glDeleteFramebuffers(1, &final_fbo_id);

    glDeleteFramebuffers(2, bloom_fbo_ids.data());

    // setup geometry fbo
    // gbuffer:
    //      position - rgb16f
    //      normal - rgb16f
    //      color - rgb16f
    //      albedo - rgb16f
    //      material - rgba16f
    //          metallic - r
    //          roughness - g
    //          occlusion - b
    //          height - a
    //      id - r32i
    glGenFramebuffers(1, &geometry_fbo_id);
    glBindFramebuffer(GL_FRAMEBUFFER, geometry_fbo_id);
    {
        geometry_position_texture = std::make_unique<liminal::texture>(
            GL_RGBA16F,
            render_width,
            render_height,
            GL_RGB,
            GL_FLOAT);
        geometry_normal_texture = std::make_unique<liminal::texture>(
            GL_RGBA16F,
            render_width,
            render_height,
            GL_RGB,
            GL_FLOAT);
        geometry_color_texture = std::make_unique<liminal::texture>(
            GL_RGBA16F,
            render_width,
            render_height,
            GL_RGB,
            GL_FLOAT);
        geometry_albedo_texture = std::make_unique<liminal::texture>(
            GL_RGBA16F,
            render_width,
            render_height,
            GL_RGB,
            GL_FLOAT);
        geometry_material_texture = std::make_unique<liminal::texture>(
            GL_RGBA16F,
            render_width,
            render_height,
            GL_RGBA,
            GL_FLOAT);
        geometry_id_texture = std::make_unique<liminal::texture>(
            GL_R32I,
            render_width,
            render_height,
            GL_RED_INTEGER,
            GL_INT);
        geometry_depth_renderbuffer = std::make_unique<liminal::renderbuffer>(
            GL_DEPTH_COMPONENT24,
            render_width,
            render_height);

        glFramebufferTexture2D(
            GL_FRAMEBUFFER,
            GL_COLOR_ATTACHMENT0,
            GL_TEXTURE_2D,
            geometry_position_texture->get_texture_id(),
            0);
        glFramebufferTexture2D(
            GL_FRAMEBUFFER,
            GL_COLOR_ATTACHMENT1,
            GL_TEXTURE_2D,
            geometry_normal_texture->get_texture_id(),
            0);
        glFramebufferTexture2D(
            GL_FRAMEBUFFER,
            GL_COLOR_ATTACHMENT2,
            GL_TEXTURE_2D,
            geometry_color_texture->get_texture_id(),
            0);
        glFramebufferTexture2D(
            GL_FRAMEBUFFER,
            GL_COLOR_ATTACHMENT3,
            GL_TEXTURE_2D,
            geometry_albedo_texture->get_texture_id(),
            0);
        glFramebufferTexture2D(
            GL_FRAMEBUFFER,
            GL_COLOR_ATTACHMENT4,
            GL_TEXTURE_2D,
            geometry_material_texture->get_texture_id(),
            0);
        glFramebufferTexture2D(
            GL_FRAMEBUFFER,
            GL_COLOR_ATTACHMENT5,
            GL_TEXTURE_2D,
            geometry_id_texture->get_texture_id(),
            0);
        glFramebufferRenderbuffer(
            GL_FRAMEBUFFER,
            GL_DEPTH_ATTACHMENT,
            GL_RENDERBUFFER,
            geometry_depth_renderbuffer->get_renderbuffer_id());

        constexpr std::array<GLenum, 6> geometry_color_attachments{
            GL_COLOR_ATTACHMENT0,
            GL_COLOR_ATTACHMENT1,
            GL_COLOR_ATTACHMENT2,
            GL_COLOR_ATTACHMENT3,
            GL_COLOR_ATTACHMENT4,
            GL_COLOR_ATTACHMENT5};
        glDrawBuffers(static_cast<GLsizei>(geometry_color_attachments.size()), geometry_color_attachments.data());

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            throw std::runtime_error("Geometry framebuffer is not complete");
        }
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // setup hdr fbo
    glGenFramebuffers(1, &hdr_fbo_id);
    glBindFramebuffer(GL_FRAMEBUFFER, hdr_fbo_id);
    {
        for (GLenum hdr_texture_index = 0; hdr_texture_index < hdr_textures.size(); hdr_texture_index++)
        {
            hdr_textures.at(hdr_texture_index) = std::make_unique<liminal::texture>(
                GL_RGBA16F,
                render_width,
                render_height,
                GL_RGB,
                GL_FLOAT,
                liminal::texture_filter::linear,
                liminal::texture_wrap::clamp_to_edge);

            glFramebufferTexture2D(
                GL_FRAMEBUFFER,
                GL_COLOR_ATTACHMENT0 + hdr_texture_index,
                GL_TEXTURE_2D,
                hdr_textures.at(hdr_texture_index)->get_texture_id(),
                0);
        }

        hdr_depth_renderbuffer = std::make_unique<liminal::renderbuffer>(
            GL_DEPTH_STENCIL,
            target_width,
            target_height);

        glFramebufferRenderbuffer(
            GL_FRAMEBUFFER,
            GL_DEPTH_ATTACHMENT,
            GL_RENDERBUFFER,
            hdr_depth_renderbuffer->get_renderbuffer_id());

        constexpr std::array<GLenum, 2> hdr_color_attachments{
            GL_COLOR_ATTACHMENT0,
            GL_COLOR_ATTACHMENT1};
        glDrawBuffers(static_cast<GLsizei>(hdr_color_attachments.size()), hdr_color_attachments.data());

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            throw std::runtime_error("HDR framebuffer is not complete");
        }
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // setup bloom fbo
    glGenFramebuffers(2, bloom_fbo_ids.data());
    for (std::size_t bloom_index = 0; bloom_index < bloom_fbo_ids.size(); bloom_index++)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, bloom_fbo_ids.at(bloom_index));
        {
            bloom_textures.at(bloom_index) = std::make_unique<liminal::texture>(
                GL_RGBA16F,
                target_width / 8,
                target_height / 8,
                GL_RGBA,
                GL_FLOAT,
                liminal::texture_filter::linear,
                liminal::texture_wrap::clamp_to_edge);

            glFramebufferTexture2D(
                GL_FRAMEBUFFER,
                GL_COLOR_ATTACHMENT0,
                GL_TEXTURE_2D,
                bloom_textures.at(bloom_index)->get_texture_id(),
                0);

            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            {
                throw std::runtime_error("Bloom framebuffer is not complete");
            }
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    // setup final fbo
    glGenFramebuffers(1, &final_fbo_id);
    glBindFramebuffer(GL_FRAMEBUFFER, final_fbo_id);
    {
        final_texture = std::make_unique<liminal::texture>(
            GL_RGBA16F,
            target_width,
            target_height,
            GL_RGBA,
            GL_UNSIGNED_BYTE);

        glFramebufferTexture2D(
            GL_FRAMEBUFFER,
            GL_COLOR_ATTACHMENT0,
            GL_TEXTURE_2D,
            final_texture->get_texture_id(),
            0);

        {
            constexpr std::array<GLenum, 1> final_color_attachments{
                GL_COLOR_ATTACHMENT0};
            glDrawBuffers(static_cast<GLsizei>(final_color_attachments.size()), final_color_attachments.data());
        }

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            throw std::runtime_error("Final framebuffer is not complete");
        }
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void liminal::renderer::set_directional_light_depth_map_size(const GLsizei size)
{
    directional_light_depth_map_size = size;

    glDeleteFramebuffers(num_directional_light_shadows, directional_light_depth_map_fbo_ids.data());

    glGenFramebuffers(num_directional_light_shadows, directional_light_depth_map_fbo_ids.data());
    for (std::size_t shadow_index = 0; shadow_index < num_directional_light_shadows; shadow_index++)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, directional_light_depth_map_fbo_ids.at(shadow_index));
        {
            glDrawBuffer(GL_NONE);
            glReadBuffer(GL_NONE);

            for (std::size_t cascade_index = 0; cascade_index < liminal::directional_light::num_cascades; cascade_index++)
            {
                constexpr std::array<GLfloat, 4> border_color = {1, 1, 1, 1};
                directional_light_depth_map_textures.at(shadow_index).at(cascade_index) = std::make_unique<liminal::texture>(
                    GL_DEPTH_COMPONENT32,
                    size,
                    size,
                    GL_DEPTH_COMPONENT,
                    GL_FLOAT,
                    liminal::texture_filter::nearest,
                    liminal::texture_wrap::clamp_to_border,
                    border_color);
            }

            glFramebufferTexture2D(
                GL_FRAMEBUFFER,
                GL_DEPTH_ATTACHMENT,
                GL_TEXTURE_2D,
                directional_light_depth_map_textures.at(shadow_index).at(0)->get_texture_id(),
                0);

            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            {
                throw std::runtime_error("Directional light depth map framebuffer is not complete");
            }
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}

void liminal::renderer::set_point_light_depth_cubemap_size(const GLsizei size)
{
    point_light_depth_cubemap_size = size;

    glDeleteFramebuffers(num_point_light_shadows, point_light_depth_cubemap_fbo_ids.data());

    glGenFramebuffers(num_point_light_shadows, point_light_depth_cubemap_fbo_ids.data());
    for (std::size_t shadow_index = 0; shadow_index < num_point_light_shadows; shadow_index++)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, point_light_depth_cubemap_fbo_ids.at(shadow_index));
        {
            glDrawBuffer(GL_NONE);
            glReadBuffer(GL_NONE);

            point_light_depth_cubemap_textures.at(shadow_index) = std::make_unique<liminal::cubemap>(
                GL_DEPTH_COMPONENT32,
                size,
                size,
                GL_DEPTH_COMPONENT,
                GL_FLOAT,
                liminal::texture_filter::nearest,
                liminal::texture_wrap::clamp_to_border);

            glFramebufferTexture(
                GL_FRAMEBUFFER,
                GL_DEPTH_ATTACHMENT,
                point_light_depth_cubemap_textures.at(shadow_index)->get_texture_id(),
                0);

            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            {
                throw std::runtime_error("Point light depth cubemap framebuffer is not complete");
            }
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}

void liminal::renderer::set_spot_light_depth_map_size(const GLsizei size)
{
    spot_light_depth_map_size = size;

    glDeleteFramebuffers(num_spot_light_shadows, spot_light_depth_map_fbo_ids.data());

    glGenFramebuffers(num_spot_light_shadows, spot_light_depth_map_fbo_ids.data());
    for (std::size_t shadow_index = 0; shadow_index < num_spot_light_shadows; shadow_index++)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, spot_light_depth_map_fbo_ids.at(shadow_index));
        {
            glDrawBuffer(GL_NONE);
            glReadBuffer(GL_NONE);

            constexpr std::array<GLfloat, 4> border_color{1, 1, 1, 1};
            spot_light_depth_map_textures.at(shadow_index) = std::make_unique<liminal::texture>(
                GL_DEPTH_COMPONENT32,
                size,
                size,
                GL_DEPTH_COMPONENT,
                GL_FLOAT,
                liminal::texture_filter::nearest,
                liminal::texture_wrap::clamp_to_border,
                border_color);

            glFramebufferTexture2D(
                GL_FRAMEBUFFER,
                GL_DEPTH_ATTACHMENT,
                GL_TEXTURE_2D,
                spot_light_depth_map_textures.at(shadow_index)->get_texture_id(),
                0);

            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            {
                throw std::runtime_error("Spot light depth map framebuffer is not complete");
            }
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}

void liminal::renderer::set_reflection_size(const GLsizei width, const GLsizei height)
{
    water_reflection_width = width;
    water_reflection_height = height;

    glDeleteFramebuffers(1, &water_reflection_fbo_id);

    // setup water reflection fbo
    glGenFramebuffers(1, &water_reflection_fbo_id);
    glBindFramebuffer(GL_FRAMEBUFFER, water_reflection_fbo_id);
    {
        water_reflection_color_texture = std::make_unique<liminal::texture>(
            GL_RGBA16F,
            width,
            height,
            GL_RGBA,
            GL_FLOAT,
            liminal::texture_filter::linear);
        water_reflection_depth_renderbuffer = std::make_unique<liminal::renderbuffer>(
            GL_DEPTH_COMPONENT24,
            width,
            height);

        glFramebufferTexture2D(
            GL_FRAMEBUFFER,
            GL_COLOR_ATTACHMENT0,
            GL_TEXTURE_2D,
            water_reflection_color_texture->get_texture_id(),
            0);
        glFramebufferRenderbuffer(
            GL_FRAMEBUFFER,
            GL_DEPTH_ATTACHMENT,
            GL_RENDERBUFFER,
            water_reflection_depth_renderbuffer->get_renderbuffer_id());

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            throw std::runtime_error("Water reflection framebuffer is not complete");
        }
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void liminal::renderer::set_refraction_size(GLsizei width, GLsizei height)
{
    water_refraction_width = width;
    water_refraction_height = height;

    glDeleteFramebuffers(1, &water_refraction_fbo_id);

    // setup water refraction fbo
    glGenFramebuffers(1, &water_refraction_fbo_id);
    glBindFramebuffer(GL_FRAMEBUFFER, water_refraction_fbo_id);
    {
        water_refraction_color_texture = std::make_unique<liminal::texture>(
            GL_RGBA16F,
            width,
            height,
            GL_RGBA,
            GL_FLOAT,
            liminal::texture_filter::linear,
            liminal::texture_wrap::clamp_to_edge);
        water_refraction_depth_texture = std::make_unique<liminal::texture>(
            GL_DEPTH_COMPONENT32,
            width,
            height,
            GL_DEPTH_COMPONENT,
            GL_FLOAT,
            liminal::texture_filter::linear);

        glFramebufferTexture2D(
            GL_FRAMEBUFFER,
            GL_COLOR_ATTACHMENT0,
            GL_TEXTURE_2D,
            water_refraction_color_texture->get_texture_id(),
            0);
        glFramebufferTexture2D(
            GL_FRAMEBUFFER,
            GL_DEPTH_ATTACHMENT,
            GL_TEXTURE_2D,
            water_refraction_depth_texture->get_texture_id(),
            0);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            throw std::runtime_error("Water refraction framebuffer is not complete");
        }
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void liminal::renderer::reload_programs()
{
    depth_mesh_program->reload();
    depth_skinned_mesh_program->reload();
    depth_cube_mesh_program->reload();
    depth_cube_skinned_mesh_program->reload();
    color_program->reload();
    geometry_mesh_program->reload();
    geometry_skinned_mesh_program->reload();
    geometry_terrain_program->reload();
    deferred_ambient_program->reload();
    deferred_directional_program->reload();
    deferred_point_program->reload();
    deferred_spot_program->reload();
    skybox_program->reload();
    water_program->reload();
    sprite_program->reload();
    gaussian_program->reload();
    postprocess_program->reload();
}

// TODO: mouse picking doesn't work if water is present in the scene
// this is because the reflection and refraction rendering overwrites the entity ids
// and because they have clipping planes it doesn't write entity ids back in certain regions
// interestingly, you can pick an entity when looking at it from underwater
liminal::entity liminal::renderer::pick(const int x, const int y, liminal::scene *scene) const
{
    entt::entity id;
    glBindFramebuffer(GL_FRAMEBUFFER, geometry_fbo_id);
    {
        glReadBuffer(GL_COLOR_ATTACHMENT5);
        glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &id);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return scene->get_entity(id);
}

void liminal::renderer::render(liminal::scene &scene, const std::uint64_t current_time, const float) const
{
    if (default_camera && default_camera_transform)
    {
        render_all(scene, *default_camera, *default_camera_transform, current_time);
    }

    for (const auto [id, camera, transform] : scene.get_entities_with<const liminal::camera, const liminal::transform>().each())
    {
        render_all(scene, camera, transform, current_time);
    }
}

void liminal::renderer::render_all(
    liminal::scene &scene,
    const liminal::camera &camera,
    const liminal::transform &camera_transform,
    const std::uint64_t current_time) const
{
    render_shadows(scene, camera_transform);
    render_objects(scene, camera, camera_transform, hdr_fbo_id, render_width, render_height);
    render_waters(scene, camera, camera_transform, current_time);
    render_sprites(scene);
    render_screen(camera);
}

void liminal::renderer::render_shadows(
    liminal::scene &scene,
    const liminal::transform &camera_transform) const
{
    for (std::size_t shadow_index = 0; const auto [id, directional_light, transform] : scene.get_entities_with<liminal::directional_light, const liminal::transform>().each())
    {
        for (std::size_t cascade_index = 0; cascade_index < liminal::directional_light::num_cascades; cascade_index++)
        {
            directional_light.view_projection_matrices.at(cascade_index) = glm::identity<glm::mat4>();
            directional_light.depth_map_textures.at(cascade_index) = {};
        }

        if (!directional_light.shadows || shadow_index >= num_directional_light_shadows)
        {
            continue;
        }

        glBindFramebuffer(GL_FRAMEBUFFER, directional_light_depth_map_fbo_ids.at(shadow_index));
        {
            glViewport(0, 0, directional_light_depth_map_size, directional_light_depth_map_size);
            glEnable(GL_CULL_FACE);

            // const auto aspect_ratio = get_aspect_ratio();
            // const auto tan_half_h_fov = glm::tan(glm::radians(90.0f / 2.0f));
            // const auto tan_half_v_fov = glm::tan(glm::radians((90.0f * aspect_ratio) / 2.0f));
            // const float cascade_end[] = {1.0f, 25.0f, 90.0f, 200.0f};

            for (std::size_t cascade_index = 0; cascade_index < liminal::directional_light::num_cascades; cascade_index++)
            {
                // const auto xn = cascade_end[j] * tan_half_h_fov;
                // const auto xf = cascade_end[j + 1] * tan_half_h_fov;
                // const auto yn = cascade_end[j] * tan_half_v_fov;
                // const auto yf = cascade_end[j + 1] * tan_half_v_fov;

                // const glm::vec4 frustum_corners[] = {
                //     // near face
                //     glm::vec4(xn, yn, cascade_end[j], 1.0),
                //     glm::vec4(-xn, yn, cascade_end[j], 1.0),
                //     glm::vec4(xn, -yn, cascade_end[j], 1.0),
                //     glm::vec4(-xn, -yn, cascade_end[j], 1.0),

                //     // far face
                //     glm::vec4(xf, yf, cascade_end[j + 1], 1.0),
                //     glm::vec4(-xf, yf, cascade_end[j + 1], 1.0),
                //     glm::vec4(xf, -yf, cascade_end[j + 1], 1.0),
                //     glm::vec4(-xf, -yf, cascade_end[j + 1], 1.0)};
                // glm::vec4 frustum_forners_l[8];

                // const auto min_x = std::numeric_limits<float>::max();
                // const auto max_x = std::numeric_limits<float>::min();
                // const auto min_y = std::numeric_limits<float>::max();
                // const auto max_y = std::numeric_limits<float>::min();
                // const auto min_z = std::numeric_limits<float>::max();
                // const auto max_z = std::numeric_limits<float>::min();

                // for (std::size_t k = 0; k < 8; k++)
                // {
                //     glm::vec4 vw = ;
                // }

                const auto projection = glm::ortho(
                    -directional_light_shadow_map_size,
                    directional_light_shadow_map_size,
                    -directional_light_shadow_map_size,
                    directional_light_shadow_map_size,
                    directional_light_near_plane,
                    directional_light_far_plane);
                const auto view = glm::lookAt(
                    camera_transform.position - transform.rotation,
                    camera_transform.position,
                    {0, 1, 0});
                directional_light.view_projection_matrices.at(cascade_index) = projection * view;
                directional_light.depth_map_textures.at(cascade_index) = directional_light_depth_map_textures.at(shadow_index).at(cascade_index);

                glFramebufferTexture2D(
                    GL_FRAMEBUFFER,
                    GL_DEPTH_ATTACHMENT,
                    GL_TEXTURE_2D,
                    directional_light_depth_map_textures.at(shadow_index).at(cascade_index)->get_texture_id(),
                    0);

                glClear(GL_DEPTH_BUFFER_BIT);

                for (const auto [_id, _mesh_renderer, _transform] : scene.get_entities_with<const liminal::mesh_renderer, const liminal::transform>().each())
                {
                    if (_mesh_renderer.model)
                    {
                        const auto model_matrix = _transform.get_model_matrix();
                        if (_mesh_renderer.model->has_animations())
                        {
                            depth_skinned_mesh_program->bind();
                            {
                                depth_skinned_mesh_program->set_mat4("mvp_matrix", directional_light.view_projection_matrices.at(cascade_index) * model_matrix);
                                depth_skinned_mesh_program->set_mat4_vector("bone_transformations", _mesh_renderer.bone_transformations);

                                _mesh_renderer.model->draw_meshes(*depth_skinned_mesh_program);
                            }
                            liminal::program::unbind();
                        }
                        else
                        {
                            depth_mesh_program->bind();
                            {
                                depth_mesh_program->set_mat4("mvp_matrix", directional_light.view_projection_matrices.at(cascade_index) * model_matrix);

                                _mesh_renderer.model->draw_meshes(*depth_mesh_program);
                            }
                            liminal::program::unbind();
                        }
                    }
                }

                depth_mesh_program->bind();
                {
                    for (const auto [_id, _terrain] : scene.get_entities_with<const liminal::terrain>().each())
                    {
                        const auto model_matrix = _terrain.get_model_matrix();

                        depth_mesh_program->set_mat4("mvp_matrix", directional_light.view_projection_matrices.at(cascade_index) * model_matrix);

                        _terrain.mesh->draw(*depth_mesh_program);
                    }
                }
                liminal::program::unbind();
            }

            glDisable(GL_CULL_FACE);
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        shadow_index++;
    }

    for (std::size_t shadow_index = 0; const auto [id, point_light, transform] : scene.get_entities_with<liminal::point_light, const liminal::transform>().each())
    {
        point_light.view_projection_matrices.fill(glm::identity<glm::mat4>());
        point_light.depth_cubemap_texture = {};

        if (shadow_index >= num_point_light_shadows)
        {
            continue;
        }

        const auto projection = glm::perspective(
            glm::radians(90.0f),
            1.0f,
            point_light_near_plane,
            point_light_far_plane);
        point_light.view_projection_matrices.at(0) = projection * glm::lookAt(transform.position, transform.position + glm::vec3(1, 0, 0), glm::vec3(0, -1, 0));
        point_light.view_projection_matrices.at(1) = projection * glm::lookAt(transform.position, transform.position + glm::vec3(-1, 0, 0), glm::vec3(0, -1, 0));
        point_light.view_projection_matrices.at(2) = projection * glm::lookAt(transform.position, transform.position + glm::vec3(0, 1, 0), glm::vec3(0, 0, 1));
        point_light.view_projection_matrices.at(3) = projection * glm::lookAt(transform.position, transform.position + glm::vec3(0, -1, 0), glm::vec3(0, 0, -1));
        point_light.view_projection_matrices.at(4) = projection * glm::lookAt(transform.position, transform.position + glm::vec3(0, 0, 1), glm::vec3(0, -1, 0));
        point_light.view_projection_matrices.at(5) = projection * glm::lookAt(transform.position, transform.position + glm::vec3(0, 0, -1), glm::vec3(0, -1, 0));
        point_light.depth_cubemap_texture = point_light_depth_cubemap_textures.at(shadow_index);

        glBindFramebuffer(GL_FRAMEBUFFER, point_light_depth_cubemap_fbo_ids.at(shadow_index));
        {
            glViewport(0, 0, point_light_depth_cubemap_size, point_light_depth_cubemap_size);
            glEnable(GL_CULL_FACE);

            glClear(GL_DEPTH_BUFFER_BIT);

            for (const auto [_id, _transform, _mesh_renderer] : scene.get_entities_with<const liminal::transform, const liminal::mesh_renderer>().each())
            {
                if (_mesh_renderer.model)
                {
                    const auto model_matrix = _transform.get_model_matrix();
                    if (_mesh_renderer.model->has_animations())
                    {
                        depth_cube_skinned_mesh_program->bind();
                        {
                            depth_cube_skinned_mesh_program->set_mat4("model_matrix", model_matrix);
                            depth_cube_skinned_mesh_program->set_mat4_vector("bone_transformations", _mesh_renderer.bone_transformations);
                            depth_cube_skinned_mesh_program->set_mat4("light.view_projection_matrices[0]", point_light.view_projection_matrices.at(0));
                            depth_cube_skinned_mesh_program->set_mat4("light.view_projection_matrices[1]", point_light.view_projection_matrices.at(1));
                            depth_cube_skinned_mesh_program->set_mat4("light.view_projection_matrices[2]", point_light.view_projection_matrices.at(2));
                            depth_cube_skinned_mesh_program->set_mat4("light.view_projection_matrices[3]", point_light.view_projection_matrices.at(3));
                            depth_cube_skinned_mesh_program->set_mat4("light.view_projection_matrices[4]", point_light.view_projection_matrices.at(4));
                            depth_cube_skinned_mesh_program->set_mat4("light.view_projection_matrices[5]", point_light.view_projection_matrices.at(5));
                            depth_cube_skinned_mesh_program->set_float("light.far_plane", point_light_far_plane);
                            depth_cube_skinned_mesh_program->set_vec3("light.position", transform.position);

                            _mesh_renderer.model->draw_meshes(*depth_cube_skinned_mesh_program);
                        }
                        liminal::program::unbind();
                    }
                    else
                    {
                        depth_cube_mesh_program->bind();
                        {
                            depth_cube_mesh_program->set_mat4("model_matrix", model_matrix);

                            depth_cube_mesh_program->set_mat4("light.view_projection_matrices[0]", point_light.view_projection_matrices.at(0));
                            depth_cube_mesh_program->set_mat4("light.view_projection_matrices[1]", point_light.view_projection_matrices.at(1));
                            depth_cube_mesh_program->set_mat4("light.view_projection_matrices[2]", point_light.view_projection_matrices.at(2));
                            depth_cube_mesh_program->set_mat4("light.view_projection_matrices[3]", point_light.view_projection_matrices.at(3));
                            depth_cube_mesh_program->set_mat4("light.view_projection_matrices[4]", point_light.view_projection_matrices.at(4));
                            depth_cube_mesh_program->set_mat4("light.view_projection_matrices[5]", point_light.view_projection_matrices.at(5));

                            depth_cube_mesh_program->set_float("light.far_plane", point_light_far_plane);
                            depth_cube_mesh_program->set_vec3("light.position", transform.position);

                            _mesh_renderer.model->draw_meshes(*depth_cube_mesh_program);
                        }
                        liminal::program::unbind();
                    }
                }
            }

            // TODO: point light shadows disabled on terrain for now due to performance; investigate this
            // depth_cube_mesh_program->bind();
            // {
            //     for (std::size_t face_index = 0; face_index < 6; face_index++)
            //     {
            //         depth_cube_mesh_program->set_mat4("light.view_projection_matrices[" + std::to_string(face_index) + "]", point_light.view_projection_matrices.at(face_index));
            //     }

            //     depth_cube_mesh_program->set_float("light.far_plane", point_light_far_plane);
            //     depth_cube_mesh_program->set_vec3("light.position", transform.position);

            //     for (const auto [_id, _terrain] : scene.get_entities_with<const liminal::terrain>())
            //     {
            //         const auto model_matrix = _terrain.get_model_matrix();

            //         depth_cube_mesh_program->set_mat4("model_matrix", model_matrix);

            //         _terrain.mesh->draw(*depth_cube_mesh_program);
            //     }
            // }
            // liminal::program::unbind();

            glDisable(GL_CULL_FACE);
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        shadow_index++;
    }

    for (std::size_t shadow_index = 0; const auto [id, spot_light, transform] : scene.get_entities_with<liminal::spot_light, const liminal::transform>().each())
    {
        spot_light.view_projection_matrix = glm::identity<glm::mat4>();
        spot_light.depth_map_texture = {};

        if (shadow_index >= num_spot_light_shadows)
        {
            continue;
        }

        const auto projection = glm::perspective(
            glm::radians(90.0f),
            1.0f,
            spot_light_near_plane,
            spot_light_far_plane);
        const auto front = glm::normalize(transform.rotation);
        const auto target = transform.position + front;
        constexpr auto up = glm::vec3(0, 1, 0);
        const auto view = glm::lookAt(transform.position, target, up);
        spot_light.view_projection_matrix = projection * view;
        spot_light.depth_map_texture = spot_light_depth_map_textures.at(shadow_index);

        glBindFramebuffer(GL_FRAMEBUFFER, spot_light_depth_map_fbo_ids.at(shadow_index));
        {
            glViewport(0, 0, spot_light_depth_map_size, spot_light_depth_map_size);
            glEnable(GL_CULL_FACE);

            glClear(GL_DEPTH_BUFFER_BIT);

            for (const auto [_id, _transform, _mesh_renderer] : scene.get_entities_with<const liminal::transform, const liminal::mesh_renderer>().each())
            {
                if (_mesh_renderer.model)
                {
                    const auto model_matrix = _transform.get_model_matrix();
                    if (_mesh_renderer.model->has_animations())
                    {
                        depth_skinned_mesh_program->bind();
                        {
                            depth_skinned_mesh_program->set_mat4("mvp_matrix", spot_light.view_projection_matrix * model_matrix);
                            depth_skinned_mesh_program->set_mat4_vector("bone_transformations", _mesh_renderer.bone_transformations);

                            _mesh_renderer.model->draw_meshes(*depth_skinned_mesh_program);
                        }
                        liminal::program::unbind();
                    }
                    else
                    {
                        depth_mesh_program->bind();
                        {
                            depth_mesh_program->set_mat4("mvp_matrix", spot_light.view_projection_matrix * model_matrix);

                            _mesh_renderer.model->draw_meshes(*depth_mesh_program);
                        }
                        liminal::program::unbind();
                    }
                }
            }

            depth_mesh_program->bind();
            {
                for (const auto [_id, _terrain] : scene.get_entities_with<const liminal::terrain>().each())
                {
                    const auto model_matrix = _terrain.get_model_matrix();

                    depth_mesh_program->set_mat4("mvp_matrix", spot_light.view_projection_matrix * model_matrix);

                    _terrain.mesh->draw(*depth_mesh_program);
                }
            }
            liminal::program::unbind();

            glDisable(GL_CULL_FACE);
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        shadow_index++;
    }
}

void liminal::renderer::render_objects(
    liminal::scene &scene,
    const liminal::camera &camera,
    const liminal::transform &camera_transform,
    GLuint fbo_id,
    GLsizei width, GLsizei height,
    const glm::vec4 &clipping_plane) const
{
    // camera
    const auto camera_projection = camera.calc_projection(get_aspect_ratio());
    const auto camera_view = camera.calc_view(camera_transform);

    // draw to gbuffer
    glBindFramebuffer(GL_FRAMEBUFFER, geometry_fbo_id);
    {
        glViewport(0, 0, render_width, render_height);
        glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);
        glEnable(GL_CULL_FACE);
        glEnable(GL_CLIP_DISTANCE0);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        constexpr auto value = -1;
        glClearTexImage(geometry_id_texture->get_texture_id(), 0, GL_RED_INTEGER, GL_INT, &value);

        for (const auto [id, transform, mesh_renderer] : scene.get_entities_with<const liminal::transform, const liminal::mesh_renderer>().each())
        {
            if (mesh_renderer.model)
            {
                const auto model_matrix = transform.get_model_matrix();
                if (mesh_renderer.model->has_animations())
                {
                    geometry_skinned_mesh_program->bind();
                    {
                        geometry_skinned_mesh_program->set_mat4("mvp_matrix", camera_projection * camera_view * model_matrix);
                        geometry_skinned_mesh_program->set_mat4_vector("bone_transformations", mesh_renderer.bone_transformations);
                        geometry_skinned_mesh_program->set_mat4("model_matrix", model_matrix);
                        geometry_skinned_mesh_program->set_vec4("clipping_plane", clipping_plane);
                        geometry_skinned_mesh_program->set_vec3("material.color", mesh_renderer.color);
                        geometry_skinned_mesh_program->set_int("id", static_cast<int>(id));

                        mesh_renderer.model->draw_meshes(*geometry_skinned_mesh_program);
                    }
                    liminal::program::unbind();
                }
                else
                {
                    geometry_mesh_program->bind();
                    {
                        geometry_mesh_program->set_mat4("mvp_matrix", camera_projection * camera_view * model_matrix);
                        geometry_mesh_program->set_mat4("model_matrix", model_matrix);
                        geometry_mesh_program->set_vec4("clipping_plane", clipping_plane);
                        geometry_mesh_program->set_vec3("material.color", mesh_renderer.color);
                        geometry_mesh_program->set_int("id", static_cast<int>(id));

                        mesh_renderer.model->draw_meshes(*geometry_mesh_program);
                    }
                    liminal::program::unbind();
                }
            }
        }

        geometry_terrain_program->bind();
        {
            geometry_terrain_program->set_vec4("clipping_plane", clipping_plane);

            for (const auto [id, terrain] : scene.get_entities_with<const liminal::terrain>().each())
            {
                const auto model_matrix = terrain.get_model_matrix();

                geometry_terrain_program->set_mat4("mvp_matrix", camera_projection * camera_view * model_matrix);
                geometry_terrain_program->set_mat4("model_matrix", model_matrix);
                geometry_terrain_program->set_float("tiling", terrain.size);

                terrain.mesh->draw(*geometry_terrain_program);
            }
        }
        liminal::program::unbind();

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glDisable(GL_CULL_FACE);
        glDisable(GL_CLIP_DISTANCE0);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // deferred lighting
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_id);
    {
        glViewport(0, 0, width, height);
        glDisable(GL_DEPTH_TEST);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // IBL
        deferred_ambient_program->bind();
        {
            deferred_ambient_program->set_vec3("camera.position", camera_transform.position);

            geometry_position_texture->bind(0);
            geometry_normal_texture->bind(1);
            geometry_color_texture->bind(2);
            geometry_albedo_texture->bind(3);
            geometry_material_texture->bind(4);
            if (scene.skybox)
            {
                scene.skybox->bind_irradiance_map(5);
                scene.skybox->bind_prefilter_map(6);
            }
            else
            {
                liminal::texture::unbind(5);
                liminal::texture::unbind(6);
            }
            brdf_texture->bind(7);

            glBindVertexArray(screen_vao_id);
            glDrawArrays(GL_TRIANGLES, 0, screen_vertices_size);
            glBindVertexArray(0);

            liminal::texture::unbind(0);
            liminal::texture::unbind(1);
            liminal::texture::unbind(2);
            liminal::texture::unbind(3);
            liminal::texture::unbind(4);
            liminal::texture::unbind(5);
            liminal::texture::unbind(6);
            liminal::texture::unbind(7);
        }
        liminal::program::unbind();

        // blend the rest of the lights
        {
            glEnable(GL_BLEND);
            glBlendFunc(GL_ONE, GL_ONE);
            glDepthMask(GL_FALSE);
            glDepthFunc(GL_EQUAL);

            deferred_directional_program->bind();
            {
                deferred_directional_program->set_vec3("camera.position", camera_transform.position);

                geometry_position_texture->bind(0);
                geometry_normal_texture->bind(1);
                geometry_color_texture->bind(2);
                geometry_albedo_texture->bind(3);
                geometry_material_texture->bind(4);

                for (const auto [id, directional_light, transform] : scene.get_entities_with<const liminal::directional_light, const liminal::transform>().each())
                {
                    deferred_directional_program->set_vec3("light.direction", transform.rotation);
                    deferred_directional_program->set_vec3("light.color", directional_light.color);

                    for (std::size_t cascade_index = 0; cascade_index < liminal::directional_light::num_cascades; cascade_index++)
                    {
                        deferred_directional_program->set_mat4(("light.view_projection_matrices[" + std::to_string(cascade_index) + "]").c_str(), directional_light.view_projection_matrices.at(cascade_index));

                        const auto directional_light_depth_map_texture = directional_light.depth_map_textures.at(cascade_index).lock();
                        if (directional_light_depth_map_texture)
                        {
                            directional_light_depth_map_texture->bind(5 + static_cast<unsigned int>(cascade_index));
                        }
                        else
                        {
                            liminal::texture::unbind(5 + static_cast<unsigned int>(cascade_index));
                        }
                    }

                    glBindVertexArray(screen_vao_id);
                    glDrawArrays(GL_TRIANGLES, 0, screen_vertices_size);
                    glBindVertexArray(0);

                    for (std::size_t cascade_index = 0; cascade_index < liminal::directional_light::num_cascades; cascade_index++)
                    {
                        liminal::texture::unbind(5 + static_cast<unsigned int>(cascade_index));
                    }
                }

                liminal::texture::unbind(0);
                liminal::texture::unbind(1);
                liminal::texture::unbind(2);
                liminal::texture::unbind(3);
                liminal::texture::unbind(4);
            }
            liminal::program::unbind();

            deferred_point_program->bind();
            {
                deferred_point_program->set_vec3("camera.position", camera_transform.position);
                deferred_point_program->set_float("light.far_plane", point_light_far_plane);

                geometry_position_texture->bind(0);
                geometry_normal_texture->bind(1);
                geometry_color_texture->bind(2);
                geometry_albedo_texture->bind(3);
                geometry_material_texture->bind(4);

                for (const auto [id, point_light, transform] : scene.get_entities_with<const liminal::point_light, const liminal::transform>().each())
                {
                    deferred_point_program->set_vec3("light.position", transform.position);
                    deferred_point_program->set_vec3("light.color", point_light.color);

                    const auto point_light_depth_cubemap_texture = point_light.depth_cubemap_texture.lock();
                    if (point_light_depth_cubemap_texture)
                    {
                        point_light_depth_cubemap_texture->bind(5);
                    }
                    else
                    {
                        liminal::texture::unbind(5);
                    }

                    glBindVertexArray(screen_vao_id);
                    glDrawArrays(GL_TRIANGLES, 0, screen_vertices_size);
                    glBindVertexArray(0);

                    liminal::cubemap::unbind(5);
                }

                liminal::texture::unbind(0);
                liminal::texture::unbind(1);
                liminal::texture::unbind(2);
                liminal::texture::unbind(3);
                liminal::texture::unbind(4);
            }
            liminal::program::unbind();

            deferred_spot_program->bind();
            {
                deferred_spot_program->set_vec3("camera.position", camera_transform.position);

                geometry_position_texture->bind(0);
                geometry_normal_texture->bind(1);
                geometry_color_texture->bind(2);
                geometry_albedo_texture->bind(3);
                geometry_material_texture->bind(4);

                for (const auto [id, spot_light, transform] : scene.get_entities_with<const liminal::spot_light, const liminal::transform>().each())
                {
                    deferred_spot_program->set_vec3("light.position", transform.position);
                    deferred_spot_program->set_vec3("light.direction", transform.rotation);
                    deferred_spot_program->set_vec3("light.color", spot_light.color);
                    deferred_spot_program->set_float("light.inner_cutoff", glm::cos(glm::radians(spot_light.inner_cutoff)));
                    deferred_spot_program->set_float("light.outer_cutoff", glm::cos(glm::radians(spot_light.outer_cutoff)));
                    deferred_spot_program->set_mat4("light.view_projection_matrix", spot_light.view_projection_matrix);

                    const auto spot_light_depth_map_texture = spot_light.depth_map_texture.lock();
                    if (spot_light_depth_map_texture)
                    {
                        spot_light_depth_map_texture->bind(5);
                    }
                    else
                    {
                        liminal::texture::unbind(5);
                    }

                    glBindVertexArray(screen_vao_id);
                    glDrawArrays(GL_TRIANGLES, 0, screen_vertices_size);
                    glBindVertexArray(0);

                    liminal::texture::unbind(5);
                }

                liminal::texture::unbind(0);
                liminal::texture::unbind(1);
                liminal::texture::unbind(2);
                liminal::texture::unbind(3);
                liminal::texture::unbind(4);
            }
            liminal::program::unbind();

            glDisable(GL_BLEND);
            glBlendFunc(GL_ONE, GL_ZERO);
            glDepthMask(GL_TRUE);
            glDepthFunc(GL_LESS);
        }

        glEnable(GL_DEPTH_TEST);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // copy depth info
    glBindFramebuffer(GL_READ_FRAMEBUFFER, geometry_fbo_id);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo_id);
    glBlitFramebuffer(0, 0, render_width, render_height, 0, 0, width, height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // forward render everything else
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_id);
    {
        glViewport(0, 0, width, height);
        glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);

        // draw skybox
        if (scene.skybox)
        {
            glDepthFunc(GL_LEQUAL);

            glm::mat4 camera_view_no_translate = camera_view;
            camera_view_no_translate[3][0] = 0;
            camera_view_no_translate[3][1] = 0;
            camera_view_no_translate[3][2] = 0;

            skybox_program->bind();
            {
                skybox_program->set_mat4("mvp_matrix", camera_projection * camera_view_no_translate);

                scene.skybox->bind_environment_map(0);

                glBindVertexArray(skybox_vao_id);
                glDrawArrays(GL_TRIANGLES, 0, skybox_vertices_size);
                glBindVertexArray(0);

                liminal::texture::unbind(0);
            }
            liminal::program::unbind();

            glDepthFunc(GL_LESS);
        }

        // DEBUG: draw point lights as a sphere
        {
            glEnable(GL_CLIP_DISTANCE0);

            for (const auto [id, point_light, transform] : scene.get_entities_with<const liminal::point_light, const liminal::transform>().each())
            {
                color_program->bind();
                {
                    auto model_matrix = glm::identity<glm::mat4>();
                    model_matrix = glm::translate(model_matrix, transform.position);
                    model_matrix = glm::scale(model_matrix, {0.25f, 0.25f, 0.25f});

                    color_program->set_mat4("mvp_matrix", camera_projection * camera_view * model_matrix);
                    color_program->set_mat4("model_matrix", model_matrix);
                    color_program->set_vec4("clipping_plane", clipping_plane);
                    color_program->set_vec3("color", point_light.color);

                    DEBUG_sphere_mesh->draw(*color_program);
                }
                liminal::program::unbind();
            }

            glDisable(GL_CLIP_DISTANCE0);
        }

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void liminal::renderer::render_waters(
    liminal::scene &scene,
    const liminal::camera &camera,
    const liminal::transform &camera_transform,
    const std::uint64_t current_time) const
{
    for (const auto [id, transform, water] : scene.get_entities_with<const liminal::transform, const liminal::water>().each())
    {
        // reflection
        glm::vec4 reflection_clipping_plane(0, 1, 0, -transform.position.y);
        if (camera_transform.position.y < transform.position.y) // flip reflection clipping plane if under the water
        {
            reflection_clipping_plane *= -1.0f;
        }
        liminal::transform reflected_transform = camera_transform;
        reflected_transform.position.y -= 2 * (camera_transform.position.y - transform.position.y);
        reflected_transform.rotation.x = -camera_transform.rotation.x;
        reflected_transform.rotation.z = -camera_transform.rotation.z;
        render_objects(scene, camera, reflected_transform, water_reflection_fbo_id, water_reflection_width, water_reflection_height, reflection_clipping_plane);

        // refraction
        glm::vec4 refraction_clipping_plane(0, -1, 0, transform.position.y);
        if (camera_transform.position.y < transform.position.y) // flip refraction clipping plane if under the water
        {
            refraction_clipping_plane *= -1.0f;
        }
        render_objects(scene, camera, camera_transform, water_refraction_fbo_id, water_refraction_width, water_refraction_height, refraction_clipping_plane);

        // draw water meshes
        glBindFramebuffer(GL_FRAMEBUFFER, hdr_fbo_id);
        {
            glViewport(0, 0, render_width, render_height);
            glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            water_program->bind();
            {
                const auto camera_projection = camera.calc_projection(get_aspect_ratio());
                const auto camera_view = camera.calc_view(camera_transform);

                const auto model_matrix = transform.get_model_matrix();

                water_program->set_mat4("mvp_matrix", camera_projection * camera_view * model_matrix);
                water_program->set_mat4("model_matrix", model_matrix);
                water_program->set_float("tiling", water.tiling);
                water_program->set_float("camera.near_plane", liminal::camera::near_plane);
                water_program->set_float("camera.far_plane", liminal::camera::far_plane);
                water_program->set_vec3("camera.position", camera_transform.position);
                auto first_directional_light = scene.get_entity(scene.get_entities_with<const liminal::directional_light>().front());
                if (first_directional_light)
                {
                    // TODO: specular reflections for all lights?
                    // or just support only one directional light in a scene, there is probably no reason to have multiple
                    water_program->set_vec3("light.direction", first_directional_light.get_component<liminal::transform>().rotation);
                    water_program->set_vec3("light.color", first_directional_light.get_component<liminal::directional_light>().color);
                }
                water_program->set_unsigned_int("current_time", static_cast<unsigned int>(current_time));

                water_reflection_color_texture->bind(0);
                water_refraction_color_texture->bind(1);
                water_refraction_depth_texture->bind(2);
                if (water_dudv_texture)
                {
                    water_dudv_texture->bind(3);
                }
                else
                {
                    liminal::texture::unbind(3);
                }
                if (water_normal_texture)
                {
                    water_normal_texture->bind(4);
                }
                else
                {
                    liminal::texture::unbind(4);
                }

                glBindVertexArray(water_vao_id);
                glDrawArrays(GL_TRIANGLES, 0, water_vertices_size);
                glBindVertexArray(0);

                liminal::texture::unbind(0);
                liminal::texture::unbind(1);
                liminal::texture::unbind(2);
                liminal::texture::unbind(3);
                liminal::texture::unbind(4);
            }
            liminal::program::unbind();

            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glDisable(GL_BLEND);
            glBlendFunc(GL_ONE, GL_ZERO);
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}

void liminal::renderer::render_sprites(liminal::scene &scene) const
{
    glBindFramebuffer(GL_FRAMEBUFFER, hdr_fbo_id);
    {
        glViewport(0, 0, target_width, target_height);

        sprite_program->bind();
        {
            const glm::mat4 projection_matrix = glm::ortho(-1, 1, -1, 1, 0, 100);

            for (const auto [id, sprite] : scene.get_entities_with<const liminal::sprite>().each())
            {
                const auto model_matrix = sprite.get_model_matrix();
                sprite_program->set_mat4("mvp_matrix", projection_matrix * model_matrix);
                sprite_program->set_vec3("sprite.color", sprite.color);

                sprite.texture->bind(0);

                glBindVertexArray(sprite_vao_id);
                glDrawArrays(GL_TRIANGLES, 0, sprite_vertices_size);
                glBindVertexArray(0);

                liminal::texture::unbind(0);
            }
        }
        liminal::program::unbind();
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void liminal::renderer::render_screen(const liminal::camera &camera) const
{
    // apply gaussian blur to brightness map
    auto horizontal = true;
    {
        glViewport(0, 0, target_width / 8, target_height / 8);

        gaussian_program->bind();
        {
            bool first_iteration = true;
            for (std::size_t pass = 0; pass < 10; pass++)
            {
                glBindFramebuffer(GL_FRAMEBUFFER, bloom_fbo_ids.at(static_cast<std::size_t>(horizontal)));
                {
                    gaussian_program->set_int("horizontal", horizontal);

                    if (first_iteration)
                    {
                        hdr_textures.at(1)->bind(0);
                    }
                    else
                    {
                        bloom_textures.at(static_cast<std::size_t>(!horizontal))->bind(0);
                    }

                    glBindVertexArray(screen_vao_id);
                    glDrawArrays(GL_TRIANGLES, 0, screen_vertices_size);
                    glBindVertexArray(0);

                    liminal::texture::unbind(0);
                }
                glBindFramebuffer(GL_FRAMEBUFFER, 0);

                horizontal = !horizontal;

                first_iteration = false;
            }
        }
        liminal::program::unbind();
    }

    // final pass
    glBindFramebuffer(GL_FRAMEBUFFER, camera.render_to_texture ? final_fbo_id : 0);
    {
        glViewport(0, 0, target_width, target_height);
        glDisable(GL_DEPTH_TEST);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        postprocess_program->bind();
        {
            postprocess_program->set_unsigned_int("greyscale", greyscale);

            hdr_textures.at(0)->bind(0);
            bloom_textures.at(static_cast<std::size_t>(!horizontal))->bind(1);

            glBindVertexArray(screen_vao_id);
            glDrawArrays(GL_TRIANGLES, 0, screen_vertices_size);
            glBindVertexArray(0);

            liminal::texture::unbind(0);
            liminal::texture::unbind(1);
        }
        liminal::program::unbind();

        glEnable(GL_DEPTH_TEST);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    if (camera.render_to_texture)
    {
        camera.render_texture_id = final_texture->get_texture_id();
    }
}
