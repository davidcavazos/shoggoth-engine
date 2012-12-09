/*
    Copyright (c) 2012 David Cavazos <davido262@gmail.com>

    Permission is hereby granted, free of charge, to any person
    obtaining a copy of this software and associated documentation
    files (the "Software"), to deal in the Software without
    restriction, including without limitation the rights to use,
    copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following
    conditions:

    The above copyright notice and this permission notice shall be
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
    OTHER DEALINGS IN THE SOFTWARE.
*/


#include "shoggoth-engine/renderer/shader.hpp"

#include <iostream>
#include <fstream>
#include <GL/glew.h>
#include "shoggoth-engine/renderer/opengl.hpp"

using namespace std;

const string GLSL_MODEL_VIEW_MATRIX = "_modelViewMatrix";
const string GLSL_MODEL_VIEW_PROJECTION_MATRIX = "_modelViewProjectionMatrix";
const string GLSL_PROJECTION_MATRIX = "_projectionMatrix";
const string GLSL_VERTEX = "_vertex";
const string GLSL_NORMAL = "_normal";



Shader::Shader():
    m_isShaderCreated(false),
    m_vertexShaderId(0),
    m_fragmentShaderId(0),
    m_shaderProgramId(0),
    m_uniform1(),
    m_uniform2(),
    m_uniform3(),
    m_uniform4(),
    m_uniform2x2(),
    m_uniform3x3(),
    m_uniform4x4()
{}

Shader::~Shader() {
    if (m_isShaderCreated) {
        gl::detachShader(m_shaderProgramId, m_fragmentShaderId);
        gl::detachShader(m_shaderProgramId, m_vertexShaderId);
        gl::deleteShader(m_fragmentShaderId);
        gl::deleteShader(m_vertexShaderId);
        gl::deleteProgram(m_shaderProgramId);
    }

    map<int, float*>::const_iterator it;
    for (it = m_uniform1.begin(); it != m_uniform1.end(); ++it)
        delete it->second;
    for (it = m_uniform2.begin(); it != m_uniform2.end(); ++it)
        delete it->second;
    for (it = m_uniform3.begin(); it != m_uniform3.end(); ++it)
        delete it->second;
    for (it = m_uniform4.begin(); it != m_uniform4.end(); ++it)
        delete it->second;
    for (it = m_uniform2x2.begin(); it != m_uniform2x2.end(); ++it)
        delete it->second;
    for (it = m_uniform3x3.begin(); it != m_uniform3x3.end(); ++it)
        delete it->second;
    for (it = m_uniform4x4.begin(); it != m_uniform4x4.end(); ++it)
        delete it->second;
}

bool Shader::loadShaderProgram(const string& vertexFile, const string& fragmentFile) {
    // create shader objects
    m_vertexShaderId = gl::createVertexShader();
    m_fragmentShaderId = gl::createFragmentShader();

    // load shader files
    if (!loadShaderFile(m_vertexShaderId, vertexFile) || !loadShaderFile(m_fragmentShaderId, fragmentFile)) {
        gl::deleteShader(m_fragmentShaderId);
        gl::deleteShader(m_vertexShaderId);
        cerr << "Error: shader program not created" << endl;
        return false;
    }

    // compile shaders
    if (!gl::compileShader(m_vertexShaderId)) {
        cerr << "Vertex shader log: " << gl::getShaderInfoLog(m_vertexShaderId) << endl;
        gl::deleteShader(m_fragmentShaderId);
        gl::deleteShader(m_vertexShaderId);
        cerr << "Error: shader program not created" << endl;
        return false;
    }
    if (!gl::compileShader(m_fragmentShaderId)) {
        cerr << "Fragment shader log: " << gl::getShaderInfoLog(m_fragmentShaderId) << endl;
        gl::deleteShader(m_fragmentShaderId);
        gl::deleteShader(m_vertexShaderId);
        cerr << "Error: shader program not created" << endl;
        return false;
    }

    // create shader program
    m_shaderProgramId = gl::createProgram();
    gl::attachShader(m_shaderProgramId, m_vertexShaderId);
    gl::attachShader(m_shaderProgramId, m_fragmentShaderId);
    bool linkedSuccesfully = gl::linkProgram(m_shaderProgramId);
    gl::deleteShader(m_fragmentShaderId);
    gl::deleteShader(m_vertexShaderId);
    if (!linkedSuccesfully) {
        cerr << "Shader program log: " << gl::getProgramInfoLog(m_shaderProgramId) << endl;
        gl::deleteProgram(m_shaderProgramId);
        cerr << "Error: shader program not created" << endl;
        return false;
    }
    m_isShaderCreated = true;
    return true;
}

