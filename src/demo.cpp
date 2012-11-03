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


#include "demo.hpp"

#include <iostream>
#include <string>
#include <fstream>
#include <iomanip>
#include <SDL/SDL.h>
#include "engine/kernel/entity.hpp"
#include "engine/kernel/terminal.hpp"
#include "engine/renderer/renderablemesh.hpp"
#include "engine/renderer/camera.hpp"
#include "engine/renderer/light.hpp"
#include "engine/physics/rigidbody.hpp"

using namespace std;

const double FIRE_SPEED = 50.0;
const double MISSILE_SIZE = 0.5;


Demo::Demo(const string& objectName,
           const string& deviceName,
           const string& rendererName,
           const string& resourcesName,
           const string& physicsWorldName,
           const string& sceneName,
           const string& rootNodeName):
    CommandObject(objectName),
    m_isRunning(false),
    m_device(deviceName),
    m_renderer(rendererName, &m_device),
    m_resources(resourcesName, &m_renderer),
    m_physicsWorld(physicsWorldName),
    m_scene(sceneName, rootNodeName, &m_device, &m_renderer, &m_resources, &m_physicsWorld)
{
    registerCommand("quit", boost::bind(&Demo::cmdQuit, this, _1));
    registerCommand("run", boost::bind(&Demo::cmdRunCommand, this, _1));
    registerCommand("print-entity", boost::bind(&Demo::cmdPrintEntity, this, _1));
    registerCommand("on-mouse-motion", boost::bind(&Demo::cmdOnMouseMotion, this, _1));
    registerCommand("fire-cube", boost::bind(&Demo::cmdFireCube, this, _1));
    registerCommand("fire-sphere", boost::bind(&Demo::cmdFireSphere, this, _1));

    Terminal::executeScript("assets/scripts/1-initialization.txt");
}

Demo::~Demo() {
    Terminal::executeScript("assets/scripts/4-shutdown.txt");
}

void Demo::loadScene() {
    cout << "Loading scene..." << endl;

    Entity* root = m_scene.root();

    Entity* floor = root->addChild("floor");
    floor->setPositionAbs(0.0f, -1.0f, 0.0f);
    RenderableMesh* floorMesh = new RenderableMesh(floor, &m_renderer, &m_resources);
    floorMesh->loadBox(100, 1, 100);
    RigidBody* floorBody = new RigidBody(floor, &m_physicsWorld);
    floorBody->addBox(100, 1, 100);

    Entity* b1 = root->addChild("b1");
    b1->setPositionAbs(5.0f, 4.0f, -10.0f);
    RenderableMesh* b1Mesh = new RenderableMesh(b1, &m_renderer, &m_resources);
    b1Mesh->loadBox(3.0f, 9.0f, 3.0f);
    RigidBody* b1Body = new RigidBody(b1, &m_physicsWorld);
    b1Body->addBox(3, 9, 3);

    // model            faces (triangles)
    // icosphere1              20
    // icosphere2              80
    // icosphere3             320
    // icosphere4           1,280
    // icosphere5           5,120
    // icosphere6          20,480
    // icosphere7          81,920
    // icosphere8         327,680
    // icosphere9       1,310,720
    // icosphere10      5,242,880
    Entity* cube = root->addChild("cube");
    cube->setPositionAbs(-1.0, 8.0, 0.0);
    cube->setOrientationAbs(0.5, 0.3, 0.2);
    RenderableMesh* cubeMesh = new RenderableMesh(cube, &m_renderer, &m_resources);
    cubeMesh->loadBox(0.5, 0.5, 0.5);
    RigidBody* cubeBody = new RigidBody(cube, &m_physicsWorld);
    cubeBody->init(1.0);
    cubeBody->addBox(0.5, 0.5, 0.5);

    Entity* mesh = root->addChild("model");
    mesh->setPositionRel(1.5f, 5.0f, 0.0f);
    RenderableMesh* renderableMesh = new RenderableMesh(mesh, &m_renderer, &m_resources);
    renderableMesh->loadFromFile("assets/meshes/materialtest.dae");
    RigidBody* meshBody = new RigidBody(mesh, &m_physicsWorld);
    meshBody->init(10.0);
    meshBody->addBox(2, 2, 2);
//     meshBody->addConvexHull("assets/meshes/suzanne-lowpoly.dae", &m_resources);

    Entity* camera = root->addChild("camera");
    camera->setPositionAbs(0.0f, 4.0f, 10.0f);
    camera->pitch(-0.2);
//     camera->lookAt(cube->getPositionAbs(), VECTOR3_UNIT_Y);
    Camera* camComponent = new Camera(camera, CAMERA_PROJECTION, &m_renderer);
    camComponent->setPerspectiveFOV(45.0);
//     RigidBody* cameraBody = new RigidBody(camera);
//     cameraBody->addSphere(1);

    Entity* light1 = root->addChild("light1");
    light1->setPositionAbs(5, 5, 5);
    Light* light1Cmp = new Light(light1, &m_renderer);
    light1Cmp->setDiffuse(1.0, 1.0, 1.0);

//     cout << Terminal::listsToString() << endl;
//     cout << m_sceneManager.sceneGraphToString() << endl;
//     cout << RenderManager::getRenderer().listsToString() << endl;
//     cout << ResourceManager::listsToString() << endl;

    Terminal::executeScript("assets/scripts/2-initialize-scene.txt");

    m_scene.saveToXML("scene.xml");
}

