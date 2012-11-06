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


#ifndef RENDERABLEMESH_HPP
#define RENDERABLEMESH_HPP

#include "engine/kernel/component.hpp"

const std::string COMPONENT_RENDERABLEMESH = "renderablemesh";

const std::string RENDERABLEMESH_BOX_DESCRIPTION = "$box";
const std::string RENDERABLEMESH_FILE_DESCRIPTION = "$file";

class Renderer;
class Resources;
class Model;

class RenderableMesh: public Component {
public:
    RenderableMesh(Entity* const entity, Renderer* renderer, Resources* resources);
    ~RenderableMesh();

    const Model* getModel() const;

    Model* model();

    void loadBox(const double lengthX, const double lengthY, const double lengthZ);
    void loadFromFile(const std::string& fileName);

    void loadFromPtree(const std::string& path, const boost::property_tree::ptree& tree);
    void saveToPtree(const std::string& path, boost::property_tree::ptree& tree) const;

private:
    Renderer* m_renderer;
    Resources* m_resources;
    Model* m_model;

    RenderableMesh(const RenderableMesh& rhs);
    RenderableMesh& operator=(const RenderableMesh&);

    void cmdLoadModelBox(const std::string& arg);
    void cmdLoadModelFile(const std::string& arg);
};



inline const Model* RenderableMesh::getModel() const {
    return m_model;
}



inline Model* RenderableMesh::model() {
    return m_model;
}

#endif // RENDERABLEMESH_HPP
