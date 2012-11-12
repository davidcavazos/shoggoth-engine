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


#include "engine/renderer/light.hpp"

#include "engine/kernel/entity.hpp"
#include "engine/renderer/renderer.hpp"

using namespace std;
using namespace boost::property_tree;

const string LIGHT_DESCRIPTION = "$light";

const string XML_LIGHT_AMBIENT = "ambient";
const string XML_LIGHT_DIFFUSE = "diffuse";
const string XML_LIGHT_SPECULAR = "specular";


Light::Light(Entity* const entity, Renderer* renderer):
    Component(COMPONENT_LIGHT, entity),
    m_renderer(renderer),
    m_ambient(0.0f, 0.0f, 0.0f, 1.0f),
    m_diffuse(1.0f, 1.0f, 1.0f, 1.0f),
    m_specular(1.0f, 1.0f, 1.0f, 1.0f)
{
    m_description = LIGHT_DESCRIPTION;

    m_renderer->registerLight(this);

    m_entity->registerAttribute("ambient-color", boost::bind(&Light::cmdAmbient, this, _1));
    m_entity->registerAttribute("diffuse-color", boost::bind(&Light::cmdDiffuse, this, _1));
    m_entity->registerAttribute("specular-color", boost::bind(&Light::cmdSpecular, this, _1));
}

Light::~Light() {
    m_entity->unregisterAttribute("specular-color");
    m_entity->unregisterAttribute("diffuse-color");
    m_entity->unregisterAttribute("ambient-color");

    m_renderer->unregisterLight(this);
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

void Light::loadFromPtree(const string& path, const ptree& tree) {
    m_ambient = tree.get<color4_t>(ptree::path_type(path + XML_LIGHT_AMBIENT, XML_DELIMITER[0]),
                                   color4_t(0.0f, 0.0f, 0.0f, 1.0f));
    m_diffuse = tree.get<color4_t>(ptree::path_type(path + XML_LIGHT_DIFFUSE, XML_DELIMITER[0]),
                                   color4_t(1.0f, 1.0f, 1.0f, 1.0f));
    m_specular = tree.get<color4_t>(ptree::path_type(path + XML_LIGHT_SPECULAR, XML_DELIMITER[0]),
                                    color4_t(1.0f, 1.0f, 1.0f, 1.0f));
}

void Light::saveToPtree(const string& path, ptree& tree) const {
    string attr;
    attr = path + XML_LIGHT_AMBIENT;
    tree.put(ptree::path_type(attr, XML_DELIMITER[0]), getAmbient());
    attr = path + XML_LIGHT_DIFFUSE;
    tree.put(ptree::path_type(attr, XML_DELIMITER[0]), getDiffuse());
    attr = path + XML_LIGHT_SPECULAR;
    tree.put(ptree::path_type(attr, XML_DELIMITER[0]), getSpecular());
}



Light::Light(const Light& rhs):
    Component(rhs.m_type, rhs.m_entity),
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



string Light::cmdAmbient(deque<string>& args) {
    if (args.size() < 4)
        return "Error: too few arguments";
    float r = boost::lexical_cast<float>(args[0]);
    float g = boost::lexical_cast<float>(args[1]);
    float b = boost::lexical_cast<float>(args[2]);
    float a = boost::lexical_cast<float>(args[3]);
    setAmbient(r, g, b, a);
    return "";
}

string Light::cmdDiffuse(deque<string>& args) {
    if (args.size() < 4)
        return "Error: too few arguments";
    float r = boost::lexical_cast<float>(args[0]);
    float g = boost::lexical_cast<float>(args[1]);
    float b = boost::lexical_cast<float>(args[2]);
    float a = boost::lexical_cast<float>(args[3]);
    setDiffuse(r, g, b, a);
    return "";
}

string Light::cmdSpecular(deque<string>& args) {
    if (args.size() < 4)
        return "Error: too few arguments";
    float r = boost::lexical_cast<float>(args[0]);
    float g = boost::lexical_cast<float>(args[1]);
    float b = boost::lexical_cast<float>(args[2]);
    float a = boost::lexical_cast<float>(args[3]);
    setSpecular(r, g, b, a);
    return "";
}
