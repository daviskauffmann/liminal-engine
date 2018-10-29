#include <engine/engine.h>

// TODO: create a way to specify paths for engine assets and game assets separately

static int render_width;
static int render_height;
static float render_scale;
static int shadow_width;
static int shadow_height;
static enum render_mode render_mode;

// standard assets
struct mesh *quad_mesh;
struct mesh *cube_mesh;

struct texture *default_diffuse_texture;
struct texture *default_specular_texture;

struct material *default_material;

// programs
static struct program *depth_program;
static struct program *forward_scene_program;
static struct program *forward_directional_program;
static struct program *forward_point_program;
static struct program *forward_spot_program;
static struct program *geometry_program;
static struct program *deferred_scene_program;
static struct program *deferred_directional_program;
static struct program *deferred_point_program;
static struct program *deferred_spot_program;
static struct program *post_program;

// framebuffers
static GLuint screen_fbo;
static GLuint screen_texture;
static GLuint screen_rbo;

static GLuint depthmap_fbo;
static GLuint depthmap_texture;

static GLuint geometry_fbo;
static GLuint geometry_position_texture;
static GLuint geometry_normal_texture;
static GLuint geometry_albedo_specular_texture;
static GLuint geometry_rbo;

// renderables
static struct camera *camera;
static struct scene *scene;

static struct object **objects;
static unsigned int num_objects;

static struct directional_light **directional_lights;
static unsigned int num_directional_lights;

static struct point_light **point_lights;
static unsigned int num_point_lights;

static struct spot_light **spot_lights;
static unsigned int num_spot_lights;

