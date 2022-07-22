#include <liminal/graphics/model.hpp>

#include <assimp/postprocess.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <iostream>
#include <liminal/core/assets.hpp>

static inline glm::vec3 vec3_cast(const aiVector3D &v) { return glm::vec3(v.x, v.y, v.z); }
static inline glm::vec2 vec2_cast(const aiVector3D &v) { return glm::vec2(v.x, v.y); }
static inline glm::quat quat_cast(const aiQuaternion &q) { return glm::quat(q.w, q.x, q.y, q.z); }
static inline glm::mat4 mat4_cast(const aiMatrix4x4 &m) { return glm::transpose(glm::make_mat4(&m.a1)); }
static inline glm::mat4 mat4_cast(const aiMatrix3x3 &m) { return glm::transpose(glm::make_mat3(&m.a1)); }

liminal::model::model(liminal::mesh *mesh)
{
    meshes.push_back(mesh);
}

liminal::model::model(const std::string &filename, bool flip_uvs)
    : directory(filename.substr(0, filename.find_last_of('/')))
{
    unsigned int flags = aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices | aiProcess_Triangulate | aiProcess_GenSmoothNormals;
    if (flip_uvs)
    {
        flags |= aiProcess_FlipUVs;
    }

    scene = importer.ReadFile(filename, flags);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cerr << "Error: Failed to load model: " << importer.GetErrorString() << std::endl;
        return;
    }

    global_inverse_transform = glm::inverse(mat4_cast(scene->mRootNode->mTransformation));
    num_bones = 0;

    process_node_meshes(scene->mRootNode, scene);
}

liminal::model::~model()
{
    for (size_t i = 0; i < meshes.size(); i++)
    {
        delete meshes[i];
    }
}

bool liminal::model::has_animations() const
{
    return scene ? scene->HasAnimations() : false;
}

void liminal::model::update_bone_transformations(unsigned int animation_index, unsigned int current_time)
{
    bone_transformations.clear();

    if (scene && scene->mNumAnimations > 0 && animation_index >= 0 && animation_index < scene->mNumAnimations)
    {
        auto ticks_per_second = scene->mAnimations[animation_index]->mTicksPerSecond != 0 ? (float)scene->mAnimations[animation_index]->mTicksPerSecond : 25.f;
        auto time_in_ticks = ticks_per_second * (current_time / 1000.f);
        auto animation_time = (float)fmod(time_in_ticks, scene->mAnimations[animation_index]->mDuration);

        process_node_animations(animation_index, animation_time, scene->mRootNode, glm::identity<glm::mat4>());

        bone_transformations.resize(num_bones);
        for (size_t i = 0; i < num_bones; i++)
        {
            bone_transformations[i] = bones[i].transformation;
        }
    }
}

void liminal::model::draw_meshes(const liminal::program &program) const
{
    for (size_t i = 0; i < meshes.size(); i++)
    {
        meshes[i]->draw(program);
    }
}

void liminal::model::process_node_meshes(const aiNode *node, const aiScene *scene)
{
    for (size_t i = 0; i < node->mNumMeshes; i++)
    {
        auto scene_mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(create_mesh(scene_mesh, scene));
    }

    for (size_t i = 0; i < node->mNumChildren; i++)
    {
        process_node_meshes(node->mChildren[i], scene);
    }
}

