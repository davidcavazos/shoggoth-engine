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
