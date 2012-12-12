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


#include "shoggoth-engine/renderer/renderer.hpp"

#include <iostream>
#include <cmath>
#include <GL/glew.h>
#include "shoggoth-engine/linearmath/transform.hpp"
#include "shoggoth-engine/kernel/device.hpp"
#include "shoggoth-engine/kernel/entity.hpp"
#include "shoggoth-engine/kernel/model.hpp"
#include "shoggoth-engine/renderer/camera.hpp"
#include "shoggoth-engine/renderer/light.hpp"
#include "shoggoth-engine/renderer/material.hpp"
#include "shoggoth-engine/renderer/opengl.hpp"
#include "shoggoth-engine/renderer/renderablemesh.hpp"
#include "shoggoth-engine/renderer/texture.hpp"

using namespace std;

Renderer::Renderer(const string& objectName, const Device* device):
    CommandObject(objectName),
    m_device(device),
    m_activeCamera(0),
    m_cameras(),
    m_lights(),
    m_renderableMeshes(),
    m_models(),
    m_materials(),
    m_textures(),
    m_defaultMaterial(new Material(this))
{
    registerAttribute("ambient-light", boost::bind(&Renderer::cmdAmbientLight, this, _1));
    registerAttribute("texture-filtering", boost::bind(&Renderer::cmdTextureFiltering, this, _1));
    registerAttribute("anisotropy", boost::bind(&Renderer::cmdAnisotropy, this, _1));

    OpenGL::detectCapabilities();
    m_defaultMaterial->loadFromFile("assets/materials/default.material");
    Culling::initialize();
}

Renderer::~Renderer() {
    Culling::shutdown();

    cout << "Destroying all textures: " << m_textures.size() << endl;
    boost::unordered_map<string, Texture*>::const_iterator itTexture;
    for (itTexture = m_textures.begin(); itTexture != m_textures.end(); ++itTexture) {
        deleteTextureFromGPU(*itTexture->second);
        delete itTexture->second;
    }

    cout << "Destroying all materials: " << m_materials.size() << endl;
    delete m_defaultMaterial;
    boost::unordered_map<string, Material*>::const_iterator itMat;
    for (itMat = m_materials.begin(); itMat != m_materials.end(); ++itMat)
        delete itMat->second; // automatically destroys and frees its shader program

    cout << "Destroying all models: " << m_models.size() << endl;
    boost::unordered_map<string, Model*>::const_iterator itModel;
    for (itModel = m_models.begin(); itModel != m_models.end(); ++itModel) {
        for (size_t i = 0; i < itModel->second->getTotalMeshes(); ++i)
            deleteMeshFromGPU(*itModel->second->mesh(i));
        delete itModel->second;
    }

    // renderablemeshes, lights and cameras destroyed when destroying root scene node

    unregisterAllCommands();
    unregisterAllAttributes();
}

