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
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include "engine/renderer/camera.hpp"
#include <engine/renderer/light.hpp>
#include <engine/renderer/renderablemesh.hpp>
#include <engine/resources/model.hpp>
#include <engine/physics/rigidbody.hpp>

using namespace std;
using namespace boost::property_tree;

const string XML_PTREE_DELIMITER = "/";
const string XML_PTREE_ATTRIBUTE = "<xmlattr>";
const string XML_PTREE_ATTR_POSITION = "position";
const string XML_PTREE_ATTR_ORIENTATION = "orientation";
const string XML_PTREE_ATTR_TYPE = "type";
const string XML_PTREE_ATTR_TYPE_ENTITY = "entity";
const string XML_PTREE_ATTR_TYPE_COMPONENT = "component";

const string XML_PTREE_CAMERA = "camera";
const string XML_PTREE_CAMERA_VIEWPORT = "viewport";
const string XML_PTREE_CAMERA_ASPECTRATIO = "aspectratio";
const string XML_PTREE_CAMERA_PERSPECTIVEFOV = "perspectivefov";
const string XML_PTREE_CAMERA_ORTHOHEIGHT = "orthoheight";
const string XML_PTREE_CAMERA_NEARDISTANCE = "neardistance";
const string XML_PTREE_CAMERA_FARDISTANCE = "fardistance";

const string XML_PTREE_LIGHT = "light";
const string XML_PTREE_LIGHT_AMBIENT = "ambient";
const string XML_PTREE_LIGHT_DIFFUSE = "diffuse";
const string XML_PTREE_LIGHT_SPECULAR = "specular";

const string XML_PTREE_RENDERABLEMESH = "renderablemesh";
const string XML_PTREE_RENDERABLEMESH_MODEL = "model";

const string XML_PTREE_RIGIDBODY = "rigidbody";
const string XML_PTREE_RIGIDBODY_COLLISIONSHAPE = "collisionshape";
const string XML_PTREE_RIGIDBODY_MASS = "mass";
const string XML_PTREE_RIGIDBODY_DAMPING = "damping";
const string XML_PTREE_RIGIDBODY_FRICTION = "friction";
const string XML_PTREE_RIGIDBODY_ROLLINGFRICTION = "rollingfriction";
const string XML_PTREE_RIGIDBODY_RESTITUTION = "restitution";
const string XML_PTREE_RIGIDBODY_SLEEPINGTHRESHOLDS = "sleepingthresholds";
const string XML_PTREE_RIGIDBODY_LINEARFACTOR = "linearfactor";
const string XML_PTREE_RIGIDBODY_LINEARVELOCITY = "linearvelocity";
const string XML_PTREE_RIGIDBODY_ANGULARFACTOR = "angularfactor";
const string XML_PTREE_RIGIDBODY_ANGULARVELOCITY = "angularvelocity";
const string XML_PTREE_RIGIDBODY_GRAVITY = "gravity";

