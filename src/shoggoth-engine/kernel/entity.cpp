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


#include "shoggoth-engine/kernel/entity.hpp"

#include <iostream>
#include <cmath>
#include "shoggoth-engine/linearmath/matrix3x3.hpp"
#include "shoggoth-engine/kernel/device.hpp"
#include "shoggoth-engine/kernel/scene.hpp"
#include "shoggoth-engine/physics/rigidbody.hpp"

using namespace std;

const size_t INDENT_SIZE = 2;

Entity::Entity(Entity* parent, const string& objectName, const Device* device):
    CommandObject(objectName),
    m_parent(parent),
    m_device(device),
    m_children(),
    m_components(),
    m_positionAbs(VECTOR3_ZERO),
    m_positionRel(VECTOR3_ZERO),
    m_orientationAbs(QUATERNION_IDENTITY),
    m_orientationRel(QUATERNION_IDENTITY),
    m_lastOrientation(QUATERNION_IDENTITY)
{
    if (m_parent != 0) {
        setPositionRel(VECTOR3_ZERO);
        setOrientationRel(QUATERNION_IDENTITY);
    }
    registerAttribute("position-abs", boost::bind(&Entity::cmdPositionAbs, this, _1));
    registerAttribute("position-rel", boost::bind(&Entity::cmdPositionRel, this, _1));
    registerAttribute("orientation-abs-ypr", boost::bind(&Entity::cmdOrientationAbsYPR, this, _1));
    registerAttribute("orientation-rel-ypr", boost::bind(&Entity::cmdOrientationRelYPR, this, _1));
    registerCommand("move-xyz", boost::bind(&Entity::cmdMoveXYZ, this, _1));
    registerCommand("move-x", boost::bind(&Entity::cmdMoveX, this, _1));
    registerCommand("move-y", boost::bind(&Entity::cmdMoveY, this, _1));
    registerCommand("move-z", boost::bind(&Entity::cmdMoveZ, this, _1));
    registerCommand("move-xyz-parent", boost::bind(&Entity::cmdMoveXYZ_parent, this, _1));
    registerCommand("move-x-parent", boost::bind(&Entity::cmdMoveX_parent, this, _1));
    registerCommand("move-y-parent", boost::bind(&Entity::cmdMoveY_parent, this, _1));
    registerCommand("move-z-parent", boost::bind(&Entity::cmdMoveZ_parent, this, _1));
    registerCommand("move-xyz-global", boost::bind(&Entity::cmdMoveXYZ_global, this, _1));
    registerCommand("move-x-global", boost::bind(&Entity::cmdMoveX_global, this, _1));
    registerCommand("move-y-global", boost::bind(&Entity::cmdMoveY_global, this, _1));
    registerCommand("move-z-global", boost::bind(&Entity::cmdMoveZ_global, this, _1));
    registerCommand("yaw", boost::bind(&Entity::cmdYaw, this, _1));
    registerCommand("pitch", boost::bind(&Entity::cmdPitch, this, _1));
    registerCommand("roll", boost::bind(&Entity::cmdRoll, this, _1));
    registerCommand("yaw-parent", boost::bind(&Entity::cmdYaw_parent, this, _1));
    registerCommand("pitch-parent", boost::bind(&Entity::cmdPitch_parent, this, _1));
    registerCommand("roll-parent", boost::bind(&Entity::cmdRoll_parent, this, _1));
    registerCommand("yaw-global", boost::bind(&Entity::cmdYaw_global, this, _1));
    registerCommand("pitch-global", boost::bind(&Entity::cmdPitch_global, this, _1));
    registerCommand("roll-global", boost::bind(&Entity::cmdRoll_global, this, _1));
    registerCommand("remove-all-children", boost::bind(&Entity::cmdRemoveAllChildren, this, _1));
}

Entity::~Entity() {
    unregisterAllCommands();
    unregisterAllAttributes();
    removeAllChildren();
}


void Entity::translate(const Vector3& displacement, const transform_space_t& relativeTo) {
    switch (relativeTo) {
    case SPACE_LOCAL:
        setPositionRel(m_positionRel + displacement.rotate(m_orientationAbs));
        break;
    case SPACE_PARENT:
        setPositionRel(m_positionRel + displacement.rotate(m_parent->m_orientationAbs));
        break;
    case SPACE_GLOBAL:
        setPositionAbs(m_positionAbs + displacement);
        break;
    default:
        cerr << "Invalid transform_space_t: " << relativeTo << endl;
    }
}

