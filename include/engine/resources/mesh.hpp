/*
    <one line to give the library's name and an idea of what it does.>
    Copyright (C) 2012  David Cavazos <davido262@gmail.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/


#ifndef MESH_HPP
#define MESH_HPP

#include "material.hpp"

class Renderer;
class ModelLoader;

class Mesh {
public:
    friend class ModelLoader;

    Mesh(Renderer* renderer);
    Mesh(const Mesh& rhs);
    Mesh& operator=(const Mesh& rhs);
    ~Mesh();

    unsigned int getMeshId() const;
    unsigned int getIndicesId() const;
    float getVertex(const size_t i) const;
    const std::vector<float>& getVertices() const;
    const float* getVerticesPtr() const;
    size_t getVerticesSize() const;
    size_t getVerticesBytes() const;
    float getNormal(const size_t i) const;
    const std::vector<float>& getNormals() const;
    const float* getNormalsPtr() const;
    size_t getNormalsSize() const;
    size_t getNormalsBytes() const;
    float getUvCoord(const size_t i) const;
    const std::vector<float>& getUvCoords() const;
    const float* getUvCoordsPtr() const;
    size_t getUvCoordsSize() const;
    size_t getUvCoordsBytes() const;
    unsigned int getIndex(const size_t i) const;
    const std::vector<unsigned int>& getIndices() const;
    const unsigned int* getIndicesPtr() const;
    size_t getIndicesSize() const;
    size_t getIndicesBytes() const;
    const Material* getMaterial() const;

    Material* material();

    void setVertices(const std::vector<float>& vertices);
    void setVertices(const float* vertices, const size_t size);
    void setNormals(const std::vector<float>& normals);
    void setNormals(const float* normals, const size_t size);
    void setUvCoords(const std::vector<float>& uvCoords);
    void setUvCoords(const float* uvCoords, const size_t size);
    void setIndices(const std::vector<unsigned int>& indices);
    void setIndices(const unsigned int* indices, const size_t size);
    void setMaterial(const Material& material);

    void uploadToGPU();

private:
    Renderer* m_renderer;
    unsigned int m_meshId;
    unsigned int m_indicesId;
    std::vector<float> m_vertices;
    std::vector<float> m_normals;
    std::vector<float> m_uvCoords;
    std::vector<unsigned int> m_indices;
    Material m_material;
};



inline unsigned int Mesh::getMeshId() const {
    return m_meshId;
}

inline unsigned int Mesh::getIndicesId() const {
    return m_indicesId;
}

inline float Mesh::getVertex(const size_t i) const {
    return m_vertices[i];
}

inline const std::vector< float >& Mesh::getVertices() const {
    return m_vertices;
}

inline const float* Mesh::getVerticesPtr() const {
    return &m_vertices[0];
}

inline size_t Mesh::getVerticesSize() const {
    return m_vertices.size();
}

inline size_t Mesh::getVerticesBytes() const {
    return m_vertices.size() * sizeof(float);
}

inline float Mesh::getNormal(const size_t i) const {
    return m_normals[i];
}

inline const std::vector< float >& Mesh::getNormals() const {
    return m_normals;
}

inline const float* Mesh::getNormalsPtr() const {
    return &m_normals[0];
}

inline size_t Mesh::getNormalsSize() const {
    return m_normals.size();
}

inline size_t Mesh::getNormalsBytes() const {
    return m_normals.size() * sizeof(float);
}

inline float Mesh::getUvCoord(const size_t i) const {
    return m_uvCoords[i];
}

inline const std::vector< float >& Mesh::getUvCoords() const {
    return m_uvCoords;
}

inline const float* Mesh::getUvCoordsPtr() const {
    return &m_uvCoords[0];
}

inline size_t Mesh::getUvCoordsSize() const {
    return m_uvCoords.size();
}

inline size_t Mesh::getUvCoordsBytes() const {
    return m_uvCoords.size() * sizeof(float);
}

inline unsigned int Mesh::getIndex(const size_t i) const {
    return m_indices[i];
}

inline const std::vector< unsigned int >& Mesh::getIndices() const {
    return m_indices;
}

inline const unsigned int* Mesh::getIndicesPtr() const {
    return &m_indices[0];
}

inline size_t Mesh::getIndicesSize() const {
    return m_indices.size();
}

inline size_t Mesh::getIndicesBytes() const {
    return m_indices.size() * sizeof(unsigned int);
}

inline const Material* Mesh::getMaterial() const {
    return &m_material;
}



inline Material* Mesh::material() {
    return &m_material;
}



inline void Mesh::setVertices(const std::vector< float >& vertices) {
    m_vertices = vertices;
}

inline void Mesh::setVertices(const float* vertices, const size_t size) {
    m_vertices.resize(size);
    for (size_t i = 0; i < size; ++i)
        m_vertices[i] = vertices[i];
}

inline void Mesh::setNormals(const std::vector< float >& normals) {
    m_normals = normals;
}

inline void Mesh::setNormals(const float* normals, const size_t size) {
    m_normals.resize(size);
    for (size_t i = 0; i < size; ++i)
        m_normals[i] = normals[i];
}

inline void Mesh::setIndices(const std::vector< unsigned int >& indices) {
    m_indices = indices;
}

inline void Mesh::setIndices(const unsigned int* indices, const size_t size) {
    m_indices.resize(size);
    for (size_t i = 0; i < size; ++i)
        m_indices[i] = indices[i];
}

inline void Mesh::setUvCoords(const std::vector< float >& uvCoords) {
    m_uvCoords = uvCoords;
}

inline void Mesh::setUvCoords(const float* uvCoords, const size_t size) {
    m_uvCoords.resize(size);
    for (size_t i = 0; i < size; ++i)
        m_uvCoords[i] = uvCoords[i];
}

inline void Mesh::setMaterial(const Material& material) {
    m_material = material;
}

#endif // MESH_HPP
