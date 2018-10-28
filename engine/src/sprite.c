#include <engine/engine.h>

static float vertices[] = {
    // position // uv
    0.0f, 1.0f, 0.0f, 1.0f,
    1.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 0.0f,

    0.0f, 1.0f, 0.0f, 1.0f,
    1.0f, 1.0f, 1.0f, 1.0f,
    1.0f, 0.0f, 1.0f, 0.0f
};

static GLuint vao;
static GLuint vbo;

int sprite_init(void)
{
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid *)0);                     // position
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid *)(2 * sizeof(GLfloat))); // uv
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

    return 0;
}

struct sprite *sprite_create(
    vec3 color,
    struct texture *image,
    vec2 position,
    float rotation,
    vec2 scale)
{
    struct sprite *sprite = malloc(sizeof(struct sprite));

    if (!sprite)
    {
        error("Couldn't allocate sprite");

        return NULL;
    }

    glm_vec_copy(color, sprite->color);
    sprite->image = image;
    glm_vec_copy(position, sprite->position);
    sprite->rotation = rotation;
    glm_vec_copy(scale, sprite->scale);

    return sprite;
}

void sprite_calc_model(struct sprite *sprite, mat4 model)
{
    glm_translate(model, sprite->position);
    vec2 center = { sprite->scale[0] * 0.5f, sprite->scale[1] * 0.5f };
    // glm_translate(model, center);
    glm_rotate(model, sprite->rotation, GLM_ZUP);
    vec2 revert = { sprite->scale[0] * -0.5f, sprite->scale[1] * -0.5f };
    // glm_translate(model, revert);
    glm_scale(model, sprite->scale);
}

void sprite_draw(struct sprite *sprite)
{
    glBindVertexArray(vao);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);

    glBindVertexArray(0);
}

void sprite_destroy(struct sprite *sprite)
{
    free(sprite);
}

void sprite_quit(void)
{
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
}
