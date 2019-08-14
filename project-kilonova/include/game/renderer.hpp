#ifndef GAME_RENDERER_HPP
#define GAME_RENDERER_HPP

namespace pk
{
    struct camera;
    struct cubemap;
    struct directional_light;
    struct object;
    struct point_light;
    class program;
    struct spot_light;
    struct sprite;
    struct sun;
    struct texture;
    struct water;

    enum render_mode
    {
        RENDER_MODE_FORWARD,
        RENDER_MODE_DEFERRED
    };

    class renderer
    {
    public:
        renderer(int render_width, int render_height, float render_scale, int shadow_width, int shadow_height);
        ~renderer();

        void set_mode(pk::render_mode render_mode);
        void add_object(pk::object *object);
        void set_sun(pk::sun *sun);
        void add_directional_light(pk::directional_light *directional_light);
        void add_point_light(pk::point_light *point_light);
        void add_spot_light(pk::spot_light *spot_light);
        void set_skybox(pk::cubemap *skybox);
        void add_water(pk::water *water);
        void add_sprite(pk::sprite *sprite);
        void draw(pk::camera *camera, float aspect, unsigned int elapsed_time, float delta_time) const;

    private:
        // settings
        int render_width;
        int render_height;
        float render_scale;
        int shadow_width;
        int shadow_height;
        int reflection_width;
        int reflection_height;
        int refraction_width;
        int refraction_height;
        pk::render_mode render_mode;

        // shader programs
        pk::program *depth_program;
        pk::program *forward_color_program;
        pk::program *forward_sun_program;
        pk::program *forward_directional_program;
        pk::program *forward_point_program;
        pk::program *forward_spot_program;
        pk::program *forward_reflection_program;
        pk::program *geometry_program;
        pk::program *deferred_sun_program;
        pk::program *deferred_directional_program;
        pk::program *deferred_point_program;
        pk::program *deferred_spot_program;
        pk::program *skybox_program;
        pk::program *water_program;
        pk::program *sprite_program;
        pk::program *screen_program;

        // framebuffers
        GLuint screen_fbo_id;
        GLuint screen_texture_id;
        GLuint screen_rbo_id;

        GLuint depthmap_fbo_id;
        GLuint depthmap_texture_id;

        GLuint geometry_fbo_id;
        GLuint geometry_position_texture_id;
        GLuint geometry_normal_texture_id;
        GLuint geometry_albedo_specular_texture_id;
        GLuint geometry_rbo_id;

        GLuint water_reflection_fbo_id;
        GLuint water_reflection_color_texture_id;
        GLuint water_reflection_depth_texture_id;
        GLuint water_reflection_depth_rbo_id;

        GLuint water_refraction_fbo_id;
        GLuint water_refraction_color_texture_id;
        GLuint water_refraction_depth_texture_id;
        GLuint water_refraction_depth_rbo_id;

        // water mesh
        unsigned int num_water_vertices;
        GLuint water_vao_id;
        GLuint water_vbo_id;

        // skybox mesh
        unsigned int num_skybox_vertices;
        GLuint skybox_vao_id;
        GLuint skybox_vbo_id;

        // sprite mesh
        unsigned int num_sprite_vertices;
        GLuint sprite_vao_id;
        GLuint sprite_vbo_id;

        // screen mesh
        unsigned int num_screen_vertices;
        GLuint screen_vao_id;
        GLuint screen_vbo_id;

        // textures
        texture *water_dudv_texture;
        texture *water_normal_texture;

        // renderables
        std::vector<pk::object *> objects;
        pk::sun *sun;
        std::vector<pk::directional_light *> directional_lights;
        std::vector<pk::point_light *> point_lights;
        std::vector<pk::spot_light *> spot_lights;
        pk::cubemap *skybox;
        std::vector<pk::water *> waters;
        std::vector<pk::sprite *> sprites;

        void render_scene(GLuint fbo_id, struct camera *camera, float aspect, unsigned int elapsed_time, vec4 clipping_plane);
    };
}

#endif