void Entity::rotate(const Quaternion& deltaRotation, const transform_space_t& relativeTo) {
    switch (relativeTo) {
    case SPACE_LOCAL:
        setOrientationAbs(m_orientationAbs * deltaRotation);
        break;
    case SPACE_PARENT:
        setOrientationRel(deltaRotation * m_orientationRel);
        break;
    case SPACE_GLOBAL:
        setOrientationAbs(deltaRotation * m_orientationAbs);
        break;
    default:
        cerr << "Invalid transform_space_t: " << relativeTo << endl;
    }
}

void Entity::lookAt(const Vector3& target, const Vector3& up) {
    // modified Mesa 9.0 glu implementation
    Quaternion result;
    Vector3 vFwd = (target - m_positionAbs).normalized();
    Vector3 vSide = vFwd.cross(up).normalized();
    Vector3 vUp = vSide.cross(vFwd);
    Matrix3x3 m(vSide.getX(), vUp.getX(), -vFwd.getX(),
                vSide.getY(), vUp.getY(), -vFwd.getY(),
                vSide.getZ(), vUp.getZ(), -vFwd.getZ());
    result = m.getRotationQuat();
    setOrientationAbs(result);
}

Entity* Entity::addChild(const string& childName) {
    Entity* child = new Entity(this, childName, m_device);
    m_children.insert(child);
    Scene::ms_entities.insert(pair<string, Entity*>(childName, child));
    return child;
}

void Entity::removeChild(Entity* const child) {
    map<string, Entity*>::iterator smIt = Scene::ms_entities.find(child->getObjectName());
    if (smIt != Scene::ms_entities.end())
        Scene::ms_entities.erase(smIt);

    set<Entity*>::iterator it = m_children.find(child);
    if (it != m_children.end()) {
        delete *it;
        m_children.erase(it);
    }
}

void Entity::removeAllChildren() {
    map<string, Component*>::iterator itComp;
    for (itComp = m_components.begin(); itComp != m_components.end(); ++itComp)
        delete itComp->second;
    m_components.clear();

    set<Entity*>::iterator itEntity;
    for (itEntity = m_children.begin(); itEntity != m_children.end(); ++itEntity)
        delete *itEntity;
    m_children.clear();

    Scene::ms_entities.clear();
}

string Entity::treeToString(const size_t indent) const {
    stringstream ss;
    for (size_t i = 0; i < indent; ++i)
        ss << " ";
    ss << m_objectName << endl;
    set<Entity*>::iterator it;
    for (it = m_children.begin(); it != m_children.end(); ++it)
        ss << (*it)->treeToString(indent + INDENT_SIZE);
    return ss.str();
}



Entity::Entity(const Entity& rhs):
    CommandObject(rhs.m_objectName),
    m_parent(rhs.m_parent),
    m_device(rhs.m_device),
    m_children(rhs.m_children),
    m_components(rhs.m_components),
    m_positionAbs(rhs.m_positionAbs),
    m_positionRel(rhs.m_positionRel),
    m_orientationAbs(rhs.m_orientationAbs),
    m_orientationRel(rhs.m_orientationRel),
    m_lastOrientation(rhs.m_lastOrientation)
{
    cerr << "Error: Entity copy constructor should not be called!" << endl;
}

Entity& Entity::operator=(const Entity&) {
    cerr << "Error: Entity assignment operator should not be called!" << endl;
    return *this;
}



void Entity::applyTranslationToChildren() {
    set<Entity*>::iterator it, itend;
    itend = m_children.end();
    for (it = m_children.begin(); it != itend; ++it) {
        Entity& child = **it;
        child.setPositionRel(child.m_positionRel);
    }
}

void Entity::applyOrientationToChildren() {
    // Qc1 = Qp1 * Inv(Qp0) * Qc0
    // Where:
    // Qp0 = Parent orientation last frame
    // Qp1 = Parent orientation this frame
    // Qc0 = Child orientation last frame
    // Qc1 = Child orientation this frame
    Quaternion relativeRotation = m_orientationAbs * m_lastOrientation.inverse();
    set<Entity*>::iterator it, itend;
    itend = m_children.end();
    for (it = m_children.begin(); it != itend; ++it) {
        Entity& child = **it;
        child.setPositionRel(child.m_positionRel.rotate(relativeRotation));
        child.setOrientationAbs(relativeRotation * child.m_orientationAbs);
    }
}

