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


#ifndef MATRIX3X3_HPP
#define MATRIX3X3_HPP

#include "vector3.hpp"
#include "quaternion.hpp"

class Matrix3x3 {
public:
    Matrix3x3();
    Matrix3x3(const Matrix3x3& m);
    Matrix3x3(const scalar_t& xx, const scalar_t& xy, const scalar_t& xz,
              const scalar_t& yx, const scalar_t& yy, const scalar_t& yz,
              const scalar_t& zx, const scalar_t& zy, const scalar_t& zz);
    Matrix3x3(const float* openGLSubMatrix);
    Matrix3x3(const Vector3& xrow, const Vector3& yrow, const Vector3& zrow);
    Matrix3x3(const Quaternion& q);
    Matrix3x3(const scalar_t& yaw, const scalar_t& pitch, const scalar_t& roll);

    Matrix3x3& operator=(const Matrix3x3& m);
    Matrix3x3 operator-() const;
    Matrix3x3& operator+=(const Matrix3x3& m);
    Matrix3x3& operator-=(const Matrix3x3& m);
    Matrix3x3& operator*=(const Matrix3x3& m);
    Matrix3x3& operator*=(const scalar_t& s);
    const Matrix3x3 operator+(const Matrix3x3& m) const;
    const Matrix3x3 operator-(const Matrix3x3& m) const;
    const Matrix3x3 operator*(const Matrix3x3& m) const;
    const Matrix3x3 operator*(const scalar_t& s) const;
    bool operator==(const Matrix3x3& m) const;
    bool operator!=(const Matrix3x3& m) const;

    const scalar_t& get(const int row, const int col) const;
    Vector3 getColumn(const int i) const;
    const Vector3& getRow(const int i) const;
    Quaternion getRotationQuat() const;
    void getEuler(scalar_t& yaw, scalar_t& pitch, scalar_t& roll) const;
    void getOpenGLSubMatrix(float* openGLSubMatrix) const;
    void setRow(const int i, const Vector3& v);
    void setValue(const Matrix3x3& m);
    void setValue(const scalar_t& xx, const scalar_t& xy, const scalar_t& xz,
                  const scalar_t& yx, const scalar_t& yy, const scalar_t& yz,
                  const scalar_t& zx, const scalar_t& zy, const scalar_t& zz);
    void setOpenGLSubMatrix(const float* openGLSubMatrix);
    void setValue(const Vector3& xrow, const Vector3& yrow, const Vector3& zrow);
    void setRotation(const Quaternion& q);
    void setEuler(const scalar_t& yaw, const scalar_t& pitch, const scalar_t& roll);
    void setIdentity();

    Matrix3x3 absolute() const;
    Matrix3x3 adjoint() const;
    scalar_t determinant() const;
    Matrix3x3 inverse() const;
    Matrix3x3 scaled(const Vector3& s) const;
    Matrix3x3 transpose() const;
    Matrix3x3 transposeTimes(const Matrix3x3& m) const;

    scalar_t tdotx(const Vector3& v) const;
    scalar_t tdoty(const Vector3& v) const;
    scalar_t tdotz(const Vector3& v) const;
    scalar_t cofac(const int r1, const int c1, const int r2, const int c2) const;

private:
    Vector3 m_rows[3];
};

#include "common.hpp"

const Matrix3x3 MATRIX3X3_IDENTITY = Matrix3x3(
     ONE, ZERO, ZERO,
    ZERO,  ONE, ZERO,
    ZERO, ZERO,  ONE
);


inline Matrix3x3::Matrix3x3() {
}

inline Matrix3x3::Matrix3x3(const Matrix3x3& m) {
    setValue(m);
}

inline Matrix3x3::Matrix3x3(const Vector3& xrow, const Vector3& yrow, const Vector3& zrow) {
    setValue(xrow, yrow, zrow);
}

inline Matrix3x3::Matrix3x3(const scalar_t& xx, const scalar_t& xy, const scalar_t& xz,
                            const scalar_t& yx, const scalar_t& yy, const scalar_t& yz,
                            const scalar_t& zx, const scalar_t& zy, const scalar_t& zz) {
    setValue(xx, xy, xz, yx, yy, yz, zx, zy, zz);
}

inline Matrix3x3::Matrix3x3(const float* openGLSubMatrix) {
    setValue(openGLSubMatrix);
}

inline Matrix3x3::Matrix3x3(const Quaternion& q) {
    setRotation(q);
}

inline Matrix3x3::Matrix3x3(const scalar_t& yaw, const scalar_t& pitch, const scalar_t& roll) {
    setEuler(yaw, pitch, roll);
}



inline Matrix3x3& Matrix3x3::operator=(const Matrix3x3& m) {
    setValue(m);
    return *this;
}

