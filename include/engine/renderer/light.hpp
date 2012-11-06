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
#include "engine/kernel/component.hpp"

const std::string COMPONENT_LIGHT = "light";

class Renderer;

class color4_t {
public:
    float rgba[4];

    color4_t() {}
    color4_t(const float r, const float g, const float b, const float a) {
        rgba[0] = r;
        rgba[1] = g;
        rgba[2] = b;
        rgba[3] = a;
    }
};

class Light: public Component {
public:
    Light(Entity*const entity, Renderer* renderer);
    ~Light();

    const color4_t& getAmbient() const;
    const float* getAmbientPtr() const;
    const color4_t& getDiffuse() const;
    const float* getDiffusePtr() const;
    const color4_t& getSpecular() const;
    const float* getSpecularPtr() const;
    void set(const color4_t& ambient, const color4_t& diffuse, const color4_t& specular);
    void setAmbient(const color4_t& color);
    void setAmbient(const float r, const float g, const float b, const float a = 1.0f);
    void setDiffuse(const color4_t& color);
    void setDiffuse(const float r, const float g, const float b, const float a = 1.0f);
    void setSpecular(const color4_t& color);
    void setSpecular(const float r, const float g, const float b, const float a = 1.0f);

    void loadFromPtree(const std::string& path, const boost::property_tree::ptree& tree);
    void saveToPtree(const std::string& path, boost::property_tree::ptree& tree) const;

private:
    Renderer* m_renderer;
    color4_t m_ambient;
    color4_t m_diffuse;
    color4_t m_specular;

    Light(const Light& rhs);
    Light& operator=(const Light&);

    void cmdAmbient(const std::string& arg);
    void cmdDiffuse(const std::string& arg);
    void cmdSpecular(const std::string& arg);
};



inline std::ostream& operator<<(std::ostream& out, const color4_t& rhs) {
    out << rhs.rgba[0] << " " << rhs.rgba[1] << " " << rhs.rgba[2] << " " << rhs.rgba[3];
    return out;
}

inline std::istream& operator>>(std::istream& in, color4_t& rhs) {
    in >> rhs.rgba[0] >> rhs.rgba[1] >> rhs.rgba[2] >> rhs.rgba[3];
    return in;
}



inline const color4_t& Light::getAmbient() const {
    return m_ambient;
}

inline const float* Light::getAmbientPtr() const {
    return m_ambient.rgba;
}

inline const color4_t& Light::getDiffuse() const {
    return m_diffuse;
}

inline const float* Light::getDiffusePtr() const {
    return m_diffuse.rgba;
}

inline const color4_t& Light::getSpecular() const {
    return m_specular;
}

inline const float* Light::getSpecularPtr() const {
    return m_specular.rgba;
}

#endif // LIGHT_HPP
