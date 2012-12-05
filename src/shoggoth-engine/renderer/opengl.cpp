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


#include "shoggoth-engine/renderer/opengl.hpp"

#include <sstream>
#include <GL/glew.h>
#include "shoggoth-engine/linearmath/scalar.hpp"

using namespace std;

float OpenGL::ms_openGLVersion = 1.1f;
float OpenGL::ms_shaderLanguageVersion = 1.0f;
data_upload_t OpenGL::ms_dataUploadMode = DATA_UPLOAD_VERTEX_ARRAY;
mipmap_generation_t OpenGL::ms_mipMapGenerationMode = MIPMAP_GENERATION_GLU;
bool OpenGL::ms_isAnisotropicFilteringSupported = false;
float OpenGL::ms_maxAnisotropy = 1.0f;
float OpenGL::ms_anisotropy = 1.0f;
texture_filtering_t OpenGL::ms_textureFilteringMode = TEXTURE_FILTERING_NEAREST;
texture_compression_t OpenGL::ms_textureCompressionMode = TEXTURE_COMPRESSION_NONE;
rendering_method_t OpenGL::ms_renderingMethod = RENDERING_METHOD_FIXED_PIPELINE;
bool OpenGL::ms_areVBOsSupported = false;
bool OpenGL::ms_areShadersSupported = false;

const size_t ERRORS_LOG_BUFFER_SIZE = 1024;


