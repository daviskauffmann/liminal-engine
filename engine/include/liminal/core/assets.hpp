#ifndef LIMINAL_CORE_ASSETS_HPP
#define LIMINAL_CORE_ASSETS_HPP

#include "../audio/sound.hpp"
#include "../graphics/model.hpp"
#include "../graphics/skybox.hpp"
#include "../graphics/texture.hpp"
#include <memory>
#include <string>
#include <unordered_map>

namespace liminal
{
    class assets
    {
    public:
        assets();
        assets(assets &&other) = delete;
        assets(const assets &other) = delete;
        assets &operator=(assets &&other) = delete;
        assets &operator=(const assets &other) = delete;
        ~assets();

        std::shared_ptr<liminal::sound> load_sound(const std::string &filename);
        std::shared_ptr<liminal::model> load_model(const std::string &filename, std::shared_ptr<liminal::assets> assets, bool flip_uvs = false);
        std::shared_ptr<liminal::skybox> load_skybox(const std::string &filename);
        std::shared_ptr<liminal::texture> load_texture(const std::string &filename, bool srgb = false, bool filter = true);

    private:
        std::unordered_map<std::string, std::shared_ptr<liminal::sound>> sounds;
        std::unordered_map<std::string, std::shared_ptr<liminal::model>> models;
        std::unordered_map<std::string, std::shared_ptr<liminal::skybox>> skyboxes;
        std::unordered_map<std::string, std::shared_ptr<liminal::texture>> textures;
    };
}

#endif
