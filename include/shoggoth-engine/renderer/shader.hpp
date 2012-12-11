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


#ifndef SHADER_HPP
#define SHADER_HPP

#include <string>
#include <map>

class Shader {
public:
    Shader();
    ~Shader();

    bool loadShaderProgram(const std::string& vertexFile, const std::string& fragmentFile);
    void useShader() const;

    void setUniform1(const std::string& name, const float value);
    void setUniform2(const std::string& name, const float* value);
    void setUniform3(const std::string& name, const float* value);
    void setUniform4(const std::string& name, const float* value);
    void setUniformMatrix2x2(const std::string& name, const float* value);
    void setUniformMatrix3x3(const std::string& name, const float* value);
    void setUniformMatrix4x4(const std::string& name, const float* value);

private:
    bool m_isShaderCreated;
    unsigned int m_vertexShaderId;
    unsigned int m_fragmentShaderId;
    unsigned int m_shaderProgramId;

    int m_viewMatrixLocation;
    int m_modelMatrixLocation;
    int m_modelViewMatrixLocation;
    int m_projectionMatrixLocation;
    int m_modelViewProjectionMatrixLocation;
    int m_normalMatrix;

    std::map<int, float*> m_uniform1;
    std::map<int, float*> m_uniform2;
    std::map<int, float*> m_uniform3;
    std::map<int, float*> m_uniform4;
    std::map<int, float*> m_uniform2x2;
    std::map<int, float*> m_uniform3x3;
    std::map<int, float*> m_uniform4x4;

    bool loadShaderFile(const unsigned int shaderId, const std::string& fileName);
};

#endif // SHADER_HPP
