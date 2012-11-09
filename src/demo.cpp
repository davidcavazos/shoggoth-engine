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
#include "testcomponent.hpp"

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
    m_componentFactory(&m_renderer, &m_resources, &m_physicsWorld),
    m_scene(sceneName, rootNodeName, &m_componentFactory, &m_device, &m_renderer, &m_resources, &m_physicsWorld)
{
    registerCommand("quit", boost::bind(&Demo::cmdQuit, this, _1));
    registerCommand("run", boost::bind(&Demo::cmdRunCommand, this, _1));
    registerCommand("print-entity", boost::bind(&Demo::cmdPrint, this, _1));
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
    Terminal::executeScript("assets/scripts/2-initialize-scene.txt");

//     // model            faces (triangles)
//     // icosphere1              20
//     // icosphere2              80
//     // icosphere3             320
//     // icosphere4           1,280
//     // icosphere5           5,120
//     // icosphere6          20,480
//     // icosphere7          81,920
//     // icosphere8         327,680
//     // icosphere9       1,310,720
//     // icosphere10      5,242,880
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

void Demo::cmdPrint(const string& arg) {
    Entity* entity;
    if (m_scene.findEntity(arg, entity)) {
        TestComponent* test = dynamic_cast<TestComponent*>(entity->component(COMPONENT_TESTCOMPONENT));
        cout << "Health: " << test->getHealth() << endl;
    }
}

void Demo::cmdOnMouseMotion(const string&) {
    float sensitivity = 0.05;
    mouse_motion_t motion = m_device.getInputs()->getLastMouseMotion();

    stringstream ssx;
    ssx << "camera yaw-global " << motion.xrel * sensitivity;
    Terminal::pushCommand(ssx.str());

    stringstream ssy;
    ssy << "camera pitch " << motion.yrel * sensitivity;
    Terminal::pushCommand(ssy.str());
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

        RigidBody* cubeBody = new RigidBody(cube, &m_resources, &m_physicsWorld);
        cubeBody->addBox(1.0, MISSILE_SIZE, MISSILE_SIZE, MISSILE_SIZE);
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

        RigidBody* cubeBody = new RigidBody(sphere, &m_resources, &m_physicsWorld);
        cubeBody->addSphere(1.0, 1.0);
//         cubeBody->addConvexHull("assets/meshes/icosphere1.dae");
        cubeBody->setLinearVelocity(orientationUnit * FIRE_SPEED);
    }
}
