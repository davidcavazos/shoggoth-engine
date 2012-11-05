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
#include "engine/kernel/commandobject.hpp"

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
    friend class Camera;
    friend class Light;
    friend class RenderableMesh;

    Renderer(const std::string& objectName, const Device* device);
    ~Renderer();

    void initialize();
    void shutdown();
    void setAmbientLight(const float r, const float g, const float b, const float a = 1.0f);
    void uploadModel(unsigned int& meshId, unsigned int& indicesId, const Mesh& mesh);
    void deleteModel(const unsigned int meshId, const unsigned int indicesId);
    void uploadTexture(unsigned int& textureId, const Texture& texture);
    void deleteTexture(const unsigned int textureId);
    void draw() const;
    std::string listsToString() const;

private:
    const Device* m_device;
    Camera* m_activeCamera;
    std::set<Camera*> m_cameras;
    std::set<Light*> m_lights;
    std::set<RenderableMesh*> m_models;

    Renderer(const Renderer& rhs);
    Renderer& operator=(const Renderer& rhs);

    void initLighting() const;
    void updateLights() const;
    void initCamera() const;
    void displayLegacyLights() const;
    void setOpenGLMatrix(float* const m, const Vector3& pos, const Quaternion& rot) const;

    void cmdInitialize(const std::string&);
    void cmdShutdown(const std::string&);
    void cmdAmbientLight(const std::string& arg);
};



inline void Renderer::cmdInitialize(const std::string&) {
    initialize();
}

inline void Renderer::cmdShutdown(const std::string&) {
    shutdown();
}

#endif // RENDERER_HPP
