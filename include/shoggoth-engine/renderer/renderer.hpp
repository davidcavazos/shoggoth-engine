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


#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <string>
#include <set>
#include "shoggoth-engine/kernel/commandobject.hpp"

class Device;
class Vector3;
class Quaternion;
class Camera;
class Light;
class RenderableMesh;
class Mesh;
class Texture;

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
    RENDERING_METHOD_SHADERS // 2.0
} rendering_method_t;

class Renderer: public CommandObject {
public:
    Renderer(const std::string& objectName, const Device* device);
    ~Renderer();

    bool isAnisotropicFilteringSupported() const;
    float getMaxAnisotropy() const;
    float getAnisotropy() const;
    texture_filtering_t getTextureFilteringMode() const;
    void setTextureFilteringMode(const texture_filtering_t& textureFiltering);
    void setAnisotropy(const float anisotropy);

    void draw();
    void registerCamera(Camera* camera);
    void unregisterCamera(Camera* camera);
    void registerLight(Light* light);
    void unregisterLight(Light* light);
    void registerRenderableMesh(RenderableMesh* model);
    void unregisterRenderableMesh(RenderableMesh* model);
    void setAmbientLight(const float r, const float g, const float b, const float a = 1.0f);

    void updateLegacyLights() const;
    void uploadModel(unsigned int& meshId, unsigned int& indicesId, const Mesh& mesh);
    void deleteModel(const unsigned int meshId, const unsigned int indicesId);
    void uploadTexture(unsigned int& textureId, const Texture& texture);
    void deleteTexture(const unsigned int textureId);
    std::string listsToString() const;

private:
    const Device* m_device;
    float m_projectionMatrix[16];
    Camera* m_activeCamera;
    std::set<Camera*> m_cameras;
    std::set<Light*> m_lights;
    std::set<RenderableMesh*> m_models;
    data_upload_t m_dataUploadMode;
    mipmap_generation_t m_mipMapGenerationMode;
    bool m_isAnisotropicFilteringSupported;
    float m_maxAnisotropy;
    float m_anisotropy;
    texture_filtering_t m_textureFilteringMode;
    texture_compression_t m_textureCompressionMode;
    rendering_method_t m_renderingMethod;

    Renderer(const Renderer& rhs);
    Renderer& operator=(const Renderer& rhs);

    void initLighting() const;
    void initCamera();
    void displayLegacyLights() const;
    void openGLProjectionMatrixOrthographic(float width, float height, float near, float far);
    void openGLProjectionMatrixPerspective(float perspectiveFOV, float aspectRatio, float near, float far);

    std::string cmdAmbientLight(std::deque<std::string>& args);
    std::string cmdTextureFiltering(std::deque<std::string>& args);
    std::string cmdAnisotropy(std::deque<std::string>& args);
};



inline bool Renderer::isAnisotropicFilteringSupported() const {
    return m_isAnisotropicFilteringSupported;
}

inline float Renderer::getMaxAnisotropy() const {
    return m_maxAnisotropy;
}

inline float Renderer::getAnisotropy() const {
    return m_anisotropy;
}

inline texture_filtering_t Renderer::getTextureFilteringMode() const {
    return m_textureFilteringMode;
}

inline void Renderer::setAnisotropy(const float anisotropy) {
    m_anisotropy = anisotropy;
    if (m_anisotropy > m_maxAnisotropy)
        m_anisotropy = m_maxAnisotropy;
    else if (m_anisotropy < 1.0f)
        m_anisotropy = 1.0f;
}

inline void Renderer::registerCamera(Camera* camera) {
    m_cameras.insert(camera);
    m_activeCamera = camera;
}

inline void Renderer::unregisterCamera(Camera* camera) {
    m_cameras.erase(camera);
}

inline void Renderer::registerLight(Light* light) {
    m_lights.insert(light);
    initLighting();
}

inline void Renderer::unregisterLight(Light* light) {
    m_lights.erase(light);
    initLighting();
}

inline void Renderer::registerRenderableMesh(RenderableMesh* model) {
    m_models.insert(model);
}

inline void Renderer::unregisterRenderableMesh(RenderableMesh* model) {
    m_models.erase(model);
}

#endif // RENDERER_HPP