void Entity::applyTransformToPhysicsComponent() {
    Component* comp = component(COMPONENT_RIGIDBODY);
    if (comp != 0) {
        RigidBody* rigidBody = dynamic_cast<RigidBody*>(comp);
        rigidBody->activate();
        rigidBody->setTransform(m_positionAbs, m_orientationAbs);
    }
}



string Entity::cmdPositionAbs(deque<string>& args) {
    if (args.size() < 3)
        return "Error: too few arguments";
    scalar_t x = boost::lexical_cast<scalar_t>(args[0]);
    scalar_t y = boost::lexical_cast<scalar_t>(args[1]);
    scalar_t z = boost::lexical_cast<scalar_t>(args[2]);
    setPositionAbs(x, y, z);
    return "";
}
string Entity::cmdPositionRel(deque<string>& args) {
    if (args.size() < 3)
        return "Error: too few arguments";
    scalar_t x = boost::lexical_cast<scalar_t>(args[0]);
    scalar_t y = boost::lexical_cast<scalar_t>(args[1]);
    scalar_t z = boost::lexical_cast<scalar_t>(args[2]);
    setPositionRel(x, y, z);
    return "";
}

string Entity::cmdOrientationAbsYPR(deque<string>& args) {
    if (args.size() < 3)
        return "Error: too few arguments";
    scalar_t yaw = boost::lexical_cast<scalar_t>(args[0]);
    scalar_t pitch = boost::lexical_cast<scalar_t>(args[1]);
    scalar_t roll = boost::lexical_cast<scalar_t>(args[2]);
    setOrientationAbs(yaw, pitch, roll);
    return "";
}

string Entity::cmdOrientationRelYPR(deque<string>& args) {
    if (args.size() < 3)
        return "Error: too few arguments";
    scalar_t yaw = boost::lexical_cast<scalar_t>(args[0]);
    scalar_t pitch = boost::lexical_cast<scalar_t>(args[1]);
    scalar_t roll = boost::lexical_cast<scalar_t>(args[2]);
    setOrientationRel(yaw, pitch, roll);
    return "";
}

string Entity::cmdMoveXYZ(deque<string>& args) {
    if (args.size() < 3)
        return "Error: too few arguments";
    scalar_t x = boost::lexical_cast<scalar_t>(args[0]);
    scalar_t y = boost::lexical_cast<scalar_t>(args[1]);
    scalar_t z = boost::lexical_cast<scalar_t>(args[2]);
    translate(x, y, z);
    return "";
}

string Entity::cmdMoveX(deque<string>& args) {
    if (args.size() < 1)
        return "Error: too few arguments";
    scalar_t dist = boost::lexical_cast<scalar_t>(args[0]);
    translateX(dist * m_device->getDeltaTime());
    return "";
}

string Entity::cmdMoveY(deque<string>& args) {
    if (args.size() < 1)
        return "Error: too few arguments";
    scalar_t dist = boost::lexical_cast<scalar_t>(args[0]);
    translateY(dist * m_device->getDeltaTime());
    return "";
}

string Entity::cmdMoveZ(deque<string>& args) {
    if (args.size() < 1)
        return "Error: too few arguments";
    scalar_t dist = boost::lexical_cast<scalar_t>(args[0]);
    translateZ(dist * m_device->getDeltaTime());
    return "";
}

string Entity::cmdMoveXYZ_parent(deque<string>& args) {
    if (args.size() < 3)
        return "Error: too few arguments";
    scalar_t x = boost::lexical_cast<scalar_t>(args[0]);
    scalar_t y = boost::lexical_cast<scalar_t>(args[1]);
    scalar_t z = boost::lexical_cast<scalar_t>(args[2]);
    translate(
        x * m_device->getDeltaTime(),
        y * m_device->getDeltaTime(),
        z * m_device->getDeltaTime(),
        SPACE_PARENT
    );
    return "";
}

string Entity::cmdMoveX_parent(deque<string>& args) {
    if (args.size() < 1)
        return "Error: too few arguments";
    scalar_t dist = boost::lexical_cast<scalar_t>(args[0]);
    translateX(dist * m_device->getDeltaTime(), SPACE_PARENT);
    return "";
}

string Entity::cmdMoveY_parent(deque<string>& args) {
    if (args.size() < 1)
        return "Error: too few arguments";
    scalar_t dist = boost::lexical_cast<scalar_t>(args[0]);
    translateY(dist * m_device->getDeltaTime(), SPACE_PARENT);
    return "";
}

