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


#include "shoggoth-engine/kernel/model.hpp"

#include <iostream>
#include "shoggoth-engine/kernel/modelloader.hpp"

using namespace std;

Model::Model(const std::string& identifier):
    m_identifier(identifier),
    m_meshes()
{}

void Model::generateBox(const double lengthX, const double lengthY, const double lengthZ) {
    float x = float(lengthX * 0.5);
    float y = float(lengthY * 0.5);
    float z = float(lengthZ * 0.5);
    float vertices[][12] = {
        { x,  y,  z,  -x,  y,  z,  -x, -y,  z,   x, -y,  z}, // front
        { x,  y,  z,   x,  y, -z,  -x,  y, -z,  -x,  y,  z}, // top
        {-x, -y, -z,   x, -y, -z,   x, -y,  z,  -x, -y,  z}, // bottom
        { x,  y,  z,   x, -y,  z,   x, -y, -z,   x,  y, -z}, // right
        {-x,  y,  z,  -x,  y, -z,  -x, -y, -z,  -x, -y,  z}, // left
        { x, -y, -z,  -x, -y, -z,  -x,  y, -z,   x,  y, -z}  // back
    };

    float normals[][12] = {
        { 0,  0,  1,   0,  0,  1,   0,  0,  1,   0,  0,  1}, // front
        { 0,  1,  0,   0,  1,  0,   0,  1,  0,   0,  1,  0}, // top
        { 0, -1,  0,   0, -1,  0,   0, -1,  0,   0, -1,  0}, // bottom
        { 1,  0,  0,   1,  0,  0,   1,  0,  0,   1,  0,  0}, // right
        {-1,  0,  0,  -1,  0,  0,  -1,  0,  0,  -1,  0,  0}, // left
        { 0,  0, -1,   0,  0, -1,   0,  0, -1,   0,  0, -1}  // back
    };

    unsigned int indices[] = {0,  1,  2,   2,  3,  0};

    m_meshes.resize(6);
    for (size_t i = 0; i < m_meshes.size(); ++i) {
        mesh(i)->setVertices(vertices[i], 12);
        mesh(i)->setNormals(normals[i], 12);
        mesh(i)->setIndices(indices, 6);
    }
}

void Model::generateFromFile(const string& fileName) {
    ModelLoader::load(fileName, *this);
}

Model::Model(const Model& rhs):
    m_identifier(rhs.m_identifier),
    m_meshes(rhs.m_meshes)
{
    cerr << "Error: Model copy constructor should not be called!" << endl;
}

Model& Model::operator=(const Model&) {
    cerr << "Error: Model assignment operator should not be called!" << endl;
    return *this;
}