void OpenGL::detectCapabilities() {
    GLint integer;

    stringstream ss;
    ss << glGetString(GL_SHADING_LANGUAGE_VERSION) << " " << glGetString(GL_VERSION);
    ss >> ms_shaderLanguageVersion >> ms_openGLVersion;

    // general information
    cout << "Detecting OpenGL driver capabilities" << endl;
    cout << "Vendor: " << glGetString(GL_VENDOR) << endl;
    cout << "Version: " << glGetString(GL_VERSION) << endl;
    cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;
    cout << "Renderer: " << glGetString(GL_RENDERER) << endl;

    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &integer);
    cout << "Max texture size: " << integer << "x" << integer << endl;
    glGetIntegerv(GL_MAX_LIGHTS, &integer);
    cout << "Max lights: " << integer << endl;
    glGetIntegerv(GL_MAX_CLIP_PLANES, &integer);
    cout << "Max clip planes: " << integer << endl;
    glGetIntegerv(GL_MAX_MODELVIEW_STACK_DEPTH, &integer);
    cout << "Max ModelView stack depth: " << integer << endl;
    glGetIntegerv(GL_MAX_PROJECTION_STACK_DEPTH, &integer);
    cout << "Max Projection stack depth: " << integer << endl;
    glGetIntegerv(GL_MAX_ATTRIB_STACK_DEPTH, &integer);
    cout << "Max Attribute stack depth: " << integer << endl;
    glGetIntegerv(GL_MAX_TEXTURE_STACK_DEPTH, &integer);
    cout << "Max Texture stack depth: " << integer << endl;
    glGetIntegerv(GL_MAX_ELEMENTS_VERTICES, &integer);
    cout << "Max vertices: " << integer << endl;
    glGetIntegerv(GL_MAX_ELEMENTS_INDICES, &integer);
    cout << "Max indices: " << integer << endl;

    // capabilities
    if (ms_openGLVersion >= 3.0f)
        cout << "Using OpenGL Core " << ms_openGLVersion << endl;
    else
        cout << "Using OpenGL Legacy " << ms_openGLVersion << endl;

    // version techniques
    int openGLVersionInt = int(floor(ms_openGLVersion * 10.0f + 0.5f));
    switch (openGLVersionInt) {
    case 43: // 4.3
    case 42: // 4.2
    case 41: // 4.1
    case 40: // 4.0
    case 33: // 3.3
    case 32: // 3.2
    case 31: // 3.1
    case 30: // 3.0
    case 21: // 2.1
    case 20: // 2.0
        ms_textureCompressionMode = TEXTURE_COMPRESSION;
        ms_dataUploadMode = DATA_UPLOAD_VERTEX_BUFFER_OBJECT;
        ms_mipMapGenerationMode = MIPMAP_GENERATION_TEX_PARAMETER;
        ms_renderingMethod = RENDERING_METHOD_SHADERS;
        break;
    case 15: // 1.5
        ms_textureCompressionMode = TEXTURE_COMPRESSION;
        ms_dataUploadMode = DATA_UPLOAD_VERTEX_BUFFER_OBJECT;
        ms_mipMapGenerationMode = MIPMAP_GENERATION_TEX_PARAMETER;
        ms_renderingMethod = RENDERING_METHOD_FIXED_PIPELINE;
        break;
    case 14: // 1.4
        ms_textureCompressionMode = TEXTURE_COMPRESSION;
        ms_dataUploadMode = DATA_UPLOAD_VERTEX_ARRAY;
        ms_mipMapGenerationMode = MIPMAP_GENERATION_TEX_PARAMETER;
        ms_renderingMethod = RENDERING_METHOD_FIXED_PIPELINE;
        break;
    case 13: // 1.3
        ms_textureCompressionMode = TEXTURE_COMPRESSION;
        ms_dataUploadMode = DATA_UPLOAD_VERTEX_ARRAY;
        ms_mipMapGenerationMode = MIPMAP_GENERATION_GLU;
        ms_renderingMethod = RENDERING_METHOD_FIXED_PIPELINE;
        break;
    case 12: // 1.2
    case 11: // 1.1
        ms_textureCompressionMode = TEXTURE_COMPRESSION_NONE;
        ms_dataUploadMode = DATA_UPLOAD_VERTEX_ARRAY;
        ms_mipMapGenerationMode = MIPMAP_GENERATION_GLU;
        ms_renderingMethod = RENDERING_METHOD_FIXED_PIPELINE;
        break;
    default:
        cerr << "Error: unsupported OpenGL version: " << ms_openGLVersion << endl;
        ms_textureCompressionMode = TEXTURE_COMPRESSION_NONE;
        ms_dataUploadMode = DATA_UPLOAD_VERTEX_ARRAY;
        ms_mipMapGenerationMode = MIPMAP_GENERATION_GLU;
        ms_renderingMethod = RENDERING_METHOD_FIXED_PIPELINE;
    }

    // extension techniques
    glGetIntegerv(GL_NUM_EXTENSIONS, &integer);
    cout << "Extensions: " << integer << endl;
    if (glewInit() != GLEW_OK)
        cerr << "Error: failed to initialize GLEW" << endl;

    if (ms_dataUploadMode == DATA_UPLOAD_VERTEX_ARRAY &&
        glewIsSupported("GL_ARB_vertex_buffer_object"))
    {
        ms_dataUploadMode = DATA_UPLOAD_VERTEX_BUFFER_OBJECT_EXT;
    }

    if (ms_mipMapGenerationMode == MIPMAP_GENERATION_GLU &&
        glewIsSupported("GL_SGIS_generate_mipmap"))
    {
        ms_mipMapGenerationMode = MIPMAP_GENERATION_TEX_PARAMETER_EXT;
    }

    if (glewIsSupported("GL_EXT_texture_filter_anisotropic")) {
        ms_isAnisotropicFilteringSupported = true;
        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &ms_maxAnisotropy);
        cout << "Anisotropic texture filtering supported, max: " << ms_maxAnisotropy << endl;
    }
    else
        cout << "Anisotropic texture filtering not supported" << endl;

    if (ms_textureCompressionMode == TEXTURE_COMPRESSION_NONE &&
        ms_mipMapGenerationMode != MIPMAP_GENERATION_GLU &&
        glewIsSupported("GL_ARB_texture_compression"))
    {
        ms_textureCompressionMode = TEXTURE_COMPRESSION_EXT;
    }

    if (ms_renderingMethod == RENDERING_METHOD_FIXED_PIPELINE &&
        glewIsSupported("GL_ARB_shader_objects") &&
        glewIsSupported("GL_ARB_vertex_shader") &&
        glewIsSupported("GL_ARB_fragment_shader"))
    {
        ms_renderingMethod = RENDERING_METHOD_SHADERS_EXT;
    }

    switch (ms_dataUploadMode) {
    case DATA_UPLOAD_VERTEX_ARRAY:
        cout << "Using Vertex Array Objects" << endl;
        ms_areVBOsSupported = false;
        break;
    case DATA_UPLOAD_VERTEX_BUFFER_OBJECT_EXT:
        cout << "Using Vertex Buffer Objects EXT extension" << endl;
        ms_areVBOsSupported = true;
        break;
    case DATA_UPLOAD_VERTEX_BUFFER_OBJECT:
        cout << "Using Vertex Buffer Objects" << endl;
        ms_areVBOsSupported = true;
        break;
    default:
        cerr << "Error: invalid data_upload_t: " << ms_dataUploadMode << endl;
    }

    switch (ms_mipMapGenerationMode) {
    case MIPMAP_GENERATION_GLU:
        cout << "Using mipmaps glu" << endl;
        break;
    case MIPMAP_GENERATION_TEX_PARAMETER_EXT:
        cout << "Using mipmaps SGIS extension" << endl;
        break;
    case MIPMAP_GENERATION_TEX_PARAMETER:
        cout << "Using mipmaps" << endl;
        break;
    default:
        cerr << "Error: invalid mipmap_generation_t: " << ms_mipMapGenerationMode << endl;
    }

    switch (ms_textureCompressionMode) {
    case TEXTURE_COMPRESSION_NONE:
        cout << "Texture compression not supported" << endl;
        break;
    case TEXTURE_COMPRESSION_EXT:
        cout << "Using texture compression ARB extension" << endl;
        break;
    case TEXTURE_COMPRESSION:
        cout << "Using texture compression" << endl;
        break;
    default:
        cerr << "Error: invalid texture_compression_t: " << ms_textureCompressionMode << endl;
    }
    cout << "Forcing no texture compression!!" << endl;
    ms_textureCompressionMode = TEXTURE_COMPRESSION_NONE;

    switch (ms_renderingMethod) {
    case RENDERING_METHOD_FIXED_PIPELINE:
        cout << "Using fixed pipeline" << endl;
        glEnable(GL_LIGHTING);
        glEnable(GL_TEXTURE_2D);
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_NORMAL_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        ms_areShadersSupported = false;
        break;
    case RENDERING_METHOD_SHADERS_EXT:
        cout << "Using shaders ARB extension" << endl;
        glEnableVertexAttribArrayARB(VERTEX_ARRAY_INDEX);
        glEnableVertexAttribArrayARB(NORMALS_ARRAY_INDEX);
        ms_areShadersSupported = true;
        break;
    case RENDERING_METHOD_SHADERS:
        cout << "Using shaders" << endl;
        glEnableVertexAttribArray(VERTEX_ARRAY_INDEX);
        glEnableVertexAttribArray(NORMALS_ARRAY_INDEX);
        ms_areShadersSupported = true;
        break;
    default:
        cerr << "Error: invalid rendering_method_t: " << ms_renderingMethod << endl;
    }
    cout << endl;

    // always
    glFrontFace(GL_CCW); // redundant
    glCullFace(GL_BACK); // redundant
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
}

