#ifndef LIMINAL_GRAPHICS_RENDERER_HPP
#define LIMINAL_GRAPHICS_RENDERER_HPP

#include <GL/glew.h>
#include <glm/matrix.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <liminal/core/scene.hpp>
#include <liminal/graphics/mesh.hpp>
#include <liminal/graphics/model.hpp>
#include <liminal/graphics/program.hpp>
#include <liminal/graphics/texture.hpp>

#define NUM_DIRECTIONAL_LIGHT_SHADOWS 1
#define NUM_POINT_LIGHT_SHADOWS 4
#define NUM_SPOT_LIGHT_SHADOWS 1

namespace liminal
{
    class renderer
    {
    public:
        bool wireframe;
        bool greyscale;

        renderer(
            GLsizei display_width, GLsizei display_height, float render_scale,
            GLsizei directional_light_depth_map_size,
            GLsizei point_light_depth_cubemap_size,
            GLsizei spot_light_depth_map_size,
            GLsizei water_reflection_width, GLsizei water_reflection_height,
            GLsizei water_refraction_width, GLsizei water_refraction_height);
        ~renderer();

        void set_screen_size(GLsizei display_width, GLsizei display_height, float render_scale);
        void set_directional_light_depth_map_size(GLsizei directional_light_depth_map_size);
        void set_point_light_depth_cubemap_size(GLsizei point_light_depth_cubemap_size);
        void set_spot_light_depth_map_size(GLsizei spot_light_depth_map_size);
        void set_reflection_size(GLsizei water_reflection_width, GLsizei water_reflection_height);
        void set_refraction_size(GLsizei water_refraction_width, GLsizei water_refraction_height);

        void reload_programs();

        void render(liminal::scene &scene, unsigned int current_time, float delta_time);

    private:
        GLsizei display_width;
        GLsizei display_height;
        GLsizei render_width;
        GLsizei render_height;

        GLuint geometry_fbo_id;
        GLuint geometry_position_texture_id;
        GLuint geometry_normal_texture_id;
        GLuint geometry_albedo_texture_id;
        GLuint geometry_material_texture_id;
        GLuint geometry_rbo_id;

        GLuint hdr_fbo_id;
        GLuint hdr_texture_ids[2];
        GLuint hdr_rbo_id;

        GLuint final_fbo_id;
        GLuint final_texture_id;

        GLsizei directional_light_depth_map_size;
        GLuint directional_light_depth_map_fbo_ids[NUM_DIRECTIONAL_LIGHT_SHADOWS];
        GLuint directional_light_depth_map_texture_ids[NUM_DIRECTIONAL_LIGHT_SHADOWS];
        glm::mat4 directional_light_transformation_matrices[NUM_DIRECTIONAL_LIGHT_SHADOWS];

        GLsizei point_light_depth_cubemap_size;
        GLuint point_light_depth_cubemap_fbo_ids[NUM_POINT_LIGHT_SHADOWS];
        GLuint point_light_depth_cubemap_texture_ids[NUM_POINT_LIGHT_SHADOWS];
        std::vector<glm::mat4> point_light_transformation_matrices[NUM_POINT_LIGHT_SHADOWS];

        GLsizei spot_light_depth_map_size;
        GLuint spot_light_depth_map_fbo_ids[NUM_SPOT_LIGHT_SHADOWS];
        GLuint spot_light_depth_map_texture_ids[NUM_SPOT_LIGHT_SHADOWS];
        glm::mat4 spot_light_transformation_matrices[NUM_SPOT_LIGHT_SHADOWS];

        GLsizei water_reflection_width;
        GLsizei water_reflection_height;
        GLuint water_reflection_fbo_id;
        GLuint water_reflection_color_texture_id;
        GLuint water_reflection_rbo_id;

        GLsizei water_refraction_width;
        GLsizei water_refraction_height;
        GLuint water_refraction_fbo_id;
        GLuint water_refraction_color_texture_id;
        GLuint water_refraction_depth_texture_id;

        GLuint bloom_fbo_ids[2];
        GLuint bloom_texture_ids[2];

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

        liminal::program *depth_mesh_program;
        liminal::program *depth_skinned_mesh_program;
        liminal::program *depth_cube_mesh_program;
        liminal::program *depth_cube_skinned_mesh_program;
        liminal::program *color_program;
        liminal::program *geometry_mesh_program;
        liminal::program *geometry_skinned_mesh_program;
        liminal::program *geometry_terrain_program;
        liminal::program *deferred_ambient_program;
        liminal::program *deferred_directional_program;
        liminal::program *deferred_point_program;
        liminal::program *deferred_spot_program;
        liminal::program *skybox_program;
        liminal::program *water_program;
        liminal::program *sprite_program;
        liminal::program *gaussian_program;
        liminal::program *postprocess_program;

        liminal::texture *water_dudv_texture;
        liminal::texture *water_normal_texture;

        liminal::mesh *DEBUG_sphere_mesh;

        void setup_samplers();

        void render_shadows(liminal::scene &scene);
        void render_objects(liminal::scene &scene, GLuint fbo_id, GLsizei width, GLsizei height, glm::vec4 clipping_plane = glm::vec4(0.0f));
        void render_waters(liminal::scene &scene, unsigned int current_time);
        void render_sprites(liminal::scene &scene);
        void render_screen(liminal::scene &scene);
    };
}

#endif
