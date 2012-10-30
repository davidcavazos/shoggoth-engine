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


#include "engine/renderer/renderablemesh.hpp"

#include <sstream>
#include "engine/kernel/entity.hpp"
#include "engine/renderer/renderer.hpp"
#include "engine/resources/model.hpp"
#include "engine/resources/resources.hpp"

using namespace std;

const string BOX_DESCRIPTION = "$box";

RenderableMesh::RenderableMesh(Entity* const entity, Renderer* renderer, Resources* resources):
    Component(COMPONENT_RENDERABLE_MESH, entity),
    m_renderer(renderer),
    m_resources(resources),
    m_model(0)
{
    m_renderer->m_model.insert(this);

    m_entity->registerCommand("load-model-box", boost::bind(&RenderableMesh::cmdLoadModelBox, this, _1));
    m_entity->registerCommand("load-model-file", boost::bind(&RenderableMesh::cmdLoadModelFile, this, _1));
}

RenderableMesh::~RenderableMesh() {
    m_renderer->m_model.erase(this);
}



void RenderableMesh::loadBox(const double lengthX, const double lengthY, const double lengthZ) {
    stringstream ss;
    ss << BOX_DESCRIPTION << " " << lengthX << " " << lengthY << " " << lengthZ;
    m_description = ss.str();
    m_model = m_resources->generateBox(m_description, lengthX, lengthY, lengthZ);
}

void RenderableMesh::loadFromFile(const string& fileName) {
    m_description = fileName;
    m_model = m_resources->generateModelFromFile(fileName);
}



RenderableMesh::RenderableMesh(const RenderableMesh& rhs):
    Component(rhs.m_type, rhs.m_entity),
    m_renderer(rhs.m_renderer),
    m_resources(rhs.m_resources),
    m_model(rhs.m_model)
{
    cerr << "Error: RenderableMesh copy constructor should not be called!" << endl;
}

RenderableMesh& RenderableMesh::operator=(const RenderableMesh&) {
    cerr << "Error: RenderableMesh assignment operator should not be called!" << endl;
    return *this;
}


void RenderableMesh::cmdLoadModelBox(const std::string& arg) {
    double x, y, z;
    stringstream ss(arg);
    ss >> x >> y >> z;
    loadBox(x, y, z);
}

void RenderableMesh::cmdLoadModelFile(const std::string& arg) {
    loadFromFile(arg);
}

