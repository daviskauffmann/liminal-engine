#include <liminal/core/assets.hpp>

#include <liminal/audio/sound.hpp>
#include <liminal/graphics/model.hpp>
#include <liminal/graphics/skybox.hpp>
#include <liminal/graphics/texture.hpp>
#include <spdlog/spdlog.h>

liminal::core::assets::assets()
{
}

liminal::core::assets::~assets()
{
    spdlog::info("Unloading assets...");
    // spdlog::info("Unloaded {} sounds", sounds.size());
    spdlog::info("Unloaded {} models", models.size());
    spdlog::info("Unloaded {} skyboxes", skyboxes.size());
    spdlog::info("Unloaded {} textures", textures.size());
}

// std::shared_ptr<liminal::audio::sound> liminal::core::assets::load_sound(const std::string &filename)
// {
//     if (!sounds.contains(filename))
//     {
//         spdlog::info("Loading sound: {}", filename);
//         sounds.insert({filename, std::make_shared<liminal::sound>(filename.c_str())});
//     }

//     return sounds.at(filename);
// }

std::shared_ptr<liminal::graphics::model> liminal::core::assets::load_model(const std::string &filename, std::shared_ptr<liminal::core::assets> assets, const bool flip_uvs)
{
    if (!models.contains(filename))
    {
        spdlog::info("Loading model: {}", filename);
        models.insert({filename, std::make_shared<liminal::graphics::model>(filename.c_str(), assets, flip_uvs)}); // TODO: why not use `this` for assets ptr?
    }

    return models.at(filename);
}

std::shared_ptr<liminal::graphics::skybox> liminal::core::assets::load_skybox(const std::string &filename)
{
    if (!skyboxes.contains(filename))
    {
        spdlog::info("Loading skybox: {}", filename);
        skyboxes.insert({filename, std::make_shared<liminal::graphics::skybox>(filename.c_str())});
    }

    return skyboxes.at(filename);
}

std::shared_ptr<liminal::graphics::texture> liminal::core::assets::load_texture(const std::string &filename, const bool srgb, const bool filter)
{
    if (!textures.contains(filename))
    {
        spdlog::info("Loading texture: {}", filename);
        textures.insert({filename, std::make_shared<liminal::graphics::texture>(filename.c_str(), srgb, filter)});
    }

    return textures.at(filename);
}
