#include <engine/engine.h>

#define WINDOW_TITLE "Example Game"
#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

#define RENDER_SCALE 1.0f

#define SHADOW_WIDTH 4096
#define SHADOW_HEIGHT 4096

#define FPS_CAP 120

enum lighting
{
    LIGHTING_FORWARD,
    LIGHTING_DEFERRED
};

int main(int argc, char *argv[])
{
    // setup engine
    if (core_init())
    {
        return 1;
    }

    if (window_init(WINDOW_TITLE, WINDOW_WIDTH, WINDOW_HEIGHT))
    {
        return 1;
    }

    if (audio_init(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 1024))
    {
        return 1;
    }

    if (image_init())
    {
        return 1;
    }

    time_cap_fps(FPS_CAP);

    // window_set_fullscreen(SDL_WINDOW_FULLSCREEN);

    SDL_SetRelativeMouseMode(true);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    // glEnable(GL_CULL_FACE);
    // glCullFace(GL_BACK);
    // glFrontFace(GL_CW);

    // create programs
    struct program *depth_program = program_create(
        "assets/shaders/depth.vert",
        "assets/shaders/depth.frag");

    if (!depth_program)
    {
        return 1;
    }

    struct program *forward_scene_program = program_create(
        "assets/shaders/forward.vert",
        "assets/shaders/forward_scene.frag");

    if (!forward_scene_program)
    {
        return 1;
    }

    struct program *forward_directional_program = program_create(
        "assets/shaders/forward.vert",
        "assets/shaders/forward_directional.frag");

    if (!forward_directional_program)
    {
        return 1;
    }

    struct program *forward_point_program = program_create(
        "assets/shaders/forward.vert",
        "assets/shaders/forward_point.frag");

    if (!forward_point_program)
    {
        return 1;
    }

    struct program *forward_spot_program = program_create(
        "assets/shaders/forward.vert",
        "assets/shaders/forward_spot.frag");

    if (!forward_spot_program)
    {
        return 1;
    }

    struct program *geometry_program = program_create(
        "assets/shaders/geometry.vert",
        "assets/shaders/geometry.frag");

    if (!geometry_program)
    {
        return 1;
    }

    struct program *deferred_scene_program = program_create(
        "assets/shaders/deferred.vert",
        "assets/shaders/deferred_scene.frag");

    if (!deferred_scene_program)
    {
        return 1;
    }

    struct program *deferred_directional_program = program_create(
        "assets/shaders/deferred.vert",
        "assets/shaders/deferred_directional.frag");

    if (!deferred_directional_program)
    {
        return 1;
    }

    struct program *deferred_point_program = program_create(
        "assets/shaders/deferred.vert",
        "assets/shaders/deferred_point.frag");

    if (!deferred_point_program)
    {
        return 1;
    }

    struct program *deferred_spot_program = program_create(
        "assets/shaders/deferred.vert",
        "assets/shaders/deferred_spot.frag");

    if (!deferred_spot_program)
    {
        return 1;
    }

    struct program *post_program = program_create(
        "assets/shaders/post.vert",
        "assets/shaders/post.frag");

    if (!post_program)
    {
        return 1;
    }

    // cache shader uniforms
    GLint depth_program_scene_sun_projection = program_get_location(depth_program, "scene.sun_projection");
    GLint depth_program_scene_sun_view = program_get_location(depth_program, "scene.sun_view");
    GLint depth_program_object_model = program_get_location(depth_program, "object.model");

    GLint forward_scene_program_camera_projection = program_get_location(forward_scene_program, "camera.projection");
    GLint forward_scene_program_camera_view = program_get_location(forward_scene_program, "camera.view");
    GLint forward_scene_program_camera_position = program_get_location(forward_scene_program, "camera.position");
    GLint forward_scene_program_object_model = program_get_location(forward_scene_program, "object.model");
    GLint forward_scene_program_material_color = program_get_location(forward_scene_program, "material.color");
    GLint forward_scene_program_material_diffuse = program_get_location(forward_scene_program, "material.diffuse");
    GLint forward_scene_program_material_specular = program_get_location(forward_scene_program, "material.specular");
    GLint forward_scene_program_material_shininess = program_get_location(forward_scene_program, "material.shininess");
    GLint forward_scene_program_material_normal = program_get_location(forward_scene_program, "material.normal");
    GLint forward_scene_program_material_emission = program_get_location(forward_scene_program, "material.emission");
    GLint forward_scene_program_material_glow = program_get_location(forward_scene_program, "material.glow");
    GLint forward_scene_program_scene_sun_direction = program_get_location(forward_scene_program, "scene.sun_direction");
    GLint forward_scene_program_scene_sun_ambient = program_get_location(forward_scene_program, "scene.sun_ambient");
    GLint forward_scene_program_scene_sun_diffuse = program_get_location(forward_scene_program, "scene.sun_diffuse");
    GLint forward_scene_program_scene_sun_specular = program_get_location(forward_scene_program, "scene.sun_specular");
    GLint forward_scene_program_scene_sun_projection = program_get_location(forward_scene_program, "scene.sun_projection");
    GLint forward_scene_program_scene_sun_view = program_get_location(forward_scene_program, "scene.sun_view");
    GLint forward_scene_program_depthmap_texture = program_get_location(forward_scene_program, "depthmap.texture");

    GLint forward_point_program_camera_projection = program_get_location(forward_point_program, "camera.projection");
    GLint forward_point_program_camera_view = program_get_location(forward_point_program, "camera.view");
    GLint forward_point_program_camera_position = program_get_location(forward_point_program, "camera.position");
    GLint forward_point_program_object_model = program_get_location(forward_point_program, "object.model");
    GLint forward_point_program_material_color = program_get_location(forward_point_program, "material.color");
    GLint forward_point_program_material_diffuse = program_get_location(forward_point_program, "material.diffuse");
    GLint forward_point_program_material_specular = program_get_location(forward_point_program, "material.specular");
    GLint forward_point_program_material_shininess = program_get_location(forward_point_program, "material.shininess");
    GLint forward_point_program_material_normal = program_get_location(forward_point_program, "material.normal");
    GLint forward_point_program_material_emission = program_get_location(forward_point_program, "material.emission");
    GLint forward_point_program_material_glow = program_get_location(forward_point_program, "material.glow");
    GLint forward_point_program_point_light_position = program_get_location(forward_point_program, "point_light.position");
    GLint forward_point_program_point_light_ambient = program_get_location(forward_point_program, "point_light.ambient");
    GLint forward_point_program_point_light_diffuse = program_get_location(forward_point_program, "point_light.diffuse");
    GLint forward_point_program_point_light_specular = program_get_location(forward_point_program, "point_light.specular");
    GLint forward_point_program_point_light_attenuation = program_get_location(forward_point_program, "point_light.attenuation");

    GLint geometry_program_camera_projection = program_get_location(geometry_program, "camera.projection");
    GLint geometry_program_camera_view = program_get_location(geometry_program, "camera.view");
    GLint geometry_program_camera_position = program_get_location(geometry_program, "camera.position");
    GLint geometry_program_object_model = program_get_location(geometry_program, "object.model");
    GLint geometry_program_material_color = program_get_location(geometry_program, "material.color");
    GLint geometry_program_material_diffuse = program_get_location(geometry_program, "material.diffuse");
    GLint geometry_program_material_specular = program_get_location(geometry_program, "material.specular");
    GLint geometry_program_material_shininess = program_get_location(geometry_program, "material.shininess");
    GLint geometry_program_material_normal = program_get_location(geometry_program, "material.normal");
    GLint geometry_program_material_emission = program_get_location(geometry_program, "material.emission");
    GLint geometry_program_material_glow = program_get_location(geometry_program, "material.glow");

    GLint deferred_scene_program_geometry_position = program_get_location(deferred_scene_program, "geometry.position");
    GLint deferred_scene_program_geometry_normal = program_get_location(deferred_scene_program, "geometry.normal");
    GLint deferred_scene_program_geometry_albedo_specular = program_get_location(deferred_scene_program, "geometry.albedo_specular");
    GLint deferred_scene_program_camera_position = program_get_location(deferred_scene_program, "camera.position");
    GLint deferred_scene_program_scene_sun_direction = program_get_location(deferred_scene_program, "scene.sun_direction");
    GLint deferred_scene_program_scene_sun_ambient = program_get_location(deferred_scene_program, "scene.sun_ambient");
    GLint deferred_scene_program_scene_sun_diffuse = program_get_location(deferred_scene_program, "scene.sun_diffuse");
    GLint deferred_scene_program_scene_sun_specular = program_get_location(deferred_scene_program, "scene.sun_specular");
    GLint deferred_scene_program_scene_sun_projection = program_get_location(deferred_scene_program, "scene.sun_projection");
    GLint deferred_scene_program_scene_sun_view = program_get_location(deferred_scene_program, "scene.sun_view");
    GLint deferred_scene_program_depthmap_texture = program_get_location(deferred_scene_program, "depthmap.texture");

    GLint deferred_point_program_geometry_position = program_get_location(deferred_point_program, "geometry.position");
    GLint deferred_point_program_geometry_normal = program_get_location(deferred_point_program, "geometry.normal");
    GLint deferred_point_program_geometry_albedo_specular = program_get_location(deferred_point_program, "geometry.albedo_specular");
    GLint deferred_point_program_camera_position = program_get_location(deferred_point_program, "camera.position");
    GLint deferred_point_program_point_light_position = program_get_location(deferred_point_program, "point_light.position");
    GLint deferred_point_program_point_light_ambient = program_get_location(deferred_point_program, "point_light.ambient");
    GLint deferred_point_program_point_light_diffuse = program_get_location(deferred_point_program, "point_light.diffuse");
    GLint deferred_point_program_point_light_specular = program_get_location(deferred_point_program, "point_light.specular");
    GLint deferred_point_program_point_light_attenuation = program_get_location(deferred_point_program, "point_light.attenuation");

    GLint post_program_screen_texture = program_get_location(post_program, "screen.texture");

    // setup shader samplers
    program_bind(forward_scene_program);
    program_set_int(forward_scene_program_material_diffuse, 0);
    program_set_int(forward_scene_program_material_specular, 1);
    program_set_int(forward_scene_program_material_normal, 2);
    program_set_int(forward_scene_program_material_emission, 3);
    program_set_int(forward_scene_program_depthmap_texture, 4);
    program_unbind();

    program_bind(forward_point_program);
    program_set_int(forward_point_program_material_diffuse, 0);
    program_set_int(forward_point_program_material_specular, 1);
    program_set_int(forward_point_program_material_normal, 2);
    program_set_int(forward_point_program_material_emission, 3);
    program_unbind();

    program_bind(geometry_program);
    program_set_int(geometry_program_material_diffuse, 0);
    program_set_int(geometry_program_material_specular, 1);
    program_set_int(geometry_program_material_normal, 2);
    program_set_int(geometry_program_material_emission, 3);
    program_unbind();

    program_bind(deferred_scene_program);
    program_set_int(deferred_scene_program_geometry_position, 0);
    program_set_int(deferred_scene_program_geometry_normal, 1);
    program_set_int(deferred_scene_program_geometry_albedo_specular, 2);
    program_set_int(deferred_scene_program_depthmap_texture, 3);
    program_unbind();

    program_bind(deferred_point_program);
    program_set_int(deferred_point_program_geometry_position, 0);
    program_set_int(deferred_point_program_geometry_normal, 1);
    program_set_int(deferred_point_program_geometry_albedo_specular, 2);
    program_unbind();

    program_bind(post_program);
    program_set_int(post_program_screen_texture, 0);
    program_unbind();

    // create meshes
    float quad_vertices[] = {
        // position          // normal            // uv
        +1.0f, +1.0f, +0.0f, +0.0f, +1.0f, +0.0f, 1.0f, 1.0f, // top right
        +1.0f, -1.0f, +0.0f, +0.0f, +1.0f, +0.0f, 1.0f, 0.0f, // bottom right
        -1.0f, -1.0f, +0.0f, +0.0f, +1.0f, +0.0f, 0.0f, 0.0f, // bottom left
        -1.0f, +1.0f, +0.0f, +0.0f, +1.0f, +0.0f, 0.0f, 1.0f  // top left
    };

    unsigned int quad_indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

    struct mesh *quad_mesh = mesh_create(
        quad_vertices,
        sizeof(quad_vertices),
        quad_indices,
        sizeof(quad_indices));

    if (!quad_mesh)
    {
        return 1;
    }

    float cube_vertices[] = {
        // position          // normal            // uv
        // back face
        -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
         1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
         1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
         1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
        -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
        -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
        // front face
        -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
         1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
         1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
         1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
        -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
        -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
        // left face
        -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
        -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
        -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
        -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
        -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
        -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
        // right face
         1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
         1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
         1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
         1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
         1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
         1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
        // bottom face
        -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
         1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
         1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
         1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
        -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
        -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
        // top face
        -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
         1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
         1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
         1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
        -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
        -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left   
    };

    unsigned int cube_indices[] = {
        0, 1, 2,
        3, 4, 5,
        6, 7, 8,
        9, 10, 11,
        12, 13, 14,
        15, 16, 17,
        18, 19, 20,
        21, 22, 23,
        24, 25, 26,
        27, 28, 29,
        30, 31, 32,
        33, 34, 35 };

    struct mesh *cube_mesh = mesh_create(
        cube_vertices,
        sizeof(cube_vertices),
        cube_indices,
        sizeof(cube_indices));

    if (!cube_mesh)
    {
        return 1;
    }

    // create textures
    struct texture *box_diffuse_texture = texture_create("assets/images/box_diffuse.png");

    if (!box_diffuse_texture)
    {
        return 1;
    }

    struct texture *box_specular_texture = texture_create("assets/images/box_specular.png");

    if (!box_specular_texture)
    {
        return 1;
    }

    struct texture *cobble_diffuse_texture = texture_create("assets/images/cobble_diffuse.jpg");

    if (!cobble_diffuse_texture)
    {
        return 1;
    }

    struct texture *cobble_specular_texture = texture_create("assets/images/cobble_specular.jpg");

    if (!cobble_specular_texture)
    {
        return 1;
    }

    // create materials
    vec3 box_material_color = { 1.0f, 1.0f, 1.0f };

    struct material *box_material = material_create(
        box_material_color,
        box_diffuse_texture,
        box_specular_texture,
        16.0f,
        NULL,
        NULL,
        1.0f);

    if (!box_material)
    {
        return 1;
    }

    vec3 cobble_material_color = { 1.0f, 1.0f, 1.0f };

    struct material *cobble_material = material_create(
        cobble_material_color,
        cobble_diffuse_texture,
        cobble_specular_texture,
        16.0f,
        NULL,
        NULL,
        1.0f);

    if (!cobble_material)
    {
        return 1;
    }

    // create scene
    vec3 scene_sun_direction = { -0.2f, -1.0f, -0.3f };
    vec3 scene_sun_ambient = { 0.1f, 0.1f, 0.1f };
    vec3 scene_sun_diffuse = { 0.8f, 0.8f, 0.8f };
    vec3 scene_sun_specular = { 1.0f, 1.0f, 1.0f };

    struct scene *scene = scene_create(
        scene_sun_direction,
        scene_sun_ambient,
        scene_sun_diffuse,
        scene_sun_specular);

    if (!scene)
    {
        return 1;
    }

    // create objects
    vec3 floor_object_position = { 0.0f, -2.0f, 0.0f };
    vec3 floor_object_rotation = { 0.0f, 0.0f, 0.0f };
    vec3 floor_object_scale = { 10.0f, 1.0f, 10.0f };

    struct object *floor_object = object_create(
        cube_mesh,
        cobble_material,
        floor_object_position,
        floor_object_rotation,
        floor_object_scale);

    if (!floor_object)
    {
        return 1;
    }

    vec3 box_1_object_position = { 0.0f, 1.0f, 0.0f };
    vec3 box_1_object_rotation = { 0.0f, 0.0f, 0.0f };
    vec3 box_1_object_scale = { 1.0f, 1.0f, 1.0f };

    struct object *box_1_object = object_create(
        cube_mesh,
        box_material,
        box_1_object_position,
        box_1_object_rotation,
        box_1_object_scale);

    if (!box_1_object)
    {
        return 1;
    }

    vec3 box_2_object_position = { 2.0f, -0.5f, 0.0f };
    vec3 box_2_object_rotation = { 0.0f, 0.0f, 0.0f };
    vec3 box_2_object_scale = { 0.5f, 0.5f, 0.5f };

    struct object *box_2_object = object_create(
        cube_mesh,
        box_material,
        box_2_object_position,
        box_2_object_rotation,
        box_2_object_scale);

    if (!box_2_object)
    {
        return 1;
    }

    vec3 box_3_object_position = { 0.0f, -0.5f, 2.0f };
    vec3 box_3_object_rotation = { 0.0f, 0.0f, 0.0f };
    vec3 box_3_object_scale = { 0.5f, 0.5f, 0.5f };

    struct object *box_3_object = object_create(
        cube_mesh,
        box_material,
        box_3_object_position,
        box_3_object_rotation,
        box_3_object_scale);

    if (!box_3_object)
    {
        return 1;
    }

    vec3 box_4_object_position = { -2.0f, -0.5f, 0.0f };
    vec3 box_4_object_rotation = { 0.0f, 0.0f, 0.0f };
    vec3 box_4_object_scale = { 0.5f, 0.5f, 0.5f };

    struct object *box_4_object = object_create(
        cube_mesh,
        box_material,
        box_4_object_position,
        box_4_object_rotation,
        box_4_object_scale);

    if (!box_4_object)
    {
        return 1;
    }

    vec3 box_5_object_position = { 0.0f, -0.5f, -2.0f };
    vec3 box_5_object_rotation = { 0.0f, 0.0f, 0.0f };
    vec3 box_5_object_scale = { 0.5f, 0.5f, 0.5f };

    struct object *box_5_object = object_create(
        cube_mesh,
        box_material,
        box_5_object_position,
        box_5_object_rotation,
        box_5_object_scale);

    if (!box_5_object)
    {
        return 1;
    }

    struct object *objects[] = {
        floor_object,
        box_1_object,
        box_2_object,
        box_3_object,
        box_4_object,
        box_5_object
    };
    const unsigned int num_objects = sizeof(objects) / sizeof(struct object *);

    // create lights
    vec3 directional_light_direction = { 0.0f, 0.0f, 0.0f };
    vec3 directional_light_ambient = { 0.0f, 0.0f, 0.0f };
    vec3 directional_light_diffuse = { 0.0f, 0.0f, 0.0f };
    vec3 directional_light_specular = { 0.0f, 0.0f, 0.0f };

    struct directional_light *directional_light = directional_light_create(
        directional_light_direction,
        directional_light_ambient,
        directional_light_diffuse,
        directional_light_specular);

    if (!directional_light)
    {
        return 1;
    }

    struct directional_light *directional_lights[] = {
        directional_light
    };
    const unsigned int num_directional_lights = sizeof(directional_lights) / sizeof(struct directional_light *);

    vec3 red_point_light_position = { 2.0f, 0.0f, 2.0f };
    vec3 red_point_light_ambient = { 0.1f, 0.0f, 0.0f };
    vec3 red_point_light_diffuse = { 0.8f, 0.0f, 0.0f };
    vec3 red_point_light_specular = { 1.0f, 0.0f, 0.0f };
    vec3 red_point_light_attenuation = { 1.0f, 0.09f, 0.32f };

    struct point_light *red_point_light = point_light_create(
        red_point_light_position,
        red_point_light_ambient,
        red_point_light_diffuse,
        red_point_light_specular,
        red_point_light_attenuation);

    if (!red_point_light)
    {
        return 1;
    }

    vec3 yellow_point_light_position = { -2.0f, 0.0f, -2.0f };
    vec3 yellow_point_light_ambient = { 0.1f, 0.1f, 0.0f };
    vec3 yellow_point_light_diffuse = { 0.8f, 0.8f, 0.0f };
    vec3 yellow_point_light_specular = { 1.0f, 1.0f, 0.0f };
    vec3 yellow_point_light_attenuation = { 1.0f, 0.09f, 0.32f };

    struct point_light *yellow_point_light = point_light_create(
        yellow_point_light_position,
        yellow_point_light_ambient,
        yellow_point_light_diffuse,
        yellow_point_light_specular,
        yellow_point_light_attenuation);

    if (!yellow_point_light)
    {
        return 1;
    }

    vec3 green_point_light_position = { 2.0f, 0.0f, -2.0f };
    vec3 green_point_light_ambient = { 0.0f, 0.1f, 0.0f };
    vec3 green_point_light_diffuse = { 0.0f, 0.8f, 0.0f };
    vec3 green_point_light_specular = { 0.0f, 1.0f, 0.0f };
    vec3 green_point_light_attenuation = { 1.0f, 0.09f, 0.32f };

    struct point_light *green_point_light = point_light_create(
        green_point_light_position,
        green_point_light_ambient,
        green_point_light_diffuse,
        green_point_light_specular,
        green_point_light_attenuation);

    if (!green_point_light)
    {
        return 1;
    }

    vec3 blue_point_light_position = { -2.0f, 0.0f, 2.0f };
    vec3 blue_point_light_ambient = { 0.0f, 0.0f, 0.1f };
    vec3 blue_point_light_diffuse = { 0.0f, 0.0f, 0.8f };
    vec3 blue_point_light_specular = { 0.0f, 0.0f, 1.0f };
    vec3 blue_point_light_attenuation = { 1.0f, 0.09f, 0.32f };

    struct point_light *blue_point_light = point_light_create(
        blue_point_light_position,
        blue_point_light_ambient,
        blue_point_light_diffuse,
        blue_point_light_specular,
        blue_point_light_attenuation);

    if (!blue_point_light)
    {
        return 1;
    }

    struct point_light *point_lights[] = {
        red_point_light,
        yellow_point_light,
        green_point_light,
        blue_point_light };
    const unsigned int num_point_lights = sizeof(point_lights) / sizeof(struct point_light *);

    vec3 spot_light_position = { 0.0f, 0.0f, 0.0f };
    vec3 spot_light_direction = { 0.0f, 0.0f, 0.0f };
    vec3 spot_light_ambient = { 0.0f, 0.0f, 0.0f };
    vec3 spot_light_diffuse = { 0.0f, 0.0f, 0.0f };
    vec3 spot_light_specular = { 0.0f, 0.0f, 0.0f };
    vec3 spot_light_attenuation = { 1.0f, 0.09f, 0.32f };

    struct spot_light *spot_light = spot_light_create(
        spot_light_position,
        spot_light_direction,
        spot_light_ambient,
        spot_light_diffuse,
        spot_light_specular,
        spot_light_attenuation,
        cosf(glm_rad(12.5f)),
        cosf(glm_rad(15.0f)));

    if (!spot_light)
    {
        return 1;
    }

    struct spot_light *spot_lights[] = {
        spot_light
    };
    const unsigned int num_spot_lights = sizeof(spot_lights) / sizeof(struct spot_light *);

    // create camera
    vec3 camera_position = { 0.0f, 0.0f, 3.0f };
    vec3 camera_front = { 0.0f, 0.0f, -1.0f };
    vec3 camera_up = { 0.0f, 1.0f, 0.0f };

    struct camera *camera = camera_create(
        camera_position,
        camera_front,
        camera_up,
        0.0f,
        -90.0f,
        0.0f,
        45.0f);

    if (!camera)
    {
        return 1;
    }

    // load music
    Mix_Music *background_music = Mix_LoadMUS("assets/audio/background.mp3");

    if (!background_music)
    {
        error(Mix_GetError());

        return 1;
    }

    // load sounds
    Mix_Chunk *shoot_sound = Mix_LoadWAV("assets/audio/shoot.wav");

    if (!shoot_sound)
    {
        error(Mix_GetError());

        return 1;
    }

    // setup screen fbo
    GLuint screen_fbo;
    glGenFramebuffers(1, &screen_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, screen_fbo);

    GLuint screen_texture;
    glGenTextures(1, &screen_texture);
    glBindTexture(GL_TEXTURE_2D, screen_texture);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGB,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        0,
        GL_RGB,
        GL_UNSIGNED_BYTE,
        NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D,
        screen_texture,
        0);

    glBindTexture(GL_TEXTURE_2D, 0);

    GLuint screen_rbo;
    glGenRenderbuffers(1, &screen_rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, screen_rbo);

    glRenderbufferStorage(
        GL_RENDERBUFFER,
        GL_DEPTH24_STENCIL8,
        WINDOW_WIDTH,
        WINDOW_HEIGHT);

    glFramebufferRenderbuffer(
        GL_FRAMEBUFFER,
        GL_DEPTH_STENCIL_ATTACHMENT,
        GL_RENDERBUFFER,
        screen_rbo);

    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        error("Couldn't complete screen framebuffer");

        return 1;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // setup depthmap fbo
    GLuint depthmap_fbo;
    glGenFramebuffers(1, &depthmap_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, depthmap_fbo);

    GLuint depthmap_texture;
    glGenTextures(1, &depthmap_texture);
    glBindTexture(GL_TEXTURE_2D, depthmap_texture);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_DEPTH_COMPONENT,
        SHADOW_WIDTH,
        SHADOW_HEIGHT,
        0,
        GL_DEPTH_COMPONENT,
        GL_FLOAT,
        NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    GLfloat border_color[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_color);

    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_DEPTH_ATTACHMENT,
        GL_TEXTURE_2D,
        depthmap_texture,
        0);

    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    glBindTexture(GL_TEXTURE_2D, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        error("Couldn't complete depthmap framebuffer");

        return 1;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // setup geometry fbo
    GLuint geometry_fbo;
    glGenFramebuffers(1, &geometry_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, geometry_fbo);

    // TODO: dont store position because it can be recreated in the fragment shader from depth
    GLuint geometry_position_texture;
    glGenTextures(1, &geometry_position_texture);
    glBindTexture(GL_TEXTURE_2D, geometry_position_texture);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGB16F,
        (GLsizei)(WINDOW_WIDTH * RENDER_SCALE),
        (GLsizei)(WINDOW_HEIGHT * RENDER_SCALE),
        0,
        GL_RGB,
        GL_FLOAT,
        NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D,
        geometry_position_texture,
        0);

    glBindTexture(GL_TEXTURE_2D, 0);

    // TODO: find a way to optimize this
    GLuint geometry_normal_texture;
    glGenTextures(1, &geometry_normal_texture);
    glBindTexture(GL_TEXTURE_2D, geometry_normal_texture);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGB16F,
        (GLsizei)(WINDOW_WIDTH * RENDER_SCALE),
        (GLsizei)(WINDOW_HEIGHT * RENDER_SCALE),
        0,
        GL_RGB,
        GL_FLOAT,
        NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT1,
        GL_TEXTURE_2D,
        geometry_normal_texture,
        0);

    glBindTexture(GL_TEXTURE_2D, 0);

    GLuint geometry_albedo_specular_texture;
    glGenTextures(1, &geometry_albedo_specular_texture);
    glBindTexture(GL_TEXTURE_2D, geometry_albedo_specular_texture);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA8,
        (GLsizei)(WINDOW_WIDTH * RENDER_SCALE),
        (GLsizei)(WINDOW_HEIGHT * RENDER_SCALE),
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT2,
        GL_TEXTURE_2D,
        geometry_albedo_specular_texture,
        0);

    glBindTexture(GL_TEXTURE_2D, 0);

    GLenum attachments[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    glDrawBuffers(sizeof(attachments) / sizeof(GLenum), attachments);

    GLuint geometry_rbo;
    glGenRenderbuffers(1, &geometry_rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, geometry_rbo);

    glRenderbufferStorage(
        GL_RENDERBUFFER,
        GL_DEPTH_COMPONENT,
        (GLsizei)(WINDOW_WIDTH * RENDER_SCALE),
        (GLsizei)(WINDOW_HEIGHT * RENDER_SCALE));

    glFramebufferRenderbuffer(
        GL_FRAMEBUFFER,
        GL_DEPTH_ATTACHMENT,
        GL_RENDERBUFFER,
        geometry_rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        error("Couldn't complete geometry framebuffer");

        return 1;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // used for switching shaders
    enum lighting lighting = LIGHTING_FORWARD;

    // main loop
    bool quit = false;
    while (!quit)
    {
        // start of frame activities
        time_frame_start();

        // update window title
        char title[256];
        sprintf_s(title, sizeof(title), "%s - FPS: %d", WINDOW_TITLE, time_fps());
        window_set_title(title);

        // get keyboard input
        int num_keys;
        const unsigned char *keys = SDL_GetKeyboardState(&num_keys);

        // get mouse input
        int mouse_x, mouse_y;
        unsigned int mouse = SDL_GetMouseState(&mouse_x, &mouse_y);

        // handle events
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_KEYDOWN:
            {
                switch (event.key.keysym.sym)
                {
                case SDLK_1:
                {
                    if (Mix_PlayingMusic())
                    {
                        Mix_HaltMusic();
                    }
                    else
                    {
                        Mix_PlayMusic(background_music, -1);
                    }
                }
                break;
                case SDLK_2:
                {
                    if (Mix_PlayingMusic())
                    {
                        if (Mix_PausedMusic())
                        {
                            Mix_ResumeMusic();
                        }
                        else
                        {
                            Mix_PauseMusic();
                        }
                    }
                }
                break;
                case SDLK_F1:
                {
                    lighting = LIGHTING_FORWARD;
                }
                break;
                case SDLK_F2:
                {
                    lighting = LIGHTING_DEFERRED;
                }
                break;
                case SDLK_F4:
                {
                    if (keys[SDL_SCANCODE_LALT])
                    {
                        quit = true;
                    }
                }
                break;
                case SDLK_RETURN:
                {
                    if (keys[SDL_SCANCODE_LALT])
                    {
                        unsigned int flags = window_get_flags();

                        if (flags & SDL_WINDOW_FULLSCREEN_DESKTOP)
                        {
                            window_set_fullscreen(0);
                        }
                        else
                        {
                            window_set_fullscreen(SDL_WINDOW_FULLSCREEN_DESKTOP);
                        }
                    }
                }
                break;
                case SDLK_TAB:
                {
                    SDL_SetRelativeMouseMode(!SDL_GetRelativeMouseMode());
                }
                break;
                }
            }
            break;
            case SDL_MOUSEMOTION:
            {
                // mouselook
                camera->pitch -= event.motion.yrel * 0.1f;
                camera->yaw += event.motion.xrel * 0.1f;

                if (camera->pitch > 89.0f)
                {
                    camera->pitch = 89.0f;
                }
                if (camera->pitch < -89.0f)
                {
                    camera->pitch = -89.0f;
                }

                vec3 front = {
                    cosf(glm_rad(camera->yaw)) * cosf(glm_rad(camera->pitch)),
                    sinf(glm_rad(camera->pitch)),
                    sinf(glm_rad(camera->yaw)) * cosf(glm_rad(camera->pitch)) };
                glm_normalize(front);
                glm_vec_copy(front, camera->front);
            }
            break;
            case SDL_MOUSEWHEEL:
            {
                // zoom
                if (camera->fov >= 1.0f && camera->fov <= 90.0f)
                {
                    camera->fov -= event.wheel.y;
                }
                if (camera->fov <= 1.0f)
                {
                    camera->fov = 1.0f;
                }
                if (camera->fov >= 90.0f)
                {
                    camera->fov = 90.0f;
                }
            }
            break;
            case SDL_QUIT:
            {
                quit = true;
            }
            break;
            case SDL_WINDOWEVENT:
            {
                switch (event.window.event)
                {
                case SDL_WINDOWEVENT_RESIZED:
                {
                    int width = event.window.data1;
                    int height = event.window.data2;

                    window_resize(width, height);
                }
                break;
                }
            }
            break;
            }
        }

        // calculate movement speed
        float speed = 5.0f * time_delta();

        // sprinting
        if (keys[SDL_SCANCODE_LSHIFT])
        {
            speed *= 2.0f;
        }

        // slow movement speed when moving diagonally
        if ((keys[SDL_SCANCODE_W] && keys[SDL_SCANCODE_D]) ||
            (keys[SDL_SCANCODE_W] && keys[SDL_SCANCODE_A]) ||
            (keys[SDL_SCANCODE_S] && keys[SDL_SCANCODE_D]) ||
            (keys[SDL_SCANCODE_S] && keys[SDL_SCANCODE_A]))
        {
            // precomputed 1 / sqrt(2)
            speed *= 0.71f;
        }

        // move forward
        if (keys[SDL_SCANCODE_W])
        {
            vec3 movement;
            glm_vec_scale(camera->front, speed, movement);
            glm_vec_add(camera->position, movement, camera->position);
        }

        // strafe left
        if (keys[SDL_SCANCODE_A])
        {
            vec3 direction;
            glm_cross(camera->front, camera->up, direction);
            glm_normalize(direction);

            vec3 movement;
            glm_vec_scale(direction, -speed, movement);
            glm_vec_add(camera->position, movement, camera->position);
        }

        // move backward
        if (keys[SDL_SCANCODE_S])
        {
            vec3 movement;
            glm_vec_scale(camera->front, -speed, movement);
            glm_vec_add(camera->position, movement, camera->position);
        }

        // strafe right
        if (keys[SDL_SCANCODE_D])
        {
            vec3 direction;
            glm_cross(camera->front, camera->up, direction);
            glm_normalize(direction);

            vec3 movement;
            glm_vec_scale(direction, speed, movement);
            glm_vec_add(camera->position, movement, camera->position);
        }

        // shooting
        static float shoot_timer = 0.0f;
        shoot_timer += time_delta();
        if (mouse & SDL_BUTTON(SDL_BUTTON_LEFT))
        {
            if (shoot_timer >= 0.25f)
            {
                shoot_timer = 0.0f;

                Mix_PlayChannel(-1, shoot_sound, 0);
            }
        }

        // update objects
        float angle = time_current() * 0.001f;
        objects[1]->rotation[0] = angle;
        objects[1]->rotation[1] = angle;
        objects[1]->rotation[2] = angle;

        scene->sun_direction[0] = sinf(angle);
        scene->sun_direction[2] = cosf(angle);

        // update lights
        glm_vec_copy(camera->position, spot_lights[0]->position);
        glm_vec_copy(camera->front, spot_lights[0]->direction);

        // calculate camera projection matrix
        mat4 camera_projection;
        camera_calc_projection_perspective(camera, camera_projection);

        // calculate camera view matrix
        mat4 camera_view;
        camera_calc_view(camera, camera_view);

        // calculate sun projection matrix
        mat4 scene_sun_projection;
        scene_calc_sun_projection(scene, scene_sun_projection);

        // calculate sun view matrix
        mat4 scene_sun_view;
        scene_calc_sun_view(scene, scene_sun_view);

        // bind depthmap fbo
        glBindFramebuffer(GL_FRAMEBUFFER, depthmap_fbo);

        // shadow pass
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glClear(GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        program_bind(depth_program);

        program_set_mat4(depth_program_scene_sun_projection, scene_sun_projection);
        program_set_mat4(depth_program_scene_sun_view, scene_sun_view);

        for (unsigned int i = 0; i < num_objects; i++)
        {
            // calculate model matrix
            mat4 model = GLM_MAT4_IDENTITY_INIT;
            object_calc_model(objects[i], model);

            program_set_mat4(depth_program_object_model, model);

            mesh_draw(objects[i]->mesh);
        }

        program_unbind();

        // unbind depthmap fbo
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        switch (lighting)
        {
        case LIGHTING_FORWARD:
        {
            // bind screen fbo
            glBindFramebuffer(GL_FRAMEBUFFER, screen_fbo);

            // forward rendering
            glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glEnable(GL_DEPTH_TEST);

            for (unsigned int i = 0; i < num_objects; i++)
            {
                // calculate model matrix
                mat4 object_model = GLM_MAT4_IDENTITY_INIT;
                object_calc_model(objects[i], object_model);

                {
                    program_bind(forward_scene_program);

                    program_set_mat4(forward_scene_program_camera_projection, camera_projection);
                    program_set_mat4(forward_scene_program_camera_view, camera_view);
                    program_set_vec3(forward_scene_program_camera_position, camera->position);

                    program_set_mat4(forward_scene_program_object_model, object_model);

                    program_set_vec3(forward_scene_program_material_color, objects[i]->material->color);
                    program_set_float(forward_scene_program_material_shininess, objects[i]->material->shininess);
                    program_set_float(forward_scene_program_material_glow, objects[i]->material->glow);

                    program_set_vec3(forward_scene_program_scene_sun_direction, scene->sun_direction);
                    program_set_vec3(forward_scene_program_scene_sun_ambient, scene->sun_ambient);
                    program_set_vec3(forward_scene_program_scene_sun_diffuse, scene->sun_diffuse);
                    program_set_vec3(forward_scene_program_scene_sun_specular, scene->sun_specular);
                    program_set_mat4(forward_scene_program_scene_sun_projection, scene_sun_projection);
                    program_set_mat4(forward_scene_program_scene_sun_view, scene_sun_view);

                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, objects[i]->material->diffuse ? objects[i]->material->diffuse->texture : 0);
                    glActiveTexture(GL_TEXTURE1);
                    glBindTexture(GL_TEXTURE_2D, objects[i]->material->specular ? objects[i]->material->specular->texture : 0);
                    glActiveTexture(GL_TEXTURE2);
                    glBindTexture(GL_TEXTURE_2D, objects[i]->material->normal ? objects[i]->material->normal->texture : 0);
                    glActiveTexture(GL_TEXTURE3);
                    glBindTexture(GL_TEXTURE_2D, objects[i]->material->emission ? objects[i]->material->emission->texture : 0);
                    glActiveTexture(GL_TEXTURE4);
                    glBindTexture(GL_TEXTURE_2D, depthmap_texture);

                    mesh_draw(objects[i]->mesh);

                    program_unbind();
                }

                glEnable(GL_BLEND);
                glBlendFunc(GL_ONE, GL_ONE);
                glDepthMask(GL_FALSE);
                glDepthFunc(GL_EQUAL);

                // for (unsigned int j = 0; j < num_directional_lights; j++)
                // {
                //     program_bind(forward_directional_program);
                // 
                //     program_set_mat4(forward_directional_program, "camera.projection", camera_projection);
                //     program_set_mat4(forward_directional_program, "camera.view", camera_view);
                //     program_set_vec3(forward_directional_program, "camera.position", camera->position);
                // 
                //     program_set_mat4(forward_directional_program, "object.model", model);
                // 
                //     program_set_vec3(forward_directional_program, "material.color", objects[i]->material->color);
                //     program_set_texture(forward_directional_program, "material.diffuse", 0, objects[i]->material->diffuse ? objects[i]->material->diffuse->texture : 0);
                //     program_set_texture(forward_directional_program, "material.specular", 1, objects[i]->material->specular ? objects[i]->material->specular->texture : 0);
                //     program_set_float(forward_directional_program, "material.shininess", objects[i]->material->shininess);
                //     program_set_texture(forward_directional_program, "material.normal", 2, objects[i]->material->normal ? objects[i]->material->normal->texture : 0);
                //     program_set_texture(forward_directional_program, "material.emission", 3, objects[i]->material->emission ? objects[i]->material->emission->texture : 0);
                //     program_set_float(forward_directional_program, "material.glow", objects[i]->material->glow);
                // 
                //     program_set_vec3(forward_directional_program, "directional_light.direction", directional_lights[j]->direction);
                //     program_set_vec3(forward_directional_program, "directional_light.ambient", directional_lights[j]->ambient);
                //     program_set_vec3(forward_directional_program, "directional_light.diffuse", directional_lights[j]->diffuse);
                //     program_set_vec3(forward_directional_program, "directional_light.specular", directional_lights[j]->specular);
                // 
                //     mesh_draw(objects[i]->mesh);
                // 
                //     program_unbind();
                // }

                for (unsigned int j = 0; j < num_point_lights; j++)
                {
                    program_bind(forward_point_program);

                    program_set_mat4(forward_point_program_camera_projection, camera_projection);
                    program_set_mat4(forward_point_program_camera_view, camera_view);
                    program_set_vec3(forward_point_program_camera_position, camera->position);

                    program_set_mat4(forward_point_program_object_model, object_model);

                    program_set_vec3(forward_point_program_material_color, objects[i]->material->color);
                    program_set_float(forward_point_program_material_shininess, objects[i]->material->shininess);
                    program_set_float(forward_point_program_material_glow, objects[i]->material->glow);

                    program_set_vec3(forward_point_program_point_light_position, point_lights[j]->position);
                    program_set_vec3(forward_point_program_point_light_ambient, point_lights[j]->ambient);
                    program_set_vec3(forward_point_program_point_light_diffuse, point_lights[j]->diffuse);
                    program_set_vec3(forward_point_program_point_light_specular, point_lights[j]->specular);
                    program_set_vec3(forward_point_program_point_light_attenuation, point_lights[j]->attenuation);

                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, objects[i]->material->diffuse ? objects[i]->material->diffuse->texture : 0);
                    glActiveTexture(GL_TEXTURE1);
                    glBindTexture(GL_TEXTURE_2D, objects[i]->material->specular ? objects[i]->material->specular->texture : 0);
                    glActiveTexture(GL_TEXTURE2);
                    glBindTexture(GL_TEXTURE_2D, objects[i]->material->normal ? objects[i]->material->normal->texture : 0);
                    glActiveTexture(GL_TEXTURE3);
                    glBindTexture(GL_TEXTURE_2D, objects[i]->material->emission ? objects[i]->material->emission->texture : 0);

                    mesh_draw(objects[i]->mesh);

                    program_unbind();
                }

                // for (unsigned int j = 0; j < num_spot_lights; j++)
                // {
                //     program_bind(forward_spot_program);
                // 
                //     program_set_mat4(forward_spot_program, "camera.projection", camera_projection);
                //     program_set_mat4(forward_spot_program, "camera.view", camera_view);
                //     program_set_vec3(forward_spot_program, "camera.position", camera->position);
                // 
                //     program_set_mat4(forward_spot_program, "object.model", model);
                // 
                //     program_set_vec3(forward_spot_program, "material.color", objects[i]->material->color);
                //     program_set_texture(forward_spot_program, "material.diffuse", 0, objects[i]->material->diffuse ? objects[i]->material->diffuse->texture : 0);
                //     program_set_texture(forward_spot_program, "material.specular", 1, objects[i]->material->specular ? objects[i]->material->specular->texture : 0);
                //     program_set_float(forward_spot_program, "material.shininess", objects[i]->material->shininess);
                //     program_set_texture(forward_spot_program, "material.normal", 2, objects[i]->material->normal ? objects[i]->material->normal->texture : 0);
                //     program_set_texture(forward_spot_program, "material.emission", 3, objects[i]->material->emission ? objects[i]->material->emission->texture : 0);
                //     program_set_float(forward_spot_program, "material.glow", objects[i]->material->glow);
                // 
                //     program_set_vec3(forward_spot_program, "spot_light.position", spot_lights[j]->position);
                //     program_set_vec3(forward_spot_program, "spot_light.direction", spot_lights[j]->direction);
                //     program_set_vec3(forward_spot_program, "spot_light.ambient", spot_lights[j]->ambient);
                //     program_set_vec3(forward_spot_program, "spot_light.diffuse", spot_lights[j]->diffuse);
                //     program_set_vec3(forward_spot_program, "spot_light.specular", spot_lights[j]->specular);
                //     program_set_vec3(forward_spot_program, "spot_light.attenuation", spot_lights[j]->attenuation);
                //     program_set_float(forward_spot_program, "spot_light.inner_cutoff", spot_lights[j]->inner_cutoff);
                //     program_set_float(forward_spot_program, "spot_light.outer_cutoff", spot_lights[j]->outer_cutoff);
                // 
                //     mesh_draw(objects[i]->mesh);
                // 
                //     program_unbind();
                // }

                glDepthFunc(GL_LESS);
                glDepthMask(GL_TRUE);
                glDisable(GL_BLEND);
            }

            // unbind screen fbo
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }
        break;
        case LIGHTING_DEFERRED:
        {
            // bind geometry fbo
            glBindFramebuffer(GL_FRAMEBUFFER, geometry_fbo);

            // geometry pass
            glViewport(0, 0, (GLsizei)(WINDOW_WIDTH * RENDER_SCALE), (GLsizei)(WINDOW_HEIGHT * RENDER_SCALE));
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glEnable(GL_DEPTH_TEST);

            program_bind(geometry_program);

            program_set_mat4(geometry_program_camera_projection, camera_projection);
            program_set_mat4(geometry_program_camera_view, camera_view);
            program_set_vec3(geometry_program_camera_position, camera->position);

            for (unsigned int i = 0; i < num_objects; i++)
            {
                // calculate model matrix
                mat4 object_model = GLM_MAT4_IDENTITY_INIT;
                object_calc_model(objects[i], object_model);

                program_set_mat4(geometry_program_object_model, object_model);

                program_set_vec3(geometry_program_material_color, objects[i]->material->color);
                program_set_float(geometry_program_material_shininess, objects[i]->material->shininess);
                program_set_float(geometry_program_material_glow, objects[i]->material->glow);

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, objects[i]->material->diffuse ? objects[i]->material->diffuse->texture : 0);
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, objects[i]->material->specular ? objects[i]->material->specular->texture : 0);
                glActiveTexture(GL_TEXTURE2);
                glBindTexture(GL_TEXTURE_2D, objects[i]->material->normal ? objects[i]->material->normal->texture : 0);
                glActiveTexture(GL_TEXTURE3);
                glBindTexture(GL_TEXTURE_2D, objects[i]->material->emission ? objects[i]->material->emission->texture : 0);

                mesh_draw(objects[i]->mesh);
            }

            program_unbind();

            // unbind geometry fbo
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            // bind screen fbo
            glBindFramebuffer(GL_FRAMEBUFFER, screen_fbo);

            // lighting pass
            glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
            glClear(GL_COLOR_BUFFER_BIT);
            glDisable(GL_DEPTH_TEST);

            {
                program_bind(deferred_scene_program);

                program_set_vec3(deferred_scene_program_camera_position, camera->position);

                program_set_vec3(deferred_scene_program_scene_sun_direction, scene->sun_direction);
                program_set_vec3(deferred_scene_program_scene_sun_ambient, scene->sun_ambient);
                program_set_vec3(deferred_scene_program_scene_sun_diffuse, scene->sun_diffuse);
                program_set_vec3(deferred_scene_program_scene_sun_specular, scene->sun_specular);
                program_set_mat4(deferred_scene_program_scene_sun_projection, scene_sun_projection);
                program_set_mat4(deferred_scene_program_scene_sun_view, scene_sun_view);

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, geometry_position_texture);
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, geometry_normal_texture);
                glActiveTexture(GL_TEXTURE2);
                glBindTexture(GL_TEXTURE_2D, geometry_albedo_specular_texture);
                glActiveTexture(GL_TEXTURE3);
                glBindTexture(GL_TEXTURE_2D, depthmap_texture);

                mesh_draw(quad_mesh);

                program_unbind();
            }

            glEnable(GL_BLEND);
            glBlendFunc(GL_ONE, GL_ONE);
            glDepthMask(GL_FALSE);
            glDepthFunc(GL_EQUAL);

            // for (unsigned int i = 0; i < num_directional_lights; i++)
            // {
            //     program_bind(deferred_directional_program);
            // 
            //     program_set_texture(deferred_directional_program, "geometry.position", 0, geometry_position_texture);
            //     program_set_texture(deferred_directional_program, "geometry.normal", 1, geometry_normal_texture);
            //     program_set_texture(deferred_directional_program, "geometry.albedo_specular", 2, geometry_albedo_specular_texture);
            // 
            //     program_set_vec3(deferred_directional_program, "camera.position", camera->position);
            // 
            //     program_set_vec3(deferred_directional_program, "directional_light.direction", directional_lights[i]->direction);
            //     program_set_vec3(deferred_directional_program, "directional_light.ambient", directional_lights[i]->ambient);
            //     program_set_vec3(deferred_directional_program, "directional_light.diffuse", directional_lights[i]->diffuse);
            //     program_set_vec3(deferred_directional_program, "directional_light.specular", directional_lights[i]->specular);
            // 
            //     mesh_draw(quad_mesh);
            // 
            //     program_unbind();
            // }

            for (unsigned int i = 0; i < num_point_lights; i++)
            {
                program_bind(deferred_point_program);

                program_set_vec3(deferred_point_program_camera_position, camera->position);

                program_set_vec3(deferred_point_program_point_light_position, point_lights[i]->position);
                program_set_vec3(deferred_point_program_point_light_ambient, point_lights[i]->ambient);
                program_set_vec3(deferred_point_program_point_light_diffuse, point_lights[i]->diffuse);
                program_set_vec3(deferred_point_program_point_light_specular, point_lights[i]->specular);
                program_set_vec3(deferred_point_program_point_light_attenuation, point_lights[i]->attenuation);

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, geometry_position_texture);
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, geometry_normal_texture);
                glActiveTexture(GL_TEXTURE2);
                glBindTexture(GL_TEXTURE_2D, geometry_albedo_specular_texture);

                mesh_draw(quad_mesh);

                program_unbind();
            }

            // for (unsigned int i = 0; i < num_spot_lights; i++)
            // {
            //     program_bind(deferred_spot_program);
            // 
            //     program_set_texture(deferred_spot_program, "geometry.position", 0, geometry_position_texture);
            //     program_set_texture(deferred_spot_program, "geometry.normal", 1, geometry_normal_texture);
            //     program_set_texture(deferred_spot_program, "geometry.albedo_specular", 2, geometry_albedo_specular_texture);
            // 
            //     program_set_vec3(deferred_spot_program, "camera.position", camera->position);
            // 
            //     program_set_vec3(deferred_spot_program, "spot_light.position", spot_lights[i]->position);
            //     program_set_vec3(deferred_spot_program, "spot_light.direction", spot_lights[i]->direction);
            //     program_set_vec3(deferred_spot_program, "spot_light.ambient", spot_lights[i]->ambient);
            //     program_set_vec3(deferred_spot_program, "spot_light.diffuse", spot_lights[i]->diffuse);
            //     program_set_vec3(deferred_spot_program, "spot_light.specular", spot_lights[i]->specular);
            //     program_set_vec3(deferred_spot_program, "spot_light.attenuation", spot_lights[i]->attenuation);
            //     program_set_float(deferred_spot_program, "spot_light.inner_cutoff", spot_lights[i]->inner_cutoff);
            //     program_set_float(deferred_spot_program, "spot_light.outer_cutoff", spot_lights[i]->outer_cutoff);
            // 
            //     mesh_draw(quad_mesh);
            // 
            //     program_unbind();
            // }

             // TODO: emissive lighting pass

            glDepthFunc(GL_LESS);
            glDepthMask(GL_TRUE);
            glDisable(GL_BLEND);

            // unbind screen fbo
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }
        break;
        }

        // post processing pass
        glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST);

        program_bind(post_program);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, screen_texture);

        mesh_draw(quad_mesh);

        program_unbind();

        // display the window
        window_swap();

        // end of frame activities
        time_frame_end();
    }

    // free resources
    glDeleteRenderbuffers(1, &geometry_rbo);
    glDeleteTextures(1, &geometry_albedo_specular_texture);
    glDeleteTextures(1, &geometry_normal_texture);
    glDeleteTextures(1, &geometry_position_texture);
    glDeleteFramebuffers(1, &geometry_fbo);

    glDeleteTextures(1, &depthmap_texture);
    glDeleteFramebuffers(1, &depthmap_fbo);

    glDeleteRenderbuffers(1, &screen_rbo);
    glDeleteTextures(1, &screen_texture);
    glDeleteFramebuffers(1, &screen_fbo);

    Mix_FreeChunk(shoot_sound);
    Mix_FreeMusic(background_music);

    spot_light_destroy(spot_light);

    point_light_destroy(red_point_light);
    point_light_destroy(green_point_light);
    point_light_destroy(blue_point_light);
    point_light_destroy(yellow_point_light);

    directional_light_destroy(directional_light);

    object_destroy(floor_object);
    object_destroy(box_1_object);
    object_destroy(box_2_object);
    object_destroy(box_3_object);
    object_destroy(box_4_object);
    object_destroy(box_5_object);

    material_destroy(cobble_material);
    material_destroy(box_material);

    texture_destroy(cobble_specular_texture);
    texture_destroy(cobble_diffuse_texture);
    texture_destroy(box_specular_texture);
    texture_destroy(box_diffuse_texture);

    mesh_destroy(cube_mesh);
    mesh_destroy(quad_mesh);

    program_destroy(depth_program);
    program_destroy(post_program);
    program_destroy(geometry_program);
    program_destroy(deferred_spot_program);
    program_destroy(deferred_point_program);
    program_destroy(deferred_directional_program);
    program_destroy(deferred_scene_program);
    program_destroy(forward_spot_program);
    program_destroy(forward_point_program);
    program_destroy(forward_directional_program);
    program_destroy(forward_scene_program);

    // close engine
    image_quit();
    audio_quit();
    window_quit();
    core_quit();

    return 0;
}
