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


#include "engine/physics/rigidbody.hpp"

#include <string>
#include <iostream>
#include <sstream>
#include <bullet/btBulletDynamicsCommon.h>
#include <bullet/BulletCollision/CollisionShapes/btShapeHull.h>
#include "engine/physics/physicsworld.hpp"
#include "engine/kernel/entity.hpp"
#include "engine/resources/model.hpp"
#include "engine/resources/resources.hpp"

using namespace std;

btDefaultMotionState* getMotionState(const Entity* entity);
btVector3 v3(const Vector3& v);
Vector3 v3(const btVector3& v);
btQuaternion quat(const Quaternion& q);
Quaternion quat(const btQuaternion& q);



RigidBody::RigidBody(Entity* const entity, PhysicsWorld* physicsWorld):
    Component(COMPONENT_RIGIDBODY, entity),
    m_physicsWorld(physicsWorld),
    m_shapeId(""),
    m_rigidBody(0),
    m_mass(0.0)
{
    m_entity->registerAttribute("mass", boost::bind(&RigidBody::cmdMass, this, _1));
    m_entity->registerAttribute("damping", boost::bind(&RigidBody::cmdDamping, this, _1));
    m_entity->registerAttribute("friction", boost::bind(&RigidBody::cmdFriction, this, _1));
    m_entity->registerAttribute("rolling-friction", boost::bind(&RigidBody::cmdRollingFriction, this, _1));
    m_entity->registerAttribute("restitution", boost::bind(&RigidBody::cmdRestitution, this, _1));
    m_entity->registerAttribute("sleeping-thresholds", boost::bind(&RigidBody::cmdSleepingThresholds, this, _1));
    m_entity->registerAttribute("linear-factor", boost::bind(&RigidBody::cmdLinearFactor, this, _1));
    m_entity->registerAttribute("linear-velocity", boost::bind(&RigidBody::cmdLinearVelocity, this, _1));
    m_entity->registerAttribute("angular-factor", boost::bind(&RigidBody::cmdAngularFactor, this, _1));
    m_entity->registerAttribute("angular-velocity", boost::bind(&RigidBody::cmdAngularVelocity, this, _1));
    m_entity->registerAttribute("gravity", boost::bind(&RigidBody::cmdGravity, this, _1));
}

RigidBody::~RigidBody() {
    m_physicsWorld->m_dynamicsWorld->removeRigidBody(m_rigidBody);
    m_physicsWorld->m_rigidBodies.erase(m_entity);
    m_entity->unregisterAttribute("gravity");
    m_entity->unregisterAttribute("angular-velocity");
    m_entity->unregisterAttribute("angular-factor");
    m_entity->unregisterAttribute("linear-velocity");
    m_entity->unregisterAttribute("linear-factor");
    m_entity->unregisterAttribute("sleeping-thresholds");
    m_entity->unregisterAttribute("restitution");
    m_entity->unregisterAttribute("rolling-friction");
    m_entity->unregisterAttribute("friction");
    m_entity->unregisterAttribute("damping");
    m_entity->unregisterAttribute("mass");
}


Vector3 RigidBody::getPosition() {
    return v3(m_rigidBody->getCenterOfMassPosition());
}

Quaternion RigidBody::getOrientation() {
    return quat(m_rigidBody->getOrientation());
}

const std::string& RigidBody::getShapeId() const {
    return m_shapeId;
}

double RigidBody::getMass() const {
    return m_mass;
}

double RigidBody::getLinearDamping() const {
    return m_rigidBody->getLinearDamping();
}

double RigidBody::getAngularDamping() const {
    return m_rigidBody->getAngularDamping();
}

double RigidBody::getFriction() const {
    return m_rigidBody->getFriction();
}

double RigidBody::getRollingFriction() const {
    return m_rigidBody->getRollingFriction();
}

double RigidBody::getRestitution() const {
    return m_rigidBody->getRestitution();
}

double RigidBody::getLinearSleepingThreshold() const {
    return m_rigidBody->getLinearSleepingThreshold();
}

double RigidBody::getAngularSleepingThreshold() const {
    return m_rigidBody->getAngularSleepingThreshold();
}

Vector3 RigidBody::getLinearFactor() const {
    return v3(m_rigidBody->getLinearFactor());
}

Vector3 RigidBody::getLinearVelocity() {
    return v3(m_rigidBody->getLinearVelocity());
}

Vector3 RigidBody::getAngularFactor() const {
    return v3(m_rigidBody->getAngularFactor());
}

