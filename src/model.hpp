#ifndef MODEL_HPP
#define MODEL_HPP

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <glm/matrix.hpp>
#include <string>
#include <unordered_map>
#include <vector>

#include "mesh.hpp"
#include "texture.hpp"

namespace pk
{
    struct bone_info
    {
        glm::mat4 offset;
        glm::mat4 transformation;
    };

    struct model
    {
    public:
        std::vector<pk::mesh *> meshes;

        model(const std::string &filename, bool flip_uvs = false);
        ~model();

        std::vector<glm::mat4> calc_bone_transformations(unsigned int current_time);

        void draw() const;

    private:
        std::string directory;
        Assimp::Importer importer;
        const aiScene *scene;
        glm::mat4 global_inverse_transform;
        unsigned int num_bones;
        std::unordered_map<std::string, unsigned int> loaded_bone_indexes;
        std::vector<bone_info> bone_infos;
        std::unordered_map<std::string, pk::texture *> loaded_textures;

        void process_node(aiNode *node, const aiScene *scene);
        pk::mesh *process_mesh(aiMesh *mesh, const aiScene *scene);

        void read_node_heirarchy(float animation_time, const aiNode *node, const glm::mat4 &parent_transformation);
        const aiNodeAnim *find_node_animation(const aiAnimation *animation, const std::string node_name);
        void calc_interpolated_position(aiVector3D &out, float animation_time, const aiNodeAnim *node_animation);
        void calc_interpolated_rotation(aiQuaternion &out, float animation_time, const aiNodeAnim *node_animation);
        void calc_interpolated_scale(aiVector3D &out, float animation_time, const aiNodeAnim *node_animation);
    };
} // namespace pk

#endif
