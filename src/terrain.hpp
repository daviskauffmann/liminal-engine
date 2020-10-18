#ifndef TERRAIN_HPP
#define TERRAIN_HPP

#include <glm/vec3.hpp>
#include <glm/matrix.hpp>
#include <SDL2/SDL.h>

#include "mesh.hpp"

namespace pk
{
    struct terrain
    {
        static float size;
        static float height_scale;

        glm::vec3 position;
        pk::mesh *mesh;

        terrain(glm::vec3 position, const std::string &heightmap_filename);
        ~terrain();

        glm::mat4 calc_model() const;

    private:
        float get_height(SDL_Surface *surface, int x, int z) const;
    };
} // namespace pk

#endif
