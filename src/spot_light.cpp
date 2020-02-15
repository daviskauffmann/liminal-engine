#include "spot_light.hpp"

#include <iostream>

#include <glm/gtc/matrix_transform.hpp>

namespace pk
{
spot_light::spot_light(
    glm::vec3 position,
    glm::vec3 direction,
    glm::vec3 color,
    float inner_cutoff,
    float outer_cutoff,
    int depth_map_size)
    : position(position),
      direction(direction),
      color(color),
      inner_cutoff(inner_cutoff),
      outer_cutoff(outer_cutoff)
{
    this->depth_map_fbo_id = 0;
    this->depth_map_texture_id = 0;
    set_depth_map_size(depth_map_size);
}

spot_light::~spot_light()
{
    glDeleteTextures(1, &depth_map_texture_id);
    glDeleteFramebuffers(1, &depth_map_fbo_id);
}

void spot_light::set_depth_map_size(int depth_map_size)
{
    this->depth_map_size = depth_map_size;

    glDeleteTextures(1, &depth_map_texture_id);
    glDeleteFramebuffers(1, &depth_map_fbo_id);

    glGenTextures(1, &depth_map_texture_id);
    glBindTexture(GL_TEXTURE_2D, depth_map_texture_id);
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
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenFramebuffers(1, &depth_map_fbo_id);
    glBindFramebuffer(GL_FRAMEBUFFER, depth_map_fbo_id);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_DEPTH_ATTACHMENT,
        GL_TEXTURE_2D,
        depth_map_texture_id,
        0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "Error: Couldn't complete depth map framebuffer" << std::endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

glm::mat4 spot_light::calc_projection() const
{
    glm::mat4 projection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    return projection;
}

glm::mat4 spot_light::calc_view() const
{
    glm::vec3 front = glm::normalize(direction);
    glm::vec3 target = position + front;
    glm::vec3 up(0.0f, 1.0f, 0.0f);
    glm::mat4 view = glm::lookAt(position, target, up);
    return view;
}
} // namespace pk
