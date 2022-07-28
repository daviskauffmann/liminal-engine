#ifndef LIMINAL_GRAPHICS_RENDERER_HPP
#define LIMINAL_GRAPHICS_RENDERER_HPP

#include <GL/glew.h>
#include <array>
#include <glm/matrix.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <imgui.h>
#include <liminal/components/camera.hpp>
#include <liminal/components/directional_light.hpp>
#include <liminal/components/transform.hpp>
#include <liminal/core/scene.hpp>
#include <liminal/graphics/mesh.hpp>
#include <liminal/graphics/model.hpp>
#include <liminal/graphics/program.hpp>
#include <liminal/graphics/texture.hpp>
#include <memory>

namespace liminal
{
    class renderer
    {
    public:
        static constexpr std::size_t num_directional_light_shadows{1};
        static constexpr std::size_t num_point_light_shadows{4};
        static constexpr std::size_t num_spot_light_shadows{1};

        static renderer *instance;

        liminal::camera *default_camera{};
        liminal::transform *default_camera_transform{};

        bool wireframe{false};
        bool greyscale{false};

        renderer(
            const GLsizei target_width, const GLsizei target_height, const float render_scale,
            const GLsizei directional_light_depth_map_size,
            const GLsizei point_light_depth_cubemap_size,
            const GLsizei spot_light_depth_map_size,
            const GLsizei water_reflection_width, const GLsizei water_reflection_height,
            const GLsizei water_refraction_width, const GLsizei water_refraction_height);
        ~renderer();

        float get_aspect_ratio() const;

        void set_target_size(const GLsizei width, const GLsizei height);
        void set_render_scale(const float scale);

        void set_directional_light_depth_map_size(const GLsizei size);
        void set_point_light_depth_cubemap_size(const GLsizei size);
        void set_spot_light_depth_map_size(const GLsizei size);
        void set_reflection_size(const GLsizei width, const GLsizei height);
        void set_refraction_size(const GLsizei width, const GLsizei height);

        void reload_programs();

        liminal::entity pick(const int x, const int y, liminal::scene *scene) const;

        void render(liminal::scene &scene, const unsigned int current_time, const float delta_time) const;

    private:
        GLsizei target_width;
        GLsizei target_height;
        float render_scale;

        GLsizei render_width;
        GLsizei render_height;

        GLuint geometry_fbo_id = 0;
        GLuint geometry_position_texture_id = 0;
        GLuint geometry_normal_texture_id = 0;
        GLuint geometry_albedo_texture_id = 0;
        GLuint geometry_material_texture_id = 0;
        GLuint geometry_id_texture_id = 0;
        GLuint geometry_rbo_id = 0;

        GLuint hdr_fbo_id = 0;
        std::array<GLuint, 2> hdr_texture_ids = {};
        GLuint hdr_rbo_id = 0;

        GLuint final_fbo_id = 0;
        GLuint final_texture_id = 0;

        GLsizei directional_light_depth_map_size;
        std::array<GLuint, num_directional_light_shadows> directional_light_depth_map_fbo_ids = {};
        std::array<std::array<GLuint, liminal::directional_light::num_cascades>, num_directional_light_shadows> directional_light_depth_map_texture_ids = {};

        GLsizei point_light_depth_cubemap_size;
        std::array<GLuint, num_point_light_shadows> point_light_depth_cubemap_fbo_ids = {};
        std::array<GLuint, num_point_light_shadows> point_light_depth_cubemap_texture_ids = {};

        GLsizei spot_light_depth_map_size;
        std::array<GLuint, num_spot_light_shadows> spot_light_depth_map_fbo_ids = {};
        std::array<GLuint, num_spot_light_shadows> spot_light_depth_map_texture_ids = {};

        GLsizei water_reflection_width;
        GLsizei water_reflection_height;
        GLuint water_reflection_fbo_id = 0;
        GLuint water_reflection_color_texture_id = 0;
        GLuint water_reflection_rbo_id = 0;

        GLsizei water_refraction_width;
        GLsizei water_refraction_height;
        GLuint water_refraction_fbo_id = 0;
        GLuint water_refraction_color_texture_id = 0;
        GLuint water_refraction_depth_texture_id = 0;

        std::array<GLuint, 2> bloom_fbo_ids = {};
        std::array<GLuint, 2> bloom_texture_ids = {};

        GLsizei water_vertices_size;
        GLuint water_vao_id;
        GLuint water_vbo_id;

        GLsizei skybox_vertices_size;
        GLuint skybox_vao_id;
        GLuint skybox_vbo_id;

        GLsizei sprite_vertices_size;
        GLuint sprite_vao_id;
        GLuint sprite_vbo_id;

        GLsizei screen_vertices_size;
        GLuint screen_vao_id;
        GLuint screen_vbo_id;

        GLuint brdf_texture_id;

        std::unique_ptr<liminal::program> depth_mesh_program;
        std::unique_ptr<liminal::program> depth_skinned_mesh_program;
        std::unique_ptr<liminal::program> depth_cube_mesh_program;
        std::unique_ptr<liminal::program> depth_cube_skinned_mesh_program;
        std::unique_ptr<liminal::program> color_program;
        std::unique_ptr<liminal::program> geometry_mesh_program;
        std::unique_ptr<liminal::program> geometry_skinned_mesh_program;
        std::unique_ptr<liminal::program> geometry_terrain_program;
        std::unique_ptr<liminal::program> deferred_ambient_program;
        std::unique_ptr<liminal::program> deferred_directional_program;
        std::unique_ptr<liminal::program> deferred_point_program;
        std::unique_ptr<liminal::program> deferred_spot_program;
        std::unique_ptr<liminal::program> skybox_program;
        std::unique_ptr<liminal::program> water_program;
        std::unique_ptr<liminal::program> sprite_program;
        std::unique_ptr<liminal::program> gaussian_program;
        std::unique_ptr<liminal::program> postprocess_program;

        std::unique_ptr<liminal::texture> water_dudv_texture;
        std::unique_ptr<liminal::texture> water_normal_texture;

        std::unique_ptr<liminal::mesh> DEBUG_sphere_mesh;

        void calc_render_size();

        void setup_samplers() const;

        void render_all(
            liminal::scene &scene,
            const liminal::camera &camera,
            const liminal::transform &camera_transform,
            unsigned int current_time) const;
        void render_shadows(
            liminal::scene &scene,
            const liminal::transform &camera_transform) const;
        void render_objects(
            liminal::scene &scene,
            const liminal::camera &camera,
            const liminal::transform &camera_transform,
            GLuint fbo_id,
            GLsizei width, GLsizei height,
            const glm::vec4 &clipping_plane = glm::vec4(0)) const;
        void render_waters(
            liminal::scene &scene,
            const liminal::camera &camera,
            const liminal::transform &camera_transform,
            unsigned int current_time) const;
        void render_sprites(liminal::scene &scene) const;
        void render_screen(const liminal::camera &camera) const;
    };
}

#endif
