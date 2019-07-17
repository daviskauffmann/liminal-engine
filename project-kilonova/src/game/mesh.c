#include <game/game.h>

struct mesh *mesh_create(
    float *vertices,
    unsigned int vertices_size,
    unsigned int *indices,
    unsigned int indices_size)
{
    struct mesh *mesh = malloc(sizeof(struct mesh));

    if (!mesh)
    {
        printf("Error: Couldn't allocate mesh\n");

        return NULL;
    }

    glGenVertexArrays(1, &mesh->vao_id);
    glGenBuffers(1, &mesh->vbo_id);
    glGenBuffers(1, &mesh->ebo_id);
    mesh->vertices_size = vertices_size;
    mesh->indices_size = indices_size;

    glBindVertexArray(mesh->vao_id);

    // setup vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo_id);
    glBufferData(GL_ARRAY_BUFFER, vertices_size, vertices, GL_STATIC_DRAW);

    // setup element buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ebo_id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_size, indices, GL_STATIC_DRAW);

    // setup vertex attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)(0 * sizeof(GLfloat))); // position
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)(3 * sizeof(GLfloat))); // normal
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)(6 * sizeof(GLfloat))); // uv
    
    // enable vertex attributes for this vertex array
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    return mesh;
}

void mesh_draw(struct mesh *mesh)
{
    glBindVertexArray(mesh->vao_id);

    glDrawElements(GL_TRIANGLES, mesh->indices_size, GL_UNSIGNED_INT, NULL);

    glBindVertexArray(0);
}

void mesh_destroy(struct mesh *mesh)
{
    glDeleteBuffers(1, &mesh->ebo_id);
    glDeleteBuffers(1, &mesh->vbo_id);
    glDeleteVertexArrays(1, &mesh->vao_id);

    free(mesh);
}
