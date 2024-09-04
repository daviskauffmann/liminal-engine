#include <liminal/graphics/model.hpp>

#include <assimp/postprocess.h>
#include <filesystem>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <liminal/core/assets.hpp>
#include <liminal/graphics/mesh.hpp>
#include <liminal/graphics/program.hpp>
#include <liminal/graphics/texture.hpp>
#include <spdlog/spdlog.h>
#include <stdexcept>

static inline glm::vec3 vec3_cast(const aiVector3D &v) { return {v.x, v.y, v.z}; }
static inline glm::vec2 vec2_cast(const aiVector3D &v) { return {v.x, v.y}; }
static inline glm::quat quat_cast(const aiQuaternion &q) { return {q.w, q.x, q.y, q.z}; }
static inline glm::mat4 mat4_cast(const aiMatrix4x4 &m) { return glm::transpose(glm::make_mat4(&m.a1)); }
static inline glm::mat4 mat4_cast(const aiMatrix3x3 &m) { return glm::transpose(glm::make_mat3(&m.a1)); }

liminal::model::model(liminal::mesh *const mesh)
{
    meshes.push_back(mesh);
}

liminal::model::model(const char *const filename, std::shared_ptr<liminal::assets> assets, const bool flip_uvs)
    : directory(std::filesystem::path(filename).parent_path().string())
{
    unsigned int flags = aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices | aiProcess_Triangulate | aiProcess_GenSmoothNormals;
    if (flip_uvs)
    {
        flags |= aiProcess_FlipUVs;
    }
    ai_scene = importer.ReadFile(filename, flags);
    if (!ai_scene || ai_scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !ai_scene->mRootNode)
    {
        throw std::runtime_error(importer.GetErrorString());
    }

    global_inverse_transform = glm::inverse(mat4_cast(ai_scene->mRootNode->mTransformation));

    process_node_meshes(ai_scene->mRootNode, assets);
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
    return ai_scene ? ai_scene->HasAnimations() : false;
}

unsigned int liminal::model::num_animations() const
{
    return ai_scene ? ai_scene->mNumAnimations : 0;
}