void Renderer::draw() {
    if (m_activeCamera->hasChanged()) {
        initCamera();
        m_activeCamera->setHasChanged(false);
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // set camera
    const Entity* cam = m_activeCamera->getEntity();
    Transform(cam->getOrientationAbs(), cam->getPositionAbs()).inverse().getOpenGLMatrix(OpenGL::ms_viewMatrix);
    if (!OpenGL::areShadersSupported()) {
        glLoadIdentity();
        glMultMatrixf(OpenGL::ms_viewMatrix);
        displayLegacyLights();
    }

    // frustum culling
    set<RenderableMesh*> modelsInFrustum;
    Culling::performFrustumCulling(OpenGL::ms_projectionMatrix, m_activeCamera->getEntity(), modelsInFrustum);

    // set meshes
    set<RenderableMesh*>::const_iterator it;
    for (it = modelsInFrustum.begin(); it != modelsInFrustum.end(); ++it) {
        const Model* model = (*it)->getModel();
        const Entity* entity = (*it)->getEntity();

        Transform(entity->getOrientationAbs(), entity->getPositionAbs()).getOpenGLMatrix(OpenGL::ms_modelMatrix);
        OpenGL::multMatrix(OpenGL::ms_modelViewMatrix, OpenGL::ms_modelMatrix, OpenGL::ms_viewMatrix);
        OpenGL::multMatrix(OpenGL::ms_modelViewProjectionMatrix, OpenGL::ms_modelViewMatrix, OpenGL::ms_projectionMatrix);
        OpenGL::inverseMatrix(OpenGL::ms_normalMatrix, OpenGL::ms_modelViewMatrix);
        OpenGL::transposeMatrix(OpenGL::ms_normalMatrix);
        if (!OpenGL::areShadersSupported()) {
            glLoadIdentity();
            glMultMatrixf(OpenGL::ms_modelViewMatrix);
        }
        for (size_t n = 0; n < model->getTotalMeshes(); ++n) {
            const Mesh* mesh = model->getMesh(n);
            const Material* material = (*it)->getMaterial(n);

            // draw mesh
            if (material != 0)
                material->useMaterial();
            else
                m_defaultMaterial->useMaterial();
            if (OpenGL::areVBOsSupported()) {
                // bind buffers
                gl::bindVboBuffer(mesh->getVboId());
                gl::bindIndexBuffer(mesh->getIndicesId());

                // draw
                if (OpenGL::areShadersSupported()) {
                    gl::vertexAttribPointer(VERTEX_ARRAY_INDEX, 0);
                    gl::vertexAttribPointer(NORMALS_ARRAY_INDEX, mesh->getVerticesBytes());
                    gl::vertexAttribPointer(UVCOORDS_ARRAY_INDEX, mesh->getVerticesBytes() + mesh->getNormalsBytes());
                }
                else {
                    glVertexPointer(3, GL_FLOAT, 0, 0);
                    glNormalPointer(GL_FLOAT, 0, (GLvoid*)mesh->getVerticesBytes());
                    glTexCoordPointer(2, GL_FLOAT, 0, (GLvoid*)(mesh->getVerticesBytes() + mesh->getNormalsBytes()));
                }
                gl::drawElements(mesh->getIndicesSize());

                // unbind buffers
                gl::bindVboBuffer(0);
                gl::bindIndexBuffer(0);
            }
            else {
                glVertexPointer(3, GL_FLOAT, 0, mesh->getVerticesPtr());
                glNormalPointer(GL_FLOAT, 0, mesh->getNormalsPtr());
                glTexCoordPointer(2, GL_FLOAT, 0, mesh->getUvCoordsPtr());
                glDrawElements(GL_TRIANGLES, GLsizei(mesh->getIndicesSize()), GL_UNSIGNED_INT, mesh->getIndicesPtr());
            }
        }
    }
    m_device->swapBuffers();
}

void Renderer::registerModel(Model* model) {
    m_models.insert(pair<string, Model*>(model->getIdentifier(), model));
}

void Renderer::unregisterModel(Model* model) {
    m_models.erase(model->getIdentifier());
}

Model* Renderer::findModel(const std::string& identifier) {
    boost::unordered_map<string, Model*>::const_iterator it;
    it = m_models.find(identifier);
    if (it != m_models.end())
        return it->second;
    return 0;
}

void Renderer::registerTexture(Texture* texture) {
    m_textures.insert(pair<string, Texture*>(texture->getFileName(), texture));
}

void Renderer::unregisterTexture(Texture* texture) {
    m_textures.erase(texture->getFileName());
}

Texture* Renderer::findTexture(const std::string& fileName) {
    boost::unordered_map<string, Texture*>::const_iterator it;
    it = m_textures.find(fileName);
    if (it != m_textures.end())
        return it->second;
    return 0;
}

void Renderer::registerMaterial(Material* material) {
    m_materials.insert(pair<string, Material*>(material->getFileName(), material));
}

void Renderer::unregisterMaterial(Material* material) {
    m_materials.erase(material->getFileName());
}

Material* Renderer::findMaterial(const std::string& fileName) {
    boost::unordered_map<string, Material*>::const_iterator it;
    it = m_materials.find(fileName);
    if (it != m_materials.end())
        return it->second;
    return 0;
}

void Renderer::setAmbientLight(const float r, const float g, const float b, const float a) {
    GLfloat global_ambient[] = {r, g, b, a};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);
}

