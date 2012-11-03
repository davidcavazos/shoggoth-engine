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

#include "engine/kernel/entity.hpp"
#include "engine/renderer/renderer.hpp"

using namespace std;

const string LIGHT_DESCRIPTION = "$light";

Light::Light(Entity* const entity, Renderer* renderer):
    Component(COMPONENT_LIGHT, entity),
    m_renderer(renderer),
    m_ambient(0.0f, 0.0f, 0.0f, 1.0f),
    m_diffuse(1.0f, 1.0f, 1.0f, 1.0f),
    m_specular(1.0f, 1.0f, 1.0f, 1.0f)
{
    m_description = LIGHT_DESCRIPTION;

    m_renderer->m_lights.insert(this);

    m_entity->registerAttribute("ambient-color", boost::bind(&Light::cmdAmbient, this, _1));
    m_entity->registerAttribute("diffuse-color", boost::bind(&Light::cmdDiffuse, this, _1));
    m_entity->registerAttribute("specular-color", boost::bind(&Light::cmdSpecular, this, _1));

    m_renderer->initLighting();
}

Light::~Light() {
    m_entity->unregisterAttribute("specular-color");
    m_entity->unregisterAttribute("diffuse-color");
    m_entity->unregisterAttribute("ambient-color");
    m_renderer->m_lights.erase(this);
    m_renderer->initLighting();
}


void Light::set(const color4_t& ambient, const color4_t& diffuse, const color4_t& specular) {
    m_ambient = ambient;
    m_diffuse = diffuse;
    m_specular = specular;
    m_renderer->updateLights();
}

void Light::setAmbient(const color4_t& color) {
    m_ambient = color;
    m_renderer->updateLights();
}

void Light::setAmbient(const float r, const float g, const float b, const float a) {
    m_ambient.rgba[0] = r;
    m_ambient.rgba[1] = g;
    m_ambient.rgba[2] = b;
    m_ambient.rgba[3] = a;
    m_renderer->updateLights();
}

void Light::setDiffuse(const color4_t& color) {
    m_diffuse = color;
    m_renderer->updateLights();
}

void Light::setDiffuse(const float r, const float g, const float b, const float a) {
    m_diffuse.rgba[0] = r;
    m_diffuse.rgba[1] = g;
    m_diffuse.rgba[2] = b;
    m_diffuse.rgba[3] = a;
    m_renderer->updateLights();
}

void Light::setSpecular(const color4_t& color) {
    m_specular = color;
    m_renderer->updateLights();
}

void Light::setSpecular(const float r, const float g, const float b, const float a) {
    m_specular.rgba[0] = r;
    m_specular.rgba[1] = g;
    m_specular.rgba[2] = b;
    m_specular.rgba[3] = a;
    m_renderer->updateLights();
}



Light::Light(const Light& rhs):
    Component(COMPONENT_LIGHT, rhs.m_entity),
    m_renderer(rhs.m_renderer),
    m_ambient(rhs.m_ambient),
    m_diffuse(rhs.m_diffuse),
    m_specular(rhs.m_specular)
{
    cerr << "Error: Light copy constructor should not be called!" << endl;
}

Light& Light::operator=(const Light&) {
    cerr << "Error: Light assignment operator should not be called!" << endl;
    return *this;
}



void Light::cmdAmbient(const std::string& arg) {
    color4_t color;
    stringstream ss(arg);
    ss >> color;
    setAmbient(color);
}

void Light::cmdDiffuse(const std::string& arg) {
    color4_t color;
    stringstream ss(arg);
    ss >> color;
    setDiffuse(color);
}

void Light::cmdSpecular(const std::string& arg) {
    color4_t color;
    stringstream ss(arg);
    ss >> color;
    setSpecular(color);
}