void saveToPTree(const string& path, ptree& tree, const Entity* node) {
    // save current node
    string curPath = path + XML_PTREE_DELIMITER + node->getObjectName();
    tree.put(ptree::path_type(curPath, XML_PTREE_DELIMITER[0]), "");

    // save attributes
    string attr;
    string attrPath = curPath + XML_PTREE_DELIMITER + XML_PTREE_ATTRIBUTE + XML_PTREE_DELIMITER;
    attr = attrPath + XML_PTREE_ATTR_TYPE;
    tree.put(ptree::path_type(attr, XML_PTREE_DELIMITER[0]), XML_PTREE_ATTR_TYPE_ENTITY);
    attr = attrPath + XML_PTREE_ATTR_POSITION;
    tree.put(ptree::path_type(attr, XML_PTREE_DELIMITER[0]), node->getPositionAbs());
    attr = attrPath + XML_PTREE_ATTR_ORIENTATION;
    tree.put(ptree::path_type(attr, XML_PTREE_DELIMITER[0]), node->getOrientationAbs());

    // components
    string comp;
    string compPath = curPath + XML_PTREE_DELIMITER;
    for (size_t i = 0; i < TOTAL_COMPONENTS_CONTAINER_SIZE; ++i) {
        const Component* component = node->getComponent((component_t)i);
        if (component != 0) {
            switch (component->getType()) {
            case COMPONENT_CAMERA: {
                Camera* camera = (Camera*)component;
                comp = compPath + XML_PTREE_CAMERA;
                attrPath = comp + XML_PTREE_DELIMITER + XML_PTREE_ATTRIBUTE + XML_PTREE_DELIMITER;
                attr = attrPath + XML_PTREE_ATTR_TYPE;
                tree.put(ptree::path_type(attr, XML_PTREE_DELIMITER[0]), XML_PTREE_ATTR_TYPE_COMPONENT);
                attr = attrPath + XML_PTREE_CAMERA_VIEWPORT;
                tree.put(ptree::path_type(attr, XML_PTREE_DELIMITER[0]), camera->getViewport());
                attr = attrPath + XML_PTREE_CAMERA_ASPECTRATIO;
                tree.put(ptree::path_type(attr, XML_PTREE_DELIMITER[0]), camera->getAspectRatio());
                attr = attrPath + XML_PTREE_CAMERA_PERSPECTIVEFOV;
                tree.put(ptree::path_type(attr, XML_PTREE_DELIMITER[0]), camera->getPerspectiveFOV());
                attr = attrPath + XML_PTREE_CAMERA_ORTHOHEIGHT;
                tree.put(ptree::path_type(attr, XML_PTREE_DELIMITER[0]), camera->getOrthoHeight());
                attr = attrPath + XML_PTREE_CAMERA_NEARDISTANCE;
                tree.put(ptree::path_type(attr, XML_PTREE_DELIMITER[0]), camera->getNearDistance());
                attr = attrPath + XML_PTREE_CAMERA_FARDISTANCE;
                tree.put(ptree::path_type(attr, XML_PTREE_DELIMITER[0]), camera->getFarDistance());
                break; }
            case COMPONENT_LIGHT: {
                Light* light = (Light*)component;
                comp = compPath + XML_PTREE_LIGHT;
                attrPath = comp + XML_PTREE_DELIMITER + XML_PTREE_ATTRIBUTE + XML_PTREE_DELIMITER;
                attr = attrPath + XML_PTREE_ATTR_TYPE;
                tree.put(ptree::path_type(attr, XML_PTREE_DELIMITER[0]), XML_PTREE_ATTR_TYPE_COMPONENT);
                attr = attrPath + XML_PTREE_LIGHT_AMBIENT;
                tree.put(ptree::path_type(attr, XML_PTREE_DELIMITER[0]), light->getAmbient());
                attr = attrPath + XML_PTREE_LIGHT_DIFFUSE;
                tree.put(ptree::path_type(attr, XML_PTREE_DELIMITER[0]), light->getDiffuse());
                attr = attrPath + XML_PTREE_LIGHT_SPECULAR;
                tree.put(ptree::path_type(attr, XML_PTREE_DELIMITER[0]), light->getSpecular());
                break; }
            case COMPONENT_RENDERABLE_MESH: {
                RenderableMesh* mesh = (RenderableMesh*)component;
                comp = compPath + XML_PTREE_RENDERABLEMESH;
                attrPath = comp + XML_PTREE_DELIMITER + XML_PTREE_ATTRIBUTE + XML_PTREE_DELIMITER;
                attr = attrPath + XML_PTREE_ATTR_TYPE;
                tree.put(ptree::path_type(attr, XML_PTREE_DELIMITER[0]), XML_PTREE_ATTR_TYPE_COMPONENT);
                attr = attrPath + XML_PTREE_RENDERABLEMESH_MODEL;
                tree.put(ptree::path_type(attr, XML_PTREE_DELIMITER[0]), mesh->getModel()->getIdentifier());
                break; }
            case COMPONENT_RIGIDBODY: {
                RigidBody* rigidbody = (RigidBody*)component;
                comp = compPath + XML_PTREE_RIGIDBODY;
                attrPath = comp + XML_PTREE_DELIMITER + XML_PTREE_ATTRIBUTE + XML_PTREE_DELIMITER;
                attr = attrPath + XML_PTREE_ATTR_TYPE;
                tree.put(ptree::path_type(attr, XML_PTREE_DELIMITER[0]), XML_PTREE_ATTR_TYPE_COMPONENT);
                attr = attrPath + XML_PTREE_RIGIDBODY_COLLISIONSHAPE;
                tree.put(ptree::path_type(attr, XML_PTREE_DELIMITER[0]), rigidbody->getShapeId());
                attr = attrPath + XML_PTREE_RIGIDBODY_MASS;
                tree.put(ptree::path_type(attr, XML_PTREE_DELIMITER[0]), rigidbody->getMass());
                attr = attrPath + XML_PTREE_RIGIDBODY_DAMPING;
                stringstream damping;
                damping << rigidbody->getLinearDamping() << " " << rigidbody->getAngularDamping();
                tree.put(ptree::path_type(attr, XML_PTREE_DELIMITER[0]), damping.str());
                attr = attrPath + XML_PTREE_RIGIDBODY_FRICTION;
                tree.put(ptree::path_type(attr, XML_PTREE_DELIMITER[0]), rigidbody->getFriction());
                attr = attrPath + XML_PTREE_RIGIDBODY_ROLLINGFRICTION;
                tree.put(ptree::path_type(attr, XML_PTREE_DELIMITER[0]), rigidbody->getRollingFriction());
                attr = attrPath + XML_PTREE_RIGIDBODY_RESTITUTION;
                tree.put(ptree::path_type(attr, XML_PTREE_DELIMITER[0]), rigidbody->getRestitution());
                attr = attrPath + XML_PTREE_RIGIDBODY_SLEEPINGTHRESHOLDS;
                stringstream sleeping;
                sleeping << rigidbody->getLinearSleepingThreshold() << " " << rigidbody->getAngularSleepingThreshold();
                tree.put(ptree::path_type(attr, XML_PTREE_DELIMITER[0]), sleeping.str());
                attr = attrPath + XML_PTREE_RIGIDBODY_LINEARFACTOR;
                tree.put(ptree::path_type(attr, XML_PTREE_DELIMITER[0]), rigidbody->getLinearFactor());
                attr = attrPath + XML_PTREE_RIGIDBODY_LINEARVELOCITY;
                tree.put(ptree::path_type(attr, XML_PTREE_DELIMITER[0]), rigidbody->getLinearVelocity());
                attr = attrPath + XML_PTREE_RIGIDBODY_ANGULARFACTOR;
                tree.put(ptree::path_type(attr, XML_PTREE_DELIMITER[0]), rigidbody->getAngularFactor());
                attr = attrPath + XML_PTREE_RIGIDBODY_ANGULARVELOCITY;
                tree.put(ptree::path_type(attr, XML_PTREE_DELIMITER[0]), rigidbody->getAngularVelocity());
                attr = attrPath + XML_PTREE_RIGIDBODY_GRAVITY;
                tree.put(ptree::path_type(attr, XML_PTREE_DELIMITER[0]), rigidbody->getGravity());
                break; }
            default:
                cerr << "Error: invalid component_t: " << component->getType() << endl;
            }
            tree.put(ptree::path_type(comp, XML_PTREE_DELIMITER[0]), "");
        }
    }

    // traverse all children
    Entity::const_child_iterator_t it;
    for (it = node->getChildrenBegin(); it != node->getChildrenEnd(); ++it)
        saveToPTree(curPath, tree, *it);
}

