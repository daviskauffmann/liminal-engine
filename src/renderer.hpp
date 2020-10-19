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
#include "skybox.hpp"
#include "sound.hpp"
#include "source.hpp"
#include "spot_light.hpp"
#include "sprite.hpp"
#include "terrain.hpp"
#include "texture.hpp"
#include "water.hpp"

namespace pk
{
    class renderer
    {
    public:
        bool wireframe;
        bool greyscale;
        pk::camera *camera;
        pk::skybox *skybox;
        std::vector<pk::object *> objects;
        std::vector<pk::directional_light *> directional_lights;
        std::vector<pk::point_light *> point_lights;
        std::vector<pk::spot_light *> spot_lights;
        std::vector<pk::water *> waters;
        std::vector<pk::terrain *> terrains;
        std::vector<pk::sprite *> sprites;

        renderer(
            int display_width, int display_height, float render_scale,
            int reflection_width, int reflection_height,
            int refraction_width, int refraction_height);
        ~renderer();

        void set_screen_size(int display_width, int display_height, float render_scale);
        void set_reflection_size(int reflection_width, int reflection_height);
        void set_refraction_size(int refraction_width, int refraction_height);

        void reload_programs();

        void flush(unsigned int current_time, float delta_time);

    private:
        int display_width;
        int display_height;
        int render_width;
        int render_height;
        int reflection_width;
        int reflection_height;
        int refraction_width;
        int refraction_height;

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

        pk::mesh *sphere_mesh;

        pk::texture *water_dudv_texture;
        pk::texture *water_normal_texture;

        pk::program *depth_program;
        pk::program *depth_cube_program;
        pk::program *color_program;
        pk::program *geometry_object_program;
        pk::program *geometry_terrain_program;
        pk::program *deferred_ambient_program;
        pk::program *deferred_directional_program;
        pk::program *deferred_point_program;
        pk::program *deferred_spot_program;
        pk::program *skybox_program;
        pk::program *water_program;
        pk::program *sprite_program;
        pk::program *gaussian_program;
        pk::program *screen_program;

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

        GLuint brdf_texture_id;

        void render_shadows();
        void render_objects(unsigned int current_time, GLuint fbo_id, int width, int height, glm::vec4 clipping_plane = glm::vec4(0.0f));
        void render_waters(unsigned int current_time);
        void render_sprites();
        void render_screen();
    };
} // namespace pk

#endif
