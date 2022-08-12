#include <liminal/graphics/model.hpp>

#include <assimp/postprocess.h>
#include <filesystem>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <liminal/core/assets.hpp>
#include <spdlog/spdlog.h>

static inline glm::vec3 vec3_cast(const aiVector3D &v) { return {v.x, v.y, v.z}; }
static inline glm::vec2 vec2_cast(const aiVector3D &v) { return {v.x, v.y}; }
static inline glm::quat quat_cast(const aiQuaternion &q) { return {q.w, q.x, q.y, q.z}; }
static inline glm::mat4 mat4_cast(const aiMatrix4x4 &m) { return glm::transpose(glm::make_mat4(&m.a1)); }
static inline glm::mat4 mat4_cast(const aiMatrix3x3 &m) { return glm::transpose(glm::make_mat3(&m.a1)); }

liminal::model::model(liminal::mesh *const mesh)
{
    meshes.push_back(mesh);
}

liminal::model::model(const char *const filename, const bool flip_uvs)
    : directory(std::filesystem::path(filename).parent_path().string())
{
    unsigned int flags = aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices | aiProcess_Triangulate | aiProcess_GenSmoothNormals;
    if (flip_uvs)
    {
        flags |= aiProcess_FlipUVs;
    }
    scene = importer.ReadFile(filename, flags);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        spdlog::error("Failed to load model: {}", importer.GetErrorString());
        return;
    }

    global_inverse_transform = glm::inverse(mat4_cast(scene->mRootNode->mTransformation));

    process_node_meshes(scene->mRootNode);
}

liminal::model::~model()
{
    for (const auto mesh : meshes)
    {
        delete mesh;
    }
}

bool liminal::model::has_animations() const
{
    return scene ? scene->HasAnimations() : false;
}

unsigned int liminal::model::num_animations() const
{
    return scene ? scene->mNumAnimations : 0;
}

std::vector<glm::mat4> liminal::model::calc_bone_transformations(const unsigned int animation_index, const unsigned int current_time) const
{
    std::vector<glm::mat4> bone_transformations;

    if (scene && scene->mNumAnimations > 0 && animation_index >= 0 && animation_index < scene->mNumAnimations)
    {
        bone_transformations.resize(num_bones);

        const auto ticks_per_second = scene->mAnimations[animation_index]->mTicksPerSecond != 0 ? static_cast<float>(scene->mAnimations[animation_index]->mTicksPerSecond) : 25.0f;
        const auto time_in_ticks = ticks_per_second * (current_time / 1000.0f);
        const auto animation_time = static_cast<float>(fmod(time_in_ticks, scene->mAnimations[animation_index]->mDuration));

        process_node_animations(animation_index, animation_time, scene->mRootNode, glm::identity<glm::mat4>(), bone_transformations);
    }

    return bone_transformations;
}

void liminal::model::draw_meshes(const liminal::program &program) const
{
    for (const auto mesh : meshes)
    {
        mesh->draw(program);
    }
}

