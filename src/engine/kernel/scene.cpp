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


#include "engine/kernel/scene.hpp"

#include <iostream>
#include <fstream>
#include <boost/foreach.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include "engine/kernel/entity.hpp"
#include "engine/renderer/camera.hpp"
#include "engine/renderer/light.hpp"
#include "engine/renderer/renderablemesh.hpp"
#include "engine/resources/model.hpp"
#include "engine/physics/rigidbody.hpp"

using namespace std;
using namespace boost::property_tree;

const string XML_SCENE = "scene";
const string XML_DELIMITER = "/";
const string XML_ATTRIBUTE = "<xmlattr>";
const string XML_ATTR_POSITION = "position";
const string XML_ATTR_ORIENTATION = "orientation";
const string XML_ATTR_TYPE = "type";
const string XML_ATTR_TYPE_ROOT = "root-node";
const string XML_ATTR_TYPE_ENTITY = "entity";
const string XML_ATTR_TYPE_COMPONENT = "component";

const string XML_CAMERA = "camera";
const string XML_CAMERA_TYPE = "cameratype";
const string XML_CAMERA_VIEWPORT = "viewport";
const string XML_CAMERA_ASPECTRATIO = "aspectratio";
const string XML_CAMERA_PERSPECTIVEFOV = "perspectivefov";
const string XML_CAMERA_ORTHOHEIGHT = "orthoheight";
const string XML_CAMERA_NEARDISTANCE = "neardistance";
const string XML_CAMERA_FARDISTANCE = "fardistance";

const string XML_LIGHT = "light";
const string XML_LIGHT_AMBIENT = "ambient";
const string XML_LIGHT_DIFFUSE = "diffuse";
const string XML_LIGHT_SPECULAR = "specular";

const string XML_RENDERABLEMESH = "renderablemesh";
const string XML_RENDERABLEMESH_MODEL = "model";

const string XML_RIGIDBODY = "rigidbody";
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


map<string, Entity*> Scene::ms_entities = map<string, Entity*>();

Scene::Scene(const std::string& objectName,
             const std::string& rootNodeName,
             const Device* device,
             Renderer* renderer,
             Resources* resources,
             PhysicsWorld* physicsWorld):
    CommandObject(objectName),
    m_device(device),
    m_renderer(renderer),
    m_resources(resources),
    m_physicsWorld(physicsWorld),
    m_rootName(rootNodeName),
    m_root(new Entity(0, m_rootName, m_device))
{
    registerCommand("initialize", boost::bind(&Scene::cmdInitialize, this, _1));
    registerCommand("shutdown", boost::bind(&Scene::cmdShutdown, this, _1));
    registerCommand("save-xml", boost::bind(&Scene::cmdSaveXML, this, _1));
    registerCommand("load-xml", boost::bind(&Scene::cmdLoadXML, this, _1));
}

Scene::~Scene() {
    unregisterAllCommands();
    unregisterAllAttributes();
    delete m_root;
}

void Scene::initialize() {
}

void Scene::shutdown() {
    cout << "Removing all entities" << endl;
    m_root->removeAllChildren();
}

void Scene::saveToXML(const string& fileName) const {
    cout << "Saving scene to XML file: " << fileName << endl;
    ptree tree;
    xml_writer_settings<char> settings(' ', 2);
    saveToPTree(XML_SCENE, tree, m_root);
    write_xml(fileName, tree, std::locale(), settings);
}

