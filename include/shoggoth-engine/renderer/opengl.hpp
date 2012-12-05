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
const unsigned int NORMALS_ARRAY_INDEX = 1;


class OpenGL {
public:
    static float* projectionMatrix();
    static float* modelViewMatrix();
    static float* modelViewProjectionMatrix();
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
    static void projectionMatrixOrthographic(float width, float height, float near, float far);
    static void projectionMatrixPerspective(float perspectiveFOV, float aspectRatio, float near, float far);

private:
    static float ms_projectionMatrix[16];
    static float ms_modelViewMatrix[16];
    static float ms_modelViewProjectionMatrix[16];
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
    void bindDataBuffer(const unsigned int vboId);
    void bindIndexBuffer(const unsigned int indicesId);
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
}



inline float* OpenGL::projectionMatrix() {
    return ms_projectionMatrix;
}

inline float* OpenGL::modelViewMatrix() {
    return ms_modelViewMatrix;
}

inline float* OpenGL::modelViewProjectionMatrix() {
    return ms_modelViewProjectionMatrix;
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

texture_filtering_t& OpenGL::textureFilteringMode() {
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
    if (textureFiltering == TEXTURE_FILTERING_ANISOTROPIC && ms_isAnisotropicFilteringSupported)
        std::cerr << "Warning: anisotropic texture filtering not supported, ignoring change" << std::endl;
    else
        ms_textureFilteringMode = textureFiltering;
}

#endif // OPENGL_H
