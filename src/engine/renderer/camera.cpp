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


#include "engine/renderer/camera.hpp"

#include "engine/kernel/entity.hpp"
#include "engine/renderer/renderer.hpp"

using namespace std;

const string CAMERA_DESCRIPTION = "$camera";
const float DEFAULT_PERSP_FOV = 45.0f;
const float DEFAULT_ORTHO_HEIGHT = 10.0f;
const float DEFAULT_NEAR_DISTANCE = 0.1f;
const float DEFAULT_FAR_DISTANCE = 1000.0f;

Camera::Camera(Entity* const entity, const camera_t type, Renderer* renderer):
    Component(COMPONENT_CAMERA, entity),
    m_type(type),
    m_renderer(renderer),
    m_hasChanged(true),
    m_viewport(),
    m_aspectRatio(0.0f),
    m_perspectiveFOV(DEFAULT_PERSP_FOV),
    m_orthoHeight(DEFAULT_ORTHO_HEIGHT),
    m_nearDistance(DEFAULT_NEAR_DISTANCE),
    m_farDistance(DEFAULT_FAR_DISTANCE)
{
    m_description = CAMERA_DESCRIPTION + "_";
    switch (m_type) {
    case CAMERA_ORTHOGRAPHIC:
        m_description.append("ORTHOGRAPHIC");
        break;
    case CAMERA_PROJECTION:
        m_description.append("PROJECTION");
        break;
    default:
        m_description.append("INVALID");
    }

    m_renderer->m_cameras.insert(this);
    m_renderer->m_activeCamera = this;

    m_entity->registerAttribute("type", boost::bind(&Camera::cmdType, this, _1));
    m_entity->registerAttribute("perspective-fov", boost::bind(&Camera::cmdPerspectiveFOV, this, _1));
    m_entity->registerAttribute("ortho-height", boost::bind(&Camera::cmdOrthoHeight, this, _1));
    m_entity->registerAttribute("near-distance", boost::bind(&Camera::cmdNearDistance, this, _1));
    m_entity->registerAttribute("far-distance", boost::bind(&Camera::cmdFarDistance, this, _1));
}

Camera::~Camera() {
    m_renderer->m_cameras.erase(this);
    m_entity->unregisterAttribute("far-distance");
    m_entity->unregisterAttribute("near-distance");
    m_entity->unregisterAttribute("ortho-height");
    m_entity->unregisterAttribute("perspective-fov");
    m_entity->unregisterAttribute("type");
}



Camera::Camera(const Camera& rhs):
    Component(COMPONENT_CAMERA, rhs.m_entity),
    m_type(rhs.m_type),
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

void Camera::cmdType(const std::string& arg) {
    int type;
    stringstream ss(arg);
    ss >> type;
    setType(static_cast<camera_t>(type));
}

void Camera::cmdPerspectiveFOV(const std::string& arg) {
    float perspectiveFOV;
    stringstream ss(arg);
    ss >> perspectiveFOV;
    setPerspectiveFOV(perspectiveFOV);
}

void Camera::cmdOrthoHeight(const std::string& arg) {
    float orthoHeight;
    stringstream ss(arg);
    ss >> orthoHeight;
    setOrthoHeight(orthoHeight);
}

void Camera::cmdNearDistance(const std::string& arg) {
    float nearDistance;
    stringstream ss(arg);
    ss >> nearDistance;
    setNearDistance(nearDistance);
}

void Camera::cmdFarDistance(const std::string& arg) {
    float farDistance;
    stringstream ss(arg);
    ss >> farDistance;
    setFarDistance(farDistance);
}
