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


#ifndef PHYSICSWORLD_HPP
#define PHYSICSWORLD_HPP

#include <boost/unordered_map.hpp>
#include "engine/kernel/commandobject.hpp"

const double FIXED_TIMESTEP = 1.0 / 60.0;

class RigidBody;
class Entity;

// Bullet classes needed for world simulation
class btBroadphaseInterface;
class btDefaultCollisionConfiguration;
class btCollisionDispatcher;
class btSequentialImpulseConstraintSolver;
class btDiscreteDynamicsWorld;
class btCollisionShape;
class btRigidBody;

class PhysicsWorld: public CommandObject {
public:
    friend class RigidBody;

    PhysicsWorld(const std::string& objectName);
    ~PhysicsWorld();

    void initialize();
    void shutdown();
    void setMinExpectedFramerate(const double minExpectedFramerate);
    void stepSimulation(const double currentTimeSeconds);

private:
    PhysicsWorld(const PhysicsWorld& rhs);

    PhysicsWorld& operator=(const PhysicsWorld& rhs);

    void updateRigidBodies();

    int m_maxSubsteps;
    double m_lastTime;
    typedef boost::unordered_map<std::string, btCollisionShape*> collision_shapes_map_t;
    typedef boost::unordered_map<Entity*, btRigidBody*> rigid_bodies_map_t;
    btBroadphaseInterface* m_broadphase;
    btDefaultCollisionConfiguration* m_collisionConfiguration;
    btCollisionDispatcher* m_collisionDispatcher;
    btSequentialImpulseConstraintSolver* m_solver;
    btDiscreteDynamicsWorld* m_dynamicsWorld;
    collision_shapes_map_t m_collisionShapes;
    rigid_bodies_map_t m_rigidBodies;

    std::string cmdInitialize(std::deque<std::string>&);
    std::string cmdShutdown(std::deque<std::string>&);
    std::string cmdMinExpectedFramerate(std::deque<std::string>& args);
};


inline void PhysicsWorld::setMinExpectedFramerate(const double minExpectedFramerate) {
    double minTimestep = 1.0 / minExpectedFramerate;
    m_maxSubsteps = minTimestep / FIXED_TIMESTEP + 2;
}



inline std::string PhysicsWorld::cmdInitialize(std::deque<std::string>&) {
    initialize();
    return "";
}

inline std::string PhysicsWorld::cmdShutdown(std::deque<std::string>&) {
    shutdown();
    return "";
}

#endif // PHYSICSWORLD_HPP
