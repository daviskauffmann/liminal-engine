#include "model.hpp"

#include <assimp/postprocess.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <iostream>

static inline glm::vec3 vec3_cast(const aiVector3D &v) { return glm::vec3(v.x, v.y, v.z); }
static inline glm::vec2 vec2_cast(const aiVector3D &v) { return glm::vec2(v.x, v.y); }
static inline glm::quat quat_cast(const aiQuaternion &q) { return glm::quat(q.w, q.x, q.y, q.z); }
static inline glm::mat4 mat4_cast(const aiMatrix4x4 &m) { return glm::transpose(glm::make_mat4(&m.a1)); }
static inline glm::mat4 mat4_cast(const aiMatrix3x3 &m) { return glm::transpose(glm::make_mat3(&m.a1)); }

pk::model::model(const std::string &filename, bool flip_uvs)
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

pk::model::~model()
{
    for (unsigned int i = 0; i < meshes.size(); i++)
    {
        delete meshes[i];
    }

    for (auto it = loaded_textures.begin(); it != loaded_textures.end(); it++)
    {
        delete it->second;
    }
}

std::vector<glm::mat4> pk::model::calc_bone_transformations(unsigned int current_time)
{
    glm::mat4 identity = glm::identity<glm::mat4>();

    unsigned int animation_index = 0;
    float ticks_per_second = scene->mAnimations[animation_index]->mTicksPerSecond != 0 ? (float)scene->mAnimations[animation_index]->mTicksPerSecond : 25.0f;
    float time_in_ticks = ticks_per_second * (current_time / 1000.0f);
    float animation_time = fmod(time_in_ticks, scene->mAnimations[animation_index]->mDuration);

    process_node_animations(animation_time, scene->mRootNode, identity);

    std::vector<glm::mat4> bone_transformations;
    bone_transformations.resize(num_bones);
    for (unsigned int i = 0; i < num_bones; i++)
    {
        bone_transformations[i] = bone_infos[i].transformation;
    }
    return bone_transformations;
}

void pk::model::draw() const
{
    for (unsigned int i = 0; i < meshes.size(); i++)
    {
        meshes[i]->draw();
    }
}

void pk::model::process_node_meshes(const aiNode *node, const aiScene *scene)
{
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh *scene_mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(create_mesh(scene_mesh, scene));
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        process_node_meshes(node->mChildren[i], scene);
    }
}