inline Matrix3x3 Matrix3x3::operator-() const {
    return Matrix3x3(-getRow(0), -getRow(1), -getRow(2));
}

inline Matrix3x3& Matrix3x3::operator+=(const Matrix3x3& m) {
    m_rows[0] += m.m_rows[0];
    m_rows[1] += m.m_rows[1];
    m_rows[2] += m.m_rows[2];
    return *this;
}

inline Matrix3x3& Matrix3x3::operator-=(const Matrix3x3& m) {
    m_rows[0] -= m.m_rows[0];
    m_rows[1] -= m.m_rows[1];
    m_rows[2] -= m.m_rows[2];
    return *this;
}

inline Matrix3x3& Matrix3x3::operator*=(const Matrix3x3& m) {
    setValue(
        m.tdotx(getRow(0)), m.tdoty(getRow(0)), m.tdotz(getRow(0)),
        m.tdotx(getRow(1)), m.tdoty(getRow(1)), m.tdotz(getRow(1)),
        m.tdotx(getRow(2)), m.tdoty(getRow(2)), m.tdotz(getRow(2))
    );
    return *this;
}

inline Matrix3x3& Matrix3x3::operator*=(const scalar_t& s) {
    m_rows[0] *= s;
    m_rows[1] *= s;
    m_rows[2] *= s;
    return *this;
}

inline const Matrix3x3 Matrix3x3::operator+(const Matrix3x3& m) const {
    return Matrix3x3(*this) += m;
}

inline const Matrix3x3 Matrix3x3::operator-(const Matrix3x3& m) const {
    return Matrix3x3(*this) -= m;
}

inline const Matrix3x3 Matrix3x3::operator*(const Matrix3x3& m) const {
    return Matrix3x3(*this) *= m;
}

inline const Matrix3x3 Matrix3x3::operator*(const scalar_t& s) const {
    return Matrix3x3(*this) *= s;
}

inline bool Matrix3x3::operator==(const Matrix3x3& m) const {
    return getRow(0) == m.getRow(0) &&
           getRow(1) == m.getRow(1) &&
           getRow(2) == m.getRow(2);
}

inline bool Matrix3x3::operator!=(const Matrix3x3& m) const {
    return !(*this == m);
}



inline const scalar_t& Matrix3x3::get(const int row, const int col) const {
    return getRow(row).get(col);
}

inline Vector3 Matrix3x3::getColumn(const int i) const {
    return Vector3(get(0, i), get(1, i), get(2, i));
}

inline const Vector3& Matrix3x3::getRow(const int i) const {
    return m_rows[i];
}

Quaternion Matrix3x3::getRotationQuat() const {
    scalar_t trace = get(0, 0) + get(1, 1) + get(2, 2);
    scalar_t temp[4];

    if (trace > ZERO) {
        scalar_t s = std::sqrt(trace + ONE);
        temp[3] = s * HALF;
        s = HALF / s;

        temp[0] = (get(2, 1) - get(1, 2)) * s;
        temp[1] = (get(0, 2) - get(2, 0)) * s;
        temp[2] = (get(1, 0) - get(0, 1)) * s;
    }
    else {
        int i = get(0, 0) < get(1, 1) ?
                (get(1, 1) < get(2, 2) ? 2 : 1) :
                (get(0, 0) < get(2, 2) ? 2 : 0);
        int j = (i + 1) % 3;
        int k = (i + 2) % 3;

        scalar_t s = std::sqrt(get(i, i) - get(j, j) - get(k, k) + ONE);
        temp[i] = s * HALF;
        s = HALF / s;

        temp[3] = (get(k, j) - get(j, k)) * s;
        temp[j] = (get(j, i) + get(i, j)) * s;
        temp[k] = (get(k, i) + get(i, k)) * s;
    }
    return Quaternion(temp[3], temp[0], temp[1], temp[2]);
}

inline void Matrix3x3::getEuler(scalar_t& yaw, scalar_t& pitch, scalar_t& roll) const {
    // first use the normal calculus
    yaw = static_cast<scalar_t>(std::atan2(get(1, 0), get(0, 0)));
    pitch = static_cast<scalar_t>(std::asin(-get(2, 0)));
    roll = static_cast<scalar_t>(std::atan2(get(2, 1), get(2, 2)));

    // on pitch = +/-HalfPI
    if (std::abs(pitch) == HALF_PI) {
        if (yaw > ZERO)
            yaw -= PI;
        else
            yaw += PI;

        if (roll > ZERO)
            roll -= PI;
        else
            roll += PI;
    }
}

