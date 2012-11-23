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


#include "shoggoth-engine/renderer/renderablemesh.hpp"

#include <sstream>
#include "shoggoth-engine/kernel/entity.hpp"
#include "shoggoth-engine/renderer/renderer.hpp"
#include "shoggoth-engine/renderer/culling.hpp"
#include "shoggoth-engine/resources/model.hpp"
#include "shoggoth-engine/resources/resources.hpp"

using namespace std;
using namespace boost::property_tree;

const string XML_RENDERABLEMESH_MODEL = "model";


RenderableMesh::RenderableMesh(Entity* const _entity, Renderer* renderer, Resources* resources):
    Component(COMPONENT_RENDERABLEMESH, _entity),
    m_renderer(renderer),
    m_resources(resources),
    m_model(0)
{
    m_renderer->registerRenderableMesh(this);

    m_entity->registerCommand("load-model-box", boost::bind(&RenderableMesh::cmdLoadModelBox, this, _1));
    m_entity->registerCommand("load-model-file", boost::bind(&RenderableMesh::cmdLoadModelFile, this, _1));
}

RenderableMesh::~RenderableMesh() {
    m_entity->unregisterCommand("load-model-file");
    m_entity->unregisterCommand("load-model-box");

    Culling::unregisterForCulling(this);
    m_renderer->unregisterRenderableMesh(this);
}



void RenderableMesh::loadBox(const double lengthX, const double lengthY, const double lengthZ) {
    m_description = RENDERABLEMESH_BOX_DESCRIPTION + " " +
            boost::lexical_cast<string>(lengthX) + " " +
            boost::lexical_cast<string>(lengthY) + " " +
            boost::lexical_cast<string>(lengthZ);
    m_model = m_resources->generateBox(m_description, lengthX, lengthY, lengthZ);
    Culling::registerForCulling(this);
}

void RenderableMesh::loadFromFile(const string& fileName) {
    m_description = RENDERABLEMESH_FILE_DESCRIPTION + " " + fileName;
    m_model = m_resources->generateModelFromFile(fileName);
    Culling::registerForCulling(this);
}

void RenderableMesh::loadFromPtree(const string& path, const ptree& tree) {
    string _model = tree.get<string>(xmlPath(path + XML_RENDERABLEMESH_MODEL), "empty");
    stringstream ss(_model);
    ss >> _model;
    if (_model.compare(RENDERABLEMESH_BOX_DESCRIPTION) == 0) {
        double x, y, z;
        ss >> x >> y >> z;
        loadBox(x, y, z);
    }
    else if (_model.compare(RENDERABLEMESH_FILE_DESCRIPTION) == 0) {
        string file;
        ss >> file;
        loadFromFile(file);
    }
    else
        cerr << "Error: unknown renderablemesh model type: " << _model << endl;
}

void RenderableMesh::saveToPtree(const string& path, ptree& tree) const {
    tree.put(xmlPath(path + XML_RENDERABLEMESH_MODEL), getDescription());
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


string RenderableMesh::cmdLoadModelBox(deque<string>& args) {
    if (args.size() < 3)
        return "Error: too few arguments";
    double x = boost::lexical_cast<double>(args[0]);
    double y = boost::lexical_cast<double>(args[1]);
    double z = boost::lexical_cast<double>(args[2]);
    loadBox(x, y, z);
    return "";
}