void OpenGL::projectionMatrixOrthographic(float width, float height, float near, float far) {
    // implementation from http://db-in.com/blog/2011/04/cameras-on-opengl-es-2-x/
    float deltaZ = far - near;

    ms_projectionMatrix[ 0] =  2.0f / width;
    ms_projectionMatrix[ 1] =  0.0f;
    ms_projectionMatrix[ 2] =  0.0f;
    ms_projectionMatrix[ 3] =  0.0f;

    ms_projectionMatrix[ 4] =  0.0f;
    ms_projectionMatrix[ 5] = -2.0f / height;
    ms_projectionMatrix[ 6] =  0.0f;
    ms_projectionMatrix[ 7] =  0.0f;

    ms_projectionMatrix[ 8] =  0.0f;
    ms_projectionMatrix[ 9] =  0.0f;
    ms_projectionMatrix[10] = -2.0f / deltaZ;
    ms_projectionMatrix[11] =  0.0f;

    ms_projectionMatrix[12] = -1.0f;
    ms_projectionMatrix[13] =  1.0f;
    ms_projectionMatrix[14] = -(far + near) / deltaZ;
    ms_projectionMatrix[15] =  1.0f;
}

void OpenGL::projectionMatrixPerspective(float perspectiveFOV, float aspectRatio, float near, float far) {
    // Mesa 9.0 glu implementation
    float deltaZ = far - near;
    float radians = degToRad(perspectiveFOV) * 0.5f;
    float sine = float(sin(radians));
    if ((deltaZ == 0.0f) || (sine == 0.0f) || (aspectRatio == 0.0f))
        return;
    float cotangent = float(cos(radians)) / sine;

    ms_projectionMatrix[ 0] =  cotangent / aspectRatio;
    ms_projectionMatrix[ 1] =  0.0f;
    ms_projectionMatrix[ 2] =  0.0f;
    ms_projectionMatrix[ 3] =  0.0f;

    ms_projectionMatrix[ 4] =  0.0f;
    ms_projectionMatrix[ 5] =  cotangent;
    ms_projectionMatrix[ 6] =  0.0f;
    ms_projectionMatrix[ 7] =  0.0f;

    ms_projectionMatrix[ 8] =  0.0f;
    ms_projectionMatrix[ 9] =  0.0f;
    ms_projectionMatrix[10] = -(far + near) / deltaZ;
    ms_projectionMatrix[11] = -1.0f;

    ms_projectionMatrix[12] =  0.0f;
    ms_projectionMatrix[13] =  0.0f;
    ms_projectionMatrix[14] = -2.0f * near * far / deltaZ;
    ms_projectionMatrix[15] =  0.0f;
}



