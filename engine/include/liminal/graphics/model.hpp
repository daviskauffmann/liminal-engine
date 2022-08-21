#ifndef LIMINAL_GRAPHICS_MODEL_HPP
#define LIMINAL_GRAPHICS_MODEL_HPP

#include "mesh.hpp"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <glm/matrix.hpp>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace liminal
{
    class assets;
    class program;

    class model
    {
    public:
        model(liminal::mesh *mesh);
        model(const char *filename, std::shared_ptr<liminal::assets> assets, bool flip_uvs = false);
        ~model();
        model(const model &other) = delete;
        model &operator=(const model &other) = delete;
        model(model &&other) = delete;
        model &operator=(model &&other) = delete;

        bool has_animations() const;
        unsigned int num_animations() const;
        std::vector<glm::mat4> calc_bone_transformations(unsigned int animation_index, std::uint64_t current_time) const;

        void draw_meshes(const liminal::program &program) const;

    private:
        std::string directory;
        Assimp::Importer importer;
        const aiScene *ai_scene = nullptr;

        std::vector<liminal::mesh *> meshes;

        glm::mat4 global_inverse_transform = {};
        unsigned int num_bones = 0;
        std::unordered_map<std::string, unsigned int> bone_ids;
        std::vector<glm::mat4> bone_offsets;

        void process_node_meshes(const aiNode *ai_node, std::shared_ptr<liminal::assets> assets);

        void process_node_animations(unsigned int animation_index, float animation_time, const aiNode *ai_node, const glm::mat4 &parent_transformation, std::vector<glm::mat4> &bone_transformations) const;
        const aiNodeAnim *find_node_animation(const aiAnimation *ai_animation, const std::string &node_name) const;
        aiVector3D calc_interpolated_position(float animation_time, const aiNodeAnim *ai_node_anim) const;
        unsigned int find_position_index(float animation_time, const aiNodeAnim *ai_node_anim) const;
        aiQuaternion calc_interpolated_rotation(float animation_time, const aiNodeAnim *ai_node_anim) const;
        unsigned int find_rotation_index(float animation_time, const aiNodeAnim *ai_node_anim) const;
        aiVector3D calc_interpolated_scale(float animation_time, const aiNodeAnim *ai_node_anim) const;
        unsigned int find_scale_index(float animation_time, const aiNodeAnim *ai_node_anim) const;
    };
}

#endif
