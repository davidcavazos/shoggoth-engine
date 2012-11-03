/*
    <one line to give the library's name and an idea of what it does.>
    Copyright (C) 2012  David Cavazos <davido262@gmail.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/


#ifndef SCENE_HPP
#define SCENE_HPP

#include <string>
#include <boost/property_tree/ptree.hpp>
#include "commandobject.hpp"

class Entity;
class Device;
class Renderer;
class Resources;
class PhysicsWorld;

class Scene: public CommandObject {
public:
    friend class Entity;

    Scene(const std::string& objectName,
          const std::string& rootNodeName,
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

private:
    const Device* m_device;
    Renderer* m_renderer;
    Resources* m_resources;
    PhysicsWorld* m_physicsWorld;
    std::string m_rootName;
    Entity* m_root;
    static std::map<std::string, Entity*> ms_entities;

    Scene(const Scene& rhs);
    Scene& operator=(const Scene&);

    void saveToPTree(const std::string& path, boost::property_tree::ptree& tree, const Entity* node) const;
    bool loadFromPTree(const std::string& path, const boost::property_tree::ptree& tree, Entity* node, Entity* parent, bool& isCameraFound);

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
