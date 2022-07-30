#include <liminal/core/assets.hpp>

#include <liminal/audio/sound.hpp>
#include <liminal/graphics/model.hpp>
#include <liminal/graphics/skybox.hpp>
#include <liminal/graphics/texture.hpp>

liminal::assets *liminal::assets::instance = nullptr;

liminal::assets::assets()
{
    instance = this;
}

liminal::assets::~assets()
{
    for (const auto [filename, sound] : sounds)
    {
        delete sound;
    }

    for (const auto [filename, model] : models)
    {
        delete model;
    }

    for (const auto [filename, skybox] : skyboxes)
    {
        delete skybox;
    }

    for (const auto [filename, texture] : textures)
    {
        delete texture;
    }
}

liminal::sound *liminal::assets::load_sound(const char *const filename)
{
    if (sounds.find(filename) == sounds.end())
    {
        sounds.insert({filename, new liminal::sound(filename)});
    }

    return sounds.at(filename);
}

liminal::model *liminal::assets::load_model(const char *const filename, const bool flip_uvs)
{
    if (models.find(filename) == models.end())
    {
        models.insert({filename, new liminal::model(filename, flip_uvs)});
    }

    return models.at(filename);
}

liminal::skybox *liminal::assets::load_skybox(const char *const filename)
{
    if (skyboxes.find(filename) == skyboxes.end())
    {
        skyboxes.insert({filename, new liminal::skybox(filename)});
    }

    return skyboxes.at(filename);
}

liminal::texture *liminal::assets::load_texture(const std::string &filename)
{
    if (textures.find(filename) == textures.end())
    {
        textures.insert({filename, new liminal::texture(filename)});
    }

    return textures.at(filename);
}
