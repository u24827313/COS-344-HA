#include "RenderObject.h"
#include "MathUtils.h"
#include "glm/glm.hpp"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

RenderObject::RenderObject(const std::vector<float>& vertexData, GLuint texture_, GLuint shaderProgram_)
    : VAO(0), VBO(0),
      vertexCount(0),
      texture(texture_),
      shaderProgram(shaderProgram_),
      position(0.0f),
      rotationEuler(0.0f),
      scale(1.0f),
      bufferInitialised(false)
{
    initBuffer(vertexData);
}

RenderObject::~RenderObject() {
    if (bufferInitialised) {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
    }
}



void RenderObject::initBuffer(const std::vector<float>& vertexData) {
    if (vertexData.empty()) return;

    // 8 floats per vertex
    vertexCount = (int)(vertexData.size() / 8);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER,
                 vertexData.size() * sizeof(float),
                 vertexData.data(),
                 GL_STATIC_DRAW);

    const GLsizei stride = 8 * sizeof(float);
    // position (location 0)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    // normal (location 1)
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    // uv (location 2)
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));

    glBindVertexArray(0);

    bufferInitialised = true;
}

glm::mat4 RenderObject::computeModelMatrix() const {
    glm::mat4 m = makeTranslate(position);
    m = m * makeRotateY(toRadians(rotationEuler.y));
    m = m * makeRotateX(toRadians(rotationEuler.x));
    m = m * makeRotateZ(toRadians(rotationEuler.z));
    m = m * makeScale(scale);
    return m;
}

void RenderObject::render(const float* view4x4,
                          const float* proj4x4,
                          const glm::vec3& cameraPos,
                          const DirectionalLight& light)
{
    if (!bufferInitialised || shaderProgram == 0) return;

    glUseProgram(shaderProgram);

    // Matrices
    glm::mat4 model = computeModelMatrix();
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"),      1, GL_FALSE, &model[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"),       1, GL_FALSE, view4x4);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, proj4x4);

    // Lighting
    glUniform3fv(glGetUniformLocation(shaderProgram, "lightDirection"), 1, &light.direction[0]);
    glUniform3fv(glGetUniformLocation(shaderProgram, "lightColour"),    1, &light.colour[0]);
    glUniform1f (glGetUniformLocation(shaderProgram, "lightAmbient"),       light.ambient);
    glUniform3fv(glGetUniformLocation(shaderProgram, "viewPos"),        1, &cameraPos[0]);

    // Texture
    if (texture != 0) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glUniform1i(glGetUniformLocation(shaderProgram, "diffuseTex"), 0);
    }

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    glBindVertexArray(0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

std::vector<float> RenderObject::createQuad() {
    
    return {
        // x      y     z     nx   ny   nz    u    v
        -0.5f, 0.0f, -0.5f,   0.0f, 1.0f, 0.0f,  0.0f, 0.0f,
         0.5f, 0.0f, -0.5f,   0.0f, 1.0f, 0.0f,  1.0f, 0.0f,
         0.5f, 0.0f,  0.5f,   0.0f, 1.0f, 0.0f,  1.0f, 1.0f,

         0.5f, 0.0f,  0.5f,   0.0f, 1.0f, 0.0f,  1.0f, 1.0f,
        -0.5f, 0.0f,  0.5f,   0.0f, 1.0f, 0.0f,  0.0f, 1.0f,
        -0.5f, 0.0f, -0.5f,   0.0f, 1.0f, 0.0f,  0.0f, 0.0f,
    };
}

std::vector<float> RenderObject::createSphere(int parralels, int meridians) {
    std::vector<float> data;

    auto pointAt = [&](int lat, int lon, float& x, float& y, float& z,
                       float& u, float& v) {
        float theta = (float)lat / parralels * (float)M_PI;        // 0..pi
        float phi   = (float)lon / meridians * 2.0f * (float)M_PI; // 0..2pi
        float sinT = sinf(theta), cosT = cosf(theta);
        float sinP = sinf(phi),   cosP = cosf(phi);
        x = sinT * cosP;
        y = cosT;
        z = sinT * sinP;
        u = (float)lon / meridians;
        v = (float)lat / parralels;
    };

    auto push = [&](float x, float y, float z, float u, float v) {
        data.push_back(x); data.push_back(y); data.push_back(z);
        data.push_back(x); data.push_back(y); data.push_back(z);  // normal = position for unit sphere
        data.push_back(u); data.push_back(v);
    };

    for (int lat = 0; lat < parralels; ++lat) {
        for (int lon = 0; lon < meridians; ++lon) {
            float x1, y1, z1, u1, v1;  pointAt(lat,     lon,     x1, y1, z1, u1, v1);
            float x2, y2, z2, u2, v2;  pointAt(lat + 1, lon,     x2, y2, z2, u2, v2);
            float x3, y3, z3, u3, v3;  pointAt(lat,     lon + 1, x3, y3, z3, u3, v3);
            float x4, y4, z4, u4, v4;  pointAt(lat + 1, lon + 1, x4, y4, z4, u4, v4);

            // Triangle 1: 1, 2, 3
            push(x1, y1, z1, u1, v1);
            push(x2, y2, z2, u2, v2);
            push(x3, y3, z3, u3, v3);
            // Triangle 2: 3, 2, 4
            push(x3, y3, z3, u3, v3);
            push(x2, y2, z2, u2, v2);
            push(x4, y4, z4, u4, v4);
        }
    }
    return data;
}

std::vector<float> RenderObject::createCylinder(int segments) {
    std::vector<float> data;
    // Build the side as `segments` quads around the Y axis.
    for (int i = 0; i < segments; ++i) {
        float a0 = (float)i       / segments * 2.0f * (float)M_PI;
        float a1 = (float)(i + 1) / segments * 2.0f * (float)M_PI;
        float c0 = cosf(a0), s0 = sinf(a0);
        float c1 = cosf(a1), s1 = sinf(a1);
        float u0 = (float)i       / segments;
        float u1 = (float)(i + 1) / segments;

        // Bottom-left, bottom-right, top-right, top-left
        // Position: (c, y, s)  Normal: (c, 0, s)  UV: (u, v)
        float bl[8] = { c0, 0, s0,  c0, 0, s0,  u0, 0 };
        float br[8] = { c1, 0, s1,  c1, 0, s1,  u1, 0 };
        float tr[8] = { c1, 1, s1,  c1, 0, s1,  u1, 1 };
        float tl[8] = { c0, 1, s0,  c0, 0, s0,  u0, 1 };

        auto pushVert = [&](const float* v) {
            for (int k = 0; k < 8; ++k) data.push_back(v[k]);
        };

        pushVert(bl); pushVert(br); pushVert(tr);
        pushVert(tr); pushVert(tl); pushVert(bl);
    }
    return data;
}