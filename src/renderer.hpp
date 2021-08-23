#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <GL/glew.h>

#include "camera.hpp"
#include "cubemap.hpp"
#include "directional_light.hpp"
#include "mesh.hpp"
#include "model.hpp"
#include "object.hpp"
#include "point_light.hpp"
#include "program.hpp"
#include "scene.hpp"
#include "skybox.hpp"
#include "sound.hpp"
#include "source.hpp"
#include "spot_light.hpp"
#include "sprite.hpp"
#include "terrain.hpp"
#include "texture.hpp"
#include "water.hpp"

namespace liminal
{
    class renderer
    {
    public:
        bool wireframe;
        bool greyscale;
        liminal::camera *camera;
        liminal::skybox *skybox;
        // std::vector<liminal::object *> objects;
        // std::vector<liminal::directional_light *> directional_lights;
        // std::vector<liminal::point_light *> point_lights;
        // std::vector<liminal::spot_light *> spot_lights;
        // std::vector<liminal::water *> waters;
        // std::vector<liminal::terrain *> terrains;
        // std::vector<liminal::sprite *> sprites;

        renderer(
            GLsizei display_width, GLsizei display_height, float render_scale,
            GLsizei reflection_width, GLsizei reflection_height,
            GLsizei refraction_width, GLsizei refraction_height);
        ~renderer();

        void set_screen_size(GLsizei display_width, GLsizei display_height, float render_scale);
        void set_reflection_size(GLsizei reflection_width, GLsizei reflection_height);
        void set_refraction_size(GLsizei refraction_width, GLsizei refraction_height);

        void reload_programs();

        void flush(liminal::scene *scene, unsigned int current_time, float delta_time);

    private:
        GLsizei display_width;
        GLsizei display_height;
        GLsizei render_width;
        GLsizei render_height;
        GLsizei reflection_width;
        GLsizei reflection_height;
        GLsizei refraction_width;
        GLsizei refraction_height;

        GLuint geometry_fbo_id;
        GLuint geometry_position_texture_id;
        GLuint geometry_normal_texture_id;
        GLuint geometry_albedo_texture_id;
        GLuint geometry_material_texture_id;
        GLuint geometry_rbo_id;

        GLuint hdr_fbo_id;
        GLuint hdr_texture_ids[2];
        GLuint hdr_rbo_id;

        GLuint water_reflection_fbo_id;
        GLuint water_reflection_color_texture_id;
        GLuint water_reflection_rbo_id;

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
        liminal::program *screen_program;

        liminal::texture *water_dudv_texture;
        liminal::texture *water_normal_texture;

        liminal::mesh *DEBUG_sphere_mesh;

        void setup_samplers();

        void render_shadows();
        void render_objects(liminal::scene *scene, GLuint fbo_id, GLsizei width, GLsizei height, glm::vec4 clipping_plane = glm::vec4(0.0f));
        void render_waters(unsigned int current_time);
        void render_sprites();
        void render_screen();
    };
} // namespace liminal

#endif
