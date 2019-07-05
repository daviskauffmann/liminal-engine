#ifndef RENDERER_H
#define RENDERER_H

enum render_mode
{
    RENDER_MODE_FORWARD,
    RENDER_MODE_DEFERRED
};

// Initializes the core rendering engine.
int ENGINE_API renderer_init(int render_width, int render_height, float render_scale, int shadow_width, int shadow_height, struct texture *water_dudv_texture);
void ENGINE_API renderer_set_mode(enum render_mode render_mode);
void ENGINE_API renderer_add_object(struct object *object);
void ENGINE_API renderer_set_sun(struct sun *sun);
void ENGINE_API renderer_add_directional_light(struct directional_light *directional_light);
void ENGINE_API renderer_add_point_light(struct point_light *point_light);
void ENGINE_API renderer_add_spot_light(struct spot_light *spot_light);
void ENGINE_API renderer_set_skybox(struct cubemap *skybox);
void ENGINE_API renderer_add_water(struct water *water);
void ENGINE_API renderer_add_sprite(struct sprite *sprite);
void ENGINE_API renderer_draw(struct camera *camera, float aspect);
void ENGINE_API renderer_quit(void);

#endif
