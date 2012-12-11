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


#ifndef OPENGL_H
#define OPENGL_H

#include <iostream>
#include <string>
#include <GL/glew.h>

typedef enum {
    DATA_UPLOAD_VERTEX_ARRAY, // 1.1
    DATA_UPLOAD_VERTEX_BUFFER_OBJECT_EXT, // extension
    DATA_UPLOAD_VERTEX_BUFFER_OBJECT // 1.5
} data_upload_t;

typedef enum {
    MIPMAP_GENERATION_GLU, // 1.0
    MIPMAP_GENERATION_TEX_PARAMETER_EXT, // extension
    MIPMAP_GENERATION_TEX_PARAMETER // 1.4
} mipmap_generation_t;

typedef enum {
    TEXTURE_FILTERING_NEAREST, // 1.0
    TEXTURE_FILTERING_LINEAR, // 1.0
    TEXTURE_FILTERING_ANISOTROPIC // extension
} texture_filtering_t;

typedef enum {
    TEXTURE_COMPRESSION_NONE, // 1.0
    TEXTURE_COMPRESSION_EXT, // 1.1
    TEXTURE_COMPRESSION // 1.3
} texture_compression_t;

typedef enum {
    RENDERING_METHOD_FIXED_PIPELINE, // 1.0
    RENDERING_METHOD_SHADERS_EXT,
    RENDERING_METHOD_SHADERS // 2.0
} rendering_method_t;

const unsigned int VERTEX_ARRAY_INDEX = 0;
const unsigned int NORMALS_ARRAY_INDEX = 2;
const unsigned int UVCOORDS_ARRAY_INDEX = 8;


class OpenGL {
public:
    static float ms_projectionMatrix[16];
    static float ms_viewMatrix[16];
    static float ms_modelMatrix[16];
    static float ms_modelViewMatrix[16];
    static float ms_modelViewProjectionMatrix[16];

    static float version();
    static float shaderLanguageVersion();
    static data_upload_t& dataUploadMode();
    static mipmap_generation_t& mipMapGenerationMode();
    static bool isAnisotropicFilteringSupported();
    static float maxAnisotropy();
    static float anisotropy();
    static texture_filtering_t& textureFilteringMode();
    static texture_compression_t& textureCompressionMode();
    static rendering_method_t& renderingMethod();
    static bool areVBOsSupported();
    static bool areShadersSupported();
    static void setTextureFilteringMode(const texture_filtering_t& textureFiltering);
    static void setAnisotropy(const float anisotropy);

    static void detectCapabilities();
    static void multMatrix(float* result, const float* a, const float* b);
    static void projectionMatrixOrthographic(float width, float height, float near, float far);
    static void projectionMatrixPerspective(float perspectiveFOV, float aspectRatio, float near, float far);

private:
    static float ms_openGLVersion;
    static float ms_shaderLanguageVersion;
    static data_upload_t ms_dataUploadMode;
    static mipmap_generation_t ms_mipMapGenerationMode;
    static bool ms_isAnisotropicFilteringSupported;
    static float ms_maxAnisotropy;
    static float ms_anisotropy;
    static texture_filtering_t ms_textureFilteringMode;
    static texture_compression_t ms_textureCompressionMode;
    static rendering_method_t ms_renderingMethod;
    static bool ms_areVBOsSupported;
    static bool ms_areShadersSupported;
};



namespace gl {
    // render
    unsigned int genBuffer();
    void deleteBuffer(const unsigned int bufferId);
    void bindVboBuffer(const unsigned int vboId);
    void vboBufferBytes(const size_t totalBytes);
    void vboBufferSubData(const size_t startByte, const size_t totalBytes, const float* data);
    size_t getVboBufferBytes();
    void bindIndexBuffer(const unsigned int indicesId);
    void indexBufferData(const size_t totalBytes, const unsigned int* data);
    size_t getIndexBufferBytes();
    void vertexAttribPointer(const unsigned int dataIndex, const size_t bytesOffset);
    void drawElements(const size_t totalIndices);