liminal::mesh *liminal::model::create_mesh(const aiMesh *scene_mesh, const aiScene *scene)
{
    std::vector<liminal::vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<std::vector<liminal::texture *>> textures;

    // process vertices
    for (size_t i = 0; i < scene_mesh->mNumVertices; i++)
    {
        liminal::vertex vertex;

        if (scene_mesh->HasPositions())
        {
            vertex.position = vec3_cast(scene_mesh->mVertices[i]);
        }

        if (scene_mesh->HasNormals())
        {
            vertex.normal = vec3_cast(scene_mesh->mNormals[i]);
        }

        if (scene_mesh->HasTextureCoords(0))
        {
            vertex.uv = vec2_cast(scene_mesh->mTextureCoords[0][i]);
        }

        if (scene_mesh->HasTangentsAndBitangents())
        {
            vertex.tangent = vec3_cast(scene_mesh->mTangents[i]);
            vertex.bitangent = vec3_cast(scene_mesh->mBitangents[i]);
        }

        for (size_t j = 0; j < NUM_BONES_PER_VERTEX; j++)
        {
            vertex.bone_ids[j] = NUM_BONES_PER_VERTEX;
            vertex.bone_weights[j] = 0;
        }

        vertices.push_back(vertex);
    }

    // process bones
    if (scene_mesh->HasBones())
    {
        for (size_t i = 0; i < scene_mesh->mNumBones; i++)
        {
            unsigned int bone_index;
            std::string bone_name(scene_mesh->mBones[i]->mName.data);
            if (bone_indices.find(bone_name) == bone_indices.end())
            {
                bone_index = num_bones++;
                bone_indices[bone_name] = bone_index;

                liminal::bone bone;
                bone.offset = mat4_cast(scene_mesh->mBones[i]->mOffsetMatrix);

                bones.push_back(bone);
            }
            else
            {
                bone_index = bone_indices[bone_name];
            }

            for (size_t j = 0; j < scene_mesh->mBones[i]->mNumWeights; j++)
            {
                auto vertex_id = scene_mesh->mBones[i]->mWeights[j].mVertexId;
                auto weight = scene_mesh->mBones[i]->mWeights[j].mWeight;
                vertices[vertex_id].add_bone_data(bone_index, weight);
            }
        }
    }

    // process indices
    if (scene_mesh->HasFaces())
    {
        for (size_t i = 0; i < scene_mesh->mNumFaces; i++)
        {
            auto face = scene_mesh->mFaces[i];
            for (size_t j = 0; j < face.mNumIndices; j++)
            {
                indices.push_back(face.mIndices[j]);
            }
        }
    }

    // process textures
    if (scene->HasMaterials())
    {
        auto scene_material = scene->mMaterials[scene_mesh->mMaterialIndex];
        for (auto type = aiTextureType_NONE; type <= AI_TEXTURE_TYPE_MAX; type = (aiTextureType)(type + 1))
        {
            std::vector<liminal::texture *> material_textures;

            for (unsigned int i = 0; i < scene_material->GetTextureCount(type); i++)
            {
                aiString path;
                scene_material->GetTexture(type, i, &path);
                auto filename = directory + "/" + path.C_Str();
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

    return new liminal::mesh(vertices, indices, textures);
}

void liminal::model::process_node_animations(unsigned int animation_index, float animation_time, const aiNode *node, const glm::mat4 &parent_transformation)
{
    std::string node_name(node->mName.data);
    auto node_transformation = mat4_cast(node->mTransformation);

    if (scene->mNumAnimations > 0 && animation_index >= 0 && animation_index < scene->mNumAnimations)
    {
        const auto scene_animation = scene->mAnimations[animation_index];
        const auto node_animation = find_node_animation(scene_animation, node_name);
        if (node_animation)
        {
            aiVector3D interpolated_position;
            calc_interpolated_position(interpolated_position, animation_time, node_animation);
            auto position_vector = glm::vec3(interpolated_position.x, interpolated_position.y, interpolated_position.z);
            auto position = glm::translate(glm::identity<glm::mat4>(), position_vector);

            aiQuaternion interpolated_rotation;
            calc_interpolated_rotation(interpolated_rotation, animation_time, node_animation);
            auto rotation_vector = quat_cast(interpolated_rotation);
            auto rotation = glm::toMat4(rotation_vector);

            aiVector3D interpolated_scale;
            calc_interpolated_scale(interpolated_scale, animation_time, node_animation);
            auto scale_vector = glm::vec3(interpolated_scale.x, interpolated_scale.y, interpolated_scale.z);
            auto scale = glm::scale(glm::identity<glm::mat4>(), scale_vector);

            node_transformation = position * rotation * scale;
        }
    }

    auto global_transformation = parent_transformation * node_transformation;

    if (bone_indices.find(node_name) != bone_indices.end())
    {
        auto bone_index = bone_indices[node_name];
        bones[bone_index].transformation = global_inverse_transform * global_transformation * bones[bone_index].offset;
    }

    for (size_t i = 0; i < node->mNumChildren; i++)
    {
        process_node_animations(animation_index, animation_time, node->mChildren[i], global_transformation);
    }
}

const aiNodeAnim *liminal::model::find_node_animation(const aiAnimation *scene_animation, const std::string node_name)
{
    for (size_t i = 0; i < scene_animation->mNumChannels; i++)
    {
        const auto node_animation = scene_animation->mChannels[i];

        if (std::string(node_animation->mNodeName.data) == node_name)
        {
            return node_animation;
        }
    }

    return nullptr;
}

void liminal::model::calc_interpolated_position(aiVector3D &out, float animation_time, const aiNodeAnim *node_animation)
{
    if (node_animation->mNumPositionKeys == 1)
    {
        out = node_animation->mPositionKeys[0].mValue;
        return;
    }

    auto position_index = find_position_index(animation_time, node_animation);
    auto next_position_index = position_index + 1;

    auto delta_time = (float)(node_animation->mPositionKeys[next_position_index].mTime - node_animation->mPositionKeys[position_index].mTime);
    auto factor = (animation_time - (float)node_animation->mPositionKeys[position_index].mTime) / delta_time;

    const auto &start = node_animation->mPositionKeys[position_index].mValue;
    const auto &end = node_animation->mPositionKeys[next_position_index].mValue;
    auto delta = end - start;

    out = start + factor * delta;
}

unsigned int liminal::model::find_position_index(float animation_time, const aiNodeAnim *node_animation)
{
    for (unsigned int i = 0; i < node_animation->mNumPositionKeys - 1; i++)
    {
        if (animation_time < (float)node_animation->mPositionKeys[i + 1].mTime)
        {
            return i;
        }
    }

    std::cerr << "Error: Unable to find position index" << std::endl;

    return 0;
}

void liminal::model::calc_interpolated_rotation(aiQuaternion &out, float animation_time, const aiNodeAnim *node_animation)
{
    if (node_animation->mNumRotationKeys == 1)
    {
        out = node_animation->mRotationKeys[0].mValue;
        return;
    }

    auto rotation_index = find_rotation_index(animation_time, node_animation);
    auto next_rotation_index = rotation_index + 1;

    auto delta_time = (float)(node_animation->mRotationKeys[next_rotation_index].mTime - node_animation->mRotationKeys[rotation_index].mTime);
    auto factor = (animation_time - (float)node_animation->mRotationKeys[rotation_index].mTime) / delta_time;

    const auto &start = node_animation->mRotationKeys[rotation_index].mValue;
    const auto &end = node_animation->mRotationKeys[next_rotation_index].mValue;
    aiQuaternion::Interpolate(out, start, end, factor);

    out = out.Normalize();
}

unsigned int liminal::model::find_rotation_index(float animation_time, const aiNodeAnim *node_animation)
{
    for (unsigned int i = 0; i < node_animation->mNumRotationKeys - 1; i++)
    {
        if (animation_time < (float)node_animation->mRotationKeys[i + 1].mTime)
        {
            return i;
        }
    }

    std::cerr << "Error: Unable to find rotation index" << std::endl;

    return 0;
}

void liminal::model::calc_interpolated_scale(aiVector3D &out, float animation_time, const aiNodeAnim *node_animation)
{
    if (node_animation->mNumScalingKeys == 1)
    {
        out = node_animation->mScalingKeys[0].mValue;
        return;
    }

    auto scale_index = find_scale_index(animation_time, node_animation);
    auto next_scale_index = scale_index + 1;

    auto delta_time = (float)(node_animation->mScalingKeys[next_scale_index].mTime - node_animation->mScalingKeys[scale_index].mTime);
    auto factor = (animation_time - (float)node_animation->mScalingKeys[scale_index].mTime) / delta_time;

    const auto &start = node_animation->mScalingKeys[scale_index].mValue;
    const auto &end = node_animation->mScalingKeys[next_scale_index].mValue;
    auto delta = end - start;

    out = start + factor * delta;
}

unsigned int liminal::model::find_scale_index(float animation_time, const aiNodeAnim *node_animation)
{
    for (unsigned int i = 0; i < node_animation->mNumScalingKeys - 1; i++)
    {
        if (animation_time < (float)node_animation->mScalingKeys[i + 1].mTime)
        {
            return i;
        }
    }

    std::cerr << "Error: Unable to find scale index" << std::endl;

    return 0;
}
