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


#include "shoggoth-engine/physics/rigidbody.hpp"

#include <string>
#include <iostream>
#include <sstream>
#include <bullet/btBulletDynamicsCommon.h>
#include <bullet/BulletCollision/CollisionShapes/btShapeHull.h>
#include "shoggoth-engine/linearmath/transform.hpp"
#include "shoggoth-engine/kernel/entity.hpp"
#include "shoggoth-engine/resources/model.hpp"
#include "shoggoth-engine/resources/resources.hpp"
#include "shoggoth-engine/physics/physicsworld.hpp"

using namespace std;
using namespace boost::property_tree;

const string XML_RIGIDBODY_MASS = "mass";
const string XML_RIGIDBODY_COLLISIONSHAPE = "collisionshape";
const string XML_RIGIDBODY_DAMPING = "damping";
const string XML_RIGIDBODY_FRICTION = "friction";
const string XML_RIGIDBODY_ROLLINGFRICTION = "rollingfriction";
const string XML_RIGIDBODY_RESTITUTION = "restitution";
const string XML_RIGIDBODY_SLEEPINGTHRESHOLDS = "sleepingthresholds";
const string XML_RIGIDBODY_LINEARFACTOR = "linearfactor";
const string XML_RIGIDBODY_LINEARVELOCITY = "linearvelocity";
const string XML_RIGIDBODY_ANGULARFACTOR = "angularfactor";
const string XML_RIGIDBODY_ANGULARVELOCITY = "angularvelocity";
const string XML_RIGIDBODY_GRAVITY = "gravity";



RigidBody::RigidBody(Entity* const _entity, Resources* resources, PhysicsWorld* physicsWorld):
    Component(COMPONENT_RIGIDBODY, _entity),
    m_resources(resources),
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
    m_physicsWorld->unregisterRigidBody(this);
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
    return vect(m_rigidBody->getCenterOfMassPosition());
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
    return vect(m_rigidBody->getLinearFactor());
}

Vector3 RigidBody::getLinearVelocity() const {
    return vect(m_rigidBody->getLinearVelocity());
}

Vector3 RigidBody::getAngularFactor() const {
    return vect(m_rigidBody->getAngularFactor());
}

Vector3 RigidBody::getAngularVelocity() const {
    return vect(m_rigidBody->getAngularVelocity());
}

Vector3 RigidBody::getGravity() const {
    return vect(m_rigidBody->getGravity());
}



void RigidBody::activate(const bool forceActivate) {
    m_rigidBody->activate(forceActivate);
}

void RigidBody::setTransform(const Vector3& position, const Quaternion& orientation) {
    m_rigidBody->setWorldTransform(btTransform(quat(orientation), vect(position)));
}

void RigidBody::setMass(const double mass) {
    m_mass = mass;
    btVector3 inertia;
    m_rigidBody->getCollisionShape()->calculateLocalInertia(btScalar(m_mass), inertia);
    m_rigidBody->setMassProps(btScalar(m_mass), inertia);
}

void RigidBody::setDamping(const double linear, const double angular) {
    m_rigidBody->setDamping(btScalar(linear), btScalar(angular));
}

void RigidBody::setFriction(const double friction) {
    m_rigidBody->setFriction(btScalar(friction));
}

void RigidBody::setRollingFriction(const double rollingFriction) {
    m_rigidBody->setRollingFriction(btScalar(rollingFriction));
}

void RigidBody::setRestitution(const double restitution) {
    m_rigidBody->setRestitution(btScalar(restitution));
}

void RigidBody::setSleepingThresholds(const double linear, const double angular) {
    m_rigidBody->setSleepingThresholds(btScalar(linear), btScalar(angular));
}

void RigidBody::setLinearFactor(const Vector3& linearFactor) {
    m_rigidBody->setLinearFactor(vect(linearFactor));
}

void RigidBody::setLinearVelocity(const Vector3& linearVelocity) {
    m_rigidBody->setLinearVelocity(vect(linearVelocity));
}

void RigidBody::setAngularFactor(const Vector3& angularFactor) {
    m_rigidBody->setAngularFactor(vect(angularFactor));
}

void RigidBody::setAngularVelocity(const Vector3& angularVelocity) {
    m_rigidBody->setAngularVelocity(vect(angularVelocity));
}

void RigidBody::setGravity(const Vector3& gravity) {
    m_rigidBody->setGravity(vect(gravity));
}

void RigidBody::addSphere(const double mass, const double radius) {
    m_shapeId = COLLISION_SHAPE_BOX + " " + boost::lexical_cast<string>(radius);

    btCollisionShape* shape = m_physicsWorld->findCollisionShape(m_shapeId);
    if (shape == 0) {
        shape = new btSphereShape(btScalar(radius));
        m_physicsWorld->registerCollisionShape(m_shapeId, shape);
    }
    addRigidBody(mass, shape);
}

