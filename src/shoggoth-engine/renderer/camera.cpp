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


#include "shoggoth-engine/renderer/camera.hpp"

#include "shoggoth-engine/kernel/entity.hpp"
#include "shoggoth-engine/renderer/renderer.hpp"

using namespace std;
using namespace boost::property_tree;

const string CAMERA_DESCRIPTION = "$camera";
const float DEFAULT_PERSP_FOV = 45.0f;
const float DEFAULT_ORTHO_HEIGHT = 10.0f;
const float DEFAULT_NEAR_DISTANCE = 0.1f;
const float DEFAULT_FAR_DISTANCE = 1000.0f;

const string XML_CAMERA_TYPE = "cameratype";
const string XML_CAMERA_VIEWPORT = "viewport";
const string XML_CAMERA_ASPECTRATIO = "aspectratio";
const string XML_CAMERA_PERSPECTIVEFOV = "perspectivefov";
const string XML_CAMERA_ORTHOHEIGHT = "orthoheight";
const string XML_CAMERA_NEARDISTANCE = "neardistance";
const string XML_CAMERA_FARDISTANCE = "fardistance";


Camera::Camera(Entity* const entity, Renderer* renderer):
    Component(COMPONENT_CAMERA, entity),
    m_cameraType(CAMERA_PROJECTION),
    m_renderer(renderer),
    m_hasChanged(true),
    m_viewport(),
    m_aspectRatio(0.0f),
    m_perspectiveFOV(DEFAULT_PERSP_FOV),
    m_orthoHeight(DEFAULT_ORTHO_HEIGHT),
    m_nearDistance(DEFAULT_NEAR_DISTANCE),
    m_farDistance(DEFAULT_FAR_DISTANCE)
{
    m_description = CAMERA_DESCRIPTION;

    m_renderer->registerCamera(this);

    m_entity->registerAttribute("type", boost::bind(&Camera::cmdCameraType, this, _1));
    m_entity->registerAttribute("perspective-fov", boost::bind(&Camera::cmdPerspectiveFOV, this, _1));
    m_entity->registerAttribute("ortho-height", boost::bind(&Camera::cmdOrthoHeight, this, _1));
    m_entity->registerAttribute("near-distance", boost::bind(&Camera::cmdNearDistance, this, _1));
    m_entity->registerAttribute("far-distance", boost::bind(&Camera::cmdFarDistance, this, _1));
}

Camera::~Camera() {
    m_entity->unregisterAttribute("far-distance");
    m_entity->unregisterAttribute("near-distance");
    m_entity->unregisterAttribute("ortho-height");
    m_entity->unregisterAttribute("perspective-fov");
    m_entity->unregisterAttribute("type");

    m_renderer->unregisterCamera(this);
}

void Camera::loadFromPtree(const string& path, const ptree& tree) {
//     viewport_t view = tree.get<viewport_t>(xmlPath(path + XML_CAMERA_VIEWPORT));
    m_cameraType = (camera_t)tree.get<int>(xmlPath(path + XML_CAMERA_TYPE), 1);
    m_perspectiveFOV = tree.get<double>(xmlPath(path + XML_CAMERA_PERSPECTIVEFOV), 45.0);
    m_orthoHeight = tree.get<double>(xmlPath(path + XML_CAMERA_ORTHOHEIGHT), 10.0);
    m_nearDistance = tree.get<double>(xmlPath(path + XML_CAMERA_NEARDISTANCE), 0.1);
    m_farDistance = tree.get<double>(xmlPath(path + XML_CAMERA_FARDISTANCE), 1000.0);
}

void Camera::saveToPtree(const string& path, ptree& tree) const {
//     tree.put(xmlPath(path + XML_CAMERA_VIEWPORT), getViewport());
    tree.put(xmlPath(path + XML_CAMERA_TYPE), (int)getCameraType());
    tree.put(xmlPath(path + XML_CAMERA_PERSPECTIVEFOV), getPerspectiveFOV());
    tree.put(xmlPath(path + XML_CAMERA_ORTHOHEIGHT), getOrthoHeight());
    tree.put(xmlPath(path + XML_CAMERA_NEARDISTANCE), getNearDistance());
    tree.put(xmlPath(path + XML_CAMERA_FARDISTANCE), getFarDistance());
}



Camera::Camera(const Camera& rhs):
    Component(rhs.m_type, rhs.m_entity),
    m_cameraType(rhs.m_cameraType),
    m_renderer(rhs.m_renderer),
    m_hasChanged(rhs.m_hasChanged),
    m_viewport(rhs.m_viewport),
    m_aspectRatio(rhs.m_aspectRatio),
    m_perspectiveFOV(rhs.m_perspectiveFOV),
    m_orthoHeight(rhs.m_orthoHeight),
    m_nearDistance(rhs.m_nearDistance),
    m_farDistance(rhs.m_farDistance)
{
    cerr << "Error: Camera copy constructor should not be called!" << endl;
}

Camera& Camera::operator=(const Camera&) {
    cerr << "Error: Camera assignment operator should not be called!" << endl;
    return *this;
}

string Camera::cmdCameraType(deque<string>& args) {
    if (args.size() < 1)
        return "Error: too few arguments";
    int type = boost::lexical_cast<int>(args[0]);
    setCameraType(static_cast<camera_t>(type));
    return "";
}

string Camera::cmdPerspectiveFOV(deque<string>& args) {
    if (args.size() < 1)
        return "Error: too few arguments";
    float perspectiveFOV = boost::lexical_cast<float>(args[0]);
    setPerspectiveFOV(perspectiveFOV);
    return "";
}

string Camera::cmdOrthoHeight(deque<string>& args) {
    if (args.size() < 1)
        return "Error: too few arguments";
    float orthoHeight = boost::lexical_cast<float>(args[0]);
    setOrthoHeight(orthoHeight);
    return "";
}

string Camera::cmdNearDistance(deque<string>& args) {
    if (args.size() < 1)
        return "Error: too few arguments";
    float nearDistance = boost::lexical_cast<float>(args[0]);
    setNearDistance(nearDistance);
    return "";
}

string Camera::cmdFarDistance(deque<string>& args) {
    if (args.size() < 1)
        return "Error: too few arguments";
    float farDistance = boost::lexical_cast<float>(args[0]);
    setFarDistance(farDistance);
    return "";
}