void Renderer::updateLegacyLights() const {
    set<Light*>::const_iterator it = m_lights.begin();
    for (size_t i = 0; i < m_lights.size(); ++i) {
        GLenum lightEnum;
        switch (i) {
        case 0:
            lightEnum = GL_LIGHT0;
            break;
        case 1:
            lightEnum = GL_LIGHT1;
            break;
        case 2:
            lightEnum = GL_LIGHT2;
            break;
        case 3:
            lightEnum = GL_LIGHT3;
            break;
        case 4:
            lightEnum = GL_LIGHT4;
            break;
        case 5:
            lightEnum = GL_LIGHT5;
            break;
        case 6:
            lightEnum = GL_LIGHT6;
            break;
        case 7: default:
            lightEnum = GL_LIGHT7;
            break;
        }
        glLightfv(lightEnum, GL_AMBIENT, (*it)->getAmbientPtr());
        glLightfv(lightEnum, GL_DIFFUSE, (*it)->getDiffusePtr());
        glLightfv(lightEnum, GL_SPECULAR, (*it)->getSpecularPtr());
        if ((*it)->getLightType() != LIGHT_POINTLIGHT) {
            glLightf(lightEnum, GL_SPOT_EXPONENT, (*it)->getSpotExponent());
            glLightf(lightEnum, GL_SPOT_CUTOFF, (*it)->getSpotCutoff());
        }
        ++it;
    }
}

void Renderer::uploadMeshToGPU(Mesh& mesh) {
    size_t verticesBytes = mesh.getVerticesBytes();
    size_t normalsBytes = mesh.getNormalsBytes();
    size_t uvCoordsBytes = mesh.getUvCoordsBytes();
    size_t indicesBytes = mesh.getIndicesBytes();
    if (OpenGL::areVBOsSupported()) {
        unsigned int id = gl::genBuffer();
        mesh.setVboId(id);
        gl::bindVboBuffer(mesh.getVboId());
        gl::vboBufferBytes(verticesBytes + normalsBytes + uvCoordsBytes);
        gl::vboBufferSubData(0, verticesBytes, mesh.getVerticesPtr());
        gl::vboBufferSubData(verticesBytes, normalsBytes, mesh.getNormalsPtr());
        gl::vboBufferSubData(verticesBytes + normalsBytes, uvCoordsBytes, mesh.getUvCoordsPtr());
        if (verticesBytes + normalsBytes + uvCoordsBytes != gl::getVboBufferBytes())
            cerr << "Error: data size is mismatch with input array" << endl;

        id = gl::genBuffer();
        mesh.setIndicesId(id);
        gl::bindIndexBuffer(mesh.getIndicesId());
        gl::indexBufferData(indicesBytes, mesh.getIndicesPtr());
        if (indicesBytes != gl::getIndexBufferBytes())
            cerr << "Error: data size is mismatch with input array" << endl;
    }
}

void Renderer::deleteMeshFromGPU(const Mesh& mesh) {
    if (OpenGL::areVBOsSupported()) {
        gl::deleteBuffer(mesh.getVboId());
        gl::deleteBuffer(mesh.getIndicesId());
    }
}

