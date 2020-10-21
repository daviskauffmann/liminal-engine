#include "skybox.hpp"

#include <glm/matrix.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <stb_image.h>
#include <vector>

#include "program.hpp"

constexpr GLsizei environment_size = 4096;
constexpr GLsizei irradiance_size = 32;
constexpr GLsizei prefilter_size = 128;

pk::skybox::skybox(const std::string &filename)
{
    this->environment_cubemap_id = 0;
    this->irradiance_cubemap_id = 0;
    this->prefilter_cubemap_id = 0;
    set_cubemap(filename);
}

pk::skybox::~skybox()
{
    glDeleteTextures(1, &environment_cubemap_id);
    glDeleteTextures(1, &irradiance_cubemap_id);
    glDeleteTextures(1, &prefilter_cubemap_id);
}

void pk::skybox::set_cubemap(const std::string &filename)
{
    glDeleteTextures(1, &environment_cubemap_id);
    glDeleteTextures(1, &irradiance_cubemap_id);
    glDeleteTextures(1, &prefilter_cubemap_id);

    // setup capture mesh
    std::vector<float> capture_vertices =
        {-1.0f, +1.0f, -1.0f,
         -1.0f, -1.0f, -1.0f,
         +1.0f, -1.0f, -1.0f,
         +1.0f, -1.0f, -1.0f,
         +1.0f, +1.0f, -1.0f,
         -1.0f, +1.0f, -1.0f,

         -1.0f, -1.0f, +1.0f,
         -1.0f, -1.0f, -1.0f,
         -1.0f, +1.0f, -1.0f,
         -1.0f, +1.0f, -1.0f,
         -1.0f, +1.0f, +1.0f,
         -1.0f, -1.0f, +1.0f,

         +1.0f, -1.0f, -1.0f,
         +1.0f, -1.0f, +1.0f,
         +1.0f, +1.0f, +1.0f,
         +1.0f, +1.0f, +1.0f,
         +1.0f, +1.0f, -1.0f,
         +1.0f, -1.0f, -1.0f,

         -1.0f, -1.0f, +1.0f,
         -1.0f, +1.0f, +1.0f,
         +1.0f, +1.0f, +1.0f,
         +1.0f, +1.0f, +1.0f,
         +1.0f, -1.0f, +1.0f,
         -1.0f, -1.0f, +1.0f,

         -1.0f, +1.0f, -1.0f,
         +1.0f, +1.0f, -1.0f,
         +1.0f, +1.0f, +1.0f,
         +1.0f, +1.0f, +1.0f,
         -1.0f, +1.0f, +1.0f,
         -1.0f, +1.0f, -1.0f,

         -1.0f, -1.0f, -1.0f,
         -1.0f, -1.0f, +1.0f,
         +1.0f, -1.0f, -1.0f,
         +1.0f, -1.0f, -1.0f,
         -1.0f, -1.0f, +1.0f,
         +1.0f, -1.0f, +1.0f};
    GLsizei capture_vertices_size = (GLsizei)(capture_vertices.size() * sizeof(float));
    GLuint capture_vao_id;
    GLuint capture_vbo_id;

    glGenVertexArrays(1, &capture_vao_id);
    glBindVertexArray(capture_vao_id);
    {
        glGenBuffers(1, &capture_vbo_id);
        glBindBuffer(GL_ARRAY_BUFFER, capture_vbo_id);
        glBufferData(GL_ARRAY_BUFFER, capture_vertices_size, capture_vertices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid *)(0 * sizeof(GLfloat)));

        glEnableVertexAttribArray(0);
    }
    glBindVertexArray(0);

    // setup capture matrices
    glm::mat4 capture_projection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    glm::mat4 capture_mvps[] =
        {capture_projection * glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
         capture_projection * glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
         capture_projection * glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
         capture_projection * glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
         capture_projection * glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
         capture_projection * glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f))};

    // setup capture fbo
    GLuint capture_fbo_id;
    GLuint capture_rbo_id;

    glGenFramebuffers(1, &capture_fbo_id);
    glBindFramebuffer(GL_FRAMEBUFFER, capture_fbo_id);
    {
        {
            glGenRenderbuffers(1, &capture_rbo_id);
            glBindRenderbuffer(GL_RENDERBUFFER, capture_rbo_id);
            {
                glRenderbufferStorage(
                    GL_RENDERBUFFER,
                    GL_DEPTH_COMPONENT24,
                    environment_size,
                    environment_size);
            }
            glBindRenderbuffer(GL_RENDERBUFFER, 0);

            glFramebufferRenderbuffer(
                GL_FRAMEBUFFER,
                GL_DEPTH_ATTACHMENT,
                GL_RENDERBUFFER,
                capture_rbo_id);
        }

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            std::cout << "Error: Couldn't complete skybox capture framebuffer" << std::endl;
        }
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // create environment cubemap from equirectangular texture
    GLuint equirectangular_texture_id;

    glGenTextures(1, &equirectangular_texture_id);
    glBindTexture(GL_TEXTURE_2D, equirectangular_texture_id);
    {
        stbi_set_flip_vertically_on_load(true);
        int width, height, num_components;
        float *image = stbi_loadf(filename.c_str(), &width, &height, &num_components, 0);
        std::cout << "Loaded: " << filename << std::endl;

        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RGB16F,
            width,
            height,
            0,
            GL_RGB,
            GL_FLOAT,
            image);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(image);
    }
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenTextures(1, &environment_cubemap_id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, environment_cubemap_id);
    {
        for (unsigned int i = 0; i < 6; i++)
        {
            glTexImage2D(
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0,
                GL_RGB16F,
                environment_size,
                environment_size,
                0,
                GL_RGB,
                GL_FLOAT,
                nullptr);
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    pk::program *equirectangular_to_cubemap_program = new pk::program(
        "assets/shaders/cubemap.vs",
        "assets/shaders/equirectangular_to_cubemap.fs");

    glBindFramebuffer(GL_FRAMEBUFFER, capture_fbo_id);
    {
        glViewport(0, 0, environment_size, environment_size);

        equirectangular_to_cubemap_program->bind();
        {
            equirectangular_to_cubemap_program->set_int("equirectangular_map", 0);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, equirectangular_texture_id);

            for (unsigned int i = 0; i < 6; i++)
            {
                glFramebufferTexture2D(
                    GL_FRAMEBUFFER,
                    GL_COLOR_ATTACHMENT0,
                    GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                    environment_cubemap_id,
                    0);

                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                equirectangular_to_cubemap_program->set_mat4("mvp", capture_mvps[i]);

                glBindVertexArray(capture_vao_id);
                glDrawArrays(GL_TRIANGLES, 0, capture_vertices_size);
                glBindVertexArray(0);
            }
        }
        equirectangular_to_cubemap_program->unbind();
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    delete equirectangular_to_cubemap_program;

    glBindTexture(GL_TEXTURE_CUBE_MAP, environment_cubemap_id);
    {
        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    }
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    glDeleteTextures(1, &equirectangular_texture_id);

    // create irradiance cubemap from environment cubemap
    glGenTextures(1, &irradiance_cubemap_id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, irradiance_cubemap_id);
    {
        for (unsigned int i = 0; i < 6; i++)
        {
            glTexImage2D(
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0,
                GL_RGB16F,
                32,
                32,
                0,
                GL_RGB,
                GL_FLOAT,
                nullptr);
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, capture_fbo_id);
    {
        glBindRenderbuffer(GL_RENDERBUFFER, capture_rbo_id);
        {
            // resize the capture rbo
            glRenderbufferStorage(
                GL_RENDERBUFFER,
                GL_DEPTH_COMPONENT24,
                irradiance_size,
                irradiance_size);
        }
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    pk::program *irradiance_convolution_program = new pk::program(
        "assets/shaders/cubemap.vs",
        "assets/shaders/irradiance_convolution.fs");

    glBindFramebuffer(GL_FRAMEBUFFER, capture_fbo_id);
    {
        glViewport(0, 0, irradiance_size, irradiance_size);

        irradiance_convolution_program->bind();
        {
            irradiance_convolution_program->set_int("environment_cubemap", 0);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, environment_cubemap_id);

            for (unsigned int i = 0; i < 6; i++)
            {
                glFramebufferTexture2D(
                    GL_FRAMEBUFFER,
                    GL_COLOR_ATTACHMENT0,
                    GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                    irradiance_cubemap_id,
                    0);

                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                irradiance_convolution_program->set_mat4("mvp", capture_mvps[i]);

                glBindVertexArray(capture_vao_id);
                glDrawArrays(GL_TRIANGLES, 0, capture_vertices_size);
                glBindVertexArray(0);
            }
        }
        irradiance_convolution_program->unbind();
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    delete irradiance_convolution_program;

    // create prefilter cubemap from environment cubemap
    glGenTextures(1, &prefilter_cubemap_id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, prefilter_cubemap_id);
    {
        for (unsigned int i = 0; i < 6; i++)
        {
            glTexImage2D(
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0,
                GL_RGB16F,
                prefilter_size,
                prefilter_size,
                0,
                GL_RGB,
                GL_FLOAT,
                nullptr);
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    }
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    pk::program *prefilter_convolution_program = new pk::program(
        "assets/shaders/cubemap.vs",
        "assets/shaders/prefilter_convolution.fs");

    glBindFramebuffer(GL_FRAMEBUFFER, capture_fbo_id);
    {
        prefilter_convolution_program->bind();
        {
            prefilter_convolution_program->set_mat4("capture.projection", capture_projection);
            prefilter_convolution_program->set_int("environment_cubemap", 0);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, environment_cubemap_id);

            const unsigned int max_mip_levels = 5;
            for (unsigned int mip_level = 0; mip_level < max_mip_levels; mip_level++)
            {
                unsigned int mip_width = (unsigned int)(prefilter_size * std::pow(0.5f, (float)mip_level));
                unsigned int mip_height = (unsigned int)(prefilter_size * std::pow(0.5f, (float)mip_level));

                glBindRenderbuffer(GL_RENDERBUFFER, capture_rbo_id);
                {
                    // resize capture rbo
                    glRenderbufferStorage(
                        GL_RENDERBUFFER,
                        GL_DEPTH_COMPONENT24,
                        mip_width,
                        mip_height);
                }
                glBindRenderbuffer(GL_RENDERBUFFER, 0);

                glViewport(0, 0, mip_width, mip_height);

                prefilter_convolution_program->set_float("roughness", (float)mip_level / (float)(max_mip_levels - 1));

                for (unsigned int i = 0; i < 6; ++i)
                {
                    glFramebufferTexture2D(
                        GL_FRAMEBUFFER,
                        GL_COLOR_ATTACHMENT0,
                        GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                        prefilter_cubemap_id,
                        mip_level);

                    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                    prefilter_convolution_program->set_mat4("mvp", capture_mvps[i]);

                    glBindVertexArray(capture_vao_id);
                    glDrawArrays(GL_TRIANGLES, 0, capture_vertices_size);
                    glBindVertexArray(0);
                }
            }
        }
        prefilter_convolution_program->unbind();
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    delete prefilter_convolution_program;

    // cleanup
    glDeleteFramebuffers(1, &capture_fbo_id);
    glDeleteRenderbuffers(1, &capture_rbo_id);

    glDeleteVertexArrays(1, &capture_vao_id);
    glDeleteBuffers(1, &capture_vbo_id);
}
