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


#include "engine/renderer/light.hpp"

#include <iostream>
#include "engine/renderer/renderer.hpp"

using namespace std;

const string LIGHT_DESCRIPTION = "$light";

Light::Light(Entity* const entity, Renderer* renderer):
    Component(COMPONENT_LIGHT, entity),
    m_renderer(renderer)
{
    m_description = LIGHT_DESCRIPTION;

    m_ambient[0] = 0.0f;
    m_ambient[1] = 0.0f;
    m_ambient[2] = 0.0f;
    m_ambient[3] = 1.0f;

    m_diffuse[0] = 1.0f;
    m_diffuse[1] = 1.0f;
    m_diffuse[2] = 1.0f;
    m_diffuse[3] = 1.0f;

    m_specular[0] = 1.0f;
    m_specular[1] = 1.0f;
    m_specular[2] = 1.0f;
    m_specular[3] = 1.0f;

    m_renderer->m_lights.insert(this);
}

Light::~Light() {
    m_renderer->m_lights.erase(this);
}



Light::Light(const Light& rhs):
    Component(COMPONENT_LIGHT, rhs.m_entity),
    m_renderer(rhs.m_renderer)
{
    cerr << "Error: Light copy constructor should not be called!" << endl;
}

Light& Light::operator=(const Light&) {
    cerr << "Error: Light assignment operator should not be called!" << endl;
    return *this;
}