Vector3 RigidBody::getAngularVelocity() {
    return v3(m_rigidBody->getAngularVelocity());
}

Vector3 RigidBody::getGravity() const {
    return v3(m_rigidBody->getGravity());
}



void RigidBody::activate(const bool forceActivate) {
    m_rigidBody->activate(forceActivate);
}

void RigidBody::setTransform(const Vector3& position, const Quaternion& orientation) {
    m_rigidBody->setWorldTransform(btTransform(quat(orientation), v3(position)));
}

void RigidBody::setMass(const double mass) {
    m_mass = mass;
    btVector3 inertia;
    m_rigidBody->getCollisionShape()->calculateLocalInertia(m_mass, inertia);
    m_rigidBody->setMassProps(mass, inertia);
}

void RigidBody::setDamping(const double linear, const double angular) {
    m_rigidBody->setDamping(linear, angular);
}

void RigidBody::setFriction(const double friction) {
    m_rigidBody->setFriction(friction);
}

void RigidBody::setRollingFriction(const double rollingFriction) {
    m_rigidBody->setRollingFriction(rollingFriction);
}

void RigidBody::setRestitution(const double restitution) {
    m_rigidBody->setRestitution(restitution);
}

void RigidBody::setSleepingThresholds(const double linear, const double angular) {
    m_rigidBody->setSleepingThresholds(linear, angular);
}

void RigidBody::setLinearFactor(const Vector3& linearFactor) {
    m_rigidBody->setLinearFactor(v3(linearFactor));
}

void RigidBody::setLinearVelocity(const Vector3& linearVelocity) {
    m_rigidBody->setLinearVelocity(v3(linearVelocity));
}

void RigidBody::setAngularFactor(const Vector3& angularFactor) {
    m_rigidBody->setAngularFactor(v3(angularFactor));
}

void RigidBody::setAngularVelocity(const Vector3& angularVelocity) {
    m_rigidBody->setAngularVelocity(v3(angularVelocity));
}

void RigidBody::setGravity(const Vector3& gravity) {
    m_rigidBody->setGravity(v3(gravity));
}

void RigidBody::addSphere(const double mass, const double radius) {
    stringstream ss;
    ss << COLLISION_SHAPE_BOX << " " << radius;
    m_shapeId = ss.str();

    PhysicsWorld::collision_shapes_map_t& collisionShapes = m_physicsWorld->m_collisionShapes;
    PhysicsWorld::collision_shapes_map_t::const_iterator it;

    btCollisionShape* shape;
    it = collisionShapes.find(m_shapeId);
    if (it == collisionShapes.end()) {
        shape = new btSphereShape(radius);
        collisionShapes.insert(std::pair<string, btCollisionShape*>(m_shapeId, shape));
    }
    else
        shape = it->second;
    addRigidBody(mass, shape);
}

void RigidBody::addBox(const double mass, const double lengthX, const double lengthY, const double lengthZ) {
    stringstream ss;
    ss << COLLISION_SHAPE_BOX << " " << lengthX << " " << lengthY << " " << lengthZ;
    m_shapeId = ss.str();

    PhysicsWorld::collision_shapes_map_t& collisionShapes = m_physicsWorld->m_collisionShapes;
    PhysicsWorld::collision_shapes_map_t::const_iterator it;

    btCollisionShape* shape;
    it = collisionShapes.find(m_shapeId);
    if (it == collisionShapes.end()) {
        shape = new btBoxShape(btVector3(lengthX * 0.5, lengthY * 0.5, lengthZ * 0.5));
        collisionShapes.insert(std::pair<string, btCollisionShape*>(m_shapeId, shape));
    }
    else
        shape = it->second;
    addRigidBody(mass, shape);
}

void RigidBody::addCylinder(const double mass, const double radius, const double height) {
    stringstream ss;
    ss << COLLISION_SHAPE_CYLINDER << " " << radius << " " << height;
    m_shapeId = ss.str();

    PhysicsWorld::collision_shapes_map_t& collisionShapes = m_physicsWorld->m_collisionShapes;
    PhysicsWorld::collision_shapes_map_t::const_iterator it;

    btCollisionShape* shape;
    it = collisionShapes.find(m_shapeId);
    if (it == collisionShapes.end()) {
        shape = new btCylinderShape(btVector3(radius, height, radius));
        collisionShapes.insert(std::pair<string, btCollisionShape*>(m_shapeId, shape));
    }
    else
        shape = it->second;
    addRigidBody(mass, shape);
}

