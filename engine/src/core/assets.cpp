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

std::shared_ptr<liminal::sound> liminal::assets::load_sound(const std::string &filename)
{
    if (!sounds.contains(filename))
    {
        sounds.insert({filename, std::make_unique<liminal::sound>(filename.c_str())});
    }

    return sounds.at(filename);
}

std::shared_ptr<liminal::model> liminal::assets::load_model(const std::string &filename, const bool flip_uvs)
{
    if (!models.contains(filename))
    {
        models.insert({filename, std::make_unique<liminal::model>(filename.c_str(), flip_uvs)});
    }

    return models.at(filename);
}

std::shared_ptr<liminal::skybox> liminal::assets::load_skybox(const std::string &filename)
{
    if (!skyboxes.contains(filename))
    {
        skyboxes.insert({filename, std::make_unique<liminal::skybox>(filename.c_str())});
    }

    return skyboxes.at(filename);
}

std::shared_ptr<liminal::texture> liminal::assets::load_texture(const std::string &filename)
{
    if (!textures.contains(filename))
    {
        textures.insert({filename, std::make_unique<liminal::texture>(filename.c_str())});
    }

    return textures.at(filename);
}
