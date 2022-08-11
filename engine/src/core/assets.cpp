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

const liminal::sound *liminal::assets::load_sound(const std::string &filename)
{
    if (sounds.find(filename) == sounds.end())
    {
        sounds.insert({filename, std::make_unique<liminal::sound>(filename.c_str())});
    }

    return sounds.at(filename).get();
}

const liminal::model *liminal::assets::load_model(const std::string &filename, const bool flip_uvs)
{
    if (models.find(filename) == models.end())
    {
        models.insert({filename, std::make_unique<liminal::model>(filename.c_str(), flip_uvs)});
    }

    return models.at(filename).get();
}

const liminal::skybox *liminal::assets::load_skybox(const std::string &filename)
{
    if (skyboxes.find(filename) == skyboxes.end())
    {
        skyboxes.insert({filename, std::make_unique<liminal::skybox>(filename.c_str())});
    }

    return skyboxes.at(filename).get();
}

const liminal::texture *liminal::assets::load_texture(const std::string &filename)
{
    if (textures.find(filename) == textures.end())
    {
        textures.insert({filename, std::make_unique<liminal::texture>(filename.c_str())});
    }

    return textures.at(filename).get();
}
