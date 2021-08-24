#ifndef MODEL_HPP
#define MODEL_HPP

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <glm/matrix.hpp>
#include <string>
#include <unordered_map>
#include <vector>

#include "mesh.hpp"
#include "program.hpp"
#include "texture.hpp"

namespace liminal
{
    struct bone
    {
        glm::mat4 offset;
        glm::mat4 transformation;
    };

    struct model
    {
    public:
        std::vector<glm::mat4> bone_transformations;

        model(const std::string &filename, bool flip_uvs = false);
        ~model();

        bool has_animations() const;
        unsigned int num_animations() const;
        void set_animation(unsigned int index);
        void update_bone_transformations(unsigned int animation_index, unsigned int current_time);

        void draw_meshes(liminal::program *program) const;

    private:
        std::string directory;
        Assimp::Importer importer;
        const aiScene *scene;

        std::vector<liminal::mesh *> meshes;

        glm::mat4 global_inverse_transform;
        unsigned int num_bones;
        std::vector<bone> bones;
        std::unordered_map<std::string, unsigned int> bone_indices;

        std::unordered_map<std::string, liminal::texture *> loaded_textures;

        void process_node_meshes(const aiNode *node, const aiScene *scene);
        liminal::mesh *create_mesh(const aiMesh *mesh, const aiScene *scene);

        void process_node_animations(unsigned int animation_index, float animation_time, const aiNode *node, const glm::mat4 &parent_transformation);
        const aiNodeAnim *find_node_animation(const aiAnimation *animation, const std::string node_name);
        void calc_interpolated_position(aiVector3D &out, float animation_time, const aiNodeAnim *node_animation);
        unsigned int find_position_index(float animation_time, const aiNodeAnim *node_animation);
        void calc_interpolated_rotation(aiQuaternion &out, float animation_time, const aiNodeAnim *node_animation);
        unsigned int find_rotation_index(float animation_time, const aiNodeAnim *node_animation);
        void calc_interpolated_scale(aiVector3D &out, float animation_time, const aiNodeAnim *node_animation);
        unsigned int find_scale_index(float animation_time, const aiNodeAnim *node_animation);
    };
}

#endif
