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


#ifndef RESOURCES_HPP
#define RESOURCES_HPP

#include <string>
#include <boost/unordered_map.hpp>
#include "engine/kernel/commandobject.hpp"

class Renderer;
class Model;
class Texture;

class Resources: public CommandObject {
public:
    Resources(const std::string& objectName, Renderer* renderer);
    ~Resources();

    void initialize();
    void shutdown();
    Model* generateBox(const std::string& identifier, const double lengthX, const double lengthY, const double lengthZ);
    Model* generateModelFromFile(const std::string& fileName);
    Texture* generateTextureFromFile(const std::string& fileName);
    std::string listsToString();

private:
    typedef boost::unordered_map<std::string, Model*> models_map_t;
    typedef boost::unordered_map<std::string, Texture*> textures_map_t;
    Renderer* m_renderer;
    models_map_t m_modelsMap;
    textures_map_t m_texturesMap;

    Resources(const Resources& rhs);
    Resources& operator=(const Resources&);

    void registerModel(Model* model);
    void registerTexture(Texture* texture);
    Model* findModel(const std::string& identifier);
    Texture* findTexture(const std::string& fileName);

    std::string cmdInitialize(std::deque<std::string>&);
    std::string cmdShutdown(std::deque<std::string>&);
};



inline std::string Resources::cmdInitialize(std::deque<std::string>&) {
    initialize();
    return std::string();
}

inline std::string Resources::cmdShutdown(std::deque<std::string>&) {
    shutdown();
    return std::string();
}

#endif // RESOURCES_HPP
