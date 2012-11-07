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


#ifndef SCENE_HPP
#define SCENE_HPP

#include <string>
#include <set>
#include <boost/property_tree/ptree.hpp>
#include "commandobject.hpp"

class Entity;
class Component;
class ComponentFactory;
class Device;
class Renderer;
class Resources;
class PhysicsWorld;

class Scene: public CommandObject {
public:
    friend class Entity;

    Scene(const std::string& objectName,
          const std::string& rootNodeName,
          const ComponentFactory* componentFactory,
          const Device* device,
          Renderer* renderer,
          Resources* resources,
          PhysicsWorld* physicsWorld);
    ~Scene();

    const Entity* getRoot() const;

    Entity* root();

    void initialize();
    void shutdown();
    void saveToXML(const std::string& fileName) const;
    bool loadFromXML(const std::string& fileName);
    bool findEntity(const std::string& name, Entity*& entity);
    std::string sceneGraphToString();

protected:
    const ComponentFactory* m_componentFactory;
    const Device* m_device;
    Renderer* m_renderer;
    Resources* m_resources;
    PhysicsWorld* m_physicsWorld;
    std::string m_rootName;
    Entity* m_root;
    static std::map<std::string, Entity*> ms_entities;

private:
    Scene(const Scene& rhs);
    Scene& operator=(const Scene&);

    void saveToPTree(const std::string& path,
                     boost::property_tree::ptree& tree,
                     const Entity* node) const;
    bool loadFromPTree(const std::string& path,
                       const boost::property_tree::ptree& tree,
                       Entity* node,
                       Entity* parent,
                       std::set<std::string>& names,
                       bool& isCameraFound);

    void cmdInitialize(const std::string&);
    void cmdShutdown(const std::string&);
    void cmdSaveXML(const std::string& arg);
    void cmdLoadXML(const std::string& arg);
};



inline const Entity* Scene::getRoot() const {
    return m_root;
}


inline Entity* Scene::root() {
    return m_root;
}



inline void Scene::cmdInitialize(const std::string&) {
    initialize();
}

inline void Scene::cmdShutdown(const std::string&) {
    shutdown();
}

inline void Scene::cmdSaveXML(const std::string& arg) {
    saveToXML(arg);
}

inline void Scene::cmdLoadXML(const std::string& arg) {
    loadFromXML(arg);
}

#endif // SCENE_HPP