void Renderer::uploadTextureToGPU(Texture& texture) {
    GLenum textureFormat;

    unsigned int id;
    glGenTextures(1, &id);
    texture.setId(id);
    glBindTexture(GL_TEXTURE_2D, texture.getId());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    switch (texture.getTextureFormat()) {
    case TEXTURE_FORMAT_RGBA:
        textureFormat = GL_RGBA;
        break;
    case TEXTURE_FORMAT_RGB:
        textureFormat = GL_RGB;
        break;
    case TEXTURE_FORMAT_BGRA:
        textureFormat = GL_BGRA;
        break;
    case TEXTURE_FORMAT_BGR:
        textureFormat = GL_BGR;
        break;
    default:
        cerr << "Error: invalid texture_format_t: " << texture.getTextureFormat() << endl;
        textureFormat = GL_RGBA;
    }
    switch (OpenGL::textureFilteringMode()) {
    case TEXTURE_FILTERING_NEAREST:
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        break;
    case TEXTURE_FILTERING_ANISOTROPIC:
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, OpenGL::anisotropy());
        // fallthrough
    case TEXTURE_FILTERING_LINEAR:
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        break;
    default:
        cerr << "Error: invalid texture_filtering_t: " << OpenGL::textureFilteringMode() << endl;
    }
    switch (OpenGL::mipMapGenerationMode()) {
    case MIPMAP_GENERATION_TEX_PARAMETER:
        glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE); // 1.4
        break;
    case MIPMAP_GENERATION_TEX_PARAMETER_EXT:
        glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);
        break;
    case MIPMAP_GENERATION_GLU:
        gluBuild2DMipmaps(GL_TEXTURE_2D, GLint(texture.getBytesPerPixel()), GLint(texture.getWidth()),
                          GLsizei(texture.getHeight()), textureFormat, GL_UNSIGNED_BYTE, texture.getPixels());
        break;
    default:
        cerr << "Error: invalid mipmap_generation_t: " << OpenGL::mipMapGenerationMode() << endl;
    }
    if (OpenGL::mipMapGenerationMode() != MIPMAP_GENERATION_GLU) {
        switch (OpenGL::textureCompressionMode()) {
        case TEXTURE_COMPRESSION_NONE:
            glTexImage2D(GL_TEXTURE_2D, 0, GLint(texture.getBytesPerPixel()), GLint(texture.getWidth()),
                    GLsizei(texture.getHeight()), 0, textureFormat, GL_UNSIGNED_BYTE, texture.getPixels());
            break;
        case TEXTURE_COMPRESSION_EXT:
            glCompressedTexImage2DARB(GL_TEXTURE_2D, 0, GLint(texture.getBytesPerPixel()), GLint(texture.getWidth()),
                    GLsizei(texture.getHeight()), 0, GLsizei(texture.getWidth() * texture.getHeight()), texture.getPixels());
            break;
        case TEXTURE_COMPRESSION:
//             glCompressedTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGBA, texture.getWidth(),
//                     texture.getHeight(), 0, (texture.getWidth() * texture.getHeight()) / 2, texture.getPixels());
            glTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGB, GLint(texture.getWidth()),
                    GLsizei(texture.getHeight()), 0, textureFormat, GL_UNSIGNED_BYTE, texture.getPixels());
            break;
        default:
            cerr << "Error: invalid texture_compression_t: " << OpenGL::textureCompressionMode() << endl;
        }
    }
}

void Renderer::deleteTextureFromGPU(const Texture& texture) {
    unsigned int id = texture.getId();
    glDeleteTextures(1, &id);
}

