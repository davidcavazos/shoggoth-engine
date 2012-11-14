/*
 *    Copyright (c) 2012 David Cavazos <davido262@gmail.com>
 *
 *    Permission is hereby granted, free of charge, to any person
 *    obtaining a copy of this software and associated documentation
 *    files (the "Software"), to deal in the Software without
 *    restriction, including without limitation the rights to use,
 *    copy, modify, merge, publish, distribute, sublicense, and/or sell
 *    copies of the Software, and to permit persons to whom the
 *    Software is furnished to do so, subject to the following
 *    conditions:
 *
 *    The above copyright notice and this permission notice shall be
 *    included in all copies or substantial portions of the Software.
 *
 *    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 *    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 *    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 *    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 *    OTHER DEALINGS IN THE SOFTWARE.
 */


#include "shoggoth-engine/resources/modelloader.hpp"

#include <iostream>
#include <fstream>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "shoggoth-engine/resources/model.hpp"
#include "shoggoth-engine/resources/resources.hpp"
#include "shoggoth-engine/resources/texture.hpp"

using namespace std;

const string OPTIMIZED_BINARY_FILE_EXTENSION = ".model";

bool ModelLoader::load(const string& fileName, Model& model, Renderer* renderer, Resources* resources) {
//     cout << "TEMPORAL: always importing model for testing purposes" << endl;
//     import(fileName, model, renderer, resources);

    if (!loadBinary(fileName, model, renderer, resources)) {
        if (!import(fileName, model, renderer, resources))
            return false;
        writeBinary(fileName, model);
    }
    return true;
}

bool ModelLoader::import(const string& fileName, Model& model, Renderer* renderer, Resources* resources) {
    model.m_meshes.clear();
    cout << "Importing mesh: " << fileName << endl;

    Assimp::Importer importer;

    //check if file exists
    ifstream fin(fileName.c_str());
    if (!fin.is_open() || !fin.good()) {
        cerr << "Error: could not open file: " << fileName << endl;
        cerr << importer.GetErrorString() << endl;
        return false;
    }
    fin.close();

    const struct aiScene* scene;
    scene = importer.ReadFile(fileName.c_str(),
                              aiProcess_CalcTangentSpace |
                              aiProcess_GenSmoothNormals |
                              aiProcess_JoinIdenticalVertices |
                              aiProcess_ImproveCacheLocality |
                              aiProcess_LimitBoneWeights |
                              aiProcess_RemoveRedundantMaterials |
                              aiProcess_SplitLargeMeshes |
                              aiProcess_Triangulate |
                              aiProcess_FlipUVs |
                              aiProcess_GenUVCoords |
                              aiProcess_SortByPType |
                              aiProcess_FindDegenerates |
                              aiProcess_FindInvalidData |
                              aiProcess_FindInstances |
                              aiProcess_ValidateDataStructure |
                              aiProcess_OptimizeMeshes |
                              aiProcess_Debone);
    if (scene == 0) {
        cerr << importer.GetErrorString() << endl;
        cerr << "Aborting importing file: " << fileName << endl;
        return false;
    }

    string modelDir = fileName.substr(0, fileName.find_last_of("/\\") + 1);

    model.m_meshes.resize(scene->mNumMeshes, renderer);
    for (size_t n = 0; n < model.getTotalMeshes(); ++n) {
        const struct aiMesh* mesh = scene->mMeshes[n];

        // vertices
        model.mesh(n)->m_vertices.reserve(mesh->mNumVertices * 3);
        for (size_t i = 0; i < mesh->mNumVertices; ++i) {
            model.mesh(n)->m_vertices.push_back(mesh->mVertices[i].x);
            model.mesh(n)->m_vertices.push_back(mesh->mVertices[i].y);
            model.mesh(n)->m_vertices.push_back(mesh->mVertices[i].z);
        }

        // normals
        model.mesh(n)->m_normals.reserve(mesh->mNumVertices * 3);
        for (size_t i = 0; i < mesh->mNumVertices; ++i) {
            model.mesh(n)->m_normals.push_back(mesh->mNormals[i].x);
            model.mesh(n)->m_normals.push_back(mesh->mNormals[i].y);
            model.mesh(n)->m_normals.push_back(mesh->mNormals[i].z);
        }

        // indices
        model.mesh(n)->m_indices.reserve(mesh->mNumFaces * 3);
        for (size_t i = 0; i < mesh->mNumFaces; ++i) {
            if (mesh->mFaces[i].mNumIndices != 3)
                cerr << "Error: non-triangle face found, check model: " << fileName << endl;
            model.mesh(n)->m_indices.push_back(mesh->mFaces[i].mIndices[0]);
            model.mesh(n)->m_indices.push_back(mesh->mFaces[i].mIndices[1]);
            model.mesh(n)->m_indices.push_back(mesh->mFaces[i].mIndices[2]);
        }

        // uv coordinates
        if (mesh->GetNumUVChannels() > 0) {
            model.mesh(n)->m_uvCoords.reserve(mesh->mNumVertices * 2);
            for (size_t i = 0; i < mesh->mNumVertices; ++i) {
                model.mesh(n)->m_uvCoords.push_back(mesh->mTextureCoords[0][i].x);
                model.mesh(n)->m_uvCoords.push_back(mesh->mTextureCoords[0][i].y);
            }
        }

        // material
        aiColor3D color;
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        // diffuse color
        material->Get(AI_MATKEY_COLOR_DIFFUSE, color);
        model.mesh(n)->material()->setColor(MATERIAL_COLOR_DIFFUSE, color.r, color.g, color.b);
        // specular color
        material->Get(AI_MATKEY_COLOR_SPECULAR, color);
        model.mesh(n)->material()->setColor(MATERIAL_COLOR_SPECULAR, color.r, color.g, color.b);
        // ambient color
        material->Get(AI_MATKEY_COLOR_AMBIENT, color);
        model.mesh(n)->material()->setColor(MATERIAL_COLOR_AMBIENT, color.r, color.g, color.b);
        // emissive color
        material->Get(AI_MATKEY_COLOR_EMISSIVE, color);
        model.mesh(n)->material()->setColor(MATERIAL_COLOR_EMISSIVE, color.r, color.g, color.b);
        // shininess
        float shininess = 0.0f;
        float strength = 1.0f;
        material->Get(AI_MATKEY_SHININESS, shininess);
        material->Get(AI_MATKEY_SHININESS_STRENGTH, strength);
        model.mesh(n)->material()->setShininess(shininess * strength);

        // texture maps
        if (scene->HasTextures()) {
            cerr << "Support for meshes with embedded textures is not implemented" << endl;
            return true;
        }
        Texture* texture;
        aiString file;
        // diffuse map
        if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
            material->GetTexture(aiTextureType_DIFFUSE, 0, &file);
            texture = resources->generateTextureFromFile(modelDir + file.C_Str());
            model.mesh(n)->material()->setTextureMap(MATERIAL_DIFFUSE_MAP, texture);
        }
    }
    return true;
}

