#ifndef LIMINAL_GRAPHICS_MODEL_HPP
#define LIMINAL_GRAPHICS_MODEL_HPP

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <glm/matrix.hpp>
#include <liminal/graphics/mesh.hpp>
#include <liminal/graphics/program.hpp>
#include <liminal/graphics/texture.hpp>
#include <string>
#include <unordered_map>
#include <vector>

namespace liminal
{
    class model
    {
    public:
        model(liminal::mesh *mesh);
        model(const char *filename, bool flip_uvs = false);
        ~model();

        bool has_animations() const;
        unsigned int num_animations() const;
        std::vector<glm::mat4> calc_bone_transformations(unsigned int animation_index, unsigned int current_time) const;

        void draw_meshes(const liminal::program &program) const;

    private:
        std::string directory;
        Assimp::Importer importer;
        const aiScene *scene = nullptr;

        std::vector<liminal::mesh *> meshes;

        glm::mat4 global_inverse_transform = {};
        unsigned int num_bones = 0;
        std::unordered_map<std::string, unsigned int> bone_ids;
        std::vector<glm::mat4> bone_offsets;

        void process_node_meshes(const aiNode *node);
        liminal::mesh *create_mesh(const aiMesh *mesh);

        void process_node_animations(unsigned int animation_index, float animation_time, const aiNode *node, const glm::mat4 &parent_transformation, std::vector<glm::mat4> &bone_transformations) const;
        const aiNodeAnim *find_node_animation(const aiAnimation *animation, const std::string &node_name) const;
        aiVector3D calc_interpolated_position(float animation_time, const aiNodeAnim *node_animation) const;
        unsigned int find_position_index(float animation_time, const aiNodeAnim *node_animation) const;
        aiQuaternion calc_interpolated_rotation(float animation_time, const aiNodeAnim *node_animation) const;
        unsigned int find_rotation_index(float animation_time, const aiNodeAnim *node_animation) const;
        aiVector3D calc_interpolated_scale(float animation_time, const aiNodeAnim *node_animation) const;
        unsigned int find_scale_index(float animation_time, const aiNodeAnim *node_animation) const;
    };
}

#endif