string Renderer::listsToString() const {
    stringstream ss;
    ss << "Renderer Cameras List:" << endl;
    set<Camera*>::const_iterator itCam;
    for (itCam = m_cameras.begin(); itCam != m_cameras.end(); ++itCam) {
        ss << "  " << (*itCam)->getDescription();
        if (*itCam == m_activeCamera)
            ss << " *";
        ss << endl;
    }

    ss << "Lights List:" << endl;
    set<Light*>::const_iterator itLight;
    for (itLight = m_lights.begin(); itLight != m_lights.end(); ++itLight)
        ss << "  " << (*itLight)->getDescription() << endl;

    ss << "Renderable Meshes List:" << endl;
    set<RenderableMesh*>::const_iterator itRend;
    for (itRend = m_renderableMeshes.begin(); itRend != m_renderableMeshes.end(); ++itRend)
        ss << "  " << (*itRend)->getDescription() << endl;

    ss << "Models List:" << endl;
    boost::unordered_map<string, Model*>::const_iterator itModel;
    for (itModel = m_models.begin(); itModel != m_models.end(); ++itModel)
        ss << "  " << itModel->second->getIdentifier() << endl;

    ss << "Textures List:" << endl;
    boost::unordered_map<string, Texture*>::const_iterator itTexture;
    for (itTexture = m_textures.begin(); itTexture != m_textures.end(); ++itTexture)
        ss << "  " << itTexture->second->getFileName() << endl;
    return ss.str();
}

Renderer::Renderer(const Renderer& rhs):
    CommandObject(rhs.m_objectName),
    m_device(rhs.m_device),
    m_activeCamera(rhs.m_activeCamera),
    m_cameras(rhs.m_cameras),
    m_lights(rhs.m_lights),
    m_renderableMeshes(rhs.m_renderableMeshes),
    m_models(rhs.m_models),
    m_materials(rhs.m_materials),
    m_textures(rhs.m_textures),
    m_defaultMaterial(rhs.m_defaultMaterial)
{
    cerr << "Renderer copy constructor should not be called" << endl;
}

Renderer& Renderer::operator=(const Renderer& rhs) {
    cerr << "Renderer assignment operator should not be called" << endl;
    if (this == &rhs)
        return *this;
    return *this;
}

void Renderer::initLighting() const {
    // enable lighting for legacy lights
    //     glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR); // 1.4

    set<Light*>::const_iterator it = m_lights.begin();
    for (size_t i = 0; i < m_lights.size(); ++i) {
        GLenum lightEnum;
        switch (i) {
        case 0:
            lightEnum = GL_LIGHT0;
            break;
        case 1:
            lightEnum = GL_LIGHT1;
            break;
        case 2:
            lightEnum = GL_LIGHT2;
            break;
        case 3:
            lightEnum = GL_LIGHT3;
            break;
        case 4:
            lightEnum = GL_LIGHT4;
            break;
        case 5:
            lightEnum = GL_LIGHT5;
            break;
        case 6:
            lightEnum = GL_LIGHT6;
            break;
        case 7: default:
            lightEnum = GL_LIGHT7;
            break;
        }
        glEnable(lightEnum);
        glLightfv(lightEnum, GL_AMBIENT, (*it)->getAmbientPtr());
        glLightfv(lightEnum, GL_DIFFUSE, (*it)->getDiffusePtr());
        glLightfv(lightEnum, GL_SPECULAR, (*it)->getSpecularPtr());
        ++it;
    }
    // disable unused lights
    for (size_t i = m_lights.size(); i < 8; ++i) {
        GLenum lightEnum;
        switch (i) {
        case 0:
            lightEnum = GL_LIGHT0;
            break;
        case 1:
            lightEnum = GL_LIGHT1;
            break;
        case 2:
            lightEnum = GL_LIGHT2;
            break;
        case 3:
            lightEnum = GL_LIGHT3;
            break;
        case 4:
            lightEnum = GL_LIGHT4;
            break;
        case 5:
            lightEnum = GL_LIGHT5;
            break;
        case 6:
            lightEnum = GL_LIGHT6;
            break;
        case 7: default:
            lightEnum = GL_LIGHT7;
            break;
        }
        glDisable(lightEnum);
    }
}

