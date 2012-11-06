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


#ifndef RIGIDBODY_HPP
#define RIGIDBODY_HPP

#include "engine/kernel/component.hpp"
#include "engine/kernel/vector3.hpp"

const std::string COMPONENT_RIGIDBODY = "rigidbody";

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
