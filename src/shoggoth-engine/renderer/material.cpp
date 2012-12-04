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


#include "shoggoth-engine/renderer/material.hpp"

#include <iostream>
#include <fstream>
#include <boost/foreach.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include "shoggoth-engine/resources/texture.hpp"

using namespace std;
using namespace boost::property_tree;

const string XML_ROOT_NODE = "material";
const string XML_COMMENT = "<xmlcomment>";
const string XML_ATTRIBUTE = "<xmlattr>";

const string TYPE_FLOAT = "float";
const string TYPE_VEC2 = "vec2";
const string TYPE_VEC3 = "vec3";
const string TYPE_VEC4 = "vec4";
const string TYPE_MAT2 = "mat2";
const string TYPE_MAT3 = "mat3";
const string TYPE_MAT4 = "mat4";

const string MATERIAL_CUSTOM_ATTRIBUTE_TYPE = "type";
const string MATERIAL_DIFFUSE_COLOR = "_diffuseColor";
const string MATERIAL_AMBIENT_COLOR = "_ambientColor";
const string MATERIAL_EMISSIVE_COLOR = "_emissiveColor";
const string MATERIAL_SPECULAR_COLOR = "_specularColor";
const string MATERIAL_SHININESS = "_shininess";
const string MATERIAL_OPACITY = "_opacity";
const string MATERIAL_DIFFUSE_MAP = "_diffuseMap";
const string MATERIAL_AMBIENT_MAP = "_ambientMap";
const string MATERIAL_EMISSIVE_MAP = "_emissiveMap";
const string MATERIAL_SPECULAR_MAP = "_specularMap";
const string MATERIAL_SHININESS_MAP = "_shininessMap";
const string MATERIAL_NORMAL_MAP = "_normalMap";
const string MATERIAL_OPACITY_MAP = "_opacityMap";
const string MATERIAL_REFLECTION_MAP = "_reflectionMap";
const string MATERIAL_REFRACTION_MAP = "_refractionMap";
const string MATERIAL_LIGHT_MAP = "_lightMap";

const string MATERIAL_DEFAULT_VERTEX_SHADER = "**defaultvertexshader**";
const string MATERIAL_DEFAULT_FRAGMENT_SHADER = "**defaultfragmentshader**";
const string MATERIAL_CUSTOM_VERTEX_SHADER = "customVertexShader";
const string MATERIAL_CUSTOM_FRAGMENT_SHADER = "customFragmentShader";


typedef struct {
    float v[2];
} vec2_t;
istream& operator>>(istream& in, vec2_t& v) {
    in >> v.v[0] >> v.v[1];
    return in;
}

typedef struct {
    float v[3];
} vec3_t;
istream& operator>>(istream& in, vec3_t& v) {
    in >> v.v[0] >> v.v[1] >> v.v[2];
    return in;
}

typedef struct {
    float v[4];
} vec4_t;
istream& operator>>(istream& in, vec4_t& v) {
    in >> v.v[0] >> v.v[1] >> v.v[2] >> v.v[3];
    return in;
}

typedef struct {
    float m[4];
} mat2_t;
istream& operator>>(istream& in, mat2_t& m) {
    in >> m.m[0] >> m.m[1] >>
          m.m[2] >> m.m[3];
    return in;
}

typedef struct {
    float m[9];
} mat3_t;
istream& operator>>(istream& in, mat3_t& m) {
    in >> m.m[0] >> m.m[1] >> m.m[2] >>
          m.m[3] >> m.m[4] >> m.m[5] >>
          m.m[6] >> m.m[7] >> m.m[8];
    return in;
}

typedef struct {
    float m[16];
} mat4_t;
istream& operator>>(istream& in, mat4_t& m) {
    in >> m.m[ 0] >> m.m[ 1] >> m.m[ 2] >> m.m[ 3] >>
          m.m[ 4] >> m.m[ 5] >> m.m[ 6] >> m.m[ 7] >>
          m.m[ 8] >> m.m[ 9] >> m.m[10] >> m.m[11] >>
          m.m[12] >> m.m[13] >> m.m[14] >> m.m[15];
    return in;
}


Material::Material(const bool useShaders_ARB):
    m_shader(useShaders_ARB),
    m_vertexShaderFile(),
    m_fragmentShaderFile(),
    m_diffuseColor(0.8f, 0.8f, 0.8f, 1.0f),
    m_ambientColor(0.2f, 0.2f, 0.2f, 1.0f),
    m_emissiveColor(COLOR_BLACK),
    m_specularColor(COLOR_BLACK),
    m_shininess(0.0f),
    m_opacity(1.0f),
    m_diffuseMap(0),
    m_ambientMap(0),
    m_emissiveMap(0),
    m_specularMap(0),
    m_shininessMap(0),
    m_normalMap(0),
    m_opacityMap(0),
    m_reflectionMap(0),
    m_refractionMap(0),
    m_lightMap(0)
{}

Material::Material(const Material& rhs):
    m_shader(rhs.m_shader),
    m_vertexShaderFile(rhs.m_vertexShaderFile),
    m_fragmentShaderFile(rhs.m_fragmentShaderFile),
    m_diffuseColor(rhs.m_diffuseColor),
    m_ambientColor(rhs.m_ambientColor),
    m_emissiveColor(rhs.m_emissiveColor),
    m_specularColor(rhs.m_specularColor),
    m_shininess(rhs.m_shininess),
    m_opacity(rhs.m_opacity),
    m_diffuseMap(rhs.m_diffuseMap),
    m_ambientMap(rhs.m_ambientMap),
    m_emissiveMap(rhs.m_emissiveMap),
    m_specularMap(rhs.m_specularMap),
    m_shininessMap(rhs.m_shininessMap),
    m_normalMap(rhs.m_normalMap),
    m_opacityMap(rhs.m_opacityMap),
    m_reflectionMap(rhs.m_reflectionMap),
    m_refractionMap(rhs.m_refractionMap),
    m_lightMap(rhs.m_lightMap)
{}

Material& Material::operator=(const Material& rhs) {
    if (this == &rhs)
        return *this;
    m_shader = rhs.m_shader;
    m_vertexShaderFile = rhs.m_vertexShaderFile;
    m_fragmentShaderFile = rhs.m_fragmentShaderFile;
    m_diffuseColor = rhs.m_diffuseColor;
    m_ambientColor = rhs.m_ambientColor;
    m_emissiveColor = rhs.m_emissiveColor;
    m_specularColor = rhs.m_specularColor;
    m_shininess = rhs.m_shininess;
    m_opacity = rhs.m_opacity;
    m_diffuseMap = rhs.m_diffuseMap;
    m_ambientMap = rhs.m_ambientMap;
    m_emissiveMap = rhs.m_emissiveMap;
    m_specularMap = rhs.m_specularMap;
    m_shininessMap = rhs.m_shininessMap;
    m_normalMap = rhs.m_normalMap;
    m_opacityMap = rhs.m_opacityMap;
    m_reflectionMap = rhs.m_reflectionMap;
    m_refractionMap = rhs.m_refractionMap;
    m_lightMap = rhs.m_lightMap;
    return *this;
}

// void Material::saveToFile(const string& fileName) {
//     ptree tree;
//     xml_writer_settings<char> settings(' ', 2);
//
//     write_xml(fileName, tree, std::locale(), settings);
// }

