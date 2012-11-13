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


#include "testcomponent.hpp"

#include "shoggoth-engine/kernel/entity.hpp"

using namespace std;
using namespace boost::property_tree;

const string XML_HEALTH = "health";

TestComponent::TestComponent(Entity* const entity):
    Component(COMPONENT_TESTCOMPONENT, entity),
    m_health(100.0)
{
    m_entity->registerAttribute("health", boost::bind(&TestComponent::cmdHealth, this, _1));
}

TestComponent::~TestComponent() {
    m_entity->unregisterAttribute("health");
}


void TestComponent::loadFromPtree(const string& path, const ptree& tree) {
    m_health = tree.get<double>(ptree::path_type(path + XML_HEALTH, XML_DELIMITER[0]), 100.0);
}

void TestComponent::saveToPtree(const string& path, ptree& tree) const {
    string attr;
    attr = path + XML_HEALTH;
    tree.put(ptree::path_type(attr, XML_DELIMITER[0]), getHealth());
}



string TestComponent::cmdHealth(deque<string>& arg) {
    stringstream ss(arg[0]);
    ss >> m_health;
    return string();
}
