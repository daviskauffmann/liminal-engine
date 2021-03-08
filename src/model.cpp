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

    process_node(scene->mRootNode, scene);
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
    // TODO: parameterize
    const unsigned int animation_index = 0;

    glm::mat4 identity = glm::identity<glm::mat4>();

    float ticks_per_second = scene->mAnimations[animation_index]->mTicksPerSecond != 0 ? (float)scene->mAnimations[animation_index]->mTicksPerSecond : 25.0f;
    float time_in_ticks = ticks_per_second * (current_time / 1000.0f);
    float animation_time = fmod(time_in_ticks, scene->mAnimations[animation_index]->mDuration);

    read_node_heirarchy(animation_time, scene->mRootNode, identity);

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

void pk::model::process_node(aiNode *node, const aiScene *scene)
{
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(process_mesh(mesh, scene));
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        process_node(node->mChildren[i], scene);
    }
}

pk::mesh *pk::model::process_mesh(aiMesh *mesh, const aiScene *scene)
{
    std::vector<pk::vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<std::vector<pk::texture *>> textures;

    // process vertices
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        pk::vertex vertex;

        vertex.position = glm::vec3(
            mesh->mVertices[i].x,
            mesh->mVertices[i].y,
            mesh->mVertices[i].z);

        if (mesh->mNormals)
        {
            vertex.normal = glm::vec3(
                mesh->mNormals[i].x,
                mesh->mNormals[i].y,
                mesh->mNormals[i].z);
        }
        else
        {
            vertex.normal = glm::vec3(1.0f, 1.0f, 1.0f);
        }

        if (mesh->mTextureCoords[0])
        {
            vertex.uv = glm::vec2(
                mesh->mTextureCoords[0][i].x,
                mesh->mTextureCoords[0][i].y);
        }
        else
        {
            vertex.uv = glm::vec2(0.0f, 0.0f);
        }

        if (mesh->mTangents)
        {
            vertex.tangent = glm::vec3(
                mesh->mTangents[i].x,
                mesh->mTangents[i].y,
                mesh->mTangents[i].z);
        }

        if (mesh->mBitangents)
        {
            vertex.bitangent = glm::vec3(
                mesh->mBitangents[i].x,
                mesh->mBitangents[i].y,
                mesh->mBitangents[i].z);
        }

        vertices.push_back(vertex);
    }

    // process bones
    for (unsigned int i = 0; i < mesh->mNumBones; i++)
    {
        unsigned int bone_index = 0;
        std::string bone_name(mesh->mBones[i]->mName.data);

        if (loaded_bone_indexes.find(bone_name) == loaded_bone_indexes.end())
        {
            bone_index = num_bones++;
            loaded_bone_indexes[bone_name] = bone_index;

            bone_info bone_info;
            bone_info.offset = mat4_cast(mesh->mBones[i]->mOffsetMatrix);
            bone_infos.push_back(bone_info);
        }
        else
        {
            bone_index = loaded_bone_indexes[bone_name];
        }

        for (unsigned int j = 0; j < mesh->mBones[i]->mNumWeights; j++)
        {
            unsigned int vertex_id = mesh->mBones[i]->mWeights[j].mVertexId;
            float weight = mesh->mBones[i]->mWeights[j].mWeight;
            for (unsigned int k = 0; k < NUM_BONES_PER_VERTEX; k++)
            {
                if (vertices[vertex_id].bone_weights[k] == 0)
                {
                    vertices[vertex_id].bone_ids[k] = bone_index;
                    vertices[vertex_id].bone_weights[k] = weight;
                    break;
                }
            }
        }
    }

    // process indices
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
        {
            indices.push_back(face.mIndices[j]);
        }
    }

    // process textures
    aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
    for (aiTextureType type = aiTextureType_NONE; type <= AI_TEXTURE_TYPE_MAX; type = (aiTextureType)(type + 1))
    {
        std::vector<pk::texture *> material_textures;

        for (unsigned int i = 0; i < material->GetTextureCount(type); i++)
        {
            aiString path;
            material->GetTexture(type, i, &path);
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

void pk::model::read_node_heirarchy(float animation_time, const aiNode *node, const glm::mat4 &parent_transformation)
{
    // TODO: parameterize
    const unsigned int animation_index = 0;

    std::string node_name(node->mName.data);

    const aiAnimation *animation = scene->mAnimations[animation_index];

    glm::mat4 node_transformation = mat4_cast(node->mTransformation);

    const aiNodeAnim *node_animation = find_node_animation(animation, node_name);

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
        read_node_heirarchy(animation_time, node->mChildren[i], global_transformation);
    }
}

const aiNodeAnim *pk::model::find_node_animation(const aiAnimation *animation, const std::string node_name)
{
    for (unsigned int i = 0; i < animation->mNumChannels; i++)
    {
        const aiNodeAnim *node_animation = animation->mChannels[i];

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

    unsigned int position_index = 0;
    for (unsigned int i = 0; i < node_animation->mNumPositionKeys - 1; i++)
    {
        if (animation_time < (float)node_animation->mPositionKeys[i + 1].mTime)
        {
            position_index = i;
        }
    }
    unsigned int next_position_index = position_index + 1;

    float delta_time = (float)(node_animation->mPositionKeys[next_position_index].mTime - node_animation->mPositionKeys[position_index].mTime);
    float factor = (animation_time - (float)node_animation->mPositionKeys[position_index].mTime) / delta_time;

    const aiVector3D &start = node_animation->mPositionKeys[position_index].mValue;
    const aiVector3D &end = node_animation->mPositionKeys[next_position_index].mValue;
    aiVector3D delta = end - start;

    out = start + factor * delta;
}

void pk::model::calc_interpolated_rotation(aiQuaternion &out, float animation_time, const aiNodeAnim *node_animation)
{
    if (node_animation->mNumRotationKeys == 1)
    {
        out = node_animation->mRotationKeys[0].mValue;
        return;
    }

    unsigned int rotation_index = 0;
    for (unsigned int i = 0; i < node_animation->mNumRotationKeys - 1; i++)
    {
        if (animation_time < (float)node_animation->mRotationKeys[i + 1].mTime)
        {
            rotation_index = i;
        }
    }
    unsigned int next_rotation_index = rotation_index + 1;

    float delta_time = (float)(node_animation->mRotationKeys[next_rotation_index].mTime - node_animation->mRotationKeys[rotation_index].mTime);
    float factor = (animation_time - (float)node_animation->mRotationKeys[rotation_index].mTime) / delta_time;

    const aiQuaternion &start = node_animation->mRotationKeys[rotation_index].mValue;
    const aiQuaternion &end = node_animation->mRotationKeys[next_rotation_index].mValue;
    aiQuaternion::Interpolate(out, start, end, factor);

    out = out.Normalize();
}

void pk::model::calc_interpolated_scale(aiVector3D &out, float animation_time, const aiNodeAnim *node_animation)
{
    if (node_animation->mNumScalingKeys == 1)
    {
        out = node_animation->mScalingKeys[0].mValue;
        return;
    }

    unsigned int scale_index = 0;
    for (unsigned int i = 0; i < node_animation->mNumScalingKeys - 1; i++)
    {
        if (animation_time < (float)node_animation->mScalingKeys[i + 1].mTime)
        {
            scale_index = i;
        }
    }
    unsigned int next_scale_index = scale_index + 1;

    float delta_time = (float)(node_animation->mScalingKeys[next_scale_index].mTime - node_animation->mScalingKeys[scale_index].mTime);
    float factor = (animation_time - (float)node_animation->mScalingKeys[scale_index].mTime) / delta_time;

    const aiVector3D &start = node_animation->mScalingKeys[scale_index].mValue;
    const aiVector3D &end = node_animation->mScalingKeys[next_scale_index].mValue;
    aiVector3D delta = end - start;

    out = start + factor * delta;
}
