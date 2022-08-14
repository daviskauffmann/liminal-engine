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
        static std::shared_ptr<liminal::sound> load_sound(const std::string &filename);
        static std::shared_ptr<liminal::model> load_model(const std::string &filename, bool flip_uvs = false); 
        static std::shared_ptr<liminal::skybox> load_skybox(const std::string &filename);
        static std::shared_ptr<liminal::texture> load_texture(const std::string &filename);

    private:
        inline static std::unordered_map<std::string, std::shared_ptr<liminal::sound>> sounds;
        inline static std::unordered_map<std::string, std::shared_ptr<liminal::model>> models;
        inline static std::unordered_map<std::string, std::shared_ptr<liminal::skybox>> skyboxes;
        inline static std::unordered_map<std::string, std::shared_ptr<liminal::texture>> textures;
    };
}

#endif
