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


#ifndef TRANSFORM_HPP
#define TRANSFORM_HPP

#include <bullet/LinearMath/btTransform.h>
#include "matrix3x3.hpp"

class Transform {
public:
    Transform();
    Transform(const Transform& t);
    Transform(const float* openGL);
    Transform(const Matrix3x3& m, const Vector3& v);
    Transform(const Quaternion& q, const Vector3& v);
    Transform(const btTransform& t);

    Transform& operator=(const Transform& t);
    Transform& operator*=(const Transform& t);
    const Transform operator*(const Transform& t) const;
    Vector3 operator*(const Vector3& v) const;
    Quaternion operator*(const Quaternion& q) const;
    bool operator==(const Transform& t) const;
    bool operator!=(const Transform& t) const;

    const Vector3& getPosition() const;
    const Matrix3x3& getRotation() const;
    Quaternion getRotationQuat() const;
    void getOpenGLMatrix(float* openGL) const;
    void setValue(const Transform& t);
    void setValue(const Matrix3x3& m, const Vector3& v);
    void setValue(const Quaternion& q, const Vector3& v);
    void setFromOpenGLMatrix(const float* openGL);
    void setPosition(const Vector3& v);
    void setRotation(const Matrix3x3& m);
    void setRotation(const Quaternion& q);
    void setValue(const btTransform& t);
    void setIdentity();

    Transform inverse() const;
    Transform inverseTimes(const Transform& t) const;

private:
    Matrix3x3 m_rotation;
    Vector3 m_position;
};



const Transform TRANSFORM_IDENTITY = Transform().inverse();



btTransform trans(const Transform& t);
btTransform trans(const Quaternion& q, const Vector3& v);
btMatrix3x3 mat3x3(const Matrix3x3& m);
btQuaternion quat(const Quaternion& q);
btVector3 vect(const Vector3& v);



inline Transform::Transform():
    m_rotation(),
    m_position()
{}

inline Transform::Transform(const Transform& t):
    m_rotation(t.m_rotation),
    m_position(t.m_position)
{}

inline Transform::Transform(const float* openGL):
    m_rotation(),
    m_position()
{
    setFromOpenGLMatrix(openGL);
}

inline Transform::Transform(const Matrix3x3& m, const Vector3& v):
    m_rotation(m),
    m_position(v)
{}

inline Transform::Transform(const Quaternion& q, const Vector3& v):
    m_rotation(),
    m_position()
{
    setValue(q, v);
}

inline Transform::Transform(const btTransform& t):
    m_rotation(t.getBasis()),
    m_position(t.getOrigin())
{}



inline Transform& Transform::operator=(const Transform& t) {
    m_rotation = t.m_rotation;
    m_position = t.m_position;
    return *this;
}

inline Transform& Transform::operator*=(const Transform& t) {
    m_position += m_rotation * t.m_position;
    m_rotation *= t.m_rotation;
    return *this;
}

inline const Transform Transform::operator*(const Transform& t) const {
    return Transform(*this) *= t;
}

inline Vector3 Transform::operator*(const Vector3& v) const {
    return Vector3(m_rotation.getRow(0).dot(v) + m_position.getX(),
                   m_rotation.getRow(1).dot(v) + m_position.getY(),
                   m_rotation.getRow(2).dot(v) + m_position.getZ());
}

inline Quaternion Transform::operator*(const Quaternion& q) const {
    return getRotationQuat() * q;
}

inline bool Transform::operator==(const Transform& t) const {
    return m_rotation == t.m_rotation && m_position == t.m_position;
}

inline bool Transform::operator!=(const Transform& t) const {
    return !(*this == t);
}



inline const Vector3& Transform::getPosition() const {
    return m_position;
}

inline const Matrix3x3& Transform::getRotation() const {
    return m_rotation;
}

inline Quaternion Transform::getRotationQuat() const {
    return m_rotation.getRotationQuat();
}

inline void Transform::getOpenGLMatrix(float* openGL) const {
    m_rotation.getOpenGLSubMatrix(openGL);
    openGL[12] = m_position.getX();
    openGL[13] = m_position.getY();
    openGL[14] = m_position.getZ();
    openGL[15] = 1.0f;
}

inline void Transform::setValue(const Transform& t) {
    *this = t;
}

inline void Transform::setValue(const Matrix3x3& m, const Vector3& v) {
    setRotation(m);
    setPosition(v);
}

inline void Transform::setValue(const Quaternion& q, const Vector3& v) {
    setRotation(q);
    setPosition(v);
}

inline void Transform::setFromOpenGLMatrix(const float* openGL) {
    m_rotation.setOpenGLSubMatrix(openGL);
    m_position.setValue(openGL[12], openGL[13], openGL[14]);
}

inline void Transform::setPosition(const Vector3& v) {
    m_position = v;
}

inline void Transform::setRotation(const Matrix3x3& m) {
    m_rotation = m;
}

inline void Transform::setRotation(const Quaternion& q) {
    m_rotation = Matrix3x3(q);
}

inline void Transform::setValue(const btTransform& t) {
    m_rotation = Matrix3x3(t.getBasis());
    m_position = Vector3(t.getOrigin());
}

inline void Transform::setIdentity() {
    m_rotation.setIdentity();
    m_position = VECTOR3_ZERO;
}



inline Transform Transform::inverse() const {
    Matrix3x3 inv = m_rotation.transpose();
    return Transform(inv, inv * -m_position);
}

inline Transform Transform::inverseTimes(const Transform& t) const {
    Vector3 v = t.getPosition() - m_position;
    return Transform(m_rotation.transposeTimes(t.m_rotation), v * m_rotation);

}



inline btTransform trans(const Transform& t) {
    return btTransform(mat3x3(t.getRotation()), vect(t.getPosition()));
}

inline btTransform trans(const Quaternion& q, const Vector3& v) {
    return btTransform(quat(q), vect(v));
}

inline btMatrix3x3 mat3x3(const Matrix3x3& m) {
    return btMatrix3x3(m.get(0, 0), m.get(0, 1), m.get(0, 2),
                       m.get(1, 0), m.get(1, 1), m.get(1, 2),
                       m.get(2, 0), m.get(2, 1), m.get(2, 2)
    );
}

inline btQuaternion quat(const Quaternion& q) {
    return btQuaternion(q.getX(), q.getY(), q.getZ(), q.getW());
}

inline btVector3 vect(const Vector3& v) {
    return btVector3(v.getX(), v.getY(), v.getZ());
}

#endif // TRANSFORM_HPP