void RigidBody::addCapsule(const double mass, const double radius, const double height) {
    stringstream ss;
    ss << COLLISION_SHAPE_CAPSULE << " " << radius << " " << height;
    m_shapeId = ss.str();

    PhysicsWorld::collision_shapes_map_t& collisionShapes = m_physicsWorld->m_collisionShapes;
    PhysicsWorld::collision_shapes_map_t::const_iterator it;

    btCollisionShape* shape;
    it = collisionShapes.find(m_shapeId);
    if (it == collisionShapes.end()) {
        shape = new btCapsuleShape(radius, height);
        collisionShapes.insert(std::pair<string, btCollisionShape*>(m_shapeId, shape));
    }
    else
        shape = it->second;
    addRigidBody(mass, shape);
}

void RigidBody::addCone(const double mass, const double radius, const double height) {
    stringstream ss;
    ss << COLLISION_SHAPE_CONE << " " << radius << " " << height;
    m_shapeId = ss.str();

    PhysicsWorld::collision_shapes_map_t& collisionShapes = m_physicsWorld->m_collisionShapes;
    PhysicsWorld::collision_shapes_map_t::const_iterator it;

    btCollisionShape* shape;
    it = collisionShapes.find(m_shapeId);
    if (it == collisionShapes.end()) {
        shape = new btConeShape(radius, height);
        collisionShapes.insert(std::pair<string, btCollisionShape*>(m_shapeId, shape));
    }
    else
        shape = it->second;
    addRigidBody(mass, shape);
}

void RigidBody::addConvexHull(const double mass, const string& fileName, Resources* resources) {
    stringstream ss;
    ss << COLLISION_SHAPE_CONVEX << " " << fileName;
    m_shapeId = ss.str();

    PhysicsWorld::collision_shapes_map_t& collisionShapes = m_physicsWorld->m_collisionShapes;
    PhysicsWorld::collision_shapes_map_t::const_iterator it;

    btCollisionShape* shape;
    it = collisionShapes.find(m_shapeId);
    if (it == collisionShapes.end()) {
        // build original mesh from file
        Model* model = resources->generateModelFromFile(fileName);
        vector<float> points;
        for (size_t n = 0; n < model->getTotalMeshes(); ++n) {
            points.reserve(points.size() + model->mesh(n)->getVerticesSize());
            for (size_t i = 0; i < model->mesh(n)->getVerticesSize(); ++i)
                points.push_back(model->mesh(n)->getVertices()[i]);
        }
        btConvexShape* originalConvexShape = new btConvexHullShape(&points[0], points.size(), sizeof(float) * 3);
        points.clear();

        // convert to low polygon hull
        btShapeHull* hull = new btShapeHull(originalConvexShape);
        btScalar margin = originalConvexShape->getMargin();
        hull->buildHull(margin);

        shape = new btConvexHullShape(&hull->getVertexPointer()->getX(), hull->numVertices());
        collisionShapes.insert(std::pair<string, btCollisionShape*>(m_shapeId, shape));

        delete originalConvexShape;
        delete hull;
    }
    else
        shape = it->second;
    addRigidBody(mass, shape);
}

void RigidBody::addConcaveHull(const double mass, const string& fileName, Resources* resources) {
    stringstream ss;
    ss << COLLISION_SHAPE_CONCAVE << " " << fileName;
    m_shapeId = ss.str();

    PhysicsWorld::collision_shapes_map_t& collisionShapes = m_physicsWorld->m_collisionShapes;
    PhysicsWorld::collision_shapes_map_t::const_iterator it;

    btCollisionShape* shape;
    it = collisionShapes.find(m_shapeId);
    if (it == collisionShapes.end()) {
        // build mesh from file
        Model* model = resources->generateModelFromFile(fileName);
        btTriangleIndexVertexArray* triangles = new btTriangleIndexVertexArray();
        for (size_t n = 0; n < model->getTotalMeshes(); ++n) {
            btIndexedMesh indexedMesh;
            indexedMesh.m_numTriangles = model->mesh(n)->getIndicesSize() / 3;
            indexedMesh.m_triangleIndexBase = (const unsigned char*)model->mesh(n)->getIndicesPtr();
            indexedMesh.m_triangleIndexStride = sizeof(unsigned int);
            indexedMesh.m_numVertices = model->mesh(n)->getVerticesSize();
            indexedMesh.m_vertexBase = (const unsigned char*)model->mesh(n)->getVerticesPtr();
            indexedMesh.m_vertexStride = sizeof(float);
            triangles->addIndexedMesh(indexedMesh);
        }

        shape = new btBvhTriangleMeshShape(triangles, true, true);
        collisionShapes.insert(std::pair<string, btCollisionShape*>(m_shapeId, shape));

        delete triangles;
    }
    else
        shape = it->second;
    addRigidBody(mass, shape);
}



