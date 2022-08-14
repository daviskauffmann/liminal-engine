#include <liminal/core/assets.hpp>

#include <liminal/audio/sound.hpp>
#include <liminal/graphics/model.hpp>
#include <liminal/graphics/skybox.hpp>
#include <liminal/graphics/texture.hpp>
#include <spdlog/spdlog.h>

std::shared_ptr<liminal::sound> liminal::assets::load_sound(const std::string &filename)
{
    if (!sounds.contains(filename))
    {
        spdlog::info("Loading sound: {}", filename);
        sounds.insert({filename, std::make_shared<liminal::sound>(filename.c_str())});
    }

    return sounds.at(filename);
}

std::shared_ptr<liminal::model> liminal::assets::load_model(const std::string &filename, const bool flip_uvs)
{
    if (!models.contains(filename))
    {
        spdlog::info("Loading model: {}", filename);
        models.insert({filename, std::make_shared<liminal::model>(filename.c_str(), flip_uvs)});
    }

    return models.at(filename);
}

std::shared_ptr<liminal::skybox> liminal::assets::load_skybox(const std::string &filename)
{
    if (!skyboxes.contains(filename))
    {
        spdlog::info("Loading skybox: {}", filename);
        skyboxes.insert({filename, std::make_shared<liminal::skybox>(filename.c_str())});
    }

    return skyboxes.at(filename);
}

std::shared_ptr<liminal::texture> liminal::assets::load_texture(const std::string &filename)
{
    if (!textures.contains(filename))
    {
        spdlog::info("Loading texture: {}", filename);
        textures.insert({filename, std::make_shared<liminal::texture>(filename.c_str())});
    }

    return textures.at(filename);
}