inline void Matrix3x3::getOpenGLSubMatrix(float* openGLSubMatrix) const {
    openGLSubMatrix[ 0] = getRow(0).getX();
    openGLSubMatrix[ 1] = getRow(1).getX();
    openGLSubMatrix[ 2] = getRow(2).getX();
    openGLSubMatrix[ 3] = 0.0f;
    openGLSubMatrix[ 4] = getRow(0).getY();
    openGLSubMatrix[ 5] = getRow(1).getY();
    openGLSubMatrix[ 6] = getRow(2).getY();
    openGLSubMatrix[ 7] = 0.0f;
    openGLSubMatrix[ 8] = getRow(0).getZ();
    openGLSubMatrix[ 9] = getRow(1).getZ();
    openGLSubMatrix[10] = getRow(2).getZ();
    openGLSubMatrix[11] = 0.0f;
}

inline void Matrix3x3::setRow(const int i, const Vector3& v) {
    m_rows[i] = v;
}

inline void Matrix3x3::setValue(const Matrix3x3& m) {
    setRow(0, m.getRow(0));
    setRow(1, m.getRow(1));
    setRow(2, m.getRow(2));
}

inline void Matrix3x3::setValue(const scalar_t& xx, const scalar_t& xy, const scalar_t& xz,
                                const scalar_t& yx, const scalar_t& yy, const scalar_t& yz,
                                const scalar_t& zx, const scalar_t& zy, const scalar_t& zz) {
    m_rows[0].setX(xx);
    m_rows[0].setY(xy);
    m_rows[0].setZ(xz);

    m_rows[1].setX(yx);
    m_rows[1].setY(yy);
    m_rows[1].setZ(yz);

    m_rows[2].setX(zx);
    m_rows[2].setY(zy);
    m_rows[2].setZ(zz);
}

inline void Matrix3x3::setOpenGLSubMatrix(const float* openGLSubMatrix) {
    setValue(
        openGLSubMatrix[ 0], openGLSubMatrix[ 4], openGLSubMatrix[ 8],
        openGLSubMatrix[ 1], openGLSubMatrix[ 5], openGLSubMatrix[ 9],
        openGLSubMatrix[ 2], openGLSubMatrix[ 6], openGLSubMatrix[10]
    );
}

inline void Matrix3x3::setValue(const Vector3& xrow, const Vector3& yrow, const Vector3& zrow) {
    setRow(0, xrow);
    setRow(1, yrow);
    setRow(2, zrow);
}

inline void Matrix3x3::setRotation(const Quaternion& q) {
    scalar_t s = TWO / q.lengthSquared();
    scalar_t xs = q.getX() * s;
    scalar_t ys = q.getY() * s;
    scalar_t zs = q.getZ() * s;
    scalar_t wx = q.getW() * xs;
    scalar_t wy = q.getW() * ys;
    scalar_t wz = q.getW() * zs;
    scalar_t xx = q.getX() * xs;
    scalar_t xy = q.getX() * ys;
    scalar_t xz = q.getX() * zs;
    scalar_t yy = q.getY() * ys;
    scalar_t yz = q.getY() * zs;
    scalar_t zz = q.getZ() * zs;
    setValue(ONE - yy - zz, xy - wz, xz + wy,
             xy + wz, ONE - xx - zz, yz - wx,
             xz - wy, yz + wx, ONE - xx - yy);
}

inline void Matrix3x3::setEuler(const scalar_t& yaw, const scalar_t& pitch, const scalar_t& roll) {
    scalar_t ci (std::cos(roll));
    scalar_t cj (std::cos(pitch));
    scalar_t ch (std::cos(yaw));
    scalar_t si (std::sin(roll));
    scalar_t sj (std::sin(pitch));
    scalar_t sh (std::sin(yaw));
    scalar_t cc = ci * ch;
    scalar_t cs = ci * sh;
    scalar_t sc = si * ch;
    scalar_t ss = si * sh;
    setValue(cj * ch, sj * sc - cs, sj * cc + ss,
             cj * sh, sj * ss + cc, sj * cs - sc,
             -sj,     cj * si,      cj * ci);
}

inline void Matrix3x3::setIdentity() {
    setValue(MATRIX3X3_IDENTITY);
}



inline Matrix3x3 Matrix3x3::absolute() const {
    return Matrix3x3(
        std::abs(get(0, 0)), std::abs(get(0, 1)), std::abs(get(0, 2)),
        std::abs(get(1, 0)), std::abs(get(1, 1)), std::abs(get(1, 2)),
        std::abs(get(2, 0)), std::abs(get(2, 1)), std::abs(get(2, 2))
    );
}

inline Matrix3x3 Matrix3x3::adjoint() const {
    return Matrix3x3(
        cofac(1, 1, 2, 2), cofac(0, 2, 2, 1), cofac(0, 1, 1, 2),
        cofac(1, 2, 2, 0), cofac(0, 0, 2, 2), cofac(0, 2, 1, 0),
        cofac(1, 0, 2, 1), cofac(0, 1, 2, 0), cofac(0, 0, 1, 1)
    );
}