string Entity::cmdMoveZ_parent(deque<string>& args) {
    if (args.size() < 1)
        return "Error: too few arguments";
    scalar_t dist = boost::lexical_cast<scalar_t>(args[0]);
    translateZ(dist * m_device->getDeltaTime(), SPACE_PARENT);
    return "";
}

string Entity::cmdMoveXYZ_global(deque<string>& args) {
    if (args.size() < 3)
        return "Error: too few arguments";
    scalar_t x = boost::lexical_cast<scalar_t>(args[0]);
    scalar_t y = boost::lexical_cast<scalar_t>(args[1]);
    scalar_t z = boost::lexical_cast<scalar_t>(args[2]);
    translate(
        x * m_device->getDeltaTime(),
        y * m_device->getDeltaTime(),
        z * m_device->getDeltaTime(),
        SPACE_GLOBAL
    );
    return "";
}

string Entity::cmdMoveX_global(deque<string>& args) {
    if (args.size() < 1)
        return "Error: too few arguments";
    scalar_t dist = boost::lexical_cast<scalar_t>(args[0]);
    translateX(dist * m_device->getDeltaTime(), SPACE_GLOBAL);
    return "";
}

string Entity::cmdMoveY_global(deque<string>& args) {
    if (args.size() < 1)
        return "Error: too few arguments";
    scalar_t dist = boost::lexical_cast<scalar_t>(args[0]);
    translateY(dist * m_device->getDeltaTime(), SPACE_GLOBAL);
    return "";
}

string Entity::cmdMoveZ_global(deque<string>& args) {
    if (args.size() < 1)
        return "Error: too few arguments";
    scalar_t dist = boost::lexical_cast<scalar_t>(args[0]);
    translateZ(dist * m_device->getDeltaTime(), SPACE_GLOBAL);
    return "";
}

string Entity::cmdYaw(deque<string>& args) {
    if (args.size() < 1)
        return "Error: too few arguments";
    scalar_t radians = boost::lexical_cast<scalar_t>(args[0]);
    yaw(radians * m_device->getDeltaTime());
    return "";
}

string Entity::cmdPitch(deque<string>& args) {
    if (args.size() < 1)
        return "Error: too few arguments";
    scalar_t radians = boost::lexical_cast<scalar_t>(args[0]);
    pitch(radians * m_device->getDeltaTime());
    return "";
}

string Entity::cmdRoll(deque<string>& args) {
    if (args.size() < 1)
        return "Error: too few arguments";
    scalar_t radians = boost::lexical_cast<scalar_t>(args[0]);
    roll(radians * m_device->getDeltaTime());
    return "";
}

string Entity::cmdYaw_parent(deque<string>& args) {
    if (args.size() < 1)
        return "Error: too few arguments";
    scalar_t radians = boost::lexical_cast<scalar_t>(args[0]);
    yaw(radians * m_device->getDeltaTime(), SPACE_PARENT);
    return "";
}

string Entity::cmdPitch_parent(deque<string>& args) {
    if (args.size() < 1)
        return "Error: too few arguments";
    scalar_t radians = boost::lexical_cast<scalar_t>(args[0]);
    pitch(radians * m_device->getDeltaTime(), SPACE_PARENT);
    return "";
}

string Entity::cmdRoll_parent(deque<string>& args) {
    if (args.size() < 1)
        return "Error: too few arguments";
    scalar_t radians = boost::lexical_cast<scalar_t>(args[0]);
    roll(radians * m_device->getDeltaTime(), SPACE_PARENT);
    return "";
}

string Entity::cmdYaw_global(deque<string>& args) {
    if (args.size() < 1)
        return "Error: too few arguments";
    scalar_t radians = boost::lexical_cast<scalar_t>(args[0]);
    yaw(radians * m_device->getDeltaTime(), SPACE_GLOBAL);
    return "";
}

string Entity::cmdPitch_global(deque<string>& args) {
    if (args.size() < 1)
        return "Error: too few arguments";
    scalar_t radians = boost::lexical_cast<scalar_t>(args[0]);
    pitch(radians * m_device->getDeltaTime(), SPACE_GLOBAL);
    return "";
}

string Entity::cmdRoll_global(deque<string>& args) {
    if (args.size() < 1)
        return "Error: too few arguments";
    scalar_t radians = boost::lexical_cast<scalar_t>(args[0]);
    roll(radians * m_device->getDeltaTime(), SPACE_GLOBAL);
    return "";
}