void RigidBody::addBox(const double mass, const double lengthX, const double lengthY, const double lengthZ) {
    m_shapeId = COLLISION_SHAPE_BOX + " " +
            boost::lexical_cast<string>(lengthX) + " " +
            boost::lexical_cast<string>(lengthY) + " " +
            boost::lexical_cast<string>(lengthZ);

    btCollisionShape* shape = m_physicsWorld->findCollisionShape(m_shapeId);
    if (shape == 0) {
        shape = new btBoxShape(btVector3(btScalar(lengthX * 0.5),
                                         btScalar(lengthY * 0.5),
                                         btScalar(lengthZ * 0.5)));
        m_physicsWorld->registerCollisionShape(m_shapeId, shape);
    }
    addRigidBody(mass, shape);
}

void RigidBody::addCylinder(const double mass, const double radius, const double height) {
    m_shapeId = COLLISION_SHAPE_CYLINDER + " " +
            boost::lexical_cast<string>(radius) + " " +
            boost::lexical_cast<string>(height);

    btCollisionShape* shape = m_physicsWorld->findCollisionShape(m_shapeId);
    if (shape == 0) {
        shape = new btCylinderShape(btVector3(btScalar(radius),
                                              btScalar(height),
                                              btScalar(radius)));
        m_physicsWorld->registerCollisionShape(m_shapeId, shape);
    }
    addRigidBody(mass, shape);
}

void RigidBody::addCapsule(const double mass, const double radius, const double height) {
    m_shapeId = COLLISION_SHAPE_CAPSULE + " " +
            boost::lexical_cast<string>(radius) + " " +
            boost::lexical_cast<string>(height);

    btCollisionShape* shape = m_physicsWorld->findCollisionShape(m_shapeId);
    if (shape == 0) {
        shape = new btCapsuleShape(btScalar(radius), btScalar(height));
        m_physicsWorld->registerCollisionShape(m_shapeId, shape);
    }
    addRigidBody(mass, shape);
}

void RigidBody::addCone(const double mass, const double radius, const double height) {
    m_shapeId = COLLISION_SHAPE_CONE + " " +
            boost::lexical_cast<string>(radius) + " " +
            boost::lexical_cast<string>(height);

    btCollisionShape* shape = m_physicsWorld->findCollisionShape(m_shapeId);
    if (shape == 0) {
        shape = new btConeShape(btScalar(radius), btScalar(height));
        m_physicsWorld->registerCollisionShape(m_shapeId, shape);
    }
    addRigidBody(mass, shape);
}

void RigidBody::addConvexHull(const double mass, const string& fileName) {
    m_shapeId = COLLISION_SHAPE_CONVEX + " " + fileName;

    btCollisionShape* shape = m_physicsWorld->findCollisionShape(m_shapeId);
    if (shape == 0) {
        // build original mesh from file
        Model* model = m_resources->generateModelFromFile(fileName);
        cout << "Generating convex hull from file: " << fileName << endl;
        vector<float> points;
        for (size_t n = 0; n < model->getTotalMeshes(); ++n) {
            points.reserve(points.size() + model->mesh(n)->getVerticesSize());
            for (size_t i = 0; i < model->mesh(n)->getVerticesSize(); ++i) {
                points.push_back(model->mesh(n)->getVertex(i));
            }
        }
        btConvexShape* originalConvexShape = new btConvexHullShape(&points[0], points.size() / 3, sizeof(float) * 3);
        points.clear();

        // convert to low polygon hull
        btShapeHull* hull = new btShapeHull(originalConvexShape);
        btScalar margin = originalConvexShape->getMargin();
        hull->buildHull(margin);

        shape = new btConvexHullShape(&hull->getVertexPointer()->getX(), hull->numVertices());
        m_physicsWorld->registerCollisionShape(m_shapeId, shape);

        delete originalConvexShape;
        delete hull;
    }
    addRigidBody(mass, shape);
}

void RigidBody::addConcaveHull(const double mass, const string& fileName) {
    m_shapeId = COLLISION_SHAPE_CONCAVE + " " + fileName;

    btCollisionShape* shape = m_physicsWorld->findCollisionShape(m_shapeId);
    if (shape == 0) {
        // build mesh from file
        Model* model = m_resources->generateModelFromFile(fileName);
        cout << "Generating concave hull from file: " << fileName << endl;
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
        m_physicsWorld->registerCollisionShape(m_shapeId, shape);

        delete triangles;
    }
    addRigidBody(mass, shape);
}

