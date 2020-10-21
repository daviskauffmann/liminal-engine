#include "point_light.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

float pk::point_light::near_plane = 1.0f;
float pk::point_light::far_plane = 25.0f;

pk::point_light::point_light(
    glm::vec3 position,
    glm::vec3 color,
    GLsizei depth_cube_size)
    : position(position),
      color(color)
{
    this->depth_cubemap_fbo_id = 0;
    this->depth_cubemap_texture_id = 0;
    set_depth_cube_size(depth_cube_size);
}

pk::point_light::~point_light()
{
    glDeleteFramebuffers(1, &depth_cubemap_fbo_id);
    glDeleteTextures(1, &depth_cubemap_texture_id);
}

void pk::point_light::set_depth_cube_size(GLsizei depth_cube_size)
{
    this->depth_cube_size = depth_cube_size;

    glDeleteFramebuffers(1, &depth_cubemap_fbo_id);
    glDeleteTextures(1, &depth_cubemap_texture_id);

    glGenFramebuffers(1, &depth_cubemap_fbo_id);
    glBindFramebuffer(GL_FRAMEBUFFER, depth_cubemap_fbo_id);
    {
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);

        {
            glGenTextures(1, &depth_cubemap_texture_id);
            glBindTexture(GL_TEXTURE_CUBE_MAP, depth_cubemap_texture_id);
            {
                for (unsigned int i = 0; i < 6; i++)
                {
                    glTexImage2D(
                        GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                        0,
                        GL_DEPTH_COMPONENT,
                        depth_cube_size,
                        depth_cube_size,
                        0,
                        GL_DEPTH_COMPONENT,
                        GL_FLOAT,
                        nullptr);
                }
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
            }
            glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

            glFramebufferTexture(
                GL_FRAMEBUFFER,
                GL_DEPTH_ATTACHMENT,
                depth_cubemap_texture_id,
                0);
        }

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            std::cout << "Error: Couldn't complete depth cubemap framebuffer" << std::endl;
        }
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void pk::point_light::update_transformation_matrices()
{
    glm::mat4 projection = glm::perspective(glm::radians(90.0f), 1.0f, near_plane, far_plane);

    transformation_matrices.clear();
    transformation_matrices.push_back(projection * glm::lookAt(position, position + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
    transformation_matrices.push_back(projection * glm::lookAt(position, position + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
    transformation_matrices.push_back(projection * glm::lookAt(position, position + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
    transformation_matrices.push_back(projection * glm::lookAt(position, position + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
    transformation_matrices.push_back(projection * glm::lookAt(position, position + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
    transformation_matrices.push_back(projection * glm::lookAt(position, position + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
}
