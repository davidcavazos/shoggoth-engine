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

    void cmdInitialize(const std::string&);
    void cmdShutdown(const std::string&);
    void cmdMinExpectedFramerate(const std::string& arg);
};


inline void PhysicsWorld::setMinExpectedFramerate(const double minExpectedFramerate) {
    double minTimestep = 1.0 / minExpectedFramerate;
    m_maxSubsteps = minTimestep / FIXED_TIMESTEP + 2;
}



inline void PhysicsWorld::cmdInitialize(const std::string&) {
    initialize();
}

inline void PhysicsWorld::cmdShutdown(const std::string&) {
    shutdown();
}

#endif // PHYSICSWORLD_HPP