bool Scene::loadFromXML(const string& fileName) {
    cout << "Loading scene from XML file: " << fileName << endl;
    // success flags
    set<string> names;
    bool isCameraFound = false;

    ifstream fin(fileName.c_str());
    if (!fin.is_open() || !fin.good()) {
        cerr << "Error: could not open file: " << fileName << endl;
        return false;
    }
    fin.close();

    ptree tree;
    read_xml(fileName, tree, xml_parser::trim_whitespace);

    delete m_root;
    m_root = new Entity(0, m_rootName, m_device);
    if (!loadFromPTree(XML_SCENE + XML_DELIMITER + m_rootName, tree, m_root, 0, names, isCameraFound)) {
        cerr << "Failed to load scene: " << fileName << endl;
        delete m_root;
        m_root = new Entity(0, m_rootName, m_device);
        return false;
    }

    if (!isCameraFound) {
        cerr << "Error: no cameras found, aborting" << endl;
        delete m_root;
        m_root = new Entity(0, m_rootName, m_device);
        return false;
    }
    return true;
}

bool Scene::findEntity(const string& name, Entity*& entity) {
    map<string, Entity*>::iterator it = ms_entities.find(name);
    if (it != ms_entities.end()) {
        entity = it->second;
        return true;
    }
    return false;
}

string Scene::sceneGraphToString() {
    stringstream ss;
    ss << "Scene Graph:" << endl;
    ss << m_root->treeToString(0);
    return ss.str();
}



Scene::Scene(const Scene& rhs):
    CommandObject(rhs.m_objectName),
    m_device(rhs.m_device),
    m_renderer(rhs.m_renderer),
    m_resources(rhs.m_resources),
    m_physicsWorld(rhs.m_physicsWorld),
    m_rootName(rhs.m_rootName),
    m_root(rhs.m_root)
{
    cerr << "Error: Scene copy constructor should not be called!" << endl;
}

Scene& Scene::operator=(const Scene&) {
    cerr << "Error: Scene assignment operator should not be called!" << endl;
    return *this;
}