void gl::bindDataBuffer(const unsigned int vboId) {
    if (OpenGL::dataUploadMode() == DATA_UPLOAD_VERTEX_BUFFER_OBJECT_EXT)
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, vboId);
    else
        glBindBuffer(GL_ARRAY_BUFFER, vboId);
}

void gl::bindIndexBuffer(const unsigned int indicesId) {
    if (OpenGL::dataUploadMode() == DATA_UPLOAD_VERTEX_BUFFER_OBJECT_EXT)
        glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, indicesId);
    else
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesId);
}

void gl::vertexAttribPointer(const unsigned int dataIndex, const size_t bytesOffset) {
    if (OpenGL::dataUploadMode() == DATA_UPLOAD_VERTEX_BUFFER_OBJECT_EXT)
        glVertexAttribPointerARB(dataIndex, 3, GL_FLOAT, GL_FALSE, 0, (void*)(bytesOffset));
    else
        glVertexAttribPointer(dataIndex, 3, GL_FLOAT, GL_FALSE, 0, (void*)(bytesOffset));
}

void gl::drawElements(const size_t totalIndices) {
    glDrawElements(GL_TRIANGLES, GLsizei(totalIndices), GL_UNSIGNED_INT, 0);
}



unsigned int gl::createVertexShader() {
    if (OpenGL::renderingMethod() == RENDERING_METHOD_SHADERS_EXT)
        return glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
    return glCreateShader(GL_VERTEX_SHADER);
}

unsigned int gl::createFragmentShader() {
    if (OpenGL::renderingMethod() == RENDERING_METHOD_SHADERS_EXT)
        return glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);
    return glCreateShader(GL_FRAGMENT_SHADER);
}

void gl::deleteShader(const unsigned int shaderId) {
    if (OpenGL::renderingMethod() == RENDERING_METHOD_SHADERS_EXT)
        glDeleteObjectARB(shaderId);
    else
        glDeleteShader(shaderId);
}