inline scalar_t Matrix3x3::determinant() const {
    return get(0, 0) * cofac(1, 1, 2, 2) +
           get(0, 1) * cofac(1, 2, 2, 0) +
           get(0, 2) * cofac(1, 0, 2, 1);

}

inline Matrix3x3 Matrix3x3::inverse() const {
    Vector3 co(cofac(1, 1, 2, 2), cofac(1, 2, 2, 0), cofac(1, 0, 2, 1));
    scalar_t s = ONE / getRow(0).dot(co);
    return Matrix3x3(
        co.getX() * s, cofac(0, 2, 2, 1) * s, cofac(0, 1, 1, 2) * s,
        co.getY() * s, cofac(0, 0, 2, 2) * s, cofac(0, 2, 1, 0) * s,
        co.getZ() * s, cofac(0, 1, 2, 0) * s, cofac(0, 0, 1, 1) * s
    );
}

inline Matrix3x3 Matrix3x3::scaled(const Vector3& s) const {
    return Matrix3x3(
        get(0, 0) * s.getX(), get(0, 1) * s.getY(), get(0, 2) * s.getZ(),
        get(1, 0) * s.getX(), get(1, 1) * s.getY(), get(1, 2) * s.getZ(),
        get(2, 0) * s.getX(), get(2, 1) * s.getY(), get(2, 2) * s.getZ()
    );
}

inline Matrix3x3 Matrix3x3::transpose() const {
    return Matrix3x3(
        get(0, 0), get(1, 0), get(2, 0),
        get(0, 1), get(1, 1), get(2, 1),
        get(0, 2), get(1, 2), get(2, 2)
    );
}

inline Matrix3x3 Matrix3x3::transposeTimes(const Matrix3x3& m) const {
    return Matrix3x3(
        getRow(0).getX() * m.getRow(0).getX() + getRow(1).getX() * m.getRow(1).getX() + getRow(2).getX() * m.getRow(2).getX(),
        getRow(0).getX() * m.getRow(0).getY() + getRow(1).getX() * m.getRow(1).getY() + getRow(2).getX() * m.getRow(2).getY(),
        getRow(0).getX() * m.getRow(0).getZ() + getRow(1).getX() * m.getRow(1).getZ() + getRow(2).getX() * m.getRow(2).getZ(),
        getRow(0).getY() * m.getRow(0).getX() + getRow(1).getY() * m.getRow(1).getX() + getRow(2).getY() * m.getRow(2).getX(),
        getRow(0).getY() * m.getRow(0).getY() + getRow(1).getY() * m.getRow(1).getY() + getRow(2).getY() * m.getRow(2).getY(),
        getRow(0).getY() * m.getRow(0).getZ() + getRow(1).getY() * m.getRow(1).getZ() + getRow(2).getY() * m.getRow(2).getZ(),
        getRow(0).getZ() * m.getRow(0).getX() + getRow(1).getZ() * m.getRow(1).getX() + getRow(2).getZ() * m.getRow(2).getX(),
        getRow(0).getZ() * m.getRow(0).getY() + getRow(1).getZ() * m.getRow(1).getY() + getRow(2).getZ() * m.getRow(2).getY(),
        getRow(0).getZ() * m.getRow(0).getZ() + getRow(1).getZ() * m.getRow(1).getZ() + getRow(2).getZ() * m.getRow(2).getZ()
    );
}



inline scalar_t Matrix3x3::tdotx(const Vector3& v) const {
    return get(0, 0) * v.getX() + get(1, 0) * v.getY() + get(2, 0) * v.getZ();
}

inline scalar_t Matrix3x3::tdoty(const Vector3& v) const {
    return get(0, 1) * v.getX() + get(1, 1) * v.getY() + get(2, 1) * v.getZ();
}

inline scalar_t Matrix3x3::tdotz(const Vector3& v) const {
    return get(0, 2) * v.getX() + get(1, 2) * v.getY() + get(2, 2) * v.getZ();
}

inline scalar_t Matrix3x3::cofac(const int r1, const int c1, const int r2, const int c2) const {
    return get(r1, c1) * get(r2, c2) - get(r1, c2) * get(r2, c1);
}



inline Vector3 operator*(const Matrix3x3& m, const Vector3& v) {
    return Vector3(m.getRow(0).dot(v), m.getRow(1).dot(v), m.getRow(2).dot(v));
}

inline Vector3 operator*(const Vector3& v, const Matrix3x3& m) {
    return Vector3(m.tdotx(v), m.tdoty(v), m.tdotz(v));
}

#endif // MATRIX3X3_HPP
