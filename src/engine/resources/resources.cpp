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


#include "engine/resources/resources.hpp"

#include "engine/resources/texture.hpp"
#include "engine/resources/model.hpp"
#include "engine/resources/modelloader.hpp"

using namespace std;

Resources::Resources(const string& objectName, Renderer* renderer):
    CommandObject(objectName),
    m_renderer(renderer),
    m_modelsMap(),
    m_texturesMap()
{
    registerCommand("initialize", boost::bind(&Resources::cmdInitialize, this, _1));
    registerCommand("shutdown", boost::bind(&Resources::cmdShutdown, this, _1));
}

Resources::~Resources() {
    unregisterAllCommands();
    unregisterAllAttributes();
}

void Resources::initialize() {
}

void Resources::shutdown() {
    models_map_t::const_iterator itMesh;
    for (itMesh = m_modelsMap.begin(); itMesh != m_modelsMap.end(); ++itMesh)
        delete itMesh->second;
}

Model* Resources::generateBox(const string& identifier, const double lengthX, const double lengthY, const double lengthZ) {
    Model* model;
    model = findModel(identifier);
    if (model != 0)
        return model;

    float x = static_cast<float>(lengthX * 0.5);
    float y = static_cast<float>(lengthY * 0.5);
    float z = static_cast<float>(lengthZ * 0.5);
    float vertices[] = {
         x,  y,  z,  -x,  y,  z,  -x, -y,  z,   x, -y,  z, // v0,v1,v2,v3 (front)
         x,  y,  z,   x, -y,  z,   x, -y, -z,   x,  y, -z, // v0,v3,v4,v5 (right)
         x,  y,  z,   x,  y, -z,  -x,  y, -z,  -x,  y,  z, // v0,v5,v6,v1 (top)
        -x,  y,  z,  -x,  y, -z,  -x, -y, -z,  -x, -y,  z, // v1,v6,v7,v2 (left)
        -x, -y, -z,   x, -y, -z,   x, -y,  z,  -x, -y,  z, // v7,v4,v3,v2 (bottom)
         x, -y, -z,  -x, -y, -z,  -x,  y, -z,   x,  y, -z  // v4,v7,v6,v5 (back)
    };
    float normals[] = {
         0,  0,  1,   0,  0,  1,   0,  0,  1,   0,  0,  1, // v0,v1,v2,v3 (front)
         1,  0,  0,   1,  0,  0,   1,  0,  0,   1,  0,  0, // v0,v3,v4,v5 (right)
         0,  1,  0,   0,  1,  0,   0,  1,  0,   0,  1,  0, // v0,v5,v6,v1 (top)
        -1,  0,  0,  -1,  0,  0,  -1,  0,  0,  -1,  0,  0, // v1,v6,v7,v2 (left)
         0, -1,  0,   0, -1,  0,   0, -1,  0,   0, -1,  0, // v7,v4,v3,v2 (bottom)
         0,  0, -1,   0,  0, -1,   0,  0, -1,   0,  0, -1  // v4,v7,v6,v5 (back)
    };
    unsigned int indices[] = {
         0,  1,  2,   2,  3,  0,  // front
         4,  5,  6,   6,  7,  4,  // right
         8,  9, 10,  10, 11,  8,  // top
        12, 13, 14,  14, 15, 12,  // left
        16, 17, 18,  18, 19, 16,  // bottom
        20, 21, 22,  22, 23, 20   // back
    };

    model = new Model(identifier, m_renderer);
    model->setTotalMeshes(1);
    model->mesh(0)->setVertices(vertices, 72);
    model->mesh(0)->setNormals(normals, 72);
    model->mesh(0)->setIndices(indices, 36);
    registerModel(model);
    model->uploadToGPU();
    return model;
}

Model* Resources::generateModelFromFile(const std::string& fileName) {
    Model* model;
    model = findModel(fileName);
    if (model != 0)
        return model;

    model = new Model(fileName, m_renderer);
    ModelLoader::load(fileName, *model, m_renderer, this);
    registerModel(model);
    model->uploadToGPU();
    return model;
}

Texture* Resources::generateTextureFromFile(const std::string& fileName) {
    Texture* texture;
    texture = findTexture(fileName);
    if (texture != 0)
        return texture;

    texture = new Texture(fileName, m_renderer);
    texture->load();
    registerTexture(texture);
    return texture;
}

string Resources::listsToString() {
    stringstream ss;
    ss << "Mesh Data List:" << endl;
    models_map_t::const_iterator itMesh;
    for (itMesh = m_modelsMap.begin(); itMesh != m_modelsMap.end(); ++itMesh)
        ss << "  " << itMesh->second->getIdentifier() << endl;
    return ss.str();
}



void Resources::registerModel(Model* model) {
    m_modelsMap.insert(pair<string, Model*>(model->getIdentifier(), model));
}

void Resources::registerTexture(Texture* texture) {
    m_texturesMap.insert(pair<string, Texture*>(texture->getFileName(), texture));
}

Model* Resources::findModel(const std::string& identifier) {
    models_map_t::const_iterator it;
    it = m_modelsMap.find(identifier);
    if (it != m_modelsMap.end())
        return it->second;
    return 0;
}

Texture* Resources::findTexture(const std::string& fileName) {
    textures_map_t::const_iterator it;
    it = m_texturesMap.find(fileName);
    if (it != m_texturesMap.end())
        return it->second;
    return 0;
}



Resources::Resources(const Resources& rhs):
    CommandObject(rhs.m_objectName),
    m_renderer(rhs.m_renderer),
    m_modelsMap(rhs.m_modelsMap),
    m_texturesMap(rhs.m_texturesMap)
{
    cerr << "Error: Resources copy constructor should not be called!" << endl;
}

Resources& Resources::operator=(const Resources&) {
    cerr << "Error: Resources assignment operator should not be called!" << endl;
    return *this;
}
