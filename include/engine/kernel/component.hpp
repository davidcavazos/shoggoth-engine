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


#ifndef COMPONENT_HPP
#define COMPONENT_HPP

#include <string>

class Entity;

typedef enum {
    COMPONENT_CAMERA,
    COMPONENT_LIGHT,
    COMPONENT_RENDERABLE_MESH,
    COMPONENT_RIGIDBODY,
    TOTAL_COMPONENTS_CONTAINER_SIZE // Do not use as a component, gives size to component container
} component_t;

class Component {
public:
    Component(const component_t type, Entity* const entity);
    virtual ~Component();

    const Entity* getEntity() const;
    Entity* entity();
    const component_t& getType() const;
    const std::string& getDescription() const;

protected:
    Entity* m_entity;
    const component_t m_type;
    std::string m_description;

private:
    Component(const Component& rhs);
    Component& operator=(const Component&);
};

std::ostream& operator<<(std::ostream& out, const Component& rhs);



inline const Entity* Component::getEntity() const {
    return m_entity;
}

inline Entity* Component::entity() {
    return m_entity;
}

inline const component_t& Component::getType() const {
    return m_type;
}

inline const std::string& Component::getDescription() const {
    return m_description;
}

#endif // COMPONENT_HPP
