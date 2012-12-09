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
#include "shoggoth-engine/kernel/entity.hpp"
#include "shoggoth-engine/kernel/terminal.hpp"
#include "shoggoth-engine/renderer/renderablemesh.hpp"
#include "shoggoth-engine/renderer/camera.hpp"
#include "shoggoth-engine/renderer/light.hpp"
#include <shoggoth-engine/renderer/opengl.hpp>
#include "shoggoth-engine/physics/rigidbody.hpp"
#include "testcomponent.hpp"

using namespace std;

const unsigned int FRAMERATE_LIMIT = 60;
const unsigned int MILLISECONDS_LIMIT = 1000 / FRAMERATE_LIMIT;

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

    m_device.setResolution(800, 600);
    OpenGL::setTextureFilteringMode(TEXTURE_FILTERING_ANISOTROPIC);
    OpenGL::setAnisotropy(4.0f);
}

Demo::~Demo() {
}

void Demo::loadScene() {
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

    cout << "Loading scene..." << endl;
    m_scene.loadFromXML("scene.xml");
}

void Demo::bindInputs() {
    cout << "Binding inputs..." << endl;

    Inputs* inputs = m_device.getInputs();
    inputs->bindInput(INPUT_KEY_RELEASE, "demo quit", SDLK_ESCAPE);
    inputs->bindInput(INPUT_KEY_RELEASE, "demo run commands.txt", SDLK_TAB);
    inputs->bindInput(INPUT_MOUSE_MOTION, "demo on-mouse-motion");

    inputs->bindInput(INPUT_KEY_PRESSED, "cube move-z -5", SDLK_UP);
    inputs->bindInput(INPUT_KEY_PRESSED, "cube move-x -5", SDLK_LEFT);
    inputs->bindInput(INPUT_KEY_PRESSED, "cube move-z  5", SDLK_DOWN);
    inputs->bindInput(INPUT_KEY_PRESSED, "cube move-x  5", SDLK_RIGHT);

    inputs->bindInput(INPUT_KEY_PRESSED, "body move-z -5", SDLK_w);
    inputs->bindInput(INPUT_KEY_PRESSED, "body move-x -5", SDLK_a);
    inputs->bindInput(INPUT_KEY_PRESSED, "body move-z  5", SDLK_s);
    inputs->bindInput(INPUT_KEY_PRESSED, "body move-x  5", SDLK_d);
    inputs->bindInput(INPUT_KEY_PRESSED, "body move-y-global  5", SDLK_SPACE);
    inputs->bindInput(INPUT_KEY_PRESSED, "body move-y-global -5", SDLK_LSHIFT);
    inputs->bindInput(INPUT_MOUSE_BUTTON_RELEASE, "demo fire-cube", 1);
    inputs->bindInput(INPUT_MOUSE_BUTTON_RELEASE, "demo fire-sphere", 3);
}

void Demo::runMainLoop() {
    Uint32 startTime;
    Uint32 deltaTime;

    // test to measure commands performance
//     startTime = SDL_GetTicks();
//     for (size_t i = 0; i < 100000; ++i)
//         Terminal::pushCommand("cube set position-abs 1 15 3");
//     Terminal::processCommandsQueue();
//     cout << SDL_GetTicks() - startTime << " ms" << endl;

    cout << endl;
    cout << "Entering main loop" << endl;
    m_isRunning = true;
    while (m_isRunning) {
        startTime = SDL_GetTicks();
        m_device.onFrameStart();

        // update
        m_physicsWorld.stepSimulation(0.001 * SDL_GetTicks());
        m_device.processEvents(m_isRunning);
        cout << Terminal::processCommandsQueue();

        // measure CPU load
        stringstream ss;
        deltaTime = SDL_GetTicks() - startTime;
        ss << "Shoggoth Engine Demo - CPU:" << setw(3) << deltaTime << " ms - ";

        // draw and measure GPU load
        startTime = SDL_GetTicks();
        m_renderer.draw();
        deltaTime = SDL_GetTicks() - startTime;
        ss << "GPU:" << setw(3) << deltaTime << " ms (16-40 ideal) - ";

        // framerate cap
        deltaTime = SDL_GetTicks() - startTime;
        if (MILLISECONDS_LIMIT > deltaTime)
            SDL_Delay(MILLISECONDS_LIMIT - deltaTime);

        // show framerate
        m_device.onFrameEnd();
        ss << setw(5) << fixed << setprecision(1) << m_device.getFps() << " fps";
        m_device.setTitle(ss.str());
    }
    cout << "Ending main loop" << endl;
    cout << endl;
}

