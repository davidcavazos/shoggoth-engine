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
#include "commandobject.hpp"
#include "entity.hpp"

class Device;
class Scene: public CommandObject {
public:
    friend class Entity;

    Scene(const std::string& objectName, const std::string& rootNodeName, const Device* device);

    const Entity* getRoot() const;

    Entity* root();

    void initialize();
    void shutdown();
    void saveToXML(const std::string& fileName) const;
    void loadFromXML(const std::string& fileName);
    bool findEntity(const std::string& name, Entity*& entity);
    std::string sceneGraphToString();

private:
    Entity m_root;
    static std::map<std::string, Entity*> ms_entities;

    void cmdInitialize(const std::string&);
    void cmdShutdown(const std::string&);
    void cmdSaveToXML(const std::string& arg);
    void cmdLoadFromXML(const std::string& arg);
};



inline const Entity* Scene::getRoot() const {
    return &m_root;
}


inline Entity* Scene::root() {
    return &m_root;
}



inline void Scene::cmdInitialize(const std::string&) {
    initialize();
}

inline void Scene::cmdShutdown(const std::string&) {
    shutdown();
}

inline void Scene::cmdSaveToXML(const std::string& arg) {
    saveToXML(arg);
}

inline void Scene::cmdLoadFromXML(const std::string& arg) {
    loadFromXML(arg);
}

#endif // SCENE_HPP