    // shaders
    unsigned int createVertexShader();
    unsigned int createFragmentShader();
    void deleteShader(const unsigned int shaderId);
    bool shaderSource(const unsigned int shaderId, const std::string& source);
    bool compileShader(const unsigned int shaderId);
    std::string getShaderInfoLog(const unsigned int shaderId);
    unsigned int createProgram();
    void deleteProgram(const unsigned int programId);
    void attachShader(const unsigned int programId, const unsigned int shaderId);
    void detachShader(const unsigned int programId, const unsigned int shaderId);
    bool linkProgram(const unsigned int programId);
    std::string getProgramInfoLog(const unsigned int programId);
    void useProgram(const unsigned int programId);
    int getUniformLocation(const unsigned int programId, const std::string& name);
    void useUniform1(const int location, const float* value);
    void useUniform2(const int location, const float* value);
    void useUniform3(const int location, const float* value);
    void useUniform4(const int location, const float* value);
    void useUniform2x2(const int location, const float* value);
    void useUniform3x3(const int location, const float* value);
    void useUniform4x4(const int location, const float* value);
    void bindAttribLocation(const unsigned int programId, const unsigned int attribArrayIndex, const std::string& name);
}



inline float OpenGL::version() {
    return ms_openGLVersion;
}

inline float OpenGL::shaderLanguageVersion() {
    return ms_shaderLanguageVersion;
}

inline data_upload_t& OpenGL::dataUploadMode() {
    return ms_dataUploadMode;
}

inline mipmap_generation_t& OpenGL::mipMapGenerationMode() {
    return ms_mipMapGenerationMode;
}

inline bool OpenGL::isAnisotropicFilteringSupported() {
    return ms_isAnisotropicFilteringSupported;
}

inline float OpenGL::maxAnisotropy() {
    return ms_maxAnisotropy;
}

inline float OpenGL::anisotropy() {
    return ms_anisotropy;
}

inline texture_filtering_t& OpenGL::textureFilteringMode() {
    return ms_textureFilteringMode;
}

inline texture_compression_t& OpenGL::textureCompressionMode() {
    return ms_textureCompressionMode;
}

inline rendering_method_t& OpenGL::renderingMethod() {
    return ms_renderingMethod;
}

inline bool OpenGL::areVBOsSupported() {
    return ms_areVBOsSupported;
}

inline bool OpenGL::areShadersSupported() {
    return ms_areShadersSupported;
}

inline void OpenGL::setAnisotropy(const float anisotropy) {
    ms_anisotropy = anisotropy;
    if (ms_anisotropy > ms_maxAnisotropy)
        ms_anisotropy = ms_maxAnisotropy;
    else if (ms_anisotropy < 1.0f)
        ms_anisotropy = 1.0f;
}

inline void OpenGL::setTextureFilteringMode(const texture_filtering_t& textureFiltering) {
    if (textureFiltering == TEXTURE_FILTERING_ANISOTROPIC && !ms_isAnisotropicFilteringSupported)
        std::cerr << "Warning: anisotropic texture filtering not supported, ignoring change" << std::endl;
    else
        ms_textureFilteringMode = textureFiltering;
}



inline unsigned int gl::genBuffer() {
    unsigned int bufferId;
    if (OpenGL::dataUploadMode() == DATA_UPLOAD_VERTEX_BUFFER_OBJECT_EXT)
        glGenBuffersARB(1, &bufferId);
    else
        glGenBuffers(1, &bufferId);
    return bufferId;
}

inline void gl::deleteBuffer(const unsigned int bufferId) {
    if (OpenGL::dataUploadMode() == DATA_UPLOAD_VERTEX_BUFFER_OBJECT_EXT)
        glDeleteBuffersARB(1, &bufferId);
    else
        glDeleteBuffers(1, &bufferId);
}

inline void gl::bindVboBuffer(const unsigned int vboId) {
    if (OpenGL::dataUploadMode() == DATA_UPLOAD_VERTEX_BUFFER_OBJECT_EXT)
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, vboId);
    else
        glBindBuffer(GL_ARRAY_BUFFER, vboId);
}

inline void gl::vboBufferBytes(const size_t totalBytes) {
    if (OpenGL::dataUploadMode() == DATA_UPLOAD_VERTEX_BUFFER_OBJECT_EXT)
        glBufferDataARB(GL_ARRAY_BUFFER_ARB, totalBytes, 0, GL_STATIC_DRAW_ARB);
    else
        glBufferData(GL_ARRAY_BUFFER, totalBytes, 0, GL_STATIC_DRAW);
}

inline void gl::vboBufferSubData(const size_t startByte, const size_t totalBytes, const float* data) {
    if (OpenGL::dataUploadMode() == DATA_UPLOAD_VERTEX_BUFFER_OBJECT_EXT)
        glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, startByte, totalBytes, data);
    else
        glBufferSubData(GL_ARRAY_BUFFER, startByte, totalBytes, data);
}