void Shader::useShader() const {
    map<int, float*>::const_iterator it;
    gl::useProgram(m_shaderProgramId);
    for (it = m_uniform1.begin(); it != m_uniform1.end(); ++it)
        gl::useUniform1(it->first, it->second);
    for (it = m_uniform2.begin(); it != m_uniform2.end(); ++it)
        gl::useUniform2(it->first, it->second);
    for (it = m_uniform3.begin(); it != m_uniform3.end(); ++it)
        gl::useUniform3(it->first, it->second);
    for (it = m_uniform4.begin(); it != m_uniform4.end(); ++it)
        gl::useUniform4(it->first, it->second);
    for (it = m_uniform2x2.begin(); it != m_uniform2x2.end(); ++it)
        gl::useUniform2x2(it->first, it->second);
    for (it = m_uniform3x3.begin(); it != m_uniform3x3.end(); ++it)
        gl::useUniform3x3(it->first, it->second);
    for (it = m_uniform4x4.begin(); it != m_uniform4x4.end(); ++it)
        gl::useUniform4x4(it->first, it->second);
}



void Shader::setUniform1(const string& name, const float value) {
    int location;
    float* val = new float[1];
    val[0] = value;
    location = gl::getUniformLocation(m_shaderProgramId, name.c_str());
    m_uniform1.insert(pair<int, float*>(location, val));
}

void Shader::setUniform2(const string& name, const float* value) {
    int location;
    float* val = new float[2];
    val[0] = value[0];
    val[0] = value[1];
    location = gl::getUniformLocation(m_shaderProgramId, name.c_str());
    m_uniform2.insert(pair<int, float*>(location, val));
}

void Shader::setUniform3(const string& name, const float* value) {
    int location;
    float* val = new float[3];
    for (size_t i = 0; i < 3; ++i)
        val[i] = value[i];
    location = gl::getUniformLocation(m_shaderProgramId, name.c_str());
    m_uniform3.insert(pair<int, float*>(location, val));
}

void Shader::setUniform4(const string& name, const float* value) {
    int location;
    float* val = new float[4];
    for (size_t i = 0; i < 4; ++i)
        val[i] = value[i];
    location = gl::getUniformLocation(m_shaderProgramId, name.c_str());
    m_uniform4.insert(pair<int, float*>(location, val));
}

void Shader::setUniformMatrix2x2(const string& name, const float* value) {
    int location;
    float* val = new float[4];
    for (size_t i = 0; i < 4; ++i)
        val[i] = value[i];
    location = gl::getUniformLocation(m_shaderProgramId, name.c_str());
    m_uniform2x2.insert(pair<int, float*>(location, val));
}

void Shader::setUniformMatrix3x3(const string& name, const float* value) {
    int location;
    float* val = new float[9];
    for (size_t i = 0; i < 9; ++i)
        val[i] = value[i];
    location = gl::getUniformLocation(m_shaderProgramId, name.c_str());
    m_uniform3x3.insert(pair<int, float*>(location, val));
}

void Shader::setUniformMatrix4x4(const string& name, const float* value) {
    int location;
    float* val = new float[16];
    for (size_t i = 0; i < 16; ++i)
        val[i] = value[i];
    location = gl::getUniformLocation(m_shaderProgramId, name.c_str());
    m_uniform4x4.insert(pair<int, float*>(location, val));
}



bool Shader::loadShaderFile(const unsigned int shaderId, const string& fileName) {
    string source;
    ifstream file(fileName.c_str());

    // read file to memory
    if (!file.is_open() || !file.good()) {
        cerr << "Error: opening file: " << fileName << endl;
        return false;
    }
    file.seekg(0, std::ios::end);
    size_t size = size_t(file.tellg());
    source.resize(size);
    file.seekg(0);
    file.read(&source[0], size);
    file.close();

    // set source for shader
    if (!gl::shaderSource(shaderId, source)) {
        cerr << "Error: loading the shader file: " << fileName << endl;
        return false;
    }
    return true;
}