void RigidBody::loadFromPtree(const string& path, const ptree& tree) {
    m_mass = tree.get<double>(xmlPath(path + XML_RIGIDBODY_MASS), 0.0);
    string shape = tree.get<string>(xmlPath(path + XML_RIGIDBODY_COLLISIONSHAPE), "empty");
    stringstream ss(shape);
    ss >> shape;
    if (shape.compare(COLLISION_SHAPE_CONVEX) == 0) {
        string file;
        ss >> file;
        addConvexHull(m_mass, file);
    }
    else if (shape.compare(COLLISION_SHAPE_BOX) == 0) {
        double x, y, z;
        ss >> x >> y >> z;
        addBox(m_mass, x, y, z);
    }
    else if (shape.compare(COLLISION_SHAPE_SPHERE) == 0) {
        double r;
        ss >> r;
        addSphere(m_mass, r);
    }
    else if (shape.compare(COLLISION_SHAPE_CAPSULE) == 0) {
        double r, h;
        ss >> r >> h;
        addCapsule(m_mass, r, h);
    }
    else if (shape.compare(COLLISION_SHAPE_CYLINDER) == 0) {
        double r, h;
        ss >> r >> h;
        addCylinder(m_mass, r, h);
    }
    else if (shape.compare(COLLISION_SHAPE_CONE) == 0) {
        double r, h;
        ss >> r >> h;
        addCone(m_mass, r, h);
    }
    else if (shape.compare(COLLISION_SHAPE_CONCAVE) == 0) {
        string file;
        ss >> file;
        addConcaveHull(m_mass, file);
    }
    else
        cerr << "Error: unknown rigidbody collisionshape: " << shape << endl;

    double x, y;
    x = tree.get<double>(xmlPath(path + XML_RIGIDBODY_FRICTION), 0.5);
    setFriction(x);
    x = tree.get<double>(xmlPath(path + XML_RIGIDBODY_ROLLINGFRICTION), 0.1);
    setRollingFriction(x);

    string str;
    str = tree.get<string>(xmlPath(path + XML_RIGIDBODY_DAMPING), "0 0");
    stringstream damping(str);
    damping >> x >> y;
    setDamping(x, y);

    str = tree.get<string>(xmlPath(path + XML_RIGIDBODY_SLEEPINGTHRESHOLDS), "0.8 1");
    stringstream sleeping(str);
    sleeping >> x >> y;
    setSleepingThresholds(x, y);

    x = tree.get<double>(xmlPath(path + XML_RIGIDBODY_RESTITUTION), 0.0);
    setRestitution(x);

    Vector3 vect;
    vect = tree.get<Vector3>(xmlPath(path + XML_RIGIDBODY_LINEARFACTOR), VECTOR3_UNIT);
    setLinearFactor(vect);
    vect = tree.get<Vector3>(xmlPath(path + XML_RIGIDBODY_LINEARVELOCITY), VECTOR3_ZERO);
    setLinearVelocity(vect);
    vect = tree.get<Vector3>(xmlPath(path + XML_RIGIDBODY_ANGULARFACTOR), VECTOR3_UNIT);
    setAngularFactor(vect);
    vect = tree.get<Vector3>(xmlPath(path + XML_RIGIDBODY_ANGULARVELOCITY), VECTOR3_ZERO);
    setAngularVelocity(vect);
    vect = tree.get<Vector3>(xmlPath(path + XML_RIGIDBODY_GRAVITY), Vector3(0.0, -9.8, 0.0));
    setGravity(vect);
}

void RigidBody::saveToPtree(const string& path, ptree& tree) const {
    tree.put(xmlPath(path + XML_RIGIDBODY_MASS), getMass());
    tree.put(xmlPath(path + XML_RIGIDBODY_COLLISIONSHAPE), getShapeId());
    stringstream damping;
    damping << getLinearDamping() << " " << getAngularDamping();
    tree.put(xmlPath(path + XML_RIGIDBODY_DAMPING), damping.str());
    tree.put(xmlPath(path + XML_RIGIDBODY_FRICTION), getFriction());
    tree.put(xmlPath(path + XML_RIGIDBODY_ROLLINGFRICTION), getRollingFriction());
    tree.put(xmlPath(path + XML_RIGIDBODY_RESTITUTION), getRestitution());
    stringstream sleeping;
    sleeping << getLinearSleepingThreshold() << " " << getAngularSleepingThreshold();
    tree.put(xmlPath(path + XML_RIGIDBODY_SLEEPINGTHRESHOLDS), sleeping.str());
    tree.put(xmlPath(path + XML_RIGIDBODY_LINEARFACTOR), getLinearFactor());
    tree.put(xmlPath(path + XML_RIGIDBODY_LINEARVELOCITY), getLinearVelocity());
    tree.put(xmlPath(path + XML_RIGIDBODY_ANGULARFACTOR), getAngularFactor());
    tree.put(xmlPath(path + XML_RIGIDBODY_ANGULARVELOCITY), getAngularVelocity());
    tree.put(xmlPath(path + XML_RIGIDBODY_GRAVITY), getGravity());
}