pk::mesh *pk::model::create_mesh(const aiMesh *scene_mesh, const aiScene *scene)
{
    std::vector<pk::vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<std::vector<pk::texture *>> textures;

    // process vertices
    for (unsigned int i = 0; i < scene_mesh->mNumVertices; i++)
    {
        pk::vertex vertex;

        vertex.position = glm::vec3(
            scene_mesh->mVertices[i].x,
            scene_mesh->mVertices[i].y,
            scene_mesh->mVertices[i].z);

        if (scene_mesh->mNormals)
        {
            vertex.normal = glm::vec3(
                scene_mesh->mNormals[i].x,
                scene_mesh->mNormals[i].y,
                scene_mesh->mNormals[i].z);
        }
        else
        {
            vertex.normal = glm::vec3(1.0f, 1.0f, 1.0f);
        }

        if (scene_mesh->mTextureCoords[0])
        {
            vertex.uv = glm::vec2(
                scene_mesh->mTextureCoords[0][i].x,
                scene_mesh->mTextureCoords[0][i].y);
        }
        else
        {
            vertex.uv = glm::vec2(0.0f, 0.0f);
        }

        if (scene_mesh->mTangents)
        {
            vertex.tangent = glm::vec3(
                scene_mesh->mTangents[i].x,
                scene_mesh->mTangents[i].y,
                scene_mesh->mTangents[i].z);
        }

        if (scene_mesh->mBitangents)
        {
            vertex.bitangent = glm::vec3(
                scene_mesh->mBitangents[i].x,
                scene_mesh->mBitangents[i].y,
                scene_mesh->mBitangents[i].z);
        }

        vertices.push_back(vertex);
    }

    // process bones
    for (unsigned int i = 0; i < scene_mesh->mNumBones; i++)
    {
        unsigned int bone_index = 0;
        std::string bone_name(scene_mesh->mBones[i]->mName.data);

        if (loaded_bone_indexes.find(bone_name) == loaded_bone_indexes.end())
        {
            bone_index = num_bones++;
            loaded_bone_indexes[bone_name] = bone_index;

            bone_info bone_info;
            bone_info.offset = mat4_cast(scene_mesh->mBones[i]->mOffsetMatrix);
            bone_infos.push_back(bone_info);
        }
        else
        {
            bone_index = loaded_bone_indexes[bone_name];
        }

        for (unsigned int j = 0; j < scene_mesh->mBones[i]->mNumWeights; j++)
        {
            unsigned int vertex_id = scene_mesh->mBones[i]->mWeights[j].mVertexId;
            float weight = scene_mesh->mBones[i]->mWeights[j].mWeight;
            std::cout << vertex_id << " " << bone_index << " " << weight << std::endl;
            vertices[vertex_id].add_bone_data(bone_index, weight);
        }
    }

    // process indices
    for (unsigned int i = 0; i < scene_mesh->mNumFaces; i++)
    {
        aiFace face = scene_mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
        {
            indices.push_back(face.mIndices[j]);
        }
    }

    // process textures
    aiMaterial *scene_material = scene->mMaterials[scene_mesh->mMaterialIndex];
    for (aiTextureType type = aiTextureType_NONE; type <= AI_TEXTURE_TYPE_MAX; type = (aiTextureType)(type + 1))
    {
        std::vector<pk::texture *> material_textures;

        for (unsigned int i = 0; i < scene_material->GetTextureCount(type); i++)
        {
            aiString path;
            scene_material->GetTexture(type, i, &path);
            const std::string filename = directory + "/" + path.C_Str();
            if (loaded_textures.find(filename) == loaded_textures.end())
            {
                pk::texture *texture = new pk::texture(filename);
                material_textures.push_back(texture);
                loaded_textures[filename] = texture;
            }
            else
            {
                material_textures.push_back(loaded_textures[filename]);
            }
        }

        textures.push_back(material_textures);
    }

    return new pk::mesh(vertices, indices, textures);
}

void pk::model::process_node_animations(float animation_time, const aiNode *node, const glm::mat4 &parent_transformation)
{
    unsigned int animation_index = 0;
    const aiAnimation *scene_animation = scene->mAnimations[animation_index];
    std::string node_name(node->mName.data);
    glm::mat4 node_transformation = mat4_cast(node->mTransformation);
    const aiNodeAnim *node_animation = find_node_animation(scene_animation, node_name);
    if (node_animation)
    {
        aiVector3D interpolated_position;
        calc_interpolated_position(interpolated_position, animation_time, node_animation);
        glm::vec3 position_vector = glm::vec3(interpolated_position.x, interpolated_position.y, interpolated_position.z);
        glm::mat4 position = glm::translate(glm::mat4(1.0f), position_vector);

        aiQuaternion interpolated_rotation;
        calc_interpolated_rotation(interpolated_rotation, animation_time, node_animation);
        glm::quat rotation_vector = quat_cast(interpolated_rotation);
        glm::mat4 rotation = glm::toMat4(rotation_vector);

        aiVector3D interpolated_scale;
        calc_interpolated_scale(interpolated_scale, animation_time, node_animation);
        glm::vec3 scale_vector = glm::vec3(interpolated_scale.x, interpolated_scale.y, interpolated_scale.z);
        glm::mat4 scale = glm::scale(glm::mat4(1.0f), scale_vector);

        node_transformation = position * rotation * scale;
    }

    glm::mat4 global_transformation = parent_transformation * node_transformation;

    if (loaded_bone_indexes.find(node_name) != loaded_bone_indexes.end())
    {
        unsigned int bone_index = loaded_bone_indexes[node_name];
        bone_infos[bone_index].transformation = global_inverse_transform * global_transformation * bone_infos[bone_index].offset;
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        process_node_animations(animation_time, node->mChildren[i], global_transformation);
    }
}

