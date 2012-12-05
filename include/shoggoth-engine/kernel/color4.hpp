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


#ifndef COLOR4_HPP
#define COLOR4_HPP

#include <iostream>

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



inline std::ostream& operator<<(std::ostream& out, const Color4& c) {
    out << c.getR() << " " << c.getG() << " " << c.getB() << " " << c.getA();
    return out;
}

inline std::istream& operator>>(std::istream& in, Color4& c) {
    float r, g, b, a;
    in >> r >> g >> b >> a;
    c.setRGBA(r, g, b, a);
    return in;
}



const Color4 COLOR_BLACK = Color4(0.0f, 0.0f, 0.0f, 1.0f);
const Color4 COLOR_RED =   Color4(1.0f, 0.0f, 0.0f, 1.0f);
const Color4 COLOR_BLUE =  Color4(0.0f, 1.0f, 0.0f, 1.0f);
const Color4 COLOR_GREEN = Color4(0.0f, 0.0f, 1.0f, 1.0f);
const Color4 COLOR_WHITE = Color4(1.0f, 1.0f, 1.0f, 1.0f);

#endif // COLOR4_HPP
