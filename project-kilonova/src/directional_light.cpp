#include "directional_light.hpp"

#include <iostream>

#include <glm/gtc/matrix_transform.hpp>

namespace pk
{
directional_light::directional_light(
    glm::vec3 direction,
    glm::vec3 color)
    : direction(direction), color(color)
{
    glGenFramebuffers(1, &this->depthmap_fbo_id);
    glBindFramebuffer(GL_FRAMEBUFFER, this->depthmap_fbo_id);

    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    glGenTextures(1, &this->depthmap_texture_id);
    glBindTexture(GL_TEXTURE_2D, this->depthmap_texture_id);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_DEPTH_COMPONENT,
        4096,
        4096,
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

    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_DEPTH_ATTACHMENT,
        GL_TEXTURE_2D,
        this->depthmap_texture_id,
        0);

    glBindTexture(GL_TEXTURE_2D, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "Error: Couldn't complete depthmap framebuffer" << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

directional_light::~directional_light()
{
}

glm::mat4 directional_light::calc_projection() const
{
    glm::mat4 projection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f);

    return projection;
}

glm::mat4 directional_light::calc_view(glm::vec3 center) const
{
    glm::vec3 position = glm::vec3(0.0f) - this->direction;
    glm::mat4 view = glm::lookAt(position, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    return view;
}
} // namespace pk
