#include "directional_light.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <spdlog/spdlog.h>

float pk::directional_light::shadow_map_size = 10.0f;
float pk::directional_light::near_plane = -10.0f;
float pk::directional_light::far_plane = 10.0f;

pk::directional_light::directional_light(
    glm::vec3 direction,
    glm::vec3 color,
    GLsizei depth_map_size)
    : direction(direction),
      color(color)
{
    this->depth_map_fbo_id = 0;
    this->depth_map_texture_id = 0;
    set_depth_map_size(depth_map_size);
}

pk::directional_light::~directional_light()
{
    glDeleteFramebuffers(1, &depth_map_fbo_id);
    glDeleteTextures(1, &depth_map_texture_id);
}

void pk::directional_light::set_depth_map_size(GLsizei depth_map_size)
{
    this->depth_map_size = depth_map_size;

    glDeleteFramebuffers(1, &depth_map_fbo_id);
    glDeleteTextures(1, &depth_map_texture_id);

    glGenFramebuffers(1, &depth_map_fbo_id);
    glBindFramebuffer(GL_FRAMEBUFFER, depth_map_fbo_id);
    {
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);

        {
            glGenTextures(1, &depth_map_texture_id);
            glBindTexture(GL_TEXTURE_2D, depth_map_texture_id);
            {
                glTexImage2D(
                    GL_TEXTURE_2D,
                    0,
                    GL_DEPTH_COMPONENT,
                    depth_map_size,
                    depth_map_size,
                    0,
                    GL_DEPTH_COMPONENT,
                    GL_FLOAT,
                    nullptr);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
                GLfloat border_color[] = {1.0f, 1.0f, 1.0f, 1.0f};
                glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_color);
            }
            glBindTexture(GL_TEXTURE_2D, 0);

            glFramebufferTexture2D(
                GL_FRAMEBUFFER,
                GL_DEPTH_ATTACHMENT,
                GL_TEXTURE_2D,
                depth_map_texture_id,
                0);
        }

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            spdlog::error("Failed to create directional light depth map framebuffer");
            return;
        }
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void pk::directional_light::update_transformation_matrix(glm::vec3 center)
{
    glm::mat4 projection = glm::ortho(-shadow_map_size, shadow_map_size, -shadow_map_size, shadow_map_size, near_plane, far_plane);
    glm::mat4 view = glm::lookAt(
        center - direction,
        center,
        glm::vec3(0.0f, 1.0f, 0.0f));

    transformation_matrix = projection * view;
}
