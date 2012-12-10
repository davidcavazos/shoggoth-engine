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


#ifndef MODEL_HPP
#define MODEL_HPP

#include <string>
#include "mesh.hpp"

class ModelLoader;

class Model {
public:
    friend class ModelLoader;

    Model(const std::string& identifier);

    const std::string& getIdentifier() const;
    size_t getTotalMeshes() const;
    const Mesh* getMesh(const size_t index) const;
    Mesh* mesh(const size_t index);
    void setIdentifier(const std::string& identifier);

    void generateBox(const double lengthX, const double lengthY, const double lengthZ);
    void generateFromFile(const std::string& fileName);

private:
    std::string m_identifier;
    std::vector<Mesh> m_meshes;

    Model(const Model& rhs);
    Model& operator=(const Model&);
};



inline const std::string& Model::getIdentifier() const {
    return m_identifier;
}

inline size_t Model::getTotalMeshes() const {
    return m_meshes.size();
}

inline const Mesh* Model::getMesh(const size_t index) const {
    return &m_meshes[index];
}

inline Mesh* Model::mesh(const size_t index) {
    return &m_meshes[index];
}

inline void Model::setIdentifier(const std::string& identifier) {
    m_identifier = identifier;
}

#endif // MODEL_HPP
