#ifndef MODEL_HPP
#define MODEL_HPP

#include <assimp/scene.h>
#include <string>
#include <unordered_map>
#include <vector>

#include "mesh.hpp"
#include "texture.hpp"

namespace pk
{
    struct model
    {
    public:
        std::vector<pk::mesh *> meshes;

        model(const std::string &filename, bool flip_uvs = false);
        ~model();

        void draw() const;

    private:
        std::string directory;
        mutable std::unordered_map<std::string, pk::texture *> loaded_textures;

        void process_node(aiNode *node, const aiScene *scene);
        pk::mesh *process_mesh(aiMesh *mesh, const aiScene *scene) const;
        std::vector<pk::texture *> load_material_textures(aiMaterial *material, aiTextureType type) const;
    };
} // namespace pk

#endif
