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

class Texture;

typedef enum {
    MATERIAL_COLOR_DIFFUSE,
    MATERIAL_COLOR_SPECULAR,
    MATERIAL_COLOR_AMBIENT,
    MATERIAL_COLOR_EMISSIVE,
    TOTAL_MATERIAL_COLORS
} material_color_t;

typedef enum {
    MATERIAL_DIFFUSE_MAP,
    MATERIAL_AMBIENT_MAP,
    MATERIAL_EMISSIVE_MAP,
    MATERIAL_HEIGHT_MAP,
    MATERIAL_NORMALS_MAP,
    MATERIAL_SHININESS_MAP,
    MATERIAL_OPACITY_MAP,
    MATERIAL_DISPLACEMENT_MAP,
    MATERIAL_LIGHT_MAP,
    MATERIAL_REFLECTION_MAP,
    TOTAL_MATERIAL_MAPS
} material_map_t;

typedef struct {
    float rgb[3];
} color_t;

class Material {
public:
    Material();

    const color_t& getColor(const material_color_t& materialColorType) const;
    float getShininess() const;
    const Texture* getTextureMap(const material_map_t& materialMapType) const;

    void setColor(const material_color_t& materialColorType, const color_t& rgb);
    void setColor(const material_color_t& materialColorType, const float r, const float g, const float b);
    void setShininess(const float shininess);
    void setTextureMap(const material_map_t& materialMapType, const Texture* texture);

private:
    std::vector<color_t> m_colors;
    float m_shininess;
    std::vector<const Texture*> m_textureMaps;
};



inline const color_t& Material::getColor(const material_color_t& materialColorType) const {
    return m_colors[materialColorType];
}

inline float Material::getShininess() const {
    return m_shininess;
}

inline const Texture* Material::getTextureMap(const material_map_t& materialMapType) const {
    return m_textureMaps[materialMapType];
}



inline void Material::setColor(const material_color_t& materialColorType, const color_t& rgba) {
    m_colors[materialColorType] = rgba;
}

inline void Material::setColor(const material_color_t& materialColorType, const float r, const float g, const float b) {
    m_colors[materialColorType].rgb[0] = r;
    m_colors[materialColorType].rgb[1] = g;
    m_colors[materialColorType].rgb[2] = b;
}

inline void Material::setTextureMap(const material_map_t& materialMapType, const Texture* texture) {
    m_textureMaps[materialMapType] = texture;
}

inline void Material::setShininess(const float shininess) {
    m_shininess = shininess;
}

#endif // MATERIAL_HPP