int renderer_init(int _render_width, int _render_height, float _render_scale, int _shadow_width, int _shadow_height)
{
    render_width = _render_width;
    render_height = _render_height;
    render_scale = _render_scale;
    shadow_width = _shadow_width;
    shadow_height = _shadow_height;
    render_mode = RENDER_MODE_FORWARD;

    // setup opengl
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    // glEnable(GL_CULL_FACE);
    // glCullFace(GL_BACK);
    // glFrontFace(GL_CW);

    // create programs
    depth_program = program_create(
        "../engine/assets/shaders/depth.vert",
        "../engine/assets/shaders/depth.frag");

    if (!depth_program)
    {
        return 1;
    }

    forward_scene_program = program_create(
        "../engine/assets/shaders/forward.vert",
        "../engine/assets/shaders/forward_scene.frag");

    if (!forward_scene_program)
    {
        return 1;
    }

    forward_directional_program = program_create(
        "../engine/assets/shaders/forward.vert",
        "../engine/assets/shaders/forward_directional.frag");

    if (!forward_directional_program)
    {
        return 1;
    }

    forward_point_program = program_create(
        "../engine/assets/shaders/forward.vert",
        "../engine/assets/shaders/forward_point.frag");

    if (!forward_point_program)
    {
        return 1;
    }

    forward_spot_program = program_create(
        "../engine/assets/shaders/forward.vert",
        "../engine/assets/shaders/forward_spot.frag");

    if (!forward_spot_program)
    {
        return 1;
    }

    geometry_program = program_create(
        "../engine/assets/shaders/geometry.vert",
        "../engine/assets/shaders/geometry.frag");

    if (!geometry_program)
    {
        return 1;
    }

    deferred_scene_program = program_create(
        "../engine/assets/shaders/deferred.vert",
        "../engine/assets/shaders/deferred_scene.frag");

    if (!deferred_scene_program)
    {
        return 1;
    }

    deferred_directional_program = program_create(
        "../engine/assets/shaders/deferred.vert",
        "../engine/assets/shaders/deferred_directional.frag");

    if (!deferred_directional_program)
    {
        return 1;
    }

    deferred_point_program = program_create(
        "../engine/assets/shaders/deferred.vert",
        "../engine/assets/shaders/deferred_point.frag");

    if (!deferred_point_program)
    {
        return 1;
    }

    deferred_spot_program = program_create(
        "../engine/assets/shaders/deferred.vert",
        "../engine/assets/shaders/deferred_spot.frag");

    if (!deferred_spot_program)
    {
        return 1;
    }

    post_program = program_create(
        "../engine/assets/shaders/post.vert",
        "../engine/assets/shaders/post.frag");

    if (!post_program)
    {
        return 1;
    }

    // setup shader samplers
    program_bind(forward_scene_program);
    program_set_int(forward_scene_program, "material.diffuse", 0);
    program_set_int(forward_scene_program, "material.specular", 1);
    program_set_int(forward_scene_program, "material.normal", 2);
    program_set_int(forward_scene_program, "material.emission", 3);
    program_set_int(forward_scene_program, "depthmap.texture", 4);
    program_unbind();

    program_bind(forward_directional_program);
    program_set_int(forward_directional_program, "material.diffuse", 0);
    program_set_int(forward_directional_program, "material.specular", 1);
    program_set_int(forward_directional_program, "material.normal", 2);
    program_set_int(forward_directional_program, "material.emission", 3);
    program_unbind();

    program_bind(forward_point_program);
    program_set_int(forward_point_program, "material.diffuse", 0);
    program_set_int(forward_point_program, "material.specular", 1);
    program_set_int(forward_point_program, "material.normal", 2);
    program_set_int(forward_point_program, "material.emission", 3);
    program_unbind();

    program_bind(forward_spot_program);
    program_set_int(forward_spot_program, "material.diffuse", 0);
    program_set_int(forward_spot_program, "material.specular", 1);
    program_set_int(forward_spot_program, "material.normal", 2);
    program_set_int(forward_spot_program, "material.emission", 3);
    program_unbind();

    program_bind(geometry_program);
    program_set_int(geometry_program, "material.diffuse", 0);
    program_set_int(geometry_program, "material.specular", 1);
    program_set_int(geometry_program, "material.normal", 2);
    program_set_int(geometry_program, "material.emission", 3);
    program_unbind();

    program_bind(deferred_scene_program);
    program_set_int(deferred_scene_program, "geometry.normal", 1);
    program_set_int(deferred_scene_program, "geometry.position", 0);
    program_set_int(deferred_scene_program, "geometry.albedo_specular", 2);
    program_set_int(deferred_scene_program, "depthmap.texture", 3);
    program_unbind();

    program_bind(deferred_directional_program);
    program_set_int(deferred_directional_program, "geometry.position", 0);
    program_set_int(deferred_directional_program, "geometry.normal", 1);
    program_set_int(deferred_directional_program, "geometry.albedo_specular", 2);
    program_unbind();

    program_bind(deferred_point_program);
    program_set_int(deferred_point_program, "geometry.position", 0);
    program_set_int(deferred_point_program, "geometry.normal", 1);
    program_set_int(deferred_point_program, "geometry.albedo_specular", 2);
    program_unbind();

    program_bind(deferred_spot_program);
    program_set_int(deferred_spot_program, "geometry.position", 0);
    program_set_int(deferred_spot_program, "geometry.normal", 1);
    program_set_int(deferred_spot_program, "geometry.albedo_specular", 2);
    program_unbind();

    program_bind(post_program);
    program_set_int(post_program, "screen.texture", 0);
    program_unbind();

    // create default meshes
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

    quad_mesh = mesh_create(
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

    cube_mesh = mesh_create(
        cube_vertices,
        sizeof(cube_vertices),
        cube_indices,
        sizeof(cube_indices));

    if (!cube_mesh)
    {
        return 1;
    }

    // create default texture
    default_diffuse_texture = texture_create("../engine/assets/images/default.png");

    if (!default_diffuse_texture)
    {
        return 1;
    }

    default_specular_texture = texture_create("../engine/assets/images/default.png");

    if (!default_specular_texture)
    {
        return 1;
    }

    // create default material
    vec3 default_material_color = { 1.0f, 1.0f, 1.0f };

    default_material = material_create(
        default_material_color,
        default_diffuse_texture,
        default_specular_texture,
        16.0f,
        NULL,
        NULL,
        1.0f);

    if (!default_material)
    {
        return 1;
    }

    // setup screen fbo
    glGenFramebuffers(1, &screen_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, screen_fbo);

    glGenTextures(1, &screen_texture);
    glBindTexture(GL_TEXTURE_2D, screen_texture);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGB,
        render_width,
        render_height,
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

    glGenRenderbuffers(1, &screen_rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, screen_rbo);

    glRenderbufferStorage(
        GL_RENDERBUFFER,
        GL_DEPTH24_STENCIL8,
        render_width,
        render_height);

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
    glGenFramebuffers(1, &depthmap_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, depthmap_fbo);

    glGenTextures(1, &depthmap_texture);
    glBindTexture(GL_TEXTURE_2D, depthmap_texture);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_DEPTH_COMPONENT,
        shadow_width,
        shadow_height,
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
    glGenFramebuffers(1, &geometry_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, geometry_fbo);

    // TODO: dont store position because it can be recreated in the fragment shader from depth
    glGenTextures(1, &geometry_position_texture);
    glBindTexture(GL_TEXTURE_2D, geometry_position_texture);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGB16F,
        (GLsizei)(render_width * render_scale),
        (GLsizei)(render_height * render_scale),
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
    glGenTextures(1, &geometry_normal_texture);
    glBindTexture(GL_TEXTURE_2D, geometry_normal_texture);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGB16F,
        (GLsizei)(render_width * render_scale),
        (GLsizei)(render_height * render_scale),
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

    glGenTextures(1, &geometry_albedo_specular_texture);
    glBindTexture(GL_TEXTURE_2D, geometry_albedo_specular_texture);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA8,
        (GLsizei)(render_width * render_scale),
        (GLsizei)(render_height * render_scale),
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

    glGenRenderbuffers(1, &geometry_rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, geometry_rbo);

    glRenderbufferStorage(
        GL_RENDERBUFFER,
        GL_DEPTH_COMPONENT,
        (GLsizei)(render_width * render_scale),
        (GLsizei)(render_height * render_scale));

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

    return 0;
}

void renderer_set_mode(enum render_mode _render_mode)
{
    render_mode = _render_mode;
}

void renderer_set_camera(struct camera *_camera)
{
    camera = _camera;
}

void renderer_set_scene(struct scene *_scene)
{
    scene = _scene;
}

void renderer_add_object(struct object *object)
{
    objects = realloc(objects, sizeof(struct object *) * (num_objects + 1));
    objects[num_objects++] = object;
}

void renderer_add_directional_light(struct directional_light *directional_light)
{
    directional_lights = realloc(directional_lights, sizeof(struct directional_light *) * (num_directional_lights + 1));
    directional_lights[num_directional_lights++] = directional_light;
}

void renderer_add_point_light(struct point_light *point_light)
{
    point_lights = realloc(point_lights, sizeof(struct point_light *) * (num_point_lights + 1));
    point_lights[num_point_lights++] = point_light;
}

void renderer_add_spot_light(struct spot_light *spot_light)
{
    spot_lights = realloc(spot_lights, sizeof(struct spot_light *) * (num_spot_lights + 1));
    spot_lights[num_spot_lights++] = spot_light;
}

void renderer_draw(void)
{
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
    glViewport(0, 0, shadow_width, shadow_height);
    glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    program_bind(depth_program);

    program_set_mat4(depth_program, "scene.sun_projection", scene_sun_projection);
    program_set_mat4(depth_program, "scene.sun_view", scene_sun_view);

    for (unsigned int i = 0; i < num_objects; i++)
    {
        // calculate model matrix
        mat4 object_model = GLM_MAT4_IDENTITY_INIT;
        object_calc_model(objects[i], object_model);

        program_set_mat4(depth_program, "object.model", object_model);

        mesh_draw(objects[i]->mesh);
    }

    program_unbind();

    // unbind depthmap fbo
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    switch (render_mode)
    {
    case RENDER_MODE_FORWARD:
    {
        // bind screen fbo
        glBindFramebuffer(GL_FRAMEBUFFER, screen_fbo);

        // forward rendering
        glViewport(0, 0, render_width, render_height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        for (unsigned int i = 0; i < num_objects; i++)
        {
            // calculate model matrix
            mat4 object_model = GLM_MAT4_IDENTITY_INIT;
            object_calc_model(objects[i], object_model);

            // scene lighting
            program_bind(forward_scene_program);

            {
                program_set_mat4(forward_scene_program, "camera.projection", camera_projection);
                program_set_mat4(forward_scene_program, "camera.view", camera_view);
                program_set_vec3(forward_scene_program, "camera.position", camera->position);

                program_set_mat4(forward_scene_program, "object.model", object_model);

                program_set_vec3(forward_scene_program, "material.color", objects[i]->material->color);
                program_set_float(forward_scene_program, "material.shininess", objects[i]->material->shininess);
                program_set_float(forward_scene_program, "material.glow", objects[i]->material->glow);

                program_set_vec3(forward_scene_program, "scene.sun_direction", scene->sun_direction);
                program_set_vec3(forward_scene_program, "scene.sun_ambient", scene->sun_ambient);
                program_set_vec3(forward_scene_program, "scene.sun_diffuse", scene->sun_diffuse);
                program_set_vec3(forward_scene_program, "scene.sun_specular", scene->sun_specular);
                program_set_mat4(forward_scene_program, "scene.sun_projection", scene_sun_projection);
                program_set_mat4(forward_scene_program, "scene.sun_view", scene_sun_view);

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
            }

            program_unbind();

            // enable blending
            glEnable(GL_BLEND);
            glBlendFunc(GL_ONE, GL_ONE);
            glDepthMask(GL_FALSE);
            glDepthFunc(GL_EQUAL);

            // directional lights
            program_bind(forward_directional_program);

            for (unsigned int j = 0; j < num_directional_lights; j++)
            {
                program_set_mat4(forward_directional_program, "camera.projection", camera_projection);
                program_set_mat4(forward_directional_program, "camera.view", camera_view);
                program_set_vec3(forward_directional_program, "camera.position", camera->position);

                program_set_mat4(forward_directional_program, "object.model", object_model);

                program_set_vec3(forward_directional_program, "material.color", objects[i]->material->color);
                program_set_float(forward_directional_program, "material.shininess", objects[i]->material->shininess);
                program_set_float(forward_directional_program, "material.glow", objects[i]->material->glow);

                program_set_vec3(forward_directional_program, "directional_light.direction", directional_lights[j]->direction);
                program_set_vec3(forward_directional_program, "directional_light.ambient", directional_lights[j]->ambient);
                program_set_vec3(forward_directional_program, "directional_light.diffuse", directional_lights[j]->diffuse);
                program_set_vec3(forward_directional_program, "directional_light.specular", directional_lights[j]->specular);

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
            }

            program_unbind();

            // point lights
            program_bind(forward_point_program);

            for (unsigned int j = 0; j < num_point_lights; j++)
            {
                program_set_mat4(forward_point_program, "camera.projection", camera_projection);
                program_set_mat4(forward_point_program, "camera.view", camera_view);
                program_set_vec3(forward_point_program, "camera.position", camera->position);

                program_set_mat4(forward_point_program, "object.model", object_model);

                program_set_vec3(forward_point_program, "material.color", objects[i]->material->color);
                program_set_float(forward_point_program, "material.shininess", objects[i]->material->shininess);
                program_set_float(forward_point_program, "material.glow", objects[i]->material->glow);

                program_set_vec3(forward_point_program, "point_light.position", point_lights[j]->position);
                program_set_vec3(forward_point_program, "point_light.ambient", point_lights[j]->ambient);
                program_set_vec3(forward_point_program, "point_light.diffuse", point_lights[j]->diffuse);
                program_set_vec3(forward_point_program, "point_light.specular", point_lights[j]->specular);
                program_set_vec3(forward_point_program, "point_light.attenuation", point_lights[j]->attenuation);

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

            // spot lights
            program_bind(forward_spot_program);

            for (unsigned int j = 0; j < num_spot_lights; j++)
            {
                program_set_mat4(forward_spot_program, "camera.projection", camera_projection);
                program_set_mat4(forward_spot_program, "camera.view", camera_view);
                program_set_vec3(forward_spot_program, "camera.position", camera->position);

                program_set_mat4(forward_spot_program, "object.model", object_model);

                program_set_vec3(forward_spot_program, "material.color", objects[i]->material->color);
                program_set_float(forward_spot_program, "material.shininess", objects[i]->material->shininess);
                program_set_float(forward_spot_program, "material.glow", objects[i]->material->glow);

                program_set_vec3(forward_spot_program, "spot_light.position", spot_lights[j]->position);
                program_set_vec3(forward_spot_program, "spot_light.direction", spot_lights[j]->direction);
                program_set_vec3(forward_spot_program, "spot_light.ambient", spot_lights[j]->ambient);
                program_set_vec3(forward_spot_program, "spot_light.diffuse", spot_lights[j]->diffuse);
                program_set_vec3(forward_spot_program, "spot_light.specular", spot_lights[j]->specular);
                program_set_vec3(forward_spot_program, "spot_light.attenuation", spot_lights[j]->attenuation);
                program_set_float(forward_spot_program, "spot_light.inner_cutoff", spot_lights[j]->inner_cutoff);
                program_set_float(forward_spot_program, "spot_light.outer_cutoff", spot_lights[j]->outer_cutoff);

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
            }

            program_unbind();

            // disable blending
            glDepthFunc(GL_LESS);
            glDepthMask(GL_TRUE);
            glDisable(GL_BLEND);
        }

        // unbind screen fbo
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    break;
    case RENDER_MODE_DEFERRED:
    {
        // bind geometry fbo
        glBindFramebuffer(GL_FRAMEBUFFER, geometry_fbo);

        // geometry pass
        glViewport(0, 0, (GLsizei)(render_width * render_scale), (GLsizei)(render_height * render_scale));
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        program_bind(geometry_program);

        program_set_mat4(geometry_program, "camera.projection", camera_projection);
        program_set_mat4(geometry_program, "camera.view", camera_view);
        program_set_vec3(geometry_program, "camera.position", camera->position);

        for (unsigned int i = 0; i < num_objects; i++)
        {
            // calculate model matrix
            mat4 object_model = GLM_MAT4_IDENTITY_INIT;
            object_calc_model(objects[i], object_model);

            program_set_mat4(geometry_program, "object.model", object_model);

            program_set_vec3(geometry_program, "material.color", objects[i]->material->color);
            program_set_float(geometry_program, "material.shininess", objects[i]->material->shininess);
            program_set_float(geometry_program, "material.glow", objects[i]->material->glow);

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
        glViewport(0, 0, render_width, render_height);
        glClear(GL_COLOR_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST);

        // scene lighting
        program_bind(deferred_scene_program);

        {
            program_set_vec3(deferred_scene_program, "camera.position", camera->position);

            program_set_vec3(deferred_scene_program, "scene.sun_direction", scene->sun_direction);
            program_set_vec3(deferred_scene_program, "scene.sun_ambient", scene->sun_ambient);
            program_set_vec3(deferred_scene_program, "scene.sun_diffuse", scene->sun_diffuse);
            program_set_vec3(deferred_scene_program, "scene.sun_specular", scene->sun_specular);
            program_set_mat4(deferred_scene_program, "scene.sun_projection", scene_sun_projection);
            program_set_mat4(deferred_scene_program, "scene.sun_view", scene_sun_view);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, geometry_position_texture);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, geometry_normal_texture);
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, geometry_albedo_specular_texture);
            glActiveTexture(GL_TEXTURE3);
            glBindTexture(GL_TEXTURE_2D, depthmap_texture);

            mesh_draw(quad_mesh);

        }

        program_unbind();

        // enable blending
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE);
        glDepthMask(GL_FALSE);
        glDepthFunc(GL_EQUAL);

        // directional lights
        program_bind(deferred_directional_program);

        for (unsigned int i = 0; i < num_directional_lights; i++)
        {
            program_set_vec3(deferred_directional_program, "camera.position", camera->position);

            program_set_vec3(deferred_directional_program, "directional_light.direction", directional_lights[i]->direction);
            program_set_vec3(deferred_directional_program, "directional_light.ambient", directional_lights[i]->ambient);
            program_set_vec3(deferred_directional_program, "directional_light.diffuse", directional_lights[i]->diffuse);
            program_set_vec3(deferred_directional_program, "directional_light.specular", directional_lights[i]->specular);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, geometry_position_texture);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, geometry_normal_texture);
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, geometry_albedo_specular_texture);

            mesh_draw(quad_mesh);
        }

        program_unbind();

        // point lights
        program_bind(deferred_point_program);

        for (unsigned int i = 0; i < num_point_lights; i++)
        {
            program_set_vec3(deferred_point_program, "camera.position", camera->position);

            program_set_vec3(deferred_point_program, "point_light.position", point_lights[i]->position);
            program_set_vec3(deferred_point_program, "point_light.ambient", point_lights[i]->ambient);
            program_set_vec3(deferred_point_program, "point_light.diffuse", point_lights[i]->diffuse);
            program_set_vec3(deferred_point_program, "point_light.specular", point_lights[i]->specular);
            program_set_vec3(deferred_point_program, "point_light.attenuation", point_lights[i]->attenuation);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, geometry_position_texture);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, geometry_normal_texture);
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, geometry_albedo_specular_texture);

            mesh_draw(quad_mesh);
        }

        program_unbind();

        // spot lights
        program_bind(deferred_spot_program);

        for (unsigned int i = 0; i < num_spot_lights; i++)
        {
            program_set_vec3(deferred_spot_program, "camera.position", camera->position);

            program_set_vec3(deferred_spot_program, "spot_light.position", spot_lights[i]->position);
            program_set_vec3(deferred_spot_program, "spot_light.direction", spot_lights[i]->direction);
            program_set_vec3(deferred_spot_program, "spot_light.ambient", spot_lights[i]->ambient);
            program_set_vec3(deferred_spot_program, "spot_light.diffuse", spot_lights[i]->diffuse);
            program_set_vec3(deferred_spot_program, "spot_light.specular", spot_lights[i]->specular);
            program_set_vec3(deferred_spot_program, "spot_light.attenuation", spot_lights[i]->attenuation);
            program_set_float(deferred_spot_program, "spot_light.inner_cutoff", spot_lights[i]->inner_cutoff);
            program_set_float(deferred_spot_program, "spot_light.outer_cutoff", spot_lights[i]->outer_cutoff);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, geometry_position_texture);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, geometry_normal_texture);
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, geometry_albedo_specular_texture);

            mesh_draw(quad_mesh);
        }

        program_unbind();

        // TODO: emissive lighting pass

        // disable blending
        glDepthFunc(GL_LESS);
        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);

        // unbind screen fbo
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    break;
    }

    // post processing pass
    glViewport(0, 0, render_width, render_height);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);

    program_bind(post_program);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, screen_texture);

    mesh_draw(quad_mesh);

    program_unbind();

    num_objects = 0;
    num_directional_lights = 0;
    num_point_lights = 0;
    num_spot_lights = 0;
}

void renderer_quit(void)
{
    free(spot_lights);
    free(point_lights);
    free(directional_lights);
    free(objects);

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

    material_destroy(default_material);

    texture_destroy(default_specular_texture);
    texture_destroy(default_diffuse_texture);

    mesh_destroy(cube_mesh);
    mesh_destroy(quad_mesh);
}
