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


#ifndef LIGHT_HPP
#define LIGHT_HPP

#include <iostream>
#include "shoggoth-engine/kernel/component.hpp"
#include "shoggoth-engine/kernel/color4.hpp"

const std::string COMPONENT_LIGHT = "light";

class Renderer;

typedef enum {
    LIGHT_POINTLIGHT,
    LIGHT_SPOTLIGHT,
    LIGHT_DIRECTIONAL
} light_t;

class Light: public Component {
public:
    Light(Entity*const _entity, Renderer* renderer);
    ~Light();

    const light_t& getLightType() const;
    const Color4& getAmbient() const;
    const float* getAmbientPtr() const;
    const Color4& getDiffuse() const;
    const float* getDiffusePtr() const;
    const Color4& getSpecular() const;
    const float* getSpecularPtr() const;
    float getSpotExponent() const;
    float getSpotCutoff() const;
    float getConstantAttenuation() const;
    float getLinearAttenuation() const;
    float getQuadraticAttenuation() const;
    void setLightType(const light_t& type);
    void setColors(const Color4& ambient, const Color4& diffuse, const Color4& specular);
    void setAmbient(const Color4& color);
    void setAmbient(const float r, const float g, const float b, const float a = 1.0f);
    void setDiffuse(const Color4& color);
    void setDiffuse(const float r, const float g, const float b, const float a = 1.0f);
    void setSpecular(const Color4& color);
    void setSpecular(const float r, const float g, const float b, const float a = 1.0f);
    void setSpotExponent(const float spotExponent);
    void setSpotCutoff(const float spotCutoff);
    void setConstantAttenuation(const float constantAttenuation);
    void setLinearAttenuation(const float linearAttenuation);
    void setQuadraticAttenuation(const float quadraticAttenuation);

    void loadFromPtree(const std::string& path, const boost::property_tree::ptree& tree);
    void saveToPtree(const std::string& path, boost::property_tree::ptree& tree) const;

private:
    Renderer* m_renderer;
    light_t m_lightType;
    Color4 m_ambient;
    Color4 m_diffuse;
    Color4 m_specular;
    float m_spotExponent;
    float m_spotCutoff;
    float m_constantAttenuation;
    float m_linearAttenuation;
    float m_quadraticAttenuation;

    Light(const Light& rhs);
    Light& operator=(const Light&);

    std::string cmdAmbient(std::deque<std::string>& args);
    std::string cmdDiffuse(std::deque<std::string>& args);
    std::string cmdSpecular(std::deque<std::string>& args);
};



inline const light_t& Light::getLightType() const {
    return m_lightType;
}

inline const Color4& Light::getAmbient() const {
    return m_ambient;
}

inline const float* Light::getAmbientPtr() const {
    return m_ambient.getRGBA();
}

inline const Color4& Light::getDiffuse() const {
    return m_diffuse;
}

inline const float* Light::getDiffusePtr() const {
    return m_diffuse.getRGBA();
}

inline const Color4& Light::getSpecular() const {
    return m_specular;
}

inline const float* Light::getSpecularPtr() const {
    return m_specular.getRGBA();
}

inline float Light::getSpotExponent() const {
    return m_spotExponent;
}

inline float Light::getSpotCutoff() const {
    return m_spotCutoff;
}

inline float Light::getConstantAttenuation() const {
    return m_constantAttenuation;
}

inline float Light::getLinearAttenuation() const {
    return m_linearAttenuation;
}

inline float Light::getQuadraticAttenuation() const {
    return m_quadraticAttenuation;
}

inline void Light::setLightType(const light_t& type) {
    m_lightType = type;
}

inline void Light::setSpotExponent(const float spotExponent) {
    m_spotExponent = spotExponent;
}

inline void Light::setSpotCutoff(const float spotCutoff) {
    m_spotCutoff = spotCutoff;
}

inline void Light::setConstantAttenuation(const float constantAttenuation) {
    m_constantAttenuation = constantAttenuation;
}

inline void Light::setLinearAttenuation(const float linearAttenuation) {
    m_linearAttenuation = linearAttenuation;
}

inline void Light::setQuadraticAttenuation(const float quadraticAttenuation) {
    m_quadraticAttenuation = quadraticAttenuation;
}

#endif // LIGHT_HPP
