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


#ifndef RESOURCES_HPP
#define RESOURCES_HPP

#include <string>
#include <boost/unordered_map.hpp>
#include "engine/kernel/commandobject.hpp"

class Renderer;
class Model;
class Texture;

class Resources: public CommandObject {
public:
    Resources(const std::string& objectName, Renderer* renderer);

    void initialize();
    void shutdown();
    Model* generateBox(const std::string& identifier, const double lengthX, const double lengthY, const double lengthZ);
    Model* generateModelFromFile(const std::string& fileName);
    Texture* generateTextureFromFile(const std::string& fileName);
    std::string listsToString();

private:
    typedef boost::unordered_map<std::string, Model*> models_map_t;
    typedef boost::unordered_map<std::string, Texture*> textures_map_t;
    Renderer* m_renderer;
    models_map_t m_modelsMap;
    textures_map_t m_texturesMap;

    Resources(const Resources& rhs);
    Resources& operator=(const Resources&);

    void registerModel(Model* model);
    void registerTexture(Texture* texture);
    Model* findModel(const std::string& identifier);
    Texture* findTexture(const std::string& fileName);

    void cmdInitialize(const std::string&);
    void cmdShutdown(const std::string&);
};



inline void Resources::cmdInitialize(const std::string&) {
    initialize();
}

inline void Resources::cmdShutdown(const std::string&) {
    shutdown();
}

#endif // RESOURCES_HPP