bool Material::loadFromFile(const std::string& fileName) {
    ifstream fin(fileName.c_str());
    if (!fin.is_open() || !fin.good()) {
        cerr << "Error: could not open material file: " << fileName << endl;
        return false;
    }
    fin.close();

    ptree tree;
    read_xml(fileName, tree, xml_parser::trim_whitespace);

    m_vertexShaderFile = tree.get<string>(MATERIAL_CUSTOM_VERTEX_SHADER, MATERIAL_DEFAULT_VERTEX_SHADER);
    m_fragmentShaderFile = tree.get<string>(MATERIAL_CUSTOM_FRAGMENT_SHADER, MATERIAL_DEFAULT_FRAGMENT_SHADER);

    string subdirectory = fileName.substr(0, fileName.find_first_of("/\\") + 1);
    m_vertexShaderFile = subdirectory + m_vertexShaderFile;
    m_fragmentShaderFile = subdirectory + m_fragmentShaderFile;
    m_shader.loadShaderProgram(m_vertexShaderFile, m_fragmentShaderFile);

    if (tree.find(XML_ROOT_NODE) == tree.not_found()) {
        cerr << "Error loading material: <material> root node not found" << endl;
        return false;
    }
    BOOST_FOREACH(ptree::value_type v, tree.get_child(XML_ROOT_NODE)) {
        if (v.first.compare(XML_COMMENT) == 0)
            continue;
        else if (v.first.compare(MATERIAL_DIFFUSE_COLOR) == 0) {
            m_diffuseColor = tree.get<Color4>(XML_ROOT_NODE + "." + MATERIAL_DIFFUSE_COLOR);
            m_shader.setUniform4(MATERIAL_DIFFUSE_COLOR, m_diffuseColor.getRGBA());
        }
        else if (v.first.compare(MATERIAL_AMBIENT_COLOR) == 0) {
            m_ambientColor = tree.get<Color4>(XML_ROOT_NODE + "." + MATERIAL_AMBIENT_COLOR);
            m_shader.setUniform4(MATERIAL_AMBIENT_COLOR, m_ambientColor.getRGBA());
        }
        else if (v.first.compare(MATERIAL_EMISSIVE_COLOR) == 0) {
            m_emissiveColor = tree.get<Color4>(XML_ROOT_NODE + "." + MATERIAL_EMISSIVE_COLOR);
            m_shader.setUniform4(MATERIAL_EMISSIVE_COLOR, m_emissiveColor.getRGBA());
        }
        else if (v.first.compare(MATERIAL_SPECULAR_COLOR) == 0) {
            m_specularColor = tree.get<Color4>(XML_ROOT_NODE + "." + MATERIAL_SPECULAR_COLOR);
            m_shader.setUniform4(MATERIAL_SPECULAR_COLOR, m_specularColor.getRGBA());
        }
        else if (v.first.compare(MATERIAL_SHININESS) == 0) {
            m_shininess = tree.get<float>(XML_ROOT_NODE + "." + MATERIAL_SHININESS);
            m_shader.setUniform1(MATERIAL_SHININESS, m_shininess);
        }
        else if (v.first.compare(MATERIAL_OPACITY) == 0) {
            m_opacity = tree.get<float>(XML_ROOT_NODE + "." + MATERIAL_OPACITY);
            m_shader.setUniform1(MATERIAL_OPACITY, m_opacity);
        }
        else { // custom attribute
            string name = XML_ROOT_NODE + "." + v.first;
            string type = tree.get<string>(name + "." + XML_ATTRIBUTE + "." + MATERIAL_CUSTOM_ATTRIBUTE_TYPE);
            if (type.compare(TYPE_FLOAT) == 0) {
                float value = tree.get<float>(name);
                m_shader.setUniform1(v.first, value);
            }
            else if (type.compare(TYPE_VEC4) == 0) {
                vec4_t vec4 = tree.get<vec4_t>(name);
                m_shader.setUniform4(v.first, vec4.v);
            }
            else if (type.compare(TYPE_VEC3) == 0) {
                vec3_t vec3 = tree.get<vec3_t>(name);
                m_shader.setUniform3(v.first, vec3.v);
            }
            else if (type.compare(TYPE_VEC2) == 0) {
                vec2_t vec2 = tree.get<vec2_t>(name);
                m_shader.setUniform2(v.first, vec2.v);
            }
            else if (type.compare(TYPE_MAT4) == 0) {
                mat4_t mat4 = tree.get<mat4_t>(name);
                m_shader.setUniformMatrix4x4(v.first, mat4.m);
            }
            else if (type.compare(TYPE_MAT3) == 0) {
                mat3_t mat3 = tree.get<mat3_t>(name);
                m_shader.setUniformMatrix3x3(v.first, mat3.m);
            }
            else if (type.compare(TYPE_MAT2) == 0) {
                mat2_t mat2 = tree.get<mat2_t>(name);
                m_shader.setUniformMatrix2x2(v.first, mat2.m);
            }
            else
                cerr << "Error: undefined data type for attribute: " << type << " " << name << endl;
        }
    }
    return true;
}

void Material::useMaterial() const {
    m_shader.useShader();
}
