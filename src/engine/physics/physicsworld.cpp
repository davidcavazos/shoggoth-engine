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


#include "engine/physics/physicsworld.hpp"

#include <iostream>
#include <bullet/btBulletDynamicsCommon.h>
#include "engine/kernel/entity.hpp"
#include "engine/physics/rigidbody.hpp"

using namespace std;

const double DEFAULT_MIN_EXPECTED_FRAMERATE = 10.0;

PhysicsWorld::PhysicsWorld(const string& objectName):
    CommandObject(objectName),
    m_maxSubsteps(1),
    m_lastTime(0.0),
    m_broadphase(0),
    m_collisionConfiguration(0),
    m_collisionDispatcher(0),
    m_solver(0),
    m_dynamicsWorld(0),
    m_collisionShapes(),
    m_rigidBodies()
{
    registerCommand("initialize", boost::bind(&PhysicsWorld::cmdInitialize, this, _1));
    registerCommand("shutdown", boost::bind(&PhysicsWorld::cmdShutdown, this, _1));
    registerAttribute("min-expected-framerate", boost::bind(&PhysicsWorld::cmdMinExpectedFramerate, this, _1));
}

PhysicsWorld::~PhysicsWorld() {
    unregisterAllCommands();
    unregisterAllAttributes();
}

void PhysicsWorld::initialize() {
    cout << "Physics simulations done with Bullet Physics" << endl;
    setMinExpectedFramerate(DEFAULT_MIN_EXPECTED_FRAMERATE);

    cout << "Creating dynamics world" << endl;
    m_broadphase = new btDbvtBroadphase;
    m_collisionConfiguration = new btDefaultCollisionConfiguration;
    m_collisionDispatcher = new btCollisionDispatcher(m_collisionConfiguration);
    m_solver = new btSequentialImpulseConstraintSolver;
    m_dynamicsWorld = new btDiscreteDynamicsWorld(m_collisionDispatcher, m_broadphase, m_solver, m_collisionConfiguration);
}

void PhysicsWorld::shutdown() {
    cout << "Destroying rigid bodies" << endl;
    rigid_bodies_map_t::iterator itRig;
    for (itRig = m_rigidBodies.begin(); itRig != m_rigidBodies.end(); ++itRig) {
        m_dynamicsWorld->removeRigidBody(itRig->second);
        delete itRig->second->getMotionState();
        delete itRig->second;
    }

    cout << "Destroying collision shapes" << endl;
    collision_shapes_map_t::iterator itCol;
    for (itCol = m_collisionShapes.begin(); itCol != m_collisionShapes.end(); ++itCol)
        delete itCol->second;

    cout << "Destroying dynamics world" << endl;
    delete m_dynamicsWorld;
    delete m_solver;
    delete m_collisionDispatcher;
    delete m_collisionConfiguration;
    delete m_broadphase;
}

void PhysicsWorld::registerRigidBody(RigidBody* body) {
    m_rigidBodies.insert(std::pair<Entity*, btRigidBody*>(body->entity(), body->bulletRigidBody()));
    m_dynamicsWorld->addRigidBody(body->bulletRigidBody());
}

void PhysicsWorld::unregisterRigidBody(RigidBody* body) {
    m_dynamicsWorld->removeRigidBody(body->bulletRigidBody());
    delete body->bulletRigidBody()->getMotionState();
    m_rigidBodies.erase(body->entity());
}

void PhysicsWorld::stepSimulation(const double currentTimeSeconds) {
    m_dynamicsWorld->stepSimulation(currentTimeSeconds - m_lastTime, m_maxSubsteps, FIXED_TIMESTEP);
    m_lastTime = currentTimeSeconds;

    updateRigidBodies();
}

PhysicsWorld::PhysicsWorld(const PhysicsWorld& rhs):
    CommandObject(rhs.m_objectName),
    m_maxSubsteps(rhs.m_maxSubsteps),
    m_lastTime(rhs.m_lastTime),
    m_broadphase(rhs.m_broadphase),
    m_collisionConfiguration(rhs.m_collisionConfiguration),
    m_collisionDispatcher(rhs.m_collisionDispatcher),
    m_solver(rhs.m_solver),
    m_dynamicsWorld(rhs.m_dynamicsWorld),
    m_collisionShapes(rhs.m_collisionShapes),
    m_rigidBodies(rhs.m_rigidBodies)
{}

PhysicsWorld& PhysicsWorld::operator=(const PhysicsWorld& rhs) {
    if (this == &rhs)
        return *this;
    m_lastTime = rhs.m_lastTime;
    m_broadphase = rhs.m_broadphase;
    m_collisionConfiguration = rhs.m_collisionConfiguration;
    m_collisionDispatcher = rhs.m_collisionDispatcher;
    m_solver = rhs.m_solver;
    m_dynamicsWorld = rhs.m_dynamicsWorld;
    m_collisionShapes = rhs.m_collisionShapes;
    m_rigidBodies = rhs.m_rigidBodies;
    return *this;
}

void PhysicsWorld::updateRigidBodies() {
    rigid_bodies_map_t::const_iterator it;
    btTransform trans;
    btQuaternion rot;
    for (it = m_rigidBodies.begin(); it != m_rigidBodies.end(); ++it) {
        btRigidBody* rigidBody = it->second;

        if (rigidBody->isActive()) {
            Entity* entity = it->first;

            rigidBody->getMotionState()->getWorldTransform(trans);
            btVector3& pos = trans.getOrigin();
            rot = trans.getRotation();

            entity->setPositionAbs(pos.getX(), pos.getY(), pos.getZ());
            entity->setOrientationAbs(rot.getW(), rot.getX(), rot.getY(), rot.getZ());
        }
    }
}



string PhysicsWorld::cmdMinExpectedFramerate(std::deque<std::string>& args) {
    if (args.size() < 1)
        return "Error: too few arguments";
    double minFrameRate = boost::lexical_cast<double>(args[0]);
    setMinExpectedFramerate(minFrameRate);
    return "";
}