bool ModelLoader::loadBinary(const string& fileName, Model& model, Renderer* renderer, Resources* resources) {
    model.m_meshes.clear();
    string fileBin = fileName + OPTIMIZED_BINARY_FILE_EXTENSION;
    ifstream file(fileBin.c_str(), ios::in | ios::binary);
    if (!file.is_open() || !file.good()) {
        cerr << "Optimized binary file not found: " << fileBin << endl;
        return false;
    }

    // temporal values
    size_t size;

    cout << "Loading mesh: " << fileBin << endl;
    // load header
    file.read(reinterpret_cast<char*>(&size), sizeof(size_t));
    model.m_meshes.resize(size, renderer);

    // load body
    for (size_t n = 0; n < model.m_meshes.size(); ++n) {
        // vertices
        file.read(reinterpret_cast<char*>(&size), sizeof(size_t));
        model.mesh(n)->m_vertices.resize(size);
        file.read(reinterpret_cast<char*>(&model.mesh(n)->m_vertices[0]), size * sizeof(float));

        // normals
        file.read(reinterpret_cast<char*>(&size), sizeof(size_t));
        model.mesh(n)->m_normals.resize(size);
        file.read(reinterpret_cast<char*>(&model.mesh(n)->m_normals[0]), size * sizeof(float));

        // indices
        file.read(reinterpret_cast<char*>(&size), sizeof(size_t));
        model.mesh(n)->m_indices.resize(size);
        file.read(reinterpret_cast<char*>(&model.mesh(n)->m_indices[0]), size * sizeof(unsigned int));

        // uv coordinates
        file.read(reinterpret_cast<char*>(&size), sizeof(size_t));
        model.mesh(n)->m_uvCoords.resize(size);
        file.read(reinterpret_cast<char*>(&model.mesh(n)->m_uvCoords[0]), size * sizeof(float));

        // material
        color_t color;
        // diffuse color
        file.read(reinterpret_cast<char*>(&color), sizeof(color_t));
        model.mesh(n)->material()->setColor(MATERIAL_COLOR_DIFFUSE, color);
        // specular color
        file.read(reinterpret_cast<char*>(&color), sizeof(color_t));
        model.mesh(n)->material()->setColor(MATERIAL_COLOR_SPECULAR, color);
        // ambient color
        file.read(reinterpret_cast<char*>(&color), sizeof(color_t));
        model.mesh(n)->material()->setColor(MATERIAL_COLOR_AMBIENT, color);
        // emissive color
        file.read(reinterpret_cast<char*>(&color), sizeof(color_t));
        model.mesh(n)->material()->setColor(MATERIAL_COLOR_EMISSIVE, color);
        // shininess
        float shininess;
        file.read(reinterpret_cast<char*>(&shininess), sizeof(float));
        model.mesh(n)->material()->setShininess(shininess);

        // texture maps
        Texture* texture;
        char* temp;
        string mapName;
        // diffuse map
        file.read(reinterpret_cast<char*>(&size), sizeof(size_t));
        temp = new char[size + 1];
        file.read(reinterpret_cast<char*>(temp), size * sizeof(char));
        temp[size] = '\0';
        mapName = temp;
        delete[] temp;
        if (mapName.length() > 0) {
            texture = resources->generateTextureFromFile(mapName);
            model.mesh(n)->material()->setTextureMap(MATERIAL_DIFFUSE_MAP, texture);
        }
    }
    file.close();
    return true;
}