void Scene::saveToPTree(const string& path,
                        ptree& tree,
                        const Entity* node) const {
    // save current node
    string curPath = path + XML_DELIMITER + node->getObjectName();
    tree.put(ptree::path_type(curPath, XML_DELIMITER[0]), "");

    // save attributes
    string attr;
    string attrPath = curPath + XML_DELIMITER + XML_ATTRIBUTE + XML_DELIMITER;
    attr = attrPath + XML_ATTR_TYPE;
    if (node->getParent() != 0) {
        tree.put(ptree::path_type(attr, XML_DELIMITER[0]), XML_ATTR_TYPE_ENTITY);
        attr = attrPath + XML_ATTR_POSITION;
        tree.put(ptree::path_type(attr, XML_DELIMITER[0]), node->getPositionAbs());
        attr = attrPath + XML_ATTR_ORIENTATION;
        tree.put(ptree::path_type(attr, XML_DELIMITER[0]), node->getOrientationAbs());
    }
    else
        tree.put(ptree::path_type(attr, XML_DELIMITER[0]), XML_ATTR_TYPE_ROOT);

    // save components
    string comp;
    string compPath = curPath + XML_DELIMITER;
    for (size_t i = 0; i < TOTAL_COMPONENTS_CONTAINER_SIZE; ++i) {
        const Component* component = node->getComponent((component_t)i);
        if (component != 0) {
            switch (component->getType()) {
            case COMPONENT_CAMERA: {
                Camera* camera = (Camera*)component;
                comp = compPath + XML_CAMERA;
                attrPath = comp + XML_DELIMITER + XML_ATTRIBUTE + XML_DELIMITER;
                attr = attrPath + XML_ATTR_TYPE;
                tree.put(ptree::path_type(attr, XML_DELIMITER[0]), XML_ATTR_TYPE_COMPONENT);
                attr = attrPath + XML_CAMERA_TYPE;
                tree.put(ptree::path_type(attr, XML_DELIMITER[0]), camera->getType());
//                 attr = attrPath + XML_CAMERA_VIEWPORT;
//                 tree.put(ptree::path_type(attr, XML_DELIMITER[0]), camera->getViewport());
                attr = attrPath + XML_CAMERA_PERSPECTIVEFOV;
                tree.put(ptree::path_type(attr, XML_DELIMITER[0]), camera->getPerspectiveFOV());
                attr = attrPath + XML_CAMERA_ORTHOHEIGHT;
                tree.put(ptree::path_type(attr, XML_DELIMITER[0]), camera->getOrthoHeight());
                attr = attrPath + XML_CAMERA_NEARDISTANCE;
                tree.put(ptree::path_type(attr, XML_DELIMITER[0]), camera->getNearDistance());
                attr = attrPath + XML_CAMERA_FARDISTANCE;
                tree.put(ptree::path_type(attr, XML_DELIMITER[0]), camera->getFarDistance());
                break; }
            case COMPONENT_LIGHT: {
                Light* light = (Light*)component;
                comp = compPath + XML_LIGHT;
                attrPath = comp + XML_DELIMITER + XML_ATTRIBUTE + XML_DELIMITER;
                attr = attrPath + XML_ATTR_TYPE;
                tree.put(ptree::path_type(attr, XML_DELIMITER[0]), XML_ATTR_TYPE_COMPONENT);
                attr = attrPath + XML_LIGHT_AMBIENT;
                tree.put(ptree::path_type(attr, XML_DELIMITER[0]), light->getAmbient());
                attr = attrPath + XML_LIGHT_DIFFUSE;
                tree.put(ptree::path_type(attr, XML_DELIMITER[0]), light->getDiffuse());
                attr = attrPath + XML_LIGHT_SPECULAR;
                tree.put(ptree::path_type(attr, XML_DELIMITER[0]), light->getSpecular());
                break; }
            case COMPONENT_RENDERABLE_MESH: {
                RenderableMesh* mesh = (RenderableMesh*)component;
                comp = compPath + XML_RENDERABLEMESH;
                attrPath = comp + XML_DELIMITER + XML_ATTRIBUTE + XML_DELIMITER;
                attr = attrPath + XML_ATTR_TYPE;
                tree.put(ptree::path_type(attr, XML_DELIMITER[0]), XML_ATTR_TYPE_COMPONENT);
                attr = attrPath + XML_RENDERABLEMESH_MODEL;
                tree.put(ptree::path_type(attr, XML_DELIMITER[0]), mesh->getDescription());
                break; }
            case COMPONENT_RIGIDBODY: {
                RigidBody* rigidbody = (RigidBody*)component;
                comp = compPath + XML_RIGIDBODY;
                attrPath = comp + XML_DELIMITER + XML_ATTRIBUTE + XML_DELIMITER;
                attr = attrPath + XML_ATTR_TYPE;
                tree.put(ptree::path_type(attr, XML_DELIMITER[0]), XML_ATTR_TYPE_COMPONENT);
                attr = attrPath + XML_RIGIDBODY_MASS;
                tree.put(ptree::path_type(attr, XML_DELIMITER[0]), rigidbody->getMass());
                attr = attrPath + XML_RIGIDBODY_COLLISIONSHAPE;
                tree.put(ptree::path_type(attr, XML_DELIMITER[0]), rigidbody->getShapeId());
                attr = attrPath + XML_RIGIDBODY_DAMPING;
                stringstream damping;
                damping << rigidbody->getLinearDamping() << " " << rigidbody->getAngularDamping();
                tree.put(ptree::path_type(attr, XML_DELIMITER[0]), damping.str());
                attr = attrPath + XML_RIGIDBODY_FRICTION;
                tree.put(ptree::path_type(attr, XML_DELIMITER[0]), rigidbody->getFriction());
                attr = attrPath + XML_RIGIDBODY_ROLLINGFRICTION;
                tree.put(ptree::path_type(attr, XML_DELIMITER[0]), rigidbody->getRollingFriction());
                attr = attrPath + XML_RIGIDBODY_RESTITUTION;
                tree.put(ptree::path_type(attr, XML_DELIMITER[0]), rigidbody->getRestitution());
                attr = attrPath + XML_RIGIDBODY_SLEEPINGTHRESHOLDS;
                stringstream sleeping;
                sleeping << rigidbody->getLinearSleepingThreshold() << " " << rigidbody->getAngularSleepingThreshold();
                tree.put(ptree::path_type(attr, XML_DELIMITER[0]), sleeping.str());
                attr = attrPath + XML_RIGIDBODY_LINEARFACTOR;
                tree.put(ptree::path_type(attr, XML_DELIMITER[0]), rigidbody->getLinearFactor());
                attr = attrPath + XML_RIGIDBODY_LINEARVELOCITY;
                tree.put(ptree::path_type(attr, XML_DELIMITER[0]), rigidbody->getLinearVelocity());
                attr = attrPath + XML_RIGIDBODY_ANGULARFACTOR;
                tree.put(ptree::path_type(attr, XML_DELIMITER[0]), rigidbody->getAngularFactor());
                attr = attrPath + XML_RIGIDBODY_ANGULARVELOCITY;
                tree.put(ptree::path_type(attr, XML_DELIMITER[0]), rigidbody->getAngularVelocity());
                attr = attrPath + XML_RIGIDBODY_GRAVITY;
                tree.put(ptree::path_type(attr, XML_DELIMITER[0]), rigidbody->getGravity());
                break; }
            default:
                cerr << "Error: invalid component_t: " << component->getType() << endl;
            }
            tree.put(ptree::path_type(comp, XML_DELIMITER[0]), "");
        }
    }

    // traverse all children
    Entity::const_child_iterator_t it;
    for (it = node->getChildrenBegin(); it != node->getChildrenEnd(); ++it)
        saveToPTree(curPath, tree, *it);
}

