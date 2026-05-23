#ifndef MATH_UTILS_H
#define MATH_UTILS_H

#include <glm/glm.hpp>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

inline float toRadians(float degrees) {
    return degrees * (float)M_PI / 180.0f;
}

inline glm::mat4 makeTranslate(const glm::vec3& v) {
    glm::mat4 m(1.0f);
    m[3][0] = v.x;
    m[3][1] = v.y;
    m[3][2] = v.z;
    return m;
}

inline glm::mat4 makeScale(const glm::vec3& v) {
    glm::mat4 m(1.0f);
    m[0][0] = v.x;
    m[1][1] = v.y;
    m[2][2] = v.z;
    return m;
}

inline glm::mat4 makeRotateX(float angleRad) {
    float c = cosf(angleRad), s = sinf(angleRad);
    glm::mat4 m(1.0f);
    m[1][1] = c;  m[2][1] = -s;
    m[1][2] = s;  m[2][2] = c;
    return m;
}

inline glm::mat4 makeRotateY(float angleRad) {
    float c = cosf(angleRad), s = sinf(angleRad);
    glm::mat4 m(1.0f);
    m[0][0] = c;  m[2][0] = s;
    m[0][2] = -s; m[2][2] = c;
    return m;
}

inline glm::mat4 makeRotateZ(float angleRad) {
    float c = cosf(angleRad), s = sinf(angleRad);
    glm::mat4 m(1.0f);
    m[0][0] = c;  m[1][0] = -s;
    m[0][1] = s;  m[1][1] = c;
    return m;
}

inline glm::mat4 makeLookAt(const glm::vec3& eye, const glm::vec3& center, const glm::vec3& up) {
    glm::vec3 f = glm::normalize(center - eye);
    glm::vec3 s = glm::normalize(glm::cross(f, up));
    glm::vec3 u = glm::cross(s, f);
    glm::mat4 m(1.0f);
    m[0][0] = s.x;  m[1][0] = s.y;  m[2][0] = s.z;
    m[0][1] = u.x;  m[1][1] = u.y;  m[2][1] = u.z;
    m[0][2] = -f.x; m[1][2] = -f.y; m[2][2] = -f.z;
    m[3][0] = -glm::dot(s, eye);
    m[3][1] = -glm::dot(u, eye);
    m[3][2] =  glm::dot(f, eye);
    return m;
}

inline glm::mat4 makePerspective(float fovRad, float aspect, float zNear, float zFar) {
    float tanHalf = tanf(fovRad / 2.0f);
    glm::mat4 m(0.0f);
    m[0][0] = 1.0f / (aspect * tanHalf);
    m[1][1] = 1.0f / tanHalf;
    m[2][2] = -(zFar + zNear) / (zFar - zNear);
    m[2][3] = -1.0f;
    m[3][2] = -(2.0f * zFar * zNear) / (zFar - zNear);
    return m;
}

#endif