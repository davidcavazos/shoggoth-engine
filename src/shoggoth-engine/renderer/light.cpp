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


#include "shoggoth-engine/renderer/light.hpp"

#include "shoggoth-engine/kernel/entity.hpp"
#include "shoggoth-engine/renderer/renderer.hpp"

using namespace std;
using namespace boost::property_tree;

const string LIGHT_DESCRIPTION = "$light";

const string XML_LIGHT_TYPE = "lighttype";
const string XML_LIGHT_AMBIENT = "ambient";
const string XML_LIGHT_DIFFUSE = "diffuse";
const string XML_LIGHT_SPECULAR = "specular";
const string XML_SPOT_EXPONENT = "spotexponent";
const string XML_SPOT_CUTOFF = "spotcutoff";
const string XML_CONSTANT_ATTENUATION = "constantattenuation";
const string XML_LINEAR_ATTENUATION = "linearattenuation";
const string XML_QUADRATIC_ATTENUATION = "quadraticattenuation";


Light::Light(Entity* const _entity, Renderer* renderer):
    Component(COMPONENT_LIGHT, _entity),
    m_renderer(renderer),
    m_lightType(LIGHT_POINTLIGHT),
    m_ambient(0.0f, 0.0f, 0.0f, 1.0f),
    m_diffuse(1.0f, 1.0f, 1.0f, 1.0f),
    m_specular(1.0f, 1.0f, 1.0f, 1.0f),
    m_spotExponent(0.0f),
    m_spotCutoff(180.0f),
    m_constantAttenuation(1.0f),
    m_linearAttenuation(0.0f),
    m_quadraticAttenuation(0.0f)
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


void Light::setColors(const Color4& ambient, const Color4& diffuse, const Color4& specular) {
    m_ambient = ambient;
    m_diffuse = diffuse;
    m_specular = specular;
    m_renderer->updateLegacyLights();
}

void Light::setAmbient(const Color4& color) {
    m_ambient = color;
    m_renderer->updateLegacyLights();
}

void Light::setAmbient(const float r, const float g, const float b, const float a) {
    m_ambient.setRGBA(r, g, b, a);
    m_renderer->updateLegacyLights();
}

void Light::setDiffuse(const Color4& color) {
    m_diffuse = color;
    m_renderer->updateLegacyLights();
}

void Light::setDiffuse(const float r, const float g, const float b, const float a) {
    m_diffuse.setRGBA(r, g, b, a);
    m_renderer->updateLegacyLights();
}

void Light::setSpecular(const Color4& color) {
    m_specular = color;
    m_renderer->updateLegacyLights();
}

void Light::setSpecular(const float r, const float g, const float b, const float a) {
    m_specular.setRGBA(r, g, b, a);
    m_renderer->updateLegacyLights();
}

void Light::loadFromPtree(const string& path, const ptree& tree) {
    m_lightType = (light_t)tree.get<int>(xmlPath(path + XML_LIGHT_TYPE), (int)LIGHT_POINTLIGHT);
    m_ambient = tree.get<Color4>(xmlPath(path + XML_LIGHT_AMBIENT), COLOR_BLACK);
    m_diffuse = tree.get<Color4>(xmlPath(path + XML_LIGHT_DIFFUSE), COLOR_WHITE);
    m_specular = tree.get<Color4>(xmlPath(path + XML_LIGHT_SPECULAR), COLOR_WHITE);
    m_spotExponent = tree.get<float>(xmlPath(path + XML_SPOT_EXPONENT), 0.0f);
    m_spotCutoff = tree.get<float>(xmlPath(path + XML_SPOT_CUTOFF), 180.0f);
    m_constantAttenuation = tree.get<float>(xmlPath(path + XML_CONSTANT_ATTENUATION), 1.0f);
    m_linearAttenuation = tree.get<float>(xmlPath(path + XML_LINEAR_ATTENUATION), 0.0f);
    m_quadraticAttenuation = tree.get<float>(xmlPath(path + XML_QUADRATIC_ATTENUATION), 0.0f);
}

void Light::saveToPtree(const string& path, ptree& tree) const {
    tree.put(xmlPath(path + XML_LIGHT_TYPE), (int)getLightType());
    tree.put(xmlPath(path + XML_LIGHT_AMBIENT), getAmbient());
    tree.put(xmlPath(path + XML_LIGHT_DIFFUSE), getDiffuse());
    tree.put(xmlPath(path + XML_LIGHT_SPECULAR), getSpecular());
    tree.put(xmlPath(path + XML_SPOT_EXPONENT), getSpotExponent());
    tree.put(xmlPath(path + XML_SPOT_CUTOFF), getSpotCutoff());
    tree.put(xmlPath(path + XML_CONSTANT_ATTENUATION), getConstantAttenuation());
    tree.put(xmlPath(path + XML_LINEAR_ATTENUATION), getLinearAttenuation());
    tree.put(xmlPath(path + XML_QUADRATIC_ATTENUATION), getQuadraticAttenuation());
}



Light::Light(const Light& rhs):
    Component(rhs.m_type, rhs.m_entity),
    m_renderer(rhs.m_renderer),
    m_lightType(rhs.m_lightType),
    m_ambient(rhs.m_ambient),
    m_diffuse(rhs.m_diffuse),
    m_specular(rhs.m_specular),
    m_spotExponent(rhs.m_spotExponent),
    m_spotCutoff(rhs.m_spotCutoff),
    m_constantAttenuation(rhs.m_constantAttenuation),
    m_linearAttenuation(rhs.m_linearAttenuation),
    m_quadraticAttenuation(rhs.m_quadraticAttenuation)
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
