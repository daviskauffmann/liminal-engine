#include <liminal/core/assets.hpp>

liminal::assets::assets()
{
}

liminal::assets::~assets()
{
    for (auto it = models.begin(); it != models.end(); it++)
    {
        delete it->second;
    }

    for (auto it = textures.begin(); it != textures.end(); it++)
    {
        delete it->second;
    }
}

liminal::model *liminal::assets::load_model(const std::string &filename, bool flip_uvs)
{
    if (models.find(filename) == models.end())
    {
        models[filename] = new liminal::model(filename, flip_uvs);
    }

    return models[filename];
}

liminal::skybox *liminal::assets::load_skybox(const std::string &filename)
{
    if (skyboxes.find(filename) == skyboxes.end())
    {
        skyboxes[filename] = new liminal::skybox(filename);
    }

    return skyboxes[filename];
}

liminal::texture *liminal::assets::load_texture(const std::string &filename)
{
    if (textures.find(filename) == textures.end())
    {
        textures[filename] = new liminal::texture(filename);
    }

    return textures[filename];
}
