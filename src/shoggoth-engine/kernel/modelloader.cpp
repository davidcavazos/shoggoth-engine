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


#include "shoggoth-engine/kernel/modelloader.hpp"

#include <iostream>
#include <fstream>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "shoggoth-engine/kernel/model.hpp"
#include "shoggoth-engine/renderer/texture.hpp"

using namespace std;

const string OPTIMIZED_BINARY_FILE_EXTENSION = ".model";

bool ModelLoader::load(const string& fileName, Model& model) {
//     cout << "TEMPORAL: always importing model for testing purposes" << endl;
//     import(fileName, model);

    if (!loadBinary(fileName, model)) {
        if (!import(fileName, model))
            return false;
        writeBinary(fileName, model);
    }
    return true;
}

bool ModelLoader::import(const string& fileName, Model& model) {
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

    model.m_meshes.resize(scene->mNumMeshes);
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
    }
    return true;
}

bool ModelLoader::loadBinary(const string& fileName, Model& model) {
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
    model.m_meshes.resize(size);

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
    }
    file.close();
    return true;
}