string Demo::cmdQuit(std::deque<std::string>&) {
    m_isRunning = false;
    return "";
}

string Demo::cmdRunCommand(std::deque<std::string>& args) {
    if (args.size() < 1)
        return "Error: too few arguments";
    cout << Terminal::runScript(args[0]);
    return "";
}

string Demo::cmdPrint(std::deque<std::string>& args) {
    if (args.size() < 1)
        return "Error: too few arguments";
    Entity* entity;
    if (m_scene.findEntity(args[0], entity)) {
        TestComponent* test = dynamic_cast<TestComponent*>(entity->component(COMPONENT_TESTCOMPONENT));
        return string("Health: ") + boost::lexical_cast<string>(test->getHealth());
    }
    return "";
}

string Demo::cmdOnMouseMotion(std::deque<std::string>&) {
    float sensitivity = 0.05f;
    mouse_motion_t motion = m_device.getInputs()->getLastMouseMotion();

    string x = string("body yaw-global ") + boost::lexical_cast<string>(float(motion.xrel) * sensitivity);
    Terminal::pushCommand(x);

    string y = string("camera1 pitch ") + boost::lexical_cast<string>(float(motion.yrel) * sensitivity);
    Terminal::pushCommand(y);
    return "";
}

string Demo::cmdFireCube(std::deque<std::string>&) {
    Entity* camera;
    if (m_scene.findEntity("camera1", camera)) {
        static size_t n = 0;
        string name = string("missile-cube-") + boost::lexical_cast<string>(++n);

        Entity* cube = m_scene.root()->addChild(name);
        Vector3 orientationUnit = VECTOR3_UNIT_Z_NEG.rotate(camera->getOrientationAbs());
        cube->setPositionAbs(camera->getPositionAbs() + orientationUnit);
        cube->setOrientationAbs(camera->getOrientationAbs());

        RenderableMesh* cubeMesh = new RenderableMesh(cube, &m_renderer, &m_resources);
        cubeMesh->loadBox(MISSILE_SIZE, MISSILE_SIZE, MISSILE_SIZE);

        RigidBody* cubeBody = new RigidBody(cube, &m_resources, &m_physicsWorld);
        cubeBody->addBox(1.0, MISSILE_SIZE, MISSILE_SIZE, MISSILE_SIZE);
        cubeBody->setLinearVelocity(orientationUnit * FIRE_SPEED);
    }
    return "";
}

string Demo::cmdFireSphere(std::deque<std::string>&) {
    Entity* camera;
    if (m_scene.findEntity("camera1", camera)) {
        static size_t n = 0;
        string name = string("missile-model-") + boost::lexical_cast<string>(++n);

        Entity* model = m_scene.root()->addChild(name);
        Vector3 orientationUnit = VECTOR3_UNIT_Z_NEG.rotate(camera->getOrientationAbs());
        model->setPositionAbs(camera->getPositionAbs() + orientationUnit);
        model->setOrientationAbs(camera->getOrientationAbs());

        string modelName = "assets/meshes/materialtest.dae";
        RenderableMesh* cubeMesh = new RenderableMesh(model, &m_renderer, &m_resources);
        cubeMesh->loadFromFile(modelName);

        RigidBody* cubeBody = new RigidBody(model, &m_resources, &m_physicsWorld);
        cubeBody->addConvexHull(1.0, modelName);
        cubeBody->setLinearVelocity(orientationUnit * FIRE_SPEED);
    }
    return "";
}
