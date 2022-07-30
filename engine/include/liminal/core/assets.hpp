#ifndef LIMINAL_CORE_ASSETS_HPP
#define LIMINAL_CORE_ASSETS_HPP

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
        ~assets();

        liminal::sound *load_sound(const std::string &filename);
        liminal::model *load_model(const std::string &filename, bool flip_uvs = false);
        liminal::skybox *load_skybox(const std::string &filename);
        liminal::texture *load_texture(const std::string &filename);

    private:
        std::unordered_map<std::string, liminal::sound *> sounds;
        std::unordered_map<std::string, liminal::model *> models;
        std::unordered_map<std::string, liminal::skybox *> skyboxes;
        std::unordered_map<std::string, liminal::texture *> textures;
    };
}

#endif
