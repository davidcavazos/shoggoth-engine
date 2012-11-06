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


#include "engine/kernel/scene.hpp"

#include <iostream>
#include <fstream>
#include <boost/foreach.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include "engine/kernel/xmlinfo.hpp"
#include "engine/kernel/entity.hpp"
#include "engine/renderer/camera.hpp"
#include "engine/renderer/light.hpp"
#include "engine/renderer/renderablemesh.hpp"
#include "engine/resources/model.hpp"
#include "engine/physics/rigidbody.hpp"

using namespace std;
using namespace boost::property_tree;

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
    cout << "Removing all entities and their components" << endl;
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
    map<string, Component*>::const_iterator it;
    for (it = node->getComponentBegin(); it != node->getComponentEnd(); ++it) {
        const Component* component = it->second;
        comp = compPath + component->getType();
        attrPath = comp + XML_DELIMITER + XML_ATTRIBUTE + XML_DELIMITER;
        attr = attrPath + XML_ATTR_TYPE;
        tree.put(ptree::path_type(attr, XML_DELIMITER[0]), XML_ATTR_TYPE_COMPONENT);
        component->saveToPtree(attrPath, tree);
        tree.put(ptree::path_type(comp, XML_DELIMITER[0]), "");
    }

    // traverse all children
    Entity::const_child_iterator_t itChild;
    for (itChild = node->getChildrenBegin(); itChild != node->getChildrenEnd(); ++itChild)
        saveToPTree(curPath, tree, *itChild);
}

bool Scene::loadFromPTree(const string& path,
                          const ptree& tree,
                          Entity* node,
                          Entity* parent,
                          set<string>& names,
                          bool& isCameraFound) {
    // load attributes
    string attrPath = path + XML_DELIMITER + XML_ATTRIBUTE + XML_DELIMITER;
    if (parent != 0) {
        Vector3 vect;
        Quaternion quat;
        vect = tree.get<Vector3>(ptree::path_type(attrPath + XML_ATTR_POSITION, XML_DELIMITER[0]));
        node->setPositionAbs(vect);
        quat = tree.get<Quaternion>(ptree::path_type(attrPath + XML_ATTR_ORIENTATION, XML_DELIMITER[0]));
        node->setOrientationAbs(quat);
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
            Component* component = 0;
            if (name.compare(COMPONENT_RENDERABLEMESH) == 0)
                component = new RenderableMesh(node, m_renderer, m_resources);
            else if (name.compare(COMPONENT_RIGIDBODY) == 0)
                component = new RigidBody(node, m_resources, m_physicsWorld);
            else if (name.compare(COMPONENT_LIGHT) == 0)
                component = new Light(node, m_renderer);
            else if (name.compare(COMPONENT_CAMERA) == 0) {
                component = new Camera(node, m_renderer);
                isCameraFound = true;
            }
            else
                cerr << "Error: unknown component: " << name << endl;
            if (component != 0)
                component->loadFromPtree(attrPath, tree);
        }
        else if (type.compare(XML_ATTR_TYPE_ROOT) == 0)
            cerr << "Error: invalid root node path: " << path << endl;
        else
            cerr << "Error: unknown node type: " << type << endl;
    }
    return true;
}
