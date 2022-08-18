#ifndef LIMINAL_COMPONENTS_TERRAIN_HPP
#define LIMINAL_COMPONENTS_TERRAIN_HPP

#include <glm/matrix.hpp>
#include <glm/vec3.hpp>
#include <memory>
#include <string>
#include <vector>

// TODO: this needs to be split up into a terrain asset, and this component should just be POD, with a pointer to the asset
// and instead of storing a position, use the transform component

struct SDL_Surface;

namespace liminal
{
    class assets;
    class mesh;

    class terrain
    {
    public:
        glm::vec3 position;
        float size;
        std::unique_ptr<liminal::mesh> mesh;

        terrain(const std::string &filename, const glm::vec3 &position, float size, float height_scale, std::shared_ptr<liminal::assets> assets);

        glm::mat4 get_model_matrix() const;

    private:
        float height_scale;

        float get_height(const SDL_Surface *surface, int x, int z) const;
    };
}

#endif