void liminal::model::process_node_meshes(const aiNode *const node)
{
    for (std::size_t mesh_index = 0; mesh_index < node->mNumMeshes; mesh_index++)
    {
        const auto mesh = scene->mMeshes[node->mMeshes[mesh_index]];

        std::vector<liminal::vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<std::vector<std::shared_ptr<liminal::texture>>> textures;

        // process vertices
        for (std::size_t vertex_index = 0; vertex_index < mesh->mNumVertices; vertex_index++)
        {
            liminal::vertex vertex;

            if (mesh->HasPositions())
            {
                vertex.position = vec3_cast(mesh->mVertices[vertex_index]);
            }

            if (mesh->HasNormals())
            {
                vertex.normal = vec3_cast(mesh->mNormals[vertex_index]);
            }

            if (mesh->HasTextureCoords(0))
            {
                vertex.uv = vec2_cast(mesh->mTextureCoords[0][vertex_index]);
            }

            if (mesh->HasTangentsAndBitangents())
            {
                vertex.tangent = vec3_cast(mesh->mTangents[vertex_index]);
                vertex.bitangent = vec3_cast(mesh->mBitangents[vertex_index]);
            }

            for (std::size_t bone_index = 0; bone_index < liminal::vertex::num_bones; bone_index++)
            {
                vertex.bone_ids.at(bone_index) = liminal::vertex::num_bones;
                vertex.bone_weights.at(bone_index) = 0;
            }

            vertices.push_back(vertex);
        }

        // process bones
        if (mesh->HasBones())
        {
            for (std::size_t bone_index = 0; bone_index < mesh->mNumBones; bone_index++)
            {
                const std::string bone_name(mesh->mBones[bone_index]->mName.data);

                if (bone_ids.find(bone_name) == bone_ids.end())
                {
                    bone_ids.insert({bone_name, num_bones++});
                    bone_offsets.push_back(mat4_cast(mesh->mBones[bone_index]->mOffsetMatrix));
                }

                for (std::size_t weight_index = 0; weight_index < mesh->mBones[bone_index]->mNumWeights; weight_index++)
                {
                    const auto vertex_id = mesh->mBones[bone_index]->mWeights[weight_index].mVertexId;
                    const auto weight = mesh->mBones[bone_index]->mWeights[weight_index].mWeight;
                    vertices.at(vertex_id).add_bone_data(bone_ids.at(bone_name), weight);
                }
            }
        }

        // process indices
        if (mesh->HasFaces())
        {
            for (std::size_t face_index = 0; face_index < mesh->mNumFaces; face_index++)
            {
                const auto face = mesh->mFaces[face_index];
                for (std::size_t index_index = 0; index_index < face.mNumIndices; index_index++)
                {
                    indices.push_back(face.mIndices[index_index]);
                }
            }
        }

        // process textures
        if (scene->HasMaterials())
        {
            auto scene_material = scene->mMaterials[mesh->mMaterialIndex];
            for (auto type = aiTextureType_NONE; type <= AI_TEXTURE_TYPE_MAX; type = (aiTextureType)(type + 1))
            {
                std::vector<std::shared_ptr<liminal::texture>> material_textures;

                for (unsigned int texture_index = 0; texture_index < scene_material->GetTextureCount(type); texture_index++)
                {
                    aiString path;
                    scene_material->GetTexture(type, texture_index, &path);
                    const auto filename = directory + "/" + path.C_Str();
                    material_textures.push_back(liminal::assets::instance->load_texture(filename));
                }

                textures.push_back(material_textures);
            }
        }

        // process animations
        if (scene->HasAnimations())
        {
            // TODO: store animations in a map to prevent calls to `find_node_animation` every frame
        }

        meshes.push_back(new liminal::mesh(vertices, indices, textures));
    }

    for (std::size_t child_index = 0; child_index < node->mNumChildren; child_index++)
    {
        process_node_meshes(node->mChildren[child_index]);
    }
}

void liminal::model::process_node_animations(const unsigned int animation_index, const float animation_time, const aiNode *const node, const glm::mat4 &parent_transformation, std::vector<glm::mat4> &bone_transformations) const
{
    const std::string node_name(node->mName.data);
    auto node_transformation = mat4_cast(node->mTransformation);

    if (scene->mNumAnimations > 0 && animation_index >= 0 && animation_index < scene->mNumAnimations)
    {
        const auto scene_animation = scene->mAnimations[animation_index];
        const auto node_animation = find_node_animation(scene_animation, node_name);
        if (node_animation)
        {
            const auto interpolated_position = calc_interpolated_position(animation_time, node_animation);
            const auto position_vector = glm::vec3(interpolated_position.x, interpolated_position.y, interpolated_position.z);
            const auto position = glm::translate(glm::identity<glm::mat4>(), position_vector);

            const auto interpolated_rotation = calc_interpolated_rotation(animation_time, node_animation);
            const auto rotation_vector = quat_cast(interpolated_rotation);
            const auto rotation = glm::toMat4(rotation_vector);

            const auto interpolated_scale = calc_interpolated_scale(animation_time, node_animation);
            const auto scale_vector = glm::vec3(interpolated_scale.x, interpolated_scale.y, interpolated_scale.z);
            const auto scale = glm::scale(glm::identity<glm::mat4>(), scale_vector);

            node_transformation = position * rotation * scale;
        }
    }

    const auto global_transformation = parent_transformation * node_transformation;

    if (bone_ids.find(node_name) != bone_ids.end())
    {
        const auto bone_id = bone_ids.at(node_name);
        bone_transformations.at(bone_id) = global_inverse_transform * global_transformation * bone_offsets.at(bone_id);
    }

    for (std::size_t child_index = 0; child_index < node->mNumChildren; child_index++)
    {
        process_node_animations(animation_index, animation_time, node->mChildren[child_index], global_transformation, bone_transformations);
    }
}

