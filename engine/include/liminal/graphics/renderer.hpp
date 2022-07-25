#ifndef LIMINAL_GRAPHICS_RENDERER_HPP
#define LIMINAL_GRAPHICS_RENDERER_HPP

#include <GL/glew.h>
#include <glm/matrix.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <imgui.h>
#include <liminal/components/camera.hpp>
#include <liminal/components/transform.hpp>
#include <liminal/core/scene.hpp>
#include <liminal/graphics/mesh.hpp>
#include <liminal/graphics/model.hpp>
#include <liminal/graphics/program.hpp>
#include <liminal/graphics/texture.hpp>
#include <memory>

#define NUM_DIRECTIONAL_LIGHT_SHADOWS 1
#define NUM_POINT_LIGHT_SHADOWS 4
#define NUM_SPOT_LIGHT_SHADOWS 1

namespace liminal
{
    class renderer
    {
    public:
        static renderer *instance;

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

        void render(liminal::scene &scene, unsigned int current_time, float delta_time) const;

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
        GLuint hdr_texture_ids[2] = {};
        GLuint hdr_rbo_id = 0;

        GLuint final_fbo_id = 0;
        GLuint final_texture_id = 0;

        GLsizei directional_light_depth_map_size;
        GLuint directional_light_depth_map_fbo_ids[NUM_DIRECTIONAL_LIGHT_SHADOWS] = {};
        GLuint directional_light_depth_map_texture_ids[NUM_DIRECTIONAL_LIGHT_SHADOWS] = {};
        mutable glm::mat4 directional_light_transformation_matrices[NUM_DIRECTIONAL_LIGHT_SHADOWS] = {};

        GLsizei point_light_depth_cubemap_size;
        GLuint point_light_depth_cubemap_fbo_ids[NUM_POINT_LIGHT_SHADOWS] = {};
        GLuint point_light_depth_cubemap_texture_ids[NUM_POINT_LIGHT_SHADOWS] = {};
        mutable glm::mat4 point_light_transformation_matrices[NUM_POINT_LIGHT_SHADOWS][6] = {};

        GLsizei spot_light_depth_map_size;
        GLuint spot_light_depth_map_fbo_ids[NUM_SPOT_LIGHT_SHADOWS] = {};
        GLuint spot_light_depth_map_texture_ids[NUM_SPOT_LIGHT_SHADOWS] = {};
        mutable glm::mat4 spot_light_transformation_matrices[NUM_SPOT_LIGHT_SHADOWS] = {};

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

        GLuint bloom_fbo_ids[2] = {};
        GLuint bloom_texture_ids[2] = {};

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
