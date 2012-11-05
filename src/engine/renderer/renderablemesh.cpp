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


#include "engine/renderer/renderablemesh.hpp"

#include <sstream>
#include "engine/kernel/entity.hpp"
#include "engine/renderer/renderer.hpp"
#include "engine/resources/model.hpp"
#include "engine/resources/resources.hpp"

using namespace std;

RenderableMesh::RenderableMesh(Entity* const entity, Renderer* renderer, Resources* resources):
    Component(COMPONENT_RENDERABLE_MESH, entity),
    m_renderer(renderer),
    m_resources(resources),
    m_model(0)
{
    m_renderer->m_models.insert(this);

    m_entity->registerCommand("load-model-box", boost::bind(&RenderableMesh::cmdLoadModelBox, this, _1));
    m_entity->registerCommand("load-model-file", boost::bind(&RenderableMesh::cmdLoadModelFile, this, _1));
}

RenderableMesh::~RenderableMesh() {
    m_renderer->m_models.erase(this);
    m_entity->unregisterCommand("load-model-file");
    m_entity->unregisterCommand("load-model-box");
}



void RenderableMesh::loadBox(const double lengthX, const double lengthY, const double lengthZ) {
    stringstream ss;
    ss << RENDERABLEMESH_BOX_DESCRIPTION << " " << lengthX << " " << lengthY << " " << lengthZ;
    m_description = ss.str();
    m_model = m_resources->generateBox(m_description, lengthX, lengthY, lengthZ);
}

void RenderableMesh::loadFromFile(const string& fileName) {
    stringstream ss;
    ss << RENDERABLEMESH_FILE_DESCRIPTION << " " << fileName;
    m_description = ss.str();
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

