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
    //    v6----- v5
    //   /|      /|
    //  v1------v0|
    //  | |     | |
    //  | |v7---|-|v4
    //  |/      |/
    //  v2------v3
    float vertices[][12] = {
        { x,  y,  z,  -x,  y,  z,  -x, -y,  z,   x, -y,  z}, // v0,v1,v2,v3 front
        { x,  y,  z,   x,  y, -z,  -x,  y, -z,  -x,  y,  z}, // v0,v5,v6,v1 top
        {-x, -y, -z,   x, -y, -z,   x, -y,  z,  -x, -y,  z}, // v7,v4,v3,v2 bottom
        { x,  y,  z,   x, -y,  z,   x, -y, -z,   x,  y, -z}, // v0,v3,v4,v5 right
        {-x,  y,  z,  -x,  y, -z,  -x, -y, -z,  -x, -y,  z}, // v1,v6,v7,v2 left
        { x, -y, -z,  -x, -y, -z,  -x,  y, -z,   x,  y, -z}  // v4,v7,v6,v5 back
    };

    float normals[][12] = {
        { 0,  0,  1,   0,  0,  1,   0,  0,  1,   0,  0,  1}, // v0,v1,v2,v3 front
        { 0,  1,  0,   0,  1,  0,   0,  1,  0,   0,  1,  0}, // v0,v5,v6,v1 top
        { 0, -1,  0,   0, -1,  0,   0, -1,  0,   0, -1,  0}, // v7,v4,v3,v2 bottom
        { 1,  0,  0,   1,  0,  0,   1,  0,  0,   1,  0,  0}, // v0,v3,v4,v5 right
        {-1,  0,  0,  -1,  0,  0,  -1,  0,  0,  -1,  0,  0}, // v1,v6,v7,v2 left
        { 0,  0, -1,   0,  0, -1,   0,  0, -1,   0,  0, -1}  // v4,v7,v6,v5 back
    };

    float uvCoords[][8] = {
        {1, 1,  0, 1,  0, 0,  1, 0}, // v0,v1,v2,v3 front
        {1, 1,  0, 1,  0, 0,  1, 0}, // v0,v5,v6,v1 top
        {1, 1,  0, 1,  0, 0,  1, 0}, // v7,v4,v3,v2 bottom
        {1, 1,  0, 1,  0, 0,  1, 0}, // v0,v3,v4,v5 right
        {1, 1,  0, 1,  0, 0,  1, 0}, // v1,v6,v7,v2 left
        {1, 1,  0, 1,  0, 0,  1, 0}  // v4,v7,v6,v5 back
    };

    unsigned int indices[] = {0,  1,  2,   2,  3,  0};

    m_meshes.resize(6);
    for (size_t i = 0; i < m_meshes.size(); ++i) {
        mesh(i)->setVertices(vertices[i], 12);
        mesh(i)->setNormals(normals[i], 12);
        mesh(i)->setUvCoords(uvCoords[i], 8);
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
