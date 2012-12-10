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


#ifndef COMPONENTFACTORY_HPP
#define COMPONENTFACTORY_HPP

#include <string>

class Entity;
class Component;
class Renderer;
class PhysicsWorld;

class ComponentFactory {
public:
    ComponentFactory(Renderer* renderer, PhysicsWorld* physicsWorld);
    ComponentFactory(const ComponentFactory& rhs);
    virtual ~ComponentFactory();

    ComponentFactory& operator=(const ComponentFactory& rhs);

    virtual Component* create(const std::string& name, Entity* entity) const = 0;

protected:
    Renderer* m_renderer;
    PhysicsWorld* m_physicsWorld;
};


class DefaultComponentFactory: public ComponentFactory {
public:
    DefaultComponentFactory(Renderer* renderer, PhysicsWorld* physicsWorld);
    Component* create(const std::string& name, Entity* entity) const;
};

#endif // COMPONENTFACTORY_HPP