bool ModelLoader::writeBinary(const std::string& fileName, Model& model) {
    string fileBin = fileName + OPTIMIZED_BINARY_FILE_EXTENSION;
    cout << "Saving optimized binary mesh: " << fileBin << endl;
    ofstream file(fileBin.c_str(), ios::out | ios::binary | ios::trunc);
    if (!file.is_open() || !file.good()) {
        cerr << "Error: could not open file: " << fileBin << endl;
        return false;
    }

    // temporal values
    size_t size;

    // write header
    size = model.getTotalMeshes();
    file.write(reinterpret_cast<char*>(&size), sizeof(size_t));

    // write body
    for (size_t n = 0; n < model.m_meshes.size(); ++n) {
        // vertices
        size = model.mesh(n)->getVerticesSize();
        file.write(reinterpret_cast<char*>(&size), sizeof(size_t));
        file.write(reinterpret_cast<char*>(&model.mesh(n)->m_vertices[0]), size * sizeof(float));

        // normals
        size = model.mesh(n)->getNormalsSize();
        file.write(reinterpret_cast<char*>(&size), sizeof(size_t));
        file.write(reinterpret_cast<char*>(&model.mesh(n)->m_normals[0]), size * sizeof(float));

        // indices
        size = model.mesh(n)->getIndicesSize();
        file.write(reinterpret_cast<char*>(&size), sizeof(size_t));
        file.write(reinterpret_cast<char*>(&model.mesh(n)->m_indices[0]), size * sizeof(unsigned int));

        // uv maps
        size = model.mesh(n)->getUvCoordsSize();
        file.write(reinterpret_cast<char*>(&size), sizeof(size_t));
        file.write(reinterpret_cast<char*>(&model.mesh(n)->m_uvCoords[0]), size * sizeof(float));

        // material
        color_t color;
        // diffuse color
        color = model.mesh(n)->material()->getColor(MATERIAL_COLOR_DIFFUSE);
        file.write(reinterpret_cast<char*>(&color), sizeof(color_t));
        // specular color
        color = model.mesh(n)->material()->getColor(MATERIAL_COLOR_SPECULAR);
        file.write(reinterpret_cast<char*>(&color), sizeof(color_t));
        // ambient color
        color = model.mesh(n)->material()->getColor(MATERIAL_COLOR_AMBIENT);
        file.write(reinterpret_cast<char*>(&color), sizeof(color_t));
        // emissive color
        color = model.mesh(n)->material()->getColor(MATERIAL_COLOR_EMISSIVE);
        file.write(reinterpret_cast<char*>(&color), sizeof(color_t));
        // shininess
        float shininess = model.mesh(n)->material()->getShininess();
        file.write(reinterpret_cast<char*>(&shininess), sizeof(float));

        // texture maps
        const Texture* texture;
        string mapName;
        // diffuse map
        texture = model.mesh(n)->getMaterial()->getTextureMap(MATERIAL_DIFFUSE_MAP);
        if (texture != 0) {
            mapName = texture->getFileName();
            size = mapName.length();
            file.write(reinterpret_cast<char*>(&size), sizeof(size_t));
            file.write(reinterpret_cast<char*>(&mapName[0]), size * sizeof(char));
        }
    }
    file.close();
    return true;
}