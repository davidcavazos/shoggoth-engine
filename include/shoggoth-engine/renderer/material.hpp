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


#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include <iostream>
#include <vector>
#include "shoggoth-engine/renderer/shader.hpp"
#include "shoggoth-engine/kernel/color4.hpp"

class Texture;


class Material {
public:
    Material(const bool useShaders_ARB);
    Material(const Material& rhs);

    Material& operator=(const Material& rhs);

    bool loadFromFile(const std::string& fileName);
    void useMaterial() const;

private:
    Shader m_shader;
    std::string m_vertexShaderFile;
    std::string m_fragmentShaderFile;
    Color4 m_diffuseColor;
    Color4 m_ambientColor;
    Color4 m_emissiveColor;
    Color4 m_specularColor;
    float m_shininess;
    float m_opacity;
    Texture* m_diffuseMap;
    Texture* m_ambientMap;
    Texture* m_emissiveMap;
    Texture* m_specularMap;
    Texture* m_shininessMap;
    Texture* m_normalMap;
    Texture* m_opacityMap;
    Texture* m_reflectionMap;
    Texture* m_refractionMap;
    Texture* m_lightMap;
};

#endif // MATERIAL_HPP