const aiNodeAnim *pk::model::find_node_animation(const aiAnimation *scene_animation, const std::string node_name)
{
    for (unsigned int i = 0; i < scene_animation->mNumChannels; i++)
    {
        const aiNodeAnim *node_animation = scene_animation->mChannels[i];

        if (std::string(node_animation->mNodeName.data) == node_name)
        {
            return node_animation;
        }
    }

    return nullptr;
}

void pk::model::calc_interpolated_position(aiVector3D &out, float animation_time, const aiNodeAnim *node_animation)
{
    if (node_animation->mNumPositionKeys == 1)
    {
        out = node_animation->mPositionKeys[0].mValue;
        return;
    }

    unsigned int position_index = find_position_index(animation_time, node_animation);
    unsigned int next_position_index = position_index + 1;

    float delta_time = (float)(node_animation->mPositionKeys[next_position_index].mTime - node_animation->mPositionKeys[position_index].mTime);
    float factor = (animation_time - (float)node_animation->mPositionKeys[position_index].mTime) / delta_time;

    const aiVector3D &start = node_animation->mPositionKeys[position_index].mValue;
    const aiVector3D &end = node_animation->mPositionKeys[next_position_index].mValue;
    aiVector3D delta = end - start;

    out = start + factor * delta;
}

unsigned int pk::model::find_position_index(float animation_time, const aiNodeAnim *node_animation)
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

void pk::model::calc_interpolated_rotation(aiQuaternion &out, float animation_time, const aiNodeAnim *node_animation)
{
    if (node_animation->mNumRotationKeys == 1)
    {
        out = node_animation->mRotationKeys[0].mValue;
        return;
    }

    unsigned int rotation_index = find_rotation_index(animation_time, node_animation);
    unsigned int next_rotation_index = rotation_index + 1;

    float delta_time = (float)(node_animation->mRotationKeys[next_rotation_index].mTime - node_animation->mRotationKeys[rotation_index].mTime);
    float factor = (animation_time - (float)node_animation->mRotationKeys[rotation_index].mTime) / delta_time;

    const aiQuaternion &start = node_animation->mRotationKeys[rotation_index].mValue;
    const aiQuaternion &end = node_animation->mRotationKeys[next_rotation_index].mValue;
    aiQuaternion::Interpolate(out, start, end, factor);

    out = out.Normalize();
}

unsigned int pk::model::find_rotation_index(float animation_time, const aiNodeAnim *node_animation)
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

void pk::model::calc_interpolated_scale(aiVector3D &out, float animation_time, const aiNodeAnim *node_animation)
{
    if (node_animation->mNumScalingKeys == 1)
    {
        out = node_animation->mScalingKeys[0].mValue;
        return;
    }

    unsigned int scale_index = find_scale_index(animation_time, node_animation);
    unsigned int next_scale_index = scale_index + 1;

    float delta_time = (float)(node_animation->mScalingKeys[next_scale_index].mTime - node_animation->mScalingKeys[scale_index].mTime);
    float factor = (animation_time - (float)node_animation->mScalingKeys[scale_index].mTime) / delta_time;

    const aiVector3D &start = node_animation->mScalingKeys[scale_index].mValue;
    const aiVector3D &end = node_animation->mScalingKeys[next_scale_index].mValue;
    aiVector3D delta = end - start;

    out = start + factor * delta;
}

unsigned int pk::model::find_scale_index(float animation_time, const aiNodeAnim *node_animation)
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