std::vector<glm::mat4> liminal::model::calc_bone_transformations(const unsigned int animation_index, const std::uint64_t current_time) const
{
    std::vector<glm::mat4> bone_transformations;

    if (ai_scene && ai_scene->mNumAnimations > 0 && animation_index >= 0 && animation_index < ai_scene->mNumAnimations)
    {
        bone_transformations.resize(num_bones);

        const auto ticks_per_second = ai_scene->mAnimations[animation_index]->mTicksPerSecond != 0 ? ai_scene->mAnimations[animation_index]->mTicksPerSecond : 25.0;
        const auto time_in_ticks = ticks_per_second * (current_time / 1000.0);
        const auto animation_time = static_cast<float>(fmod(time_in_ticks, ai_scene->mAnimations[animation_index]->mDuration));

        process_node_animations(animation_index, animation_time, ai_scene->mRootNode, glm::identity<glm::mat4>(), bone_transformations);
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

void liminal::model::process_node_meshes(const aiNode *const ai_node, std::shared_ptr<liminal::assets> assets)
{
    for (std::size_t mesh_index = 0; mesh_index < ai_node->mNumMeshes; mesh_index++)
    {
        const auto ai_mesh = ai_scene->mMeshes[ai_node->mMeshes[mesh_index]];

        std::vector<liminal::mesh::vertex> vertices;
        std::vector<unsigned int> indices;
        std::array<std::vector<std::shared_ptr<liminal::texture>>, liminal::mesh::num_textures> textures;

        // process vertices
        for (std::size_t vertex_index = 0; vertex_index < ai_mesh->mNumVertices; vertex_index++)
        {
            liminal::mesh::vertex vertex;

            if (ai_mesh->HasPositions())
            {
                vertex.position = vec3_cast(ai_mesh->mVertices[vertex_index]);
            }

            if (ai_mesh->HasNormals())
            {
                vertex.normal = vec3_cast(ai_mesh->mNormals[vertex_index]);
            }

            if (ai_mesh->HasTextureCoords(0))
            {
                vertex.uv = vec2_cast(ai_mesh->mTextureCoords[0][vertex_index]);
            }

            if (ai_mesh->HasTangentsAndBitangents())
            {
                vertex.tangent = vec3_cast(ai_mesh->mTangents[vertex_index]);
                vertex.bitangent = vec3_cast(ai_mesh->mBitangents[vertex_index]);
            }

            for (std::size_t bone_index = 0; bone_index < liminal::mesh::vertex::num_bones; bone_index++)
            {
                vertex.bone_ids.at(bone_index) = liminal::mesh::vertex::num_bones;
                vertex.bone_weights.at(bone_index) = 0;
            }

            vertices.push_back(vertex);
        }

        // process bones
        if (ai_mesh->HasBones())
        {
            for (std::size_t bone_index = 0; bone_index < ai_mesh->mNumBones; bone_index++)
            {
                const std::string bone_name(ai_mesh->mBones[bone_index]->mName.data);

                if (!bone_ids.contains(bone_name))
                {
                    bone_ids.insert({bone_name, num_bones++});
                    bone_offsets.push_back(mat4_cast(ai_mesh->mBones[bone_index]->mOffsetMatrix));
                }

                for (std::size_t weight_index = 0; weight_index < ai_mesh->mBones[bone_index]->mNumWeights; weight_index++)
                {
                    const auto vertex_id = ai_mesh->mBones[bone_index]->mWeights[weight_index].mVertexId;
                    const auto weight = ai_mesh->mBones[bone_index]->mWeights[weight_index].mWeight;
                    vertices.at(vertex_id).add_bone_data(bone_ids.at(bone_name), weight);
                }
            }
        }

        // process indices
        if (ai_mesh->HasFaces())
        {
            for (std::size_t face_index = 0; face_index < ai_mesh->mNumFaces; face_index++)
            {
                const auto ai_face = ai_mesh->mFaces[face_index];
                for (std::size_t index_index = 0; index_index < ai_face.mNumIndices; index_index++)
                {
                    indices.push_back(ai_face.mIndices[index_index]);
                }
            }
        }

        // process textures
        if (ai_scene->HasMaterials())
        {
            const auto ai_material = ai_scene->mMaterials[ai_mesh->mMaterialIndex];
            for (auto ai_texture_type = aiTextureType_NONE; ai_texture_type <= AI_TEXTURE_TYPE_MAX; ai_texture_type = (aiTextureType)(ai_texture_type + 1))
            {
                for (unsigned int texture_index = 0; texture_index < ai_material->GetTextureCount(ai_texture_type); texture_index++)
                {
                    aiString path;
                    ai_material->GetTexture(ai_texture_type, texture_index, &path);

                    const auto filename = directory + "/" + path.C_Str();
                    try
                    {
                        textures.at(ai_texture_type).push_back(assets->load_texture(filename));
                    }
                    catch (const std::exception &e)
                    {
                        spdlog::error("Failed to load texture: {}", e.what());
                    }
                }
            }
        }

        // process animations
        if (ai_scene->HasAnimations())
        {
            // TODO: store animations in a map to prevent calls to `find_node_animation` every frame
        }

        meshes.push_back(new liminal::mesh(vertices, indices, textures));
    }

    for (std::size_t child_index = 0; child_index < ai_node->mNumChildren; child_index++)
    {
        process_node_meshes(ai_node->mChildren[child_index], assets);
    }
}

void liminal::model::process_node_animations(const unsigned int animation_index, const float animation_time, const aiNode *const ai_node, const glm::mat4 &parent_transformation, std::vector<glm::mat4> &bone_transformations) const
{
    const std::string node_name(ai_node->mName.data);
    auto node_transformation = mat4_cast(ai_node->mTransformation);

    if (ai_scene->mNumAnimations > 0 && animation_index >= 0 && animation_index < ai_scene->mNumAnimations)
    {
        const auto ai_animation = ai_scene->mAnimations[animation_index];
        const auto ai_node_anim = find_node_animation(ai_animation, node_name);
        if (ai_node_anim)
        {
            const auto interpolated_position = calc_interpolated_position(animation_time, ai_node_anim);
            const auto position_vector = vec3_cast(interpolated_position);
            const auto position = glm::translate(glm::identity<glm::mat4>(), position_vector);

            const auto interpolated_rotation = calc_interpolated_rotation(animation_time, ai_node_anim);
            const auto rotation_vector = quat_cast(interpolated_rotation);
            const auto rotation = glm::toMat4(rotation_vector);

            const auto interpolated_scale = calc_interpolated_scale(animation_time, ai_node_anim);
            const auto scale_vector = vec3_cast(interpolated_scale);
            const auto scale = glm::scale(glm::identity<glm::mat4>(), scale_vector);

            node_transformation = position * rotation * scale;
        }
    }

    const auto global_transformation = parent_transformation * node_transformation;

    if (bone_ids.contains(node_name))
    {
        const auto bone_id = bone_ids.at(node_name);
        bone_transformations.at(bone_id) = global_inverse_transform * global_transformation * bone_offsets.at(bone_id);
    }

    for (std::size_t child_index = 0; child_index < ai_node->mNumChildren; child_index++)
    {
        process_node_animations(animation_index, animation_time, ai_node->mChildren[child_index], global_transformation, bone_transformations);
    }
}

const aiNodeAnim *liminal::model::find_node_animation(const aiAnimation *const ai_animation, const std::string &node_name) const
{
    for (std::size_t channel_index = 0; channel_index < ai_animation->mNumChannels; channel_index++)
    {
        const auto node_animation = ai_animation->mChannels[channel_index];

        if (std::string(node_animation->mNodeName.data) == node_name)
        {
            return node_animation;
        }
    }

    return nullptr;
}

aiVector3D liminal::model::calc_interpolated_position(const float animation_time, const aiNodeAnim *const ai_node_anim) const
{
    if (ai_node_anim->mNumPositionKeys == 1)
    {
        return ai_node_anim->mPositionKeys[0].mValue;
    }

    const auto position_index = find_position_index(animation_time, ai_node_anim);
    const auto next_position_index = position_index + 1;

    const auto delta_time = static_cast<float>(ai_node_anim->mPositionKeys[next_position_index].mTime - ai_node_anim->mPositionKeys[position_index].mTime);
    const auto factor = (animation_time - static_cast<float>(ai_node_anim->mPositionKeys[position_index].mTime) / delta_time);

    const auto &start = ai_node_anim->mPositionKeys[position_index].mValue;
    const auto &end = ai_node_anim->mPositionKeys[next_position_index].mValue;
    const auto delta = end - start;

    return start + factor * delta;
}

unsigned int liminal::model::find_position_index(const float animation_time, const aiNodeAnim *const ai_node_anim) const
{
    for (unsigned int i = 0; i < ai_node_anim->mNumPositionKeys - 1; i++)
    {
        if (animation_time < static_cast<float>(ai_node_anim->mPositionKeys[i + 1].mTime))
        {
            return i;
        }
    }

    throw std::runtime_error("Could not find position index");
}

aiQuaternion liminal::model::calc_interpolated_rotation(const float animation_time, const aiNodeAnim *const ai_node_anim) const
{
    if (ai_node_anim->mNumRotationKeys == 1)
    {
        return ai_node_anim->mRotationKeys[0].mValue;
    }

    const auto rotation_index = find_rotation_index(animation_time, ai_node_anim);
    const auto next_rotation_index = rotation_index + 1;

    const auto delta_time = static_cast<float>(ai_node_anim->mRotationKeys[next_rotation_index].mTime - ai_node_anim->mRotationKeys[rotation_index].mTime);
    const auto factor = (animation_time - static_cast<float>(ai_node_anim->mRotationKeys[rotation_index].mTime) / delta_time);

    const auto &start = ai_node_anim->mRotationKeys[rotation_index].mValue;
    const auto &end = ai_node_anim->mRotationKeys[next_rotation_index].mValue;

    aiQuaternion interpolated_rotation;
    aiQuaternion::Interpolate(interpolated_rotation, start, end, factor);

    return interpolated_rotation.Normalize();
}

unsigned int liminal::model::find_rotation_index(const float animation_time, const aiNodeAnim *const ai_node_anim) const
{
    for (unsigned int i = 0; i < ai_node_anim->mNumRotationKeys - 1; i++)
    {
        if (animation_time < static_cast<float>(ai_node_anim->mRotationKeys[i + 1].mTime))
        {
            return i;
        }
    }

    throw std::runtime_error("Could not find rotation index");
}

aiVector3D liminal::model::calc_interpolated_scale(const float animation_time, const aiNodeAnim *const ai_node_anim) const
{
    if (ai_node_anim->mNumScalingKeys == 1)
    {
        return ai_node_anim->mScalingKeys[0].mValue;
    }

    const auto scale_index = find_scale_index(animation_time, ai_node_anim);
    const auto next_scale_index = scale_index + 1;

    const auto delta_time = static_cast<float>(ai_node_anim->mScalingKeys[next_scale_index].mTime - ai_node_anim->mScalingKeys[scale_index].mTime);
    const auto factor = (animation_time - static_cast<float>(ai_node_anim->mScalingKeys[scale_index].mTime) / delta_time);

    const auto &start = ai_node_anim->mScalingKeys[scale_index].mValue;
    const auto &end = ai_node_anim->mScalingKeys[next_scale_index].mValue;
    const auto delta = end - start;

    return start + factor * delta;
}

unsigned int liminal::model::find_scale_index(const float animation_time, const aiNodeAnim *const ai_node_anim) const
{
    for (unsigned int i = 0; i < ai_node_anim->mNumScalingKeys - 1; i++)
    {
        if (animation_time < static_cast<float>(ai_node_anim->mScalingKeys[i + 1].mTime))
        {
            return i;
        }
    }

    throw std::runtime_error("Could not find scale index");
}
