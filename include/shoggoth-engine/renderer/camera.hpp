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


#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <iostream>
#include "shoggoth-engine/kernel/component.hpp"

const std::string COMPONENT_CAMERA = "camera";

class Renderer;

typedef enum {
    CAMERA_ORTHOGRAPHIC,
    CAMERA_PROJECTION
} camera_t;

typedef struct {
    int posX;
    int posY;
    size_t width;
    size_t height;
} viewport_t;

class Camera: public Component {
public:
    Camera(Entity*const _entity, Renderer* renderer);
    ~Camera();

    camera_t getCameraType() const;
    bool hasChanged() const;
    const viewport_t& getViewport() const;
    float getAspectRatio() const;
    float getPerspectiveFOV() const;
    float getOrthoWidth() const;
    float getOrthoHeight() const;
    float getNearDistance() const;
    float getFarDistance() const;
    void setCameraType(const camera_t cameraType);
    void setHasChanged(const bool _hasChanged);
    void setViewport(const int posX, const int posY, const size_t width, const size_t height);
    void setPerspectiveFOV(const float fov);
    void setOrthoHeight(const float orthoHeight);
    void setNearDistance(const float nearDistance);
    void setFarDistance(const float farDistance);

    void loadFromPtree(const std::string& path, const boost::property_tree::ptree& tree);
    void saveToPtree(const std::string& path, boost::property_tree::ptree& tree) const;

private:
    camera_t m_cameraType;
    Renderer* m_renderer;
    bool m_hasChanged;
    viewport_t m_viewport;
    float m_aspectRatio;
    float m_perspectiveFOV;
    float m_orthoHeight;
    float m_nearDistance;
    float m_farDistance;

    Camera(const Camera& rhs);
    Camera& operator=(const Camera&);

    std::string cmdCameraType(std::deque<std::string>& args);
    std::string cmdPerspectiveFOV(std::deque<std::string>& args);
    std::string cmdOrthoHeight(std::deque<std::string>& args);
    std::string cmdNearDistance(std::deque<std::string>& args);
    std::string cmdFarDistance(std::deque<std::string>& args);
};



inline std::ostream& operator<<(std::ostream& out, const viewport_t& rhs) {
    out << rhs.posX << " " << rhs.posY << " " << rhs.width << " " << rhs.height;
    return out;
}

inline std::istream& operator>>(std::istream& in, viewport_t& rhs) {
    in >> rhs.posX >> rhs.posY >> rhs.width >> rhs.height;
    return in;
}



inline camera_t Camera::getCameraType() const {
    return m_cameraType;
}

inline bool Camera::hasChanged() const {
    return m_hasChanged;
}

inline const viewport_t& Camera::getViewport() const {
    return m_viewport;
}

inline float Camera::getAspectRatio() const {
    return m_aspectRatio;
}

inline float Camera::getPerspectiveFOV() const {
    return m_perspectiveFOV;
}

inline float Camera::getOrthoWidth() const {
    return m_orthoHeight * m_aspectRatio;
}

inline float Camera::getOrthoHeight() const {
    return m_orthoHeight;
}

inline float Camera::getNearDistance() const {
    return m_nearDistance;
}

inline float Camera::getFarDistance() const {
    return m_farDistance;
}

inline void Camera::setCameraType(const camera_t cameraType) {
    m_cameraType = cameraType;
    m_hasChanged = true;
}

inline void Camera::setHasChanged(const bool _hasChanged) {
    m_hasChanged = _hasChanged;
}

inline void Camera::setViewport(const int posX, const int posY, const size_t width, const size_t height) {
    m_viewport.posX = posX;
    m_viewport.posY = posY;
    m_viewport.width = width;
    m_viewport.height = height;
    m_aspectRatio = float(m_viewport.width) / float(m_viewport.height);
    m_hasChanged = true;
}

inline void Camera::setPerspectiveFOV(const float fov) {
    m_perspectiveFOV = fov;
    m_hasChanged = true;
}

inline void Camera::setOrthoHeight(const float orthoHeight) {
    m_orthoHeight = orthoHeight;
    m_hasChanged = true;
}

inline void Camera::setNearDistance(const float nearDistance) {
    if (nearDistance < m_farDistance) {
        m_nearDistance = nearDistance;
        m_hasChanged = true;
    }
}

inline void Camera::setFarDistance(const float farDistance) {
    if (farDistance > m_nearDistance) {
        m_farDistance = farDistance;
        m_hasChanged = true;
    }
}

#endif // CAMERA_HPP
