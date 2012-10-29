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


#ifndef RENDERABLEMESH_HPP
#define RENDERABLEMESH_HPP

#include "engine/kernel/component.hpp"

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
