#ifndef GAME_RENDERER_HPP
#define GAME_RENDERER_HPP

#include <GL/glew.h>

#include "camera.hpp"
#include "cubemap.hpp"
#include "directional_light.hpp"
#include "object.hpp"
#include "point_light.hpp"
#include "program.hpp"
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
    renderer(
        int display_width, int display_height, float render_scale,
        int reflection_width, int reflection_height,
        int refraction_width, int refraction_height);
    ~renderer();

    void set_screen_size(int display_width, int display_height, float render_scale);
    void set_reflection_size(int reflection_width, int reflection_height);
    void set_refraction_size(int refraction_width, int refraction_height);

    void add_object(pk::object *object);
    void add_directional_light(pk::directional_light *directional_light);
    void add_point_light(pk::point_light *point_light);
    void add_spot_light(pk::spot_light *spot_light);
    void add_water(pk::water *water);
    void add_terrain(pk::terrain *terrain);
    void add_sprite(pk::sprite *sprite);
    void flush(pk::camera *camera, unsigned int elapsed_time, float delta_time);

private:
    // settings
    int display_width;
    int display_height;
    int render_width;
    int render_height;
    int reflection_width;
    int reflection_height;
    int refraction_width;
    int refraction_height;

    // framebuffers
    GLuint screen_fbo_id;
    GLuint screen_texture_id;
    GLuint screen_rbo_id;

    GLuint geometry_fbo_id;
    GLuint geometry_position_texture_id;
    GLuint geometry_normal_texture_id;
    GLuint geometry_rbo_id;

    GLuint water_reflection_fbo_id;
    GLuint water_reflection_color_texture_id;
    GLuint water_reflection_rbo_id;

    GLuint water_refraction_fbo_id;
    GLuint water_refraction_color_texture_id;
    GLuint water_refraction_depth_texture_id;
    GLuint water_refraction_rbo_id;

    // water mesh
    GLsizei water_vertices_size;
    GLuint water_vao_id;
    GLuint water_vbo_id;

    // skybox mesh
    GLsizei skybox_vertices_size;
    GLuint skybox_vao_id;
    GLuint skybox_vbo_id;

    // sprite mesh
    GLsizei sprite_vertices_size;
    GLuint sprite_vao_id;
    GLuint sprite_vbo_id;

    // screen mesh
    GLsizei screen_vertices_size;
    GLuint screen_vao_id;
    GLuint screen_vbo_id;

    // textures
    pk::texture *water_dudv_texture;
    pk::texture *water_normal_texture;

    // shader programs
    pk::program *equirectangular_to_cubemap_program;
    pk::program *irradiance_convolution_program;
    pk::program *depth_program;
    pk::program *depth_cube_program;
    pk::program *color_program;
    pk::program *texture_program;
    pk::program *forward_program;
    pk::program *geometry_program;
    pk::program *background_program;
    pk::program *water_program;
    pk::program *terrain_program;
    pk::program *sprite_program;
    pk::program *screen_program;

    // renderables
    std::vector<pk::object *> objects;
    std::vector<pk::directional_light *> directional_lights;
    std::vector<pk::point_light *> point_lights;
    std::vector<pk::spot_light *> spot_lights;
    std::vector<pk::water *> waters;
    std::vector<pk::terrain *> terrains;
    std::vector<pk::sprite *> sprites;

    GLuint environment_cubemap_id;
    GLuint irradiance_cubemap_id;

    void render_scene(GLuint fbo_id, int width, int height, pk::camera *camera, unsigned int elapsed_time, glm::vec4 clipping_plane);
    void render_waters(GLuint fbo_id, pk::camera *camera, unsigned int elapsed_time);
    void render_sprites(GLuint fbo_id);
    void render_screen(GLuint fbo_id);
};
} // namespace pk

#endif
