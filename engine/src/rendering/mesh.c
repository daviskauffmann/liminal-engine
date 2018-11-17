#include <engine/engine.h>

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

    // setup vertices
    glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo_id);
    glBufferData(GL_ARRAY_BUFFER, vertices_size, vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)0);                     // position
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)(3 * sizeof(GLfloat))); // normal
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)(6 * sizeof(GLfloat))); // uv
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // setup indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ebo_id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_size, indices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

    return mesh;
}

void mesh_draw(struct mesh *mesh)
{
    glBindVertexArray(mesh->vao_id);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    // glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);
    // glDrawArrays(GL_TRIANGLES, 0, mesh->vertices_size);
    // glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ebo_id);
    glDrawElements(GL_TRIANGLES, mesh->indices_size, GL_UNSIGNED_INT, NULL);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);

    glBindVertexArray(0);
}

void mesh_destroy(struct mesh *mesh)
{
    glDeleteBuffers(1, &mesh->ebo_id);
    glDeleteBuffers(1, &mesh->vbo_id);
    glDeleteVertexArrays(1, &mesh->vao_id);

    free(mesh);
}
