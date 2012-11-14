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

const std::string COMPONENT_LIGHT = "light";

class Renderer;

class Color4 {
public:
    Color4() {}
    Color4(const float* rgba) {setRGBA(rgba);}
    Color4(const float r, const float g, const float b, const float a) {setRGBA(r, g, b, a);}

    const float* getRGBA() const {return m_rgba;}
    float getR() const {return m_rgba[0];}
    float getG() const {return m_rgba[1];}
    float getB() const {return m_rgba[2];}
    float getA() const {return m_rgba[3];}
    void setRGBA(const float* rgba) {
        m_rgba[0] = rgba[0];
        m_rgba[1] = rgba[1];
        m_rgba[2] = rgba[2];
        m_rgba[3] = rgba[3];
    }
    void setRGBA(const float r, const float g, const float b, const float a) {
        m_rgba[0] = r;
        m_rgba[1] = g;
        m_rgba[2] = b;
        m_rgba[3] = a;
    }
    void setR(const float r) {m_rgba[0] = r;}
    void setG(const float g) {m_rgba[1] = g;}
    void setB(const float b) {m_rgba[2] = b;}
    void setA(const float a) {m_rgba[3] = a;}

private:
    float m_rgba[4];
};

const Color4 COLOR_BLACK = Color4(0.0f, 0.0f, 0.0f, 1.0f);
const Color4 COLOR_RED =   Color4(1.0f, 0.0f, 0.0f, 1.0f);
const Color4 COLOR_BLUE =  Color4(0.0f, 1.0f, 0.0f, 1.0f);
const Color4 COLOR_GREEN = Color4(0.0f, 0.0f, 1.0f, 1.0f);
const Color4 COLOR_WHITE = Color4(1.0f, 1.0f, 1.0f, 1.0f);


class Light: public Component {
public:
    Light(Entity*const entity, Renderer* renderer);
    ~Light();

    const Color4& getAmbient() const;
    const float* getAmbientPtr() const;
    const Color4& getDiffuse() const;
    const float* getDiffusePtr() const;
    const Color4& getSpecular() const;
    const float* getSpecularPtr() const;
    void set(const Color4& ambient, const Color4& diffuse, const Color4& specular);
    void setAmbient(const Color4& color);
    void setAmbient(const float r, const float g, const float b, const float a = 1.0f);
    void setDiffuse(const Color4& color);
    void setDiffuse(const float r, const float g, const float b, const float a = 1.0f);
    void setSpecular(const Color4& color);
    void setSpecular(const float r, const float g, const float b, const float a = 1.0f);

    void loadFromPtree(const std::string& path, const boost::property_tree::ptree& tree);
    void saveToPtree(const std::string& path, boost::property_tree::ptree& tree) const;

private:
    Renderer* m_renderer;
    Color4 m_ambient;
    Color4 m_diffuse;
    Color4 m_specular;

    Light(const Light& rhs);
    Light& operator=(const Light&);

    std::string cmdAmbient(std::deque<std::string>& args);
    std::string cmdDiffuse(std::deque<std::string>& args);
    std::string cmdSpecular(std::deque<std::string>& args);
};



inline std::ostream& operator<<(std::ostream& out, const Color4& rhs) {
    out << rhs.getR() << " " << rhs.getG() << " " << rhs.getB() << " " << rhs.getA();
    return out;
}

inline std::istream& operator>>(std::istream& in, Color4& rhs) {
    float r, g, b, a;
    in >> r >> g >> b >> a;
    rhs.setRGBA(r, g, b, a);
    return in;
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

#endif // LIGHT_HPP
