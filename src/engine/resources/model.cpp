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


#include "engine/resources/model.hpp"

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