inline size_t gl::getVboBufferBytes() {
    GLint bufferSize;
    if (OpenGL::dataUploadMode() == DATA_UPLOAD_VERTEX_BUFFER_OBJECT_EXT)
        glGetBufferParameterivARB(GL_ARRAY_BUFFER_ARB, GL_BUFFER_SIZE_ARB, &bufferSize);
    else
        glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &bufferSize);
    return size_t(bufferSize);
}

inline void gl::bindIndexBuffer(const unsigned int indicesId) {
    if (OpenGL::dataUploadMode() == DATA_UPLOAD_VERTEX_BUFFER_OBJECT_EXT)
        glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, indicesId);
    else
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesId);
}

inline void gl::indexBufferData(const size_t totalBytes, const unsigned int* data) {
    if (OpenGL::dataUploadMode() == DATA_UPLOAD_VERTEX_BUFFER_OBJECT_EXT)
        glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, totalBytes, data, GL_STATIC_DRAW_ARB);
    else
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, totalBytes, data, GL_STATIC_DRAW);
}

inline size_t gl::getIndexBufferBytes() {
    GLint bufferSize;
    if (OpenGL::dataUploadMode() == DATA_UPLOAD_VERTEX_BUFFER_OBJECT_EXT)
        glGetBufferParameterivARB(GL_ELEMENT_ARRAY_BUFFER_ARB, GL_BUFFER_SIZE_ARB, &bufferSize);
    else
        glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &bufferSize);
    return size_t(bufferSize);
}

inline void gl::vertexAttribPointer(const unsigned int dataIndex, const size_t bytesOffset) {
    if (OpenGL::dataUploadMode() == DATA_UPLOAD_VERTEX_BUFFER_OBJECT_EXT)
        glVertexAttribPointerARB(dataIndex, 3, GL_FLOAT, GL_FALSE, 0, (void*)(bytesOffset));
    else
        glVertexAttribPointer(dataIndex, 3, GL_FLOAT, GL_FALSE, 0, (void*)(bytesOffset));
}

inline void gl::drawElements(const size_t totalIndices) {
    glDrawElements(GL_TRIANGLES, GLsizei(totalIndices), GL_UNSIGNED_INT, 0);
}



inline unsigned int gl::createVertexShader() {
    if (OpenGL::renderingMethod() == RENDERING_METHOD_SHADERS_EXT)
        return glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
    return glCreateShader(GL_VERTEX_SHADER);
}

inline unsigned int gl::createFragmentShader() {
    if (OpenGL::renderingMethod() == RENDERING_METHOD_SHADERS_EXT)
        return glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);
    return glCreateShader(GL_FRAGMENT_SHADER);
}

inline void gl::deleteShader(const unsigned int shaderId) {
    if (OpenGL::renderingMethod() == RENDERING_METHOD_SHADERS_EXT)
        glDeleteObjectARB(shaderId);
    else
        glDeleteShader(shaderId);
}

inline bool gl::shaderSource(const unsigned int shaderId, const std::string& source) {
    GLint length;
    const char* src = source.c_str();
    if (OpenGL::renderingMethod() == RENDERING_METHOD_SHADERS_EXT) {
        glShaderSourceARB(shaderId, 1, &src, 0);
        glGetObjectParameterivARB(shaderId, GL_OBJECT_SHADER_SOURCE_LENGTH_ARB, &length);
    }
    else {
        glShaderSource(shaderId, 1, &src, 0);
        glGetShaderiv(shaderId, GL_SHADER_SOURCE_LENGTH, &length);
    }
    if (length <= 0)
        return false;
    return true;
}

inline bool gl::compileShader(const unsigned int shaderId) {
    GLint success;
    if (OpenGL::renderingMethod() == RENDERING_METHOD_SHADERS_EXT) {
        glCompileShaderARB(shaderId);
        glGetObjectParameterivARB(shaderId, GL_OBJECT_COMPILE_STATUS_ARB, &success);
    }
    else {
        glCompileShader(shaderId);
        glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
    }
    if (success == GL_FALSE)
        return false;
    return true;
}

inline std::string gl::getShaderInfoLog(const unsigned int shaderId) {
    char shaderLog[1024];
    if (OpenGL::renderingMethod() == RENDERING_METHOD_SHADERS_EXT)
        glGetInfoLogARB(shaderId, 1024, 0, shaderLog);
    else
        glGetShaderInfoLog(shaderId, 1024, 0, shaderLog);
    return std::string(shaderLog);
}

inline unsigned int gl::createProgram() {
    if (OpenGL::renderingMethod() == RENDERING_METHOD_SHADERS_EXT)
        return glCreateProgramObjectARB();
    return glCreateProgram();
}

