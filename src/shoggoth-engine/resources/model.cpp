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


#include "shoggoth-engine/resources/model.hpp"

#include <iostream>

using namespace std;

Model::Model(const std::string& identifier, Renderer* renderer):
    m_identifier(identifier),
    m_renderer(renderer),
    m_meshes()
{}

void Model::uploadToGPU() {
    for (size_t i = 0; i < m_meshes.size(); ++i)
        m_meshes[i].uploadToGPU();
}

Model::Model(const Model& rhs):
    m_identifier(rhs.m_identifier),
    m_renderer(rhs.m_renderer),
    m_meshes(rhs.m_meshes)
{
    cerr << "Error: Model copy constructor should not be called!" << endl;
}

Model& Model::operator=(const Model&) {
    cerr << "Error: Model assignment operator should not be called!" << endl;
    return *this;
}
