#ifndef LIMINAL_CORE_RENDERER_HPP
#define LIMINAL_CORE_RENDERER_HPP

#include "../components/camera.hpp"
#include "../components/directional_light.hpp"
#include "../components/transform.hpp"
#include "../entities/scene.hpp"
#include "../graphics/cubemap.hpp"
#include "../graphics/framebuffer.hpp"
#include "../graphics/mesh.hpp"
#include "../graphics/model.hpp"
#include "../graphics/program.hpp"
#include "../graphics/renderbuffer.hpp"
#include "../graphics/texture.hpp"
#include <array>
#include <gl/glew.h>
#include <glm/matrix.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <imgui.h>
#include <memory>

namespace liminal
{
    class renderer
    {
    public:
        static constexpr std::size_t num_directional_light_shadows = 1;
        static constexpr std::size_t num_point_light_shadows = 4;
        static constexpr std::size_t num_spot_light_shadows = 1;

        liminal::camera *default_camera = nullptr;
        liminal::transform *default_camera_transform = nullptr;

        bool wireframe = false;
        bool greyscale = false;

        renderer(
            GLsizei target_width, GLsizei target_height, float render_scale,
            GLsizei directional_light_depth_map_size,
            GLsizei point_light_depth_cubemap_size,
            GLsizei spot_light_depth_map_size,
            GLsizei water_reflection_width, GLsizei water_reflection_height,
            GLsizei water_refraction_width, GLsizei water_refraction_height);
        ~renderer();
        renderer(const renderer &other) = delete;
        renderer &operator=(const renderer &other) = delete;
        renderer(renderer &&other) = delete;
        renderer &operator=(renderer &&other) = delete;

        float get_aspect_ratio() const;

        void set_target_size(GLsizei width, GLsizei height);
        void set_render_scale(float scale);

        void set_directional_light_depth_map_size(GLsizei size);
        void set_point_light_depth_cubemap_size(GLsizei size);
        void set_spot_light_depth_map_size(GLsizei size);
        void set_reflection_size(GLsizei width, GLsizei height);
        void set_refraction_size(GLsizei width, GLsizei height);

        void reload_programs();

        liminal::entity pick(int x, int y, liminal::scene *scene) const;

        void render(liminal::scene &scene, std::uint64_t current_time, float delta_time) const;

    private:
        GLsizei target_width;
        GLsizei target_height;
        float render_scale;

        GLsizei render_width;
        GLsizei render_height;

        std::unique_ptr<liminal::texture> geometry_position_texture;
        std::unique_ptr<liminal::texture> geometry_normal_texture;
        std::unique_ptr<liminal::texture> geometry_color_texture;
        std::unique_ptr<liminal::texture> geometry_albedo_texture;
        std::unique_ptr<liminal::texture> geometry_material_texture;
        std::unique_ptr<liminal::texture> geometry_id_texture;
        std::unique_ptr<liminal::renderbuffer> geometry_depth_renderbuffer;
        std::unique_ptr<liminal::framebuffer> geometry_framebuffer;

        std::unique_ptr<liminal::texture> hdr_color_texture;
        std::unique_ptr<liminal::texture> hdr_brightness_texture;
        std::unique_ptr<liminal::renderbuffer> hdr_depth_renderbuffer;
        std::unique_ptr<liminal::framebuffer> hdr_framebuffer;

        std::array<std::unique_ptr<liminal::texture>, 2> bloom_textures;
        std::array<std::unique_ptr<liminal::framebuffer>, 2> bloom_framebuffers;

        std::unique_ptr<liminal::texture> final_texture;
        std::unique_ptr<liminal::framebuffer> final_framebuffer;

        std::array<std::shared_ptr<liminal::texture>, num_directional_light_shadows> directional_light_depth_textures;
        std::array<std::unique_ptr<liminal::framebuffer>, num_directional_light_shadows> directional_light_framebuffers;

        std::array<std::unique_ptr<liminal::cubemap>, num_point_light_shadows> point_light_depth_cubemaps;
        std::array<std::unique_ptr<liminal::framebuffer>, num_point_light_shadows> point_light_framebuffers;

        std::array<std::shared_ptr<liminal::texture>, num_spot_light_shadows> spot_light_depth_textures;
        std::array<std::unique_ptr<liminal::framebuffer>, num_spot_light_shadows> spot_light_framebuffers;

        std::unique_ptr<liminal::texture> water_reflection_color_texture;
        std::unique_ptr<liminal::renderbuffer> water_reflection_depth_renderbuffer;
        std::unique_ptr<liminal::framebuffer> water_reflection_framebuffer;

        std::unique_ptr<liminal::texture> water_refraction_color_texture;
        std::unique_ptr<liminal::texture> water_refraction_depth_texture;
        std::unique_ptr<liminal::framebuffer> water_refraction_framebuffer;

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

        std::unique_ptr<liminal::texture> brdf_texture;

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

        void set_render_size(GLsizei width, GLsizei height, float scale);

        void render_all(
            liminal::scene &scene,
            const liminal::camera &camera,
            const liminal::transform &camera_transform,
            std::uint64_t current_time) const;
        void render_shadows(
            liminal::scene &scene,
            const liminal::transform &camera_transform) const;
        void render_geometry(
            liminal::scene &scene,
            const liminal::camera &camera,
            const liminal::transform &camera_transform,
            const liminal::framebuffer &framebuffer,
            const glm::vec4 &clipping_plane = glm::vec4(0),
            bool write_id = false) const;
        void render_waters(
            liminal::scene &scene,
            const liminal::camera &camera,
            const liminal::transform &camera_transform,
            std::uint64_t current_time) const;
        void render_sprites(liminal::scene &scene) const;
        void render_screen(const liminal::camera &camera) const;
    };
}

#endif
