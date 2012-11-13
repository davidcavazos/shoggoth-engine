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

class Renderer: public CommandObject {
public:
    Renderer(const std::string& objectName, const Device* device);
    ~Renderer();

    void initialize();
    void shutdown();
    void registerCamera(Camera* camera);
    void unregisterCamera(Camera* camera);
    void registerLight(Light* light);
    void unregisterLight(Light* light);
    void registerRenderableMesh(RenderableMesh* model);
    void unregisterRenderableMesh(RenderableMesh* model);
    void setAmbientLight(const float r, const float g, const float b, const float a = 1.0f);
    void updateLights() const;
    void uploadModel(unsigned int& meshId, unsigned int& indicesId, const Mesh& mesh);
    void deleteModel(const unsigned int meshId, const unsigned int indicesId);
    void uploadTexture(unsigned int& textureId, const Texture& texture);
    void deleteTexture(const unsigned int textureId);
    void draw();
    std::string listsToString() const;

private:
    const Device* m_device;
    float m_projectionMatrix[16];
    Camera* m_activeCamera;
    std::set<Camera*> m_cameras;
    std::set<Light*> m_lights;
    std::set<RenderableMesh*> m_models;

    Renderer(const Renderer& rhs);
    Renderer& operator=(const Renderer& rhs);

    void initLighting() const;
    void initCamera();
    void displayLegacyLights() const;
    void openGLProjectionMatrixOrthographic(float width, float height, float near, float far);
    void openGLProjectionMatrixPerspective(float perspectiveFOV, float aspectRatio, float near, float far);

    std::string cmdInitialize(std::deque<std::string>&);
    std::string cmdShutdown(std::deque<std::string>&);
    std::string cmdAmbientLight(std::deque<std::string>& args);
};



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

inline std::string Renderer::cmdInitialize(std::deque<std::string>&) {
    initialize();
    return "";
}

inline std::string Renderer::cmdShutdown(std::deque<std::string>&) {
    shutdown();
    return "";
}

#endif // RENDERER_HPP