void Demo::bindInputs() {
    cout << "Binding inputs..." << endl;
    Terminal::executeScript("assets/scripts/3-bind-inputs.txt");

    Inputs* inputs = m_device.getInputs();
    inputs->bindInput(INPUT_KEY_RELEASE, "demo quit", SDLK_ESCAPE);
    inputs->bindInput(INPUT_KEY_RELEASE, "demo run commands.txt", SDLK_TAB);
    inputs->bindInput(INPUT_MOUSE_MOTION, "demo on-mouse-motion");

    inputs->bindInput(INPUT_KEY_PRESSED, "cube move-z -5", SDLK_UP);
    inputs->bindInput(INPUT_KEY_PRESSED, "cube move-x -5", SDLK_LEFT);
    inputs->bindInput(INPUT_KEY_PRESSED, "cube move-z  5", SDLK_DOWN);
    inputs->bindInput(INPUT_KEY_PRESSED, "cube move-x  5", SDLK_RIGHT);

    inputs->bindInput(INPUT_KEY_PRESSED, "camera move-z -5", SDLK_w);
    inputs->bindInput(INPUT_KEY_PRESSED, "camera move-x -5", SDLK_a);
    inputs->bindInput(INPUT_KEY_PRESSED, "camera move-z  5", SDLK_s);
    inputs->bindInput(INPUT_KEY_PRESSED, "camera move-x  5", SDLK_d);
    inputs->bindInput(INPUT_KEY_PRESSED, "camera move-y-global  5", SDLK_SPACE);
    inputs->bindInput(INPUT_KEY_PRESSED, "camera move-y-global -5", SDLK_LSHIFT);
    inputs->bindInput(INPUT_MOUSE_BUTTON_RELEASE, "demo fire-cube", 1);
    inputs->bindInput(INPUT_MOUSE_BUTTON_RELEASE, "demo fire-sphere", 3);
}

