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
    namespace core
    {
        class assets
        {
        public:
            assets();
            ~assets();
            assets(const assets &other) = delete;
            assets &operator=(const assets &other) = delete;
            assets(assets &&other) = delete;
            assets &operator=(assets &&other) = delete;

            std::shared_ptr<liminal::audio::sound> load_sound(const std::string &filename);
            std::shared_ptr<liminal::graphics::model> load_model(const std::string &filename, std::shared_ptr<liminal::core::assets> assets, bool flip_uvs = false);
            std::shared_ptr<liminal::graphics::skybox> load_skybox(const std::string &filename);
            std::shared_ptr<liminal::graphics::texture> load_texture(const std::string &filename, bool srgb = false, bool filter = true);

        private:
            std::unordered_map<std::string, std::shared_ptr<liminal::audio::sound>> sounds;
            std::unordered_map<std::string, std::shared_ptr<liminal::graphics::model>> models;
            std::unordered_map<std::string, std::shared_ptr<liminal::graphics::skybox>> skyboxes;
            std::unordered_map<std::string, std::shared_ptr<liminal::graphics::texture>> textures;
        };
    }
}

#endif