inline void gl::deleteProgram(const unsigned int programId) {
    if (OpenGL::renderingMethod() == RENDERING_METHOD_SHADERS_EXT)
        glDeleteObjectARB(programId);
    else
        glDeleteProgram(programId);
}

inline void gl::attachShader(const unsigned int programId, const unsigned int shaderId) {
    if (OpenGL::renderingMethod() == RENDERING_METHOD_SHADERS_EXT)
        glAttachObjectARB(programId, shaderId);
    else
        glAttachShader(programId, shaderId);
}

inline void gl::detachShader(const unsigned int programId, const unsigned int shaderId) {
    if (OpenGL::renderingMethod() == RENDERING_METHOD_SHADERS_EXT)
        glDetachObjectARB(programId, shaderId);
    else
        glDetachShader(programId, shaderId);
}

inline bool gl::linkProgram(const unsigned int programId) {
    GLint success;
    if (OpenGL::renderingMethod() == RENDERING_METHOD_SHADERS_EXT) {
        glLinkProgramARB(programId);
        glGetProgramivARB(programId, GL_OBJECT_LINK_STATUS_ARB, &success);
    }
    else {
        glLinkProgram(programId);
        glGetProgramiv(programId, GL_LINK_STATUS, &success);
    }
    if (success == GL_FALSE)
        return false;
    return true;
}

inline std::string gl::getProgramInfoLog(const unsigned int programId) {
    char programLog[1024];
    if (OpenGL::renderingMethod() == RENDERING_METHOD_SHADERS_EXT)
        glGetInfoLogARB(programId, 1024, 0, programLog);
    else
        glGetProgramInfoLog(programId, 1024, 0, programLog);
    return std::string(programLog);
}

inline void gl::useProgram(const unsigned int programId) {
    if (OpenGL::renderingMethod() == RENDERING_METHOD_SHADERS_EXT)
        glUseProgramObjectARB(programId);
    else
        glUseProgram(programId);
}

inline int gl::getUniformLocation(const unsigned int programId, const std::string& name) {
    if (OpenGL::renderingMethod() == RENDERING_METHOD_SHADERS_EXT)
        return glGetUniformLocationARB(programId, name.c_str());
    return glGetUniformLocation(programId, name.c_str());
}

inline void gl::useUniform1(const int location, const float* value) {
    if (OpenGL::renderingMethod() == RENDERING_METHOD_SHADERS_EXT)
        glUniform1fvARB(location, 1, value);
    else
        glUniform1fv(location, 1, value);
}

inline void gl::useUniform2(const int location, const float* value) {
    if (OpenGL::renderingMethod() == RENDERING_METHOD_SHADERS_EXT)
        glUniform2fvARB(location, 1, value);
    else
        glUniform2fv(location, 1, value);
}

inline void gl::useUniform3(const int location, const float* value) {
    if (OpenGL::renderingMethod() == RENDERING_METHOD_SHADERS_EXT)
        glUniform3fvARB(location, 1, value);
    else
        glUniform3fv(location, 1, value);
}

inline void gl::useUniform4(const int location, const float* value) {
    if (OpenGL::renderingMethod() == RENDERING_METHOD_SHADERS_EXT)
        glUniform4fvARB(location, 1, value);
    else
        glUniform4fv(location, 1, value);
}

inline void gl::useUniform2x2(const int location, const float* value) {
    if (OpenGL::renderingMethod() == RENDERING_METHOD_SHADERS_EXT)
        glUniformMatrix2fvARB(location, 1, GL_FALSE, value);
    else
        glUniformMatrix2fv(location, 1, GL_FALSE, value);
}

inline void gl::useUniform3x3(const int location, const float* value) {
    if (OpenGL::renderingMethod() == RENDERING_METHOD_SHADERS_EXT)
        glUniformMatrix3fvARB(location, 1, GL_FALSE, value);
    else
        glUniformMatrix3fv(location, 1, GL_FALSE, value);
}

inline void gl::useUniform4x4(const int location, const float* value) {
    if (OpenGL::renderingMethod() == RENDERING_METHOD_SHADERS_EXT)
        glUniformMatrix4fvARB(location, 1, GL_FALSE, value);
    else
        glUniformMatrix4fv(location, 1, GL_FALSE, value);
}

inline void gl::bindAttribLocation(const unsigned int programId, const unsigned int attribArrayIndex, const std::string& name) {
    if (OpenGL::renderingMethod() == RENDERING_METHOD_SHADERS_EXT)
        glBindAttribLocationARB(programId, attribArrayIndex, name.c_str());
    else
        glBindAttribLocation(programId, attribArrayIndex, name.c_str());
}

#endif // OPENGL_H
