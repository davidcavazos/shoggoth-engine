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
#include "shoggoth-engine/linearmath/scalar.hpp"

using namespace std;

float OpenGL::ms_projectionMatrix[16] = {0};
float OpenGL::ms_viewMatrix[16] = {0};
float OpenGL::ms_modelMatrix[16] = {0};
float OpenGL::ms_modelViewMatrix[16] = {0};
float OpenGL::ms_modelViewProjectionMatrix[16] = {0};
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
    else {
        ms_isAnisotropicFilteringSupported = false;
        cout << "Anisotropic texture filtering not supported" << endl;
    }

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
//     cout << "NOTE: OpenGL.detectCapabilities: Forcing Fixed Pipeline" << endl;
//     ms_renderingMethod = RENDERING_METHOD_FIXED_PIPELINE;

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
        glEnableVertexAttribArrayARB(UVCOORDS_ARRAY_INDEX);
        ms_areShadersSupported = true;
        break;
    case RENDERING_METHOD_SHADERS:
        cout << "Using shaders" << endl;
        glEnableVertexAttribArray(VERTEX_ARRAY_INDEX);
        glEnableVertexAttribArray(NORMALS_ARRAY_INDEX);
        glEnableVertexAttribArray(UVCOORDS_ARRAY_INDEX);
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

void OpenGL::multMatrix(float* result, const float* a, const float* b) {
    // Note: this is optimized for OpenGL matrices, it will not work correctly for other matrices
    result[ 0] = a[ 0] * b[ 0] + a[ 1] * b[ 4] + a[ 2] * b[ 8] + a[ 3] * b[12];
    result[ 1] = a[ 0] * b[ 1] + a[ 1] * b[ 5] + a[ 2] * b[ 9] + a[ 3] * b[13];
    result[ 2] = a[ 0] * b[ 2] + a[ 1] * b[ 6] + a[ 2] * b[10] + a[ 3] * b[14];
    result[ 3] = a[ 0] * b[ 3] + a[ 1] * b[ 7] + a[ 2] * b[11] + a[ 3] * b[15];

    result[ 4] = a[ 4] * b[ 0] + a[ 5] * b[ 4] + a[ 6] * b[ 8] + a[ 7] * b[12];
    result[ 5] = a[ 4] * b[ 1] + a[ 5] * b[ 5] + a[ 6] * b[ 9] + a[ 7] * b[13];
    result[ 6] = a[ 4] * b[ 2] + a[ 5] * b[ 6] + a[ 6] * b[10] + a[ 7] * b[14];
    result[ 7] = a[ 4] * b[ 3] + a[ 5] * b[ 7] + a[ 6] * b[11] + a[ 7] * b[15];

    result[ 8] = a[ 8] * b[ 0] + a[ 9] * b[ 4] + a[10] * b[ 8] + a[11] * b[12];
    result[ 9] = a[ 8] * b[ 1] + a[ 9] * b[ 5] + a[10] * b[ 9] + a[11] * b[13];
    result[10] = a[ 8] * b[ 2] + a[ 9] * b[ 6] + a[10] * b[10] + a[11] * b[14];
    result[11] = a[ 8] * b[ 3] + a[ 9] * b[ 7] + a[10] * b[11] + a[11] * b[15];

    result[12] = a[12] * b[ 0] + a[13] * b[ 4] + a[14] * b[ 8] + a[15] * b[12];
    result[13] = a[12] * b[ 1] + a[13] * b[ 5] + a[14] * b[ 9] + a[15] * b[13];
    result[14] = a[12] * b[ 2] + a[13] * b[ 6] + a[14] * b[10] + a[15] * b[14];
    result[15] = a[12] * b[ 3] + a[13] * b[ 7] + a[14] * b[11] + a[15] * b[15];
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
