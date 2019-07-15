#ifndef ENGINE_RENDERER_H
#define ENGINE_RENDERER_H

enum render_mode
{
    RENDER_MODE_FORWARD,
    RENDER_MODE_DEFERRED
};

int renderer_init(int render_width, int render_height, float render_scale, int shadow_width, int shadow_height);
void renderer_set_mode(enum render_mode render_mode);
void renderer_add_object(struct object *object);
void renderer_set_sun(struct sun *sun);
void renderer_add_directional_light(struct directional_light *directional_light);
void renderer_add_point_light(struct point_light *point_light);
void renderer_add_spot_light(struct spot_light *spot_light);
void renderer_set_skybox(struct cubemap *skybox);
void renderer_add_water(struct water *water);
void renderer_add_sprite(struct sprite *sprite);
void renderer_draw(struct camera *camera, float aspect, unsigned int elapsed_time, float delta_time);
void renderer_quit(void);

#endif