bool gl::shaderSource(const unsigned int shaderId, const std::string& source) {
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

bool gl::compileShader(const unsigned int shaderId) {
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

string gl::getShaderInfoLog(const unsigned int shaderId) {
    char shaderLog[ERRORS_LOG_BUFFER_SIZE];
    if (OpenGL::renderingMethod() == RENDERING_METHOD_SHADERS_EXT)
        glGetInfoLogARB(shaderId, ERRORS_LOG_BUFFER_SIZE, 0, shaderLog);
    else
        glGetShaderInfoLog(shaderId, ERRORS_LOG_BUFFER_SIZE, 0, shaderLog);
    return string(shaderLog);
}

unsigned int gl::createProgram() {
    if (OpenGL::renderingMethod() == RENDERING_METHOD_SHADERS_EXT)
        return glCreateProgramObjectARB();
    return glCreateProgram();
}

void gl::deleteProgram(const unsigned int programId) {
    if (OpenGL::renderingMethod() == RENDERING_METHOD_SHADERS_EXT)
        glDeleteObjectARB(programId);
    else
        glDeleteProgram(programId);
}

void gl::attachShader(const unsigned int programId, const unsigned int shaderId) {
    if (OpenGL::renderingMethod() == RENDERING_METHOD_SHADERS_EXT)
        glAttachObjectARB(programId, shaderId);
    else
        glAttachShader(programId, shaderId);
}

void gl::detachShader(const unsigned int programId, const unsigned int shaderId) {
    if (OpenGL::renderingMethod() == RENDERING_METHOD_SHADERS_EXT)
        glDetachObjectARB(programId, shaderId);
    else
        glDetachShader(programId, shaderId);
}

bool gl::linkProgram(const unsigned int programId) {
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

string gl::getProgramInfoLog(const unsigned int programId) {
    char programLog[ERRORS_LOG_BUFFER_SIZE];
    if (OpenGL::renderingMethod() == RENDERING_METHOD_SHADERS_EXT)
        glGetInfoLogARB(programId, ERRORS_LOG_BUFFER_SIZE, 0, programLog);
    else
        glGetProgramInfoLog(programId, ERRORS_LOG_BUFFER_SIZE, 0, programLog);
    return string(programLog);
}

void gl::useProgram(const unsigned int programId) {
    if (OpenGL::renderingMethod() == RENDERING_METHOD_SHADERS_EXT)
        glUseProgramObjectARB(programId);
    else
        glUseProgram(programId);
}

int gl::getUniformLocation(const unsigned int programId, const string& name) {
    if (OpenGL::renderingMethod() == RENDERING_METHOD_SHADERS_EXT)
        return glGetUniformLocationARB(programId, name.c_str());
    return glGetUniformLocation(programId, name.c_str());
}

void gl::useUniform1(const int location, const float* value) {
    if (OpenGL::renderingMethod() == RENDERING_METHOD_SHADERS_EXT)
        glUniform1fvARB(location, 1, value);
    else
        glUniform1fv(location, 1, value);
}

void gl::useUniform2(const int location, const float* value) {
    if (OpenGL::renderingMethod() == RENDERING_METHOD_SHADERS_EXT)
        glUniform2fvARB(location, 1, value);
    else
        glUniform2fv(location, 1, value);
}

void gl::useUniform3(const int location, const float* value) {
    if (OpenGL::renderingMethod() == RENDERING_METHOD_SHADERS_EXT)
        glUniform3fvARB(location, 1, value);
    else
        glUniform3fv(location, 1, value);
}

void gl::useUniform4(const int location, const float* value) {
    if (OpenGL::renderingMethod() == RENDERING_METHOD_SHADERS_EXT)
        glUniform4fvARB(location, 1, value);
    else
        glUniform4fv(location, 1, value);
}

void gl::useUniform2x2(const int location, const float* value) {
    if (OpenGL::renderingMethod() == RENDERING_METHOD_SHADERS_EXT)
        glUniformMatrix2fvARB(location, 1, GL_FALSE, value);
    else
        glUniformMatrix2fv(location, 1, GL_FALSE, value);
}

void gl::useUniform3x3(const int location, const float* value) {
    if (OpenGL::renderingMethod() == RENDERING_METHOD_SHADERS_EXT)
        glUniformMatrix3fvARB(location, 1, GL_FALSE, value);
    else
        glUniformMatrix3fv(location, 1, GL_FALSE, value);
}

void gl::useUniform4x4(const int location, const float* value) {
    if (OpenGL::renderingMethod() == RENDERING_METHOD_SHADERS_EXT)
        glUniformMatrix4fvARB(location, 1, GL_FALSE, value);
    else
        glUniformMatrix4fv(location, 1, GL_FALSE, value);
}
