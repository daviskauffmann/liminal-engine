#ifndef RENDERER_H
#define RENDERER_H

enum render_mode
{
    RENDER_MODE_FORWARD,
    RENDER_MODE_DEFERRED
};

extern struct mesh ENGINE_API *quad_mesh;
extern struct mesh ENGINE_API *cube_mesh;
extern struct texture ENGINE_API *default_texture;
extern struct material ENGINE_API *default_material;

int ENGINE_API renderer_init(int render_width, int render_height, float render_scale, int shadow_width, int shadow_height);
void ENGINE_API renderer_set_mode(enum render_mode render_mode);
void ENGINE_API renderer_set_camera(struct camera *camera);
void ENGINE_API renderer_add_object(struct object *object);
void ENGINE_API renderer_set_directional_light(struct directional_light *directional_light);
void ENGINE_API renderer_add_point_light(struct point_light *point_light);
void ENGINE_API renderer_add_spot_light(struct spot_light *spot_light);
void ENGINE_API renderer_set_skybox(struct cubemap *skybox);
void ENGINE_API renderer_add_water(struct water *water);
void ENGINE_API renderer_add_sprite(struct sprite *sprite);
void ENGINE_API renderer_draw(bool ortho);
void ENGINE_API renderer_quit(void);

#endif
