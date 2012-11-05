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


#ifndef RIGIDBODY_HPP
#define RIGIDBODY_HPP

#include "engine/kernel/component.hpp"
#include "engine/kernel/vector3.hpp"

const std::string COLLISION_SHAPE_BOX = "#box";
const std::string COLLISION_SHAPE_SPHERE = "#sphere";
const std::string COLLISION_SHAPE_CYLINDER = "#cylinder";
const std::string COLLISION_SHAPE_CAPSULE = "#capsule";
const std::string COLLISION_SHAPE_CONE = "#cone";
const std::string COLLISION_SHAPE_CONVEX = "#convex";
const std::string COLLISION_SHAPE_CONCAVE = "#concave";

class PhysicsWorld;
class Resources;
class btRigidBody;
class btCollisionShape;
class btDefaultMotionState;
class btVector3;

typedef enum {
    SHAPE_UP_X,
    SHAPE_UP_Y,
    SHAPE_UP_Z
} shape_up_t;

class RigidBody: public Component {
public:
    RigidBody(Entity* const entity, PhysicsWorld* physicsWorld);
    ~RigidBody();

    Vector3 getPosition();
    Quaternion getOrientation();
    const std::string& getShapeId() const;
    double getMass() const;
    double getLinearDamping() const;
    double getAngularDamping() const;
    double getFriction() const;
    double getRollingFriction() const;
    double getRestitution() const;
    double getLinearSleepingThreshold() const;
    double getAngularSleepingThreshold() const;
    Vector3 getLinearFactor() const;
    Vector3 getLinearVelocity();
    Vector3 getAngularFactor() const;
    Vector3 getAngularVelocity();
    Vector3 getGravity() const;

    void activate(const bool forceActivate = true);
    void setTransform(const Vector3& position, const Quaternion& orientation);
    void setMass(const double mass);
    void setDamping(const double linear, const double angular);
    void setFriction(const double friction);
    void setRollingFriction(const double rollingFriction);
    void setRestitution(const double restitution);
    void setSleepingThresholds(const double linear, const double angular);
    void setLinearFactor(const Vector3& linearFactor);
    void setLinearVelocity(const Vector3& linearVelocity);
    void setAngularFactor(const Vector3& angularFactor);
    void setAngularVelocity(const Vector3& angularVelocity);
    void setGravity(const Vector3& gravity);

    void addSphere(const double mass, const double radius);
    void addBox(const double mass, const double lengthX, const double lengthY, const double lengthZ);
    void addCylinder(const double mass, const double radius, const double height);
    void addCapsule(const double mass, const double radius, const double height);
    void addCone(const double mass, const double radius, const double height);
    void addConvexHull(const double mass, const std::string& fileName, Resources* resources);
    void addConcaveHull(const double mass, const std::string& fileName, Resources* resources);

private:
    PhysicsWorld* m_physicsWorld;
    std::string m_shapeId;
    btRigidBody* m_rigidBody;
    double m_mass;

    RigidBody(const RigidBody& rhs);
    RigidBody& operator=(const RigidBody&);

    void addRigidBody(const double mass, btCollisionShape* shape);

    void cmdIsActive(const std::string& arg);
    void cmdMass(const std::string& arg);
    void cmdDamping(const std::string& arg);
    void cmdFriction(const std::string& arg);
    void cmdRollingFriction(const std::string& arg);
    void cmdRestitution(const std::string& arg);
    void cmdSleepingThresholds(const std::string& arg);
    void cmdLinearFactor(const std::string& arg);
    void cmdLinearVelocity(const std::string& arg);
    void cmdAngularFactor(const std::string& arg);
    void cmdAngularVelocity(const std::string& arg);
    void cmdGravity(const std::string& arg);
};

#endif // RIGIDBODY_HPP
