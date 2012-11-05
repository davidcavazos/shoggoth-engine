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


#include "engine/resources/mesh.hpp"

#include <iostream>
#include "engine/renderer/renderer.hpp"

using namespace std;

Mesh::Mesh(Renderer* renderer):
    m_renderer(renderer),
    m_meshId(0),
    m_indicesId(0),
    m_vertices(),
    m_normals(),
    m_uvCoords(),
    m_indices(),
    m_material()
{}

Mesh::Mesh(const Mesh& rhs):
    m_renderer(rhs.m_renderer),
    m_meshId(rhs.m_meshId),
    m_indicesId(rhs.m_indicesId),
    m_vertices(rhs.m_vertices),
    m_normals(rhs.m_normals),
    m_uvCoords(rhs.m_uvCoords),
    m_indices(rhs.m_indices),
    m_material(rhs.m_material)
{}

Mesh& Mesh::operator=(const Mesh& rhs) {
    if (this == &rhs)
        return *this;
    m_renderer = rhs.m_renderer;
    m_meshId = rhs.m_meshId;
    m_indicesId = rhs.m_indicesId;
    m_vertices = rhs.m_vertices;
    m_normals = rhs.m_normals;
    m_uvCoords = rhs.m_uvCoords;
    m_indices = rhs.m_indices;
    m_material = rhs.m_material;
    return *this;
}

Mesh::~Mesh() {
    m_renderer->deleteModel(m_meshId, m_indicesId);
}

void Mesh::uploadToGPU() {
    m_renderer->uploadModel(m_meshId, m_indicesId, *this);
}