RigidBody::RigidBody(const RigidBody& rhs):
    Component(rhs.m_type, rhs.m_entity),
    m_resources(rhs.m_resources),
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
    shape->calculateLocalInertia(btScalar(m_mass), inertia);
    btDefaultMotionState* motion = new btDefaultMotionState(
            trans(m_entity->getOrientationAbs(), m_entity->getPositionAbs()));
    m_rigidBody = new btRigidBody(btScalar(m_mass), motion, shape, inertia);
    m_physicsWorld->registerRigidBody(this);
}


string RigidBody::cmdIsActive(deque<string>& args) {
    if (args.size() < 1)
        return "Error: too few arguments";
    bool isActive = boost::lexical_cast<bool>(args[0]);
    activate(isActive);
    return "";
}

string RigidBody::cmdMass(deque<string>& args) {
    if (args.size() < 1)
        return "Error: too few arguments";
    double mass = boost::lexical_cast<double>(args[0]);
    setMass(mass);
    return "";
}

string RigidBody::cmdDamping(deque<string>& args) {
    if (args.size() < 2)
        return "Error: too few arguments";
    double linear = boost::lexical_cast<double>(args[0]);
    double angular = boost::lexical_cast<double>(args[1]);
    setDamping(linear, angular);
    return "";
}

string RigidBody::cmdFriction(deque<string>& args) {
    if (args.size() < 1)
        return "Error: too few arguments";
    double friction = boost::lexical_cast<double>(args[0]);
    setFriction(friction);
    return "";
}

string RigidBody::cmdRollingFriction(deque<string>& args) {
    if (args.size() < 1)
        return "Error: too few arguments";
    double rollingFriction = boost::lexical_cast<double>(args[0]);
    setRollingFriction(rollingFriction);
    return "";
}

string RigidBody::cmdRestitution(deque<string>& args) {
    if (args.size() < 1)
        return "Error: too few arguments";
    double restitution = boost::lexical_cast<double>(args[0]);
    setRestitution(restitution);
    return "";
}

string RigidBody::cmdSleepingThresholds(deque<string>& args) {
    if (args.size() < 2)
        return "Error: too few arguments";
    double linear = boost::lexical_cast<double>(args[0]);
    double angular = boost::lexical_cast<double>(args[1]);
    setSleepingThresholds(linear, angular);
    return "";
}

string RigidBody::cmdLinearFactor(deque<string>& args) {
    if (args.size() < 3)
        return "Error: too few arguments";
    double x = boost::lexical_cast<double>(args[0]);
    double y = boost::lexical_cast<double>(args[1]);
    double z = boost::lexical_cast<double>(args[2]);
    setLinearFactor(Vector3(x, y, z));
    return "";
}

string RigidBody::cmdLinearVelocity(deque<string>& args) {
    if (args.size() < 3)
        return "Error: too few arguments";
    double x = boost::lexical_cast<double>(args[0]);
    double y = boost::lexical_cast<double>(args[1]);
    double z = boost::lexical_cast<double>(args[2]);
    setLinearVelocity(Vector3(x, y, z));
    return "";
}

string RigidBody::cmdAngularFactor(deque<string>& args) {
    if (args.size() < 3)
        return "Error: too few arguments";
    double x = boost::lexical_cast<double>(args[0]);
    double y = boost::lexical_cast<double>(args[1]);
    double z = boost::lexical_cast<double>(args[2]);
    setAngularFactor(Vector3(x, y, z));
    return "";
}

string RigidBody::cmdAngularVelocity(deque<string>& args) {
    if (args.size() < 3)
        return "Error: too few arguments";
    double x = boost::lexical_cast<double>(args[0]);
    double y = boost::lexical_cast<double>(args[1]);
    double z = boost::lexical_cast<double>(args[2]);
    setAngularVelocity(Vector3(x, y, z));
    return "";
}

string RigidBody::cmdGravity(deque<string>& args) {
    if (args.size() < 3)
        return "Error: too few arguments";
    double x = boost::lexical_cast<double>(args[0]);
    double y = boost::lexical_cast<double>(args[1]);
    double z = boost::lexical_cast<double>(args[2]);
    setGravity(Vector3(x, y, z));
    return "";
}
