#include "model.hpp"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <iostream>

pk::model::model(const std::string &filename, bool flip_uvs)
{
    Assimp::Importer importer;
    unsigned int flags = aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices;
    if (flip_uvs)
    {
        flags |= aiProcess_FlipUVs;
    }
    const aiScene *scene = importer.ReadFile(filename, flags);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cout << "Error: " << importer.GetErrorString() << std::endl;
    }
    directory = filename.substr(0, filename.find_last_of('/'));
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

pk::mesh *pk::model::process_mesh(aiMesh *mesh, const aiScene *scene) const
{
    std::vector<pk::vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<std::vector<pk::texture *>> textures;

    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        pk::vertex vertex;

        glm::vec3 position;
        position.x = mesh->mVertices[i].x;
        position.y = mesh->mVertices[i].y;
        position.z = mesh->mVertices[i].z;
        vertex.position = position;

        glm::vec3 normal;
        normal.x = mesh->mNormals[i].x;
        normal.y = mesh->mNormals[i].y;
        normal.z = mesh->mNormals[i].z;
        vertex.normal = normal;

        if (mesh->mTextureCoords[0])
        {
            glm::vec2 uv;
            uv.x = mesh->mTextureCoords[0][i].x;
            uv.y = mesh->mTextureCoords[0][i].y;
            vertex.uv = uv;
        }
        else
        {
            vertex.uv = glm::vec2(0.0f, 0.0f);
        }

        glm::vec3 tangent;
        tangent.x = mesh->mTangents[i].x;
        tangent.y = mesh->mTangents[i].y;
        tangent.z = mesh->mTangents[i].z;
        vertex.tangent = tangent;

        glm::vec3 bitangent;
        bitangent.x = mesh->mBitangents[i].x;
        bitangent.y = mesh->mBitangents[i].y;
        bitangent.z = mesh->mBitangents[i].z;
        vertex.bitangent = bitangent;

        vertices.push_back(vertex);
    }

    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
        {
            indices.push_back(face.mIndices[j]);
        }
    }

    aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
    for (aiTextureType type = aiTextureType_NONE; type < aiTextureType_UNKNOWN + 1; type = (aiTextureType)(type + 1))
    {
        textures.push_back(load_material_textures(material, type));
    }

    return new pk::mesh(vertices, indices, textures);
}

std::vector<pk::texture *> pk::model::load_material_textures(aiMaterial *material, aiTextureType type) const
{
    std::vector<pk::texture *> textures;

    for (unsigned int i = 0; i < material->GetTextureCount(type); i++)
    {
        aiString path;
        material->GetTexture(type, i, &path);
        const std::string filename = directory + "/" + path.C_Str();
        if (loaded_textures.find(filename) == loaded_textures.end())
        {
            pk::texture *texture = new pk::texture(filename);
            textures.push_back(texture);
            loaded_textures[filename] = texture;
        }
        else
        {
            textures.push_back(loaded_textures[filename]);
        }
    }

    return textures;
}