void Demo::runMainLoop() {
    Uint32 startTime;
    Uint32 deltaTime;

    // test to measure commands performance
//     startTime = SDL_GetTicks();
//     for (size_t i = 0; i < 100000; ++i)
//         Terminal::pushCommand("cube set position-abs 0 15 0");
//     Terminal::processCommandsQueue();
//     cout << SDL_GetTicks() - startTime << " ms" << endl;

    cout << "Entering game loop" << endl;
    m_isRunning = true;
    while (m_isRunning) {
        startTime = SDL_GetTicks();
        m_device.onFrameStart();

        m_physicsWorld.stepSimulation(0.001 * SDL_GetTicks());
        m_device.processEvents(m_isRunning);
        Terminal::processCommandsQueue();

        stringstream ss;
        deltaTime = SDL_GetTicks() - startTime;
        ss << "Shoggoth Engine Demo - CPU:" << setw(3) << deltaTime << " ms - ";

        startTime = SDL_GetTicks();
        m_renderer.draw();
        m_device.swapBuffers();
        deltaTime = SDL_GetTicks() - startTime;
        ss << "GPU:" << setw(3) << deltaTime << " ms (16-40 ideal) - ";

        m_device.onFrameEnd();
        ss << setw(5) << fixed << setprecision(1) << m_device.getFps() << " fps";
        m_device.setTitle(ss.str());
    }
}

void Demo::cmdQuit(const string&) {
    m_isRunning = false;
}

void Demo::cmdRunCommand(const string& arg) {
    Terminal::executeScript(arg);
}

void Demo::cmdPrintEntity(const string& arg) {
    Entity* entity;
    if (m_scene.findEntity(arg, entity))
        cout << *entity << endl;
}

void Demo::cmdOnMouseMotion(const string&) {
    static Command moveXCmd("camera yaw-global");
    static Command moveYCmd("camera pitch");

    mouse_motion_t motion = m_device.getInputs()->getLastMouseMotion();

    float sensitivity = 0.05;
    stringstream ssx;
    ssx << motion.xrel * sensitivity;
    moveXCmd.setArguments(ssx.str());
    Terminal::pushCommand(moveXCmd);

    stringstream ssy;
    ssy << motion.yrel * sensitivity;
    moveYCmd.setArguments(ssy.str());
    Terminal::pushCommand(moveYCmd);
}

void Demo::cmdFireCube(const std::string&) {
    Entity* camera;
    if (m_scene.findEntity("camera", camera)) {
        static size_t n = 0;
        stringstream ss;
        ss << "missile-cube-" << ++n;

        Entity* cube = m_scene.root()->addChild(ss.str());
        Vector3 orientationUnit = VECTOR3_UNIT_Z_NEG.rotate(camera->getOrientationAbs());
        cube->setPositionAbs(camera->getPositionAbs() + orientationUnit);
        cube->setOrientationAbs(camera->getOrientationAbs());

        RenderableMesh* cubeMesh = new RenderableMesh(cube, &m_renderer, &m_resources);
        cubeMesh->loadBox(MISSILE_SIZE, MISSILE_SIZE, MISSILE_SIZE);

        RigidBody* cubeBody = new RigidBody(cube, &m_physicsWorld);
        cubeBody->init(1.0, 0.8);
        cubeBody->addBox(MISSILE_SIZE, MISSILE_SIZE, MISSILE_SIZE);
        cubeBody->setLinearVelocity(orientationUnit * FIRE_SPEED);
    }
}

void Demo::cmdFireSphere(const std::string&) {
    Entity* camera;
    if (m_scene.findEntity("camera", camera)) {
        static size_t n = 0;
        stringstream ss;
        ss << "missile-sphere-" << ++n;

        Entity* sphere = m_scene.root()->addChild(ss.str());
        Vector3 orientationUnit = VECTOR3_UNIT_Z_NEG.rotate(camera->getOrientationAbs());
        sphere->setPositionAbs(camera->getPositionAbs() + orientationUnit);
        sphere->setOrientationAbs(camera->getOrientationAbs());

        RenderableMesh* cubeMesh = new RenderableMesh(sphere, &m_renderer, &m_resources);
        cubeMesh->loadFromFile("assets/meshes/icosphere3.dae");

        RigidBody* cubeBody = new RigidBody(sphere, &m_physicsWorld);
        cubeBody->init(1.0, 0.8);
        cubeBody->addSphere(1.0);
//         cubeBody->addConvexHull("assets/meshes/icosphere1.dae");
        cubeBody->setLinearVelocity(orientationUnit * FIRE_SPEED);
    }
}