void Renderer::initCamera() {
    m_activeCamera->setViewport(0, 0, m_device->getWinWidth(), m_device->getWinHeight());
    viewport_t view = m_activeCamera->getViewport();
    glViewport(view.posX, view.posY, GLsizei(view.width), GLsizei(view.height));

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    switch (m_activeCamera->getCameraType()) {
    case CAMERA_ORTHOGRAPHIC:
        OpenGL::projectionMatrixOrthographic(
            m_activeCamera->getOrthoWidth(),
            m_activeCamera->getOrthoHeight(),
            m_activeCamera->getNearDistance(),
            m_activeCamera->getFarDistance()
        );
        break;
    case CAMERA_PROJECTION:
        OpenGL::projectionMatrixPerspective(
            m_activeCamera->getPerspectiveFOV(),
            m_activeCamera->getAspectRatio(),
            m_activeCamera->getNearDistance(),
            m_activeCamera->getFarDistance()
        );
        break;
    default:
        cerr << "Error: invalid camera_t: " << m_activeCamera->getCameraType() << endl;
    }
    glMultMatrixf(OpenGL::ms_projectionMatrix);
    glMatrixMode(GL_MODELVIEW);
}

void Renderer::displayLegacyLights() const {
    set<Light*>::const_iterator itLight = m_lights.begin();
    for (size_t i = 0; i < m_lights.size(); ++i) {
        GLenum lightEnum;
        switch (i) {
        case 0:
            lightEnum = GL_LIGHT0;
            break;
        case 1:
            lightEnum = GL_LIGHT1;
            break;
        case 2:
            lightEnum = GL_LIGHT2;
            break;
        case 3:
            lightEnum = GL_LIGHT3;
            break;
        case 4:
            lightEnum = GL_LIGHT4;
            break;
        case 5:
            lightEnum = GL_LIGHT5;
            break;
        case 6:
            lightEnum = GL_LIGHT6;
            break;
        case 7: default:
            lightEnum = GL_LIGHT7;
            break;
        }
        float w = ((*itLight)->getLightType() == LIGHT_DIRECTIONAL)? 0.0f : 1.0f;
        const Vector3& pos = (*itLight)->getEntity()->getPositionAbs();
        float lightPosition[] = {float(pos.getX()), float(pos.getY()), float(pos.getZ()), w};
        glLightfv(lightEnum, GL_POSITION, lightPosition);
        glLightf(lightEnum, GL_CONSTANT_ATTENUATION, (*itLight)->getConstantAttenuation());
        glLightf(lightEnum, GL_LINEAR_ATTENUATION, (*itLight)->getLinearAttenuation());
        glLightf(lightEnum, GL_QUADRATIC_ATTENUATION, (*itLight)->getQuadraticAttenuation());
        if ((*itLight)->getLightType() != LIGHT_POINTLIGHT) {
            const Vector3& rot = VECTOR3_UNIT_Z_NEG.rotate((*itLight)->getEntity()->getOrientationAbs());
            float lightOrientation[] = {float(rot.getX()), float(rot.getY()), float(rot.getZ())};
            glLightfv(lightEnum, GL_SPOT_DIRECTION, lightOrientation);
        }
        ++itLight;
    }
}



string Renderer::cmdAmbientLight(deque<string>& args) {
    if (args.size() < 4)
        return "Error: too few arguments";
    float r = boost::lexical_cast<float>(args[0]);
    float g = boost::lexical_cast<float>(args[1]);
    float b = boost::lexical_cast<float>(args[2]);
    float a = boost::lexical_cast<float>(args[3]);
    setAmbientLight(r, g, b, a);
    return "";
}

string Renderer::cmdTextureFiltering(deque<string>& args) {
    if (args.size() < 1)
        return "Error: too few arguments";
    texture_filtering_t filtering = (texture_filtering_t)boost::lexical_cast<int>(args[0]);
    OpenGL::setTextureFilteringMode(filtering);
    return "";
}

string Renderer::cmdAnisotropy(deque<string>& args) {
    if (args.size() < 1)
        return "Error: too few arguments";
    float anisotropy = boost::lexical_cast<float>(args[0]);
    OpenGL::setAnisotropy(anisotropy);
    return "";
}
