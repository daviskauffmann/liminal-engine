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
    class sound;
    class model;
    class skybox;
    class texture;

    class assets
    {
    public:
        static assets *instance;

        assets();

        const liminal::sound *load_sound(const std::string &filename);
        const liminal::model *load_model(const std::string &filename, bool flip_uvs = false); 
        const liminal::skybox *load_skybox(const std::string &filename);
        const liminal::texture *load_texture(const std::string &filename);

    private:
        std::unordered_map<std::string, std::unique_ptr<liminal::sound>> sounds;
        std::unordered_map<std::string, std::unique_ptr<liminal::model>> models;
        std::unordered_map<std::string, std::unique_ptr<liminal::skybox>> skyboxes;
        std::unordered_map<std::string, std::unique_ptr<liminal::texture>> textures;
    };
}

#endif
