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
#include <boost/unordered_map.hpp>
#include "shoggoth-engine/kernel/commandobject.hpp"

class Device;
class Vector3;
class Quaternion;
class Camera;
class Light;
class RenderableMesh;
class Mesh;
class Model;
class Material;
class Texture;

class Renderer: public CommandObject {
public:
    Renderer(const std::string& objectName, const Device* device);
    ~Renderer();

    void draw();
    void registerCamera(Camera* camera);
    void unregisterCamera(Camera* camera);
    void registerLight(Light* light);
    void unregisterLight(Light* light);
    void registerRenderableMesh(RenderableMesh* renderable);
    void unregisterRenderableMesh(RenderableMesh* renderable);

    void registerModel(Model* model);
    void unregisterModel(Model* model);
    Model* findModel(const std::string& identifier);
    void registerTexture(Texture* texture);
    void unregisterTexture(Texture* texture);
    Texture* findTexture(const std::string& fileName);
    void registerMaterial(Material* material);
    void unregisterMaterial(Material* material);
    Material* findMaterial(const std::string& fileName);
    void setAmbientLight(const float r, const float g, const float b, const float a = 1.0f);

    void updateLegacyLights() const;
    void uploadMeshToGPU(Mesh& mesh);
    void deleteMeshFromGPU(const Mesh& mesh);
    void uploadTextureToGPU(Texture& texture);
    void deleteTextureFromGPU(const Texture& texture);
    std::string listsToString() const;

private:
    const Device* m_device;
    Camera* m_activeCamera;
    std::set<Camera*> m_cameras;
    std::set<Light*> m_lights;
    std::set<RenderableMesh*> m_renderableMeshes;
    boost::unordered_map<std::string, Model*> m_models;
    boost::unordered_map<std::string, Material*> m_materials;
    boost::unordered_map<std::string, Texture*> m_textures;
    Material* m_defaultMaterial;

    Renderer(const Renderer& rhs);
    Renderer& operator=(const Renderer& rhs);

    void initLighting() const;
    void initCamera();
    void displayLegacyLights() const;

    std::string cmdAmbientLight(std::deque<std::string>& args);
    std::string cmdTextureFiltering(std::deque<std::string>& args);
    std::string cmdAnisotropy(std::deque<std::string>& args);
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

inline void Renderer::registerRenderableMesh(RenderableMesh* renderable) {
    m_renderableMeshes.insert(renderable);
}

inline void Renderer::unregisterRenderableMesh(RenderableMesh* renderable) {
    m_renderableMeshes.erase(renderable);
}

#endif // RENDERER_HPP