void loadFromPTree(const ptree& tree, Entity* node) {
}



map<string, Entity*> Scene::ms_entities = map<string, Entity*>();

Scene::Scene(const string& objectName, const string& rootNodeName, const Device* device):
    CommandObject(objectName),
    m_root(0, rootNodeName, device)
{
    registerCommand("initialize", boost::bind(&Scene::cmdInitialize, this, _1));
    registerCommand("shutdown", boost::bind(&Scene::cmdShutdown, this, _1));
    registerCommand("save-to-xml", boost::bind(&Scene::cmdSaveToXML, this, _1));
    registerCommand("load-from-xml", boost::bind(&Scene::cmdLoadFromXML, this, _1));
}

Scene::~Scene() {
    unregisterAllCommands();
    unregisterAllAttributes();
}

void Scene::initialize() {
}

void Scene::shutdown() {
    cout << "Removing all entities" << endl;
    m_root.removeAllChildren();
}

void Scene::saveToXML(const string& fileName) const {
    cout << "Saving scene to XML file: " << fileName << endl;
    ptree tree;
    xml_writer_settings<char> settings(' ', 2);
    saveToPTree("scene", tree, &m_root);
    write_xml(fileName, tree, std::locale(), settings);
}

void Scene::loadFromXML(const string& fileName) {
    cout << "Loading scene from XML file: " << fileName << endl;
    ptree tree;
    m_root.removeAllChildren();
    read_xml(fileName, tree, xml_parser::trim_whitespace);
    loadFromPTree(tree, &m_root);
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
    ss << m_root.treeToString(0);
    return ss.str();
}
