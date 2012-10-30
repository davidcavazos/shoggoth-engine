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


#include "engine/kernel/component.hpp"

#include <iostream>
#include <ostream>
#include "engine/kernel/entity.hpp"

using namespace std;

Component::Component(const component_t type, Entity* const entity):
    m_entity(entity),
    m_type(type),
    m_description()
{
    m_entity->m_components[m_type] = this;
}

Component::~Component() {
    m_entity->m_components[m_type] = 0;
}

Component::Component(const Component& rhs):
    m_entity(rhs.m_entity),
    m_type(rhs.m_type),
    m_description(rhs.m_description)
{
    cerr << "Error: Component copy constructor should not be called!" << endl;
}

Component& Component::operator=(const Component&) {
    cerr << "Error: Component assignment operator should not be called!" << endl;
    return *this;
}

ostream& operator<<(ostream& out, const Component& rhs) {
    switch (rhs.getType()) {
    case COMPONENT_CAMERA:
        out << "CAMERA";
        break;
    case COMPONENT_RENDERABLE_MESH:
        out << "RENDERABLE_MESH";
        break;
    case COMPONENT_RIGIDBODY:
        out << "PHYSICS";
        break;
    default:
        out << "INVALID";
    }
    return out;
}