RigidBody::RigidBody(const RigidBody& rhs):
    Component(COMPONENT_RIGIDBODY, rhs.m_entity),
    m_physicsWorld(rhs.m_physicsWorld),
    m_shapeId(rhs.m_shapeId),
    m_rigidBody(rhs.m_rigidBody),
    m_mass(rhs.m_mass)
{
    cerr << "Error: RigidBody copy constructor should not be called!" << endl;
}

RigidBody& RigidBody::operator=(const RigidBody&) {
    cerr << "Error: RigidBody assignment operator should not be called!" << endl;
    return *this;
}

void RigidBody::addRigidBody(const double mass, btCollisionShape* shape) {
    m_mass = mass;
    btVector3 inertia;
    shape->calculateLocalInertia(m_mass, inertia);
    btDefaultMotionState* motion = getMotionState(m_entity);
    m_rigidBody = new btRigidBody(m_mass, motion, shape, inertia);
    m_physicsWorld->m_rigidBodies.insert(
        std::pair<Entity*, btRigidBody*>(m_entity, m_rigidBody));
    m_physicsWorld->m_dynamicsWorld->addRigidBody(m_rigidBody);
}


void RigidBody::cmdIsActive(const string& arg) {
    bool isActive;
    stringstream ss(arg);
    ss >> isActive;
    activate(isActive);
}

void RigidBody::cmdMass(const string& arg) {
    double mass;
    stringstream ss(arg);
    ss >> mass;
    setMass(mass);
}

void RigidBody::cmdDamping(const string& arg) {
    double linear, angular;
    stringstream ss(arg);
    ss >> linear >> angular;
    setDamping(linear, angular);
}

void RigidBody::cmdFriction(const string& arg) {
    double friction;
    stringstream ss(arg);
    ss >> friction;
    setFriction(friction);
}

void RigidBody::cmdRollingFriction(const string& arg) {
    double rollingFriction;
    stringstream ss(arg);
    ss >> rollingFriction;
    setRollingFriction(rollingFriction);
}

void RigidBody::cmdRestitution(const string& arg) {
    double restitution;
    stringstream ss(arg);
    ss >> restitution;
    setRestitution(restitution);
}

void RigidBody::cmdSleepingThresholds(const string& arg) {
    double linear, angular;
    stringstream ss(arg);
    ss >> linear >> angular;
    setSleepingThresholds(linear, angular);
}

void RigidBody::cmdLinearFactor(const string& arg) {
    double x, y, z;
    stringstream ss(arg);
    ss >> x >> y >> z;
    setLinearFactor(Vector3(x, y, z));
}

void RigidBody::cmdLinearVelocity(const string& arg) {
    double x, y, z;
    stringstream ss(arg);
    ss >> x >> y >> z;
    setLinearVelocity(Vector3(x, y, z));
}

void RigidBody::cmdAngularFactor(const string& arg) {
    double x, y, z;
    stringstream ss(arg);
    ss >> x >> y >> z;
    setAngularFactor(Vector3(x, y, z));
}

void RigidBody::cmdAngularVelocity(const string& arg) {
    double x, y, z;
    stringstream ss(arg);
    ss >> x >> y >> z;
    setAngularVelocity(Vector3(x, y, z));
}

void RigidBody::cmdGravity(const string& arg) {
    double x, y, z;
    stringstream ss(arg);
    ss >> x >> y >> z;
    setGravity(Vector3(x, y, z));
}



btDefaultMotionState* getMotionState(const Entity* entity) {
    const Quaternion& rot = entity->getOrientationAbs();
    const Vector3& pos = entity->getPositionAbs();
    btDefaultMotionState* motion =
    new btDefaultMotionState(btTransform(
        btQuaternion(rot.getX(), rot.getY(), rot.getZ(), rot.getW()),
        btVector3(pos.getX(), pos.getY(), pos.getZ()))
    );
    return motion;
}

btVector3 v3(const Vector3& v) {
    return btVector3(v.getX(), v.getY(), v.getZ());
}

Vector3 v3(const btVector3& v) {
    return Vector3(v.getX(), v.getY(), v.getZ());
}

btQuaternion quat(const Quaternion& q) {
    return btQuaternion(q.getX(), q.getY(), q.getZ(), q.getW());
}

Quaternion quat(const btQuaternion& q) {
    return Quaternion(q.getX(), q.getY(), q.getZ(), q.getW());
}
