/*
    Copyright (c) 2012 David Cavazos <davido262@gmail.com>

    Permission is hereby granted, free of charge, to any person
    obtaining a copy of this software and associated documentation
    files (the "Software"), to deal in the Software without
    restriction, including without limitation the rights to use,
    copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following
    conditions:

    The above copyright notice and this permission notice shall be
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
    OTHER DEALINGS IN THE SOFTWARE.
*/


#include "engine/kernel/componentfactory.hpp"

#include "engine/renderer/camera.hpp"
#include "engine/renderer/light.hpp"
#include "engine/renderer/renderablemesh.hpp"
#include "engine/resources/model.hpp"
#include "engine/physics/rigidbody.hpp"

using namespace std;

ComponentFactory::ComponentFactory(Renderer* renderer, Resources* resources, PhysicsWorld* physicsWorld):
    m_renderer(renderer),
    m_resources(resources),
    m_physicsWorld(physicsWorld)
{}

ComponentFactory::ComponentFactory(const ComponentFactory& rhs):
    m_renderer(rhs.m_renderer),
    m_resources(rhs.m_resources),
    m_physicsWorld(rhs.m_physicsWorld)
{}

ComponentFactory::~ComponentFactory() {
}

ComponentFactory& ComponentFactory::operator=(const ComponentFactory& rhs) {
    if (this == &rhs)
        return *this;
    m_renderer = rhs.m_renderer;
    m_resources = rhs.m_resources;
    m_physicsWorld = rhs.m_physicsWorld;
    return *this;
}



DefaultComponentFactory::DefaultComponentFactory(Renderer* renderer, Resources* resources, PhysicsWorld* physicsWorld):
    ComponentFactory(renderer, resources, physicsWorld)
{}

Component* DefaultComponentFactory::create(const string& name, Entity* entity) const {
    if (name.compare(COMPONENT_RENDERABLEMESH) == 0)
        return new RenderableMesh(entity, m_renderer, m_resources);
    else if (name.compare(COMPONENT_RIGIDBODY) == 0)
        return new RigidBody(entity, m_resources, m_physicsWorld);
    else if (name.compare(COMPONENT_LIGHT) == 0)
        return new Light(entity, m_renderer);
    else if (name.compare(COMPONENT_CAMERA) == 0)
        return new Camera(entity, m_renderer);
    return 0;
}

