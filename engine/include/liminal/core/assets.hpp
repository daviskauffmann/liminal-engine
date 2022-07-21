#ifndef LIMINAL_CORE_ASSETS_HPP
#define LIMINAL_CORE_ASSETS_HPP

#include <liminal/graphics/model.hpp>
#include <liminal/graphics/skybox.hpp>
#include <liminal/graphics/texture.hpp>
#include <unordered_map>

namespace liminal
{
    // TODO: templated load methods
    class assets
    {
    public:
        assets();
        ~assets();

        liminal::model *load_model(const std::string &filename, bool flip_uvs);
        liminal::skybox *load_skybox(const std::string &filename);
        liminal::texture *load_texture(const std::string &filename);

    private:
        std::unordered_map<std::string, liminal::model *> models;
        std::unordered_map<std::string, liminal::skybox *> skyboxes;
        std::unordered_map<std::string, liminal::texture *> textures;
    };
}

#endif
