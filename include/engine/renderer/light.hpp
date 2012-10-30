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


#ifndef LIGHT_HPP
#define LIGHT_HPP

#include <iostream>
#include "engine/kernel/component.hpp"

class Renderer;

class color4_t {
public:
    float rgba[4];

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
    void setAmbient(const float r, const float g, const float b, const float a = 1.0f);
    void setDiffuse(const float r, const float g, const float b, const float a = 1.0f);
    void setSpecular(const float r, const float g, const float b, const float a = 1.0f);

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

inline void Light::setAmbient(const float r, const float g, const float b, const float a) {
    m_ambient.rgba[0] = r;
    m_ambient.rgba[1] = g;
    m_ambient.rgba[2] = b;
    m_ambient.rgba[3] = a;
}

inline void Light::setDiffuse(const float r, const float g, const float b, const float a) {
    m_diffuse.rgba[0] = r;
    m_diffuse.rgba[1] = g;
    m_diffuse.rgba[2] = b;
    m_diffuse.rgba[3] = a;
}

inline void Light::setSpecular(const float r, const float g, const float b, const float a) {
    m_specular.rgba[0] = r;
    m_specular.rgba[1] = g;
    m_specular.rgba[2] = b;
    m_specular.rgba[3] = a;
}

#endif // LIGHT_HPP
