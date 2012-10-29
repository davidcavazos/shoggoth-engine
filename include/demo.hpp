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


#ifndef GAME_HPP
#define GAME_HPP

#include "engine/kernel/device.hpp"
#include "engine/kernel/scene.hpp"
#include "engine/renderer/renderer.hpp"
#include "engine/physics/physicsworld.hpp"
#include "engine/resources/resources.hpp"

class Demo: public CommandObject {
public:
    Demo(const std::string& objectName,
         const std::string& deviceName,
         const std::string& sceneName,
         const std::string& rootNodeName,
         const std::string& rendererName,
         const std::string& resourcesName,
         const std::string& physicsWorldName);
    ~Demo();

    void loadScene();
    void bindInputs();
    void runMainLoop();

private:
    bool m_isRunning;
    Device m_device;
    Scene m_scene;
    Renderer m_renderer;
    Resources m_resources;
    PhysicsWorld m_physicsWorld;

    void cmdQuit(const std::string&);
    void cmdRunCommand(const std::string& arg);
    void cmdPrintEntity(const std::string& arg);
    void cmdOnMouseMotion(const std::string&);
    void cmdFireCube(const std::string&);
    void cmdFireSphere(const std::string&);
};

#endif // GAME_HPP