bool Scene::loadFromPTree(const string& path,
                          const ptree& tree,
                          Entity* node,
                          Entity* parent,
                          set<string>& names,
                          bool& isCameraFound) {
    // temporal variables to read to
    int tmpInt;
    double tmpDouble1;
    double tmpDouble2;
    string tmpStr;
    Vector3 tmpVect;
    Quaternion tmpQuat;

    // load attributes
    string attrPath = path + XML_DELIMITER + XML_ATTRIBUTE + XML_DELIMITER;
    if (parent != 0) {
        tmpVect = tree.get<Vector3>(ptree::path_type(attrPath + XML_ATTR_POSITION, XML_DELIMITER[0]));
        node->setPositionAbs(tmpVect);
        tmpQuat = tree.get<Quaternion>(ptree::path_type(attrPath + XML_ATTR_ORIENTATION, XML_DELIMITER[0]));
        node->setOrientationAbs(tmpQuat);
    }

    // traverse all children
    string name;
    string curPath;
    string type;
    BOOST_FOREACH(ptree::value_type v, tree.get_child(ptree::path_type(path, XML_DELIMITER[0]))) {
        name = v.first;
        if (name.compare(XML_ATTRIBUTE) == 0)
            continue;
        curPath = path + XML_DELIMITER + name;
        attrPath = curPath + XML_DELIMITER + XML_ATTRIBUTE + XML_DELIMITER;
        type = tree.get<string>(ptree::path_type(attrPath + XML_ATTR_TYPE, XML_DELIMITER[0]), "empty");
        if (type.compare(XML_ATTR_TYPE_ENTITY) == 0) {
            if (names.find(name) != names.end()) {
                cerr << "Error: ignoring repeated entity name: " << name << endl;
                continue;
            }
            else
                names.insert(name);
            Entity* child = node->addChild(name);
            if (!loadFromPTree(curPath, tree, child, node, names, isCameraFound))
                return false;
        }
        else if (type.compare(XML_ATTR_TYPE_COMPONENT) == 0) {
            if (name.compare(XML_RENDERABLEMESH) == 0) {
                tmpStr = tree.get<string>(ptree::path_type(attrPath + XML_RENDERABLEMESH_MODEL, XML_DELIMITER[0]), "empty");
                stringstream ss(tmpStr);
                ss >> tmpStr;
                RenderableMesh* mesh = new RenderableMesh(node, m_renderer, m_resources);
                if (tmpStr.compare(RENDERABLEMESH_BOX_DESCRIPTION) == 0) {
                    double x, y, z;
                    ss >> x >> y >> z;
                    mesh->loadBox(x, y, z);
                }
                else if (tmpStr.compare(RENDERABLEMESH_FILE_DESCRIPTION) == 0) {
                    string file;
                    ss >> file;
                    mesh->loadFromFile(file);
                }
                else
                    cerr << "Error: unknown renderablemesh model type: " << tmpStr << endl;
            }
            else if (name.compare(XML_RIGIDBODY) == 0) {
                RigidBody* rigidbody = new RigidBody(node, m_physicsWorld);

                        tmpDouble1 = tree.get<double>(ptree::path_type(attrPath + XML_RIGIDBODY_MASS, XML_DELIMITER[0]), 0.0);

                tmpStr = tree.get<string>(ptree::path_type(attrPath + XML_RIGIDBODY_COLLISIONSHAPE, XML_DELIMITER[0]), "empty");
                stringstream ss(tmpStr);
                ss >> tmpStr;
                if (tmpStr.compare(COLLISION_SHAPE_CONVEX) == 0) {
                    string file;
                    ss >> file;
                    rigidbody->addConvexHull(tmpDouble1, file, m_resources);
                }
                else if (tmpStr.compare(COLLISION_SHAPE_BOX) == 0) {
                    double x, y, z;
                    ss >> x >> y >> z;
                    rigidbody->addBox(tmpDouble1, x, y, z);
                }
                else if (tmpStr.compare(COLLISION_SHAPE_SPHERE) == 0) {
                    double r;
                    ss >> r;
                    rigidbody->addSphere(tmpDouble1, r);
                }
                else if (tmpStr.compare(COLLISION_SHAPE_CAPSULE) == 0) {
                    double r, h;
                    ss >> r >> h;
                    rigidbody->addCapsule(tmpDouble1, r, h);
                }
                else if (tmpStr.compare(COLLISION_SHAPE_CYLINDER) == 0) {
                    double r, h;
                    ss >> r >> h;
                    rigidbody->addCylinder(tmpDouble1, r, h);
                }
                else if (tmpStr.compare(COLLISION_SHAPE_CONE) == 0) {
                    double r, h;
                    ss >> r >> h;
                    rigidbody->addCone(tmpDouble1, r, h);
                }
                else if (tmpStr.compare(COLLISION_SHAPE_CONCAVE) == 0) {
                    string file;
                    ss >> file;
                    rigidbody->addConvexHull(tmpDouble1, file, m_resources);
                }
                else
                    cerr << "Error: unknown rigidbody collisionshape: " << tmpStr << endl;

                tmpDouble1 = tree.get<double>(ptree::path_type(attrPath + XML_RIGIDBODY_FRICTION, XML_DELIMITER[0]), 0.5);
                rigidbody->setFriction(tmpDouble1);
                tmpDouble1 = tree.get<double>(ptree::path_type(attrPath + XML_RIGIDBODY_ROLLINGFRICTION, XML_DELIMITER[0]), 0.1);
                rigidbody->setRollingFriction(tmpDouble1);

                tmpStr = tree.get<string>(ptree::path_type(attrPath + XML_RIGIDBODY_DAMPING, XML_DELIMITER[0]), "0 0");
                stringstream damping;
                damping >> tmpDouble1 >> tmpDouble2;
                rigidbody->setDamping(tmpDouble1, tmpDouble2);

                tmpStr = tree.get<string>(ptree::path_type(attrPath + XML_RIGIDBODY_SLEEPINGTHRESHOLDS, XML_DELIMITER[0]), "0.8 1");
                stringstream sleeping;
                sleeping >> tmpDouble1 >> tmpDouble2;
                rigidbody->setSleepingThresholds(tmpDouble1, tmpDouble2);

                tmpDouble1 = tree.get<double>(ptree::path_type(attrPath + XML_RIGIDBODY_RESTITUTION, XML_DELIMITER[0]), 0.0);
                rigidbody->setRestitution(tmpDouble1);

                tmpVect = tree.get<Vector3>(ptree::path_type(attrPath + XML_RIGIDBODY_LINEARFACTOR, XML_DELIMITER[0]), VECTOR3_UNIT);
                rigidbody->setLinearFactor(tmpVect);
                tmpVect = tree.get<Vector3>(ptree::path_type(attrPath + XML_RIGIDBODY_LINEARVELOCITY, XML_DELIMITER[0]), VECTOR3_ZERO);
                rigidbody->setLinearVelocity(tmpVect);
                tmpVect = tree.get<Vector3>(ptree::path_type(attrPath + XML_RIGIDBODY_ANGULARFACTOR, XML_DELIMITER[0]), VECTOR3_UNIT);
                rigidbody->setAngularFactor(tmpVect);
                tmpVect = tree.get<Vector3>(ptree::path_type(attrPath + XML_RIGIDBODY_ANGULARVELOCITY, XML_DELIMITER[0]), VECTOR3_ZERO);
                rigidbody->setAngularVelocity(tmpVect);
                tmpVect = tree.get<Vector3>(ptree::path_type(attrPath + XML_RIGIDBODY_GRAVITY, XML_DELIMITER[0]), Vector3(0.0, -9.8, 0.0));
                rigidbody->setGravity(tmpVect);
            }
            else if (name.compare(XML_LIGHT) == 0) {
                Light* light = new Light(node, m_renderer);
                color4_t ambient = tree.get<color4_t>(ptree::path_type(attrPath + XML_LIGHT_AMBIENT, XML_DELIMITER[0]),
                                                      color4_t(0.0f, 0.0f, 0.0f, 1.0f));
                color4_t diffuse = tree.get<color4_t>(ptree::path_type(attrPath + XML_LIGHT_DIFFUSE, XML_DELIMITER[0]),
                                                      color4_t(1.0f, 1.0f, 1.0f, 1.0f));
                color4_t specular = tree.get<color4_t>(ptree::path_type(attrPath + XML_LIGHT_SPECULAR, XML_DELIMITER[0]),
                                                      color4_t(1.0f, 1.0f, 1.0f, 1.0f));
                light->set(ambient, diffuse, specular);
            }
            else if (name.compare(XML_CAMERA) == 0) {
                isCameraFound = true;
                tmpInt = tree.get<int>(ptree::path_type(attrPath + XML_CAMERA_TYPE, XML_DELIMITER[0]), 1);
                Camera* camera = new Camera(node, (camera_t)tmpInt, m_renderer);
//                 viewport_t view = tree.get<viewport_t>(ptree::path_type(attrPath + XML_CAMERA_VIEWPORT));
                                tmpDouble1 = tree.get<double>(ptree::path_type(attrPath + XML_CAMERA_PERSPECTIVEFOV), 45.0);
                camera->setPerspectiveFOV(tmpDouble1);
                                tmpDouble1 = tree.get<double>(ptree::path_type(attrPath + XML_CAMERA_ORTHOHEIGHT), 10.0);
                camera->setOrthoHeight(tmpDouble1);
                                tmpDouble1 = tree.get<double>(ptree::path_type(attrPath + XML_CAMERA_NEARDISTANCE), 0.1);
                camera->setNearDistance(tmpDouble1);
                                tmpDouble1 = tree.get<double>(ptree::path_type(attrPath + XML_CAMERA_FARDISTANCE), 1000.0);
                camera->setFarDistance(tmpDouble1);
            }
        }
        else if (type.compare(XML_ATTR_TYPE_ROOT) == 0) {
            cerr << "Error: invalid root node path: " << path << endl;
        }
        else
            cerr << "Error: unknown node type: " << type << endl;
    }
    return true;
}