const aiNodeAnim *liminal::model::find_node_animation(const aiAnimation *const scene_animation, const std::string &node_name) const
{
    for (std::size_t channel_index = 0; channel_index < scene_animation->mNumChannels; channel_index++)
    {
        const auto node_animation = scene_animation->mChannels[channel_index];

        if (std::string(node_animation->mNodeName.data) == node_name)
        {
            return node_animation;
        }
    }

    return nullptr;
}

aiVector3D liminal::model::calc_interpolated_position(const float animation_time, const aiNodeAnim *const node_animation) const
{
    if (node_animation->mNumPositionKeys == 1)
    {
        return node_animation->mPositionKeys[0].mValue;
    }

    const auto position_index = find_position_index(animation_time, node_animation);
    const auto next_position_index = position_index + 1;

    const auto delta_time = static_cast<float>(node_animation->mPositionKeys[next_position_index].mTime - node_animation->mPositionKeys[position_index].mTime);
    const auto factor = (animation_time - static_cast<float>(node_animation->mPositionKeys[position_index].mTime) / delta_time);

    const auto &start = node_animation->mPositionKeys[position_index].mValue;
    const auto &end = node_animation->mPositionKeys[next_position_index].mValue;
    const auto delta = end - start;

    return start + factor * delta;
}

unsigned int liminal::model::find_position_index(const float animation_time, const aiNodeAnim *const node_animation) const
{
    for (unsigned int i = 0; i < node_animation->mNumPositionKeys - 1; i++)
    {
        if (animation_time < static_cast<float>(node_animation->mPositionKeys[i + 1].mTime))
        {
            return i;
        }
    }

    spdlog::error("Unable to find position index");

    return 0;
}

aiQuaternion liminal::model::calc_interpolated_rotation(const float animation_time, const aiNodeAnim *const node_animation) const
{
    if (node_animation->mNumRotationKeys == 1)
    {
        return node_animation->mRotationKeys[0].mValue;
    }

    const auto rotation_index = find_rotation_index(animation_time, node_animation);
    const auto next_rotation_index = rotation_index + 1;

    const auto delta_time = static_cast<float>(node_animation->mRotationKeys[next_rotation_index].mTime - node_animation->mRotationKeys[rotation_index].mTime);
    const auto factor = (animation_time - static_cast<float>(node_animation->mRotationKeys[rotation_index].mTime) / delta_time);

    const auto &start = node_animation->mRotationKeys[rotation_index].mValue;
    const auto &end = node_animation->mRotationKeys[next_rotation_index].mValue;

    aiQuaternion interpolated_rotation;
    aiQuaternion::Interpolate(interpolated_rotation, start, end, factor);

    return interpolated_rotation.Normalize();
}

unsigned int liminal::model::find_rotation_index(const float animation_time, const aiNodeAnim *const node_animation) const
{
    for (unsigned int i = 0; i < node_animation->mNumRotationKeys - 1; i++)
    {
        if (animation_time < static_cast<float>(node_animation->mRotationKeys[i + 1].mTime))
        {
            return i;
        }
    }

    spdlog::error("Unable to find rotation index");

    return 0;
}

aiVector3D liminal::model::calc_interpolated_scale(const float animation_time, const aiNodeAnim *const node_animation) const
{
    if (node_animation->mNumScalingKeys == 1)
    {
        return node_animation->mScalingKeys[0].mValue;
    }

    const auto scale_index = find_scale_index(animation_time, node_animation);
    const auto next_scale_index = scale_index + 1;

    const auto delta_time = static_cast<float>(node_animation->mScalingKeys[next_scale_index].mTime - node_animation->mScalingKeys[scale_index].mTime);
    const auto factor = (animation_time - static_cast<float>(node_animation->mScalingKeys[scale_index].mTime) / delta_time);

    const auto &start = node_animation->mScalingKeys[scale_index].mValue;
    const auto &end = node_animation->mScalingKeys[next_scale_index].mValue;
    const auto delta = end - start;

    return start + factor * delta;
}

unsigned int liminal::model::find_scale_index(const float animation_time, const aiNodeAnim *const node_animation) const
{
    for (unsigned int i = 0; i < node_animation->mNumScalingKeys - 1; i++)
    {
        if (animation_time < static_cast<float>(node_animation->mScalingKeys[i + 1].mTime))
        {
            return i;
        }
    }

    spdlog::error("Unable to find scale index");

    return 0;
}
