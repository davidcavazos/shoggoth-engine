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


#include "shoggoth-engine/resources/material.hpp"

#include "shoggoth-engine/resources/texture.hpp"

Material::Material():
    m_colors(),
    m_shininess(0.0f),
    m_textureMaps()
{
    m_colors.resize(TOTAL_MATERIAL_COLORS);
    setColor(MATERIAL_COLOR_DIFFUSE, 0.8f, 0.8f, 0.8f);
    setColor(MATERIAL_COLOR_SPECULAR, 0.0f, 0.0f, 0.0f);
    setColor(MATERIAL_COLOR_AMBIENT, 0.2f, 0.2f, 0.2f);
    setColor(MATERIAL_COLOR_EMISSIVE, 0.0f, 0.0f, 0.0f);
    m_textureMaps.resize(TOTAL_MATERIAL_MAPS);
}
