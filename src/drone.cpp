#include <GL/glew.h> // 1. Use GLEW instead of GLAD at the very top
#include "drone.h"
#include "shader.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <cmath>
#include <GL/glew.h>

static void pushFace(std::vector<float>& dst, const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, const glm::vec3& d, const glm::vec3& normal){
    auto pushVert = [&](const glm::vec3& v){
        dst.push_back(v.x); dst.push_back(v.y); dst.push_back(v.z);
        dst.push_back(normal.x); dst.push_back(normal.y); dst.push_back(normal.z);
    };
    pushVert(a); pushVert(b); pushVert(c);
    pushVert(a); pushVert(c); pushVert(d);
}

static void pushRotatedBox(std::vector<float>& dst, const glm::vec3& mn, const glm::vec3& mx, float angleDegrees) {
    glm::mat4 rot = glm::rotate(glm::mat4(1.0f), glm::radians(angleDegrees), glm::vec3(0.0f, 1.0f, 0.0f));
    auto transform = [&](const glm::vec3& v){ return glm::vec3(rot * glm::vec4(v, 1.0f)); };
    auto transformNormal = [&](const glm::vec3& n){ return glm::vec3(rot * glm::vec4(n, 0.0f)); };

    glm::vec3 v000 = transform(glm::vec3(mn.x, mn.y, mn.z)); glm::vec3 v001 = transform(glm::vec3(mn.x, mn.y, mx.z));
    glm::vec3 v010 = transform(glm::vec3(mn.x, mx.y, mn.z)); glm::vec3 v011 = transform(glm::vec3(mn.x, mx.y, mx.z));
    glm::vec3 v100 = transform(glm::vec3(mx.x, mn.y, mn.z)); glm::vec3 v101 = transform(glm::vec3(mx.x, mn.y, mx.z));
    glm::vec3 v110 = transform(glm::vec3(mx.x, mx.y, mn.z)); glm::vec3 v111 = transform(glm::vec3(mx.x, mx.y, mx.z));

    pushFace(dst, v001, v000, v010, v011, transformNormal(glm::vec3(-1,0,0)));
    pushFace(dst, v100, v101, v111, v110, transformNormal(glm::vec3(1,0,0)));
    pushFace(dst, v000, v100, v110, v010, transformNormal(glm::vec3(0,-1,0)));
    pushFace(dst, v011, v111, v101, v001, transformNormal(glm::vec3(0,1,0)));
    pushFace(dst, v100, v000, v001, v101, transformNormal(glm::vec3(0,0,-1)));
    pushFace(dst, v010, v110, v111, v011, transformNormal(glm::vec3(0,0,1)));
}

static void pushRotatedCylinder(std::vector<float>& dst, const glm::vec3& baseCenter, float radius, float height, int segments, float angleDegrees) {
    glm::mat4 rot = glm::rotate(glm::mat4(1.0f), glm::radians(angleDegrees), glm::vec3(0.0f, 1.0f, 0.0f));
    auto transform = [&](const glm::vec3& v){ return glm::vec3(rot * glm::vec4(v, 1.0f)); };
    auto transformNormal = [&](const glm::vec3& n){ return glm::vec3(rot * glm::vec4(n, 0.0f)); };

    glm::vec3 topCenter = transform(baseCenter + glm::vec3(0.0f, height, 0.0f));
    glm::vec3 botCenter = transform(baseCenter);
    glm::vec3 topNormal = transformNormal(glm::vec3(0.0f, 1.0f, 0.0f));
    glm::vec3 botNormal = transformNormal(glm::vec3(0.0f, -1.0f, 0.0f));

    auto pushV = [&](const glm::vec3& v, const glm::vec3& n) {
        dst.push_back(v.x); dst.push_back(v.y); dst.push_back(v.z);
        dst.push_back(n.x); dst.push_back(n.y); dst.push_back(n.z);
    };

    float angleStep = 2.0f * 3.14159265359f / segments;
    for (int i = 0; i < segments; ++i) {
        float a1 = i * angleStep; float a2 = (i + 1) * angleStep;

        glm::vec3 p1Offset(std::cos(a1) * radius, 0.0f, std::sin(a1) * radius);
        glm::vec3 p2Offset(std::cos(a2) * radius, 0.0f, std::sin(a2) * radius);

        glm::vec3 p1Bot = transform(baseCenter + p1Offset);
        glm::vec3 p2Bot = transform(baseCenter + p2Offset);
        glm::vec3 p1Top = transform(baseCenter + p1Offset + glm::vec3(0.0f, height, 0.0f));
        glm::vec3 p2Top = transform(baseCenter + p2Offset + glm::vec3(0.0f, height, 0.0f));

        pushV(topCenter, topNormal); pushV(p1Top, topNormal); pushV(p2Top, topNormal);
        pushV(botCenter, botNormal); pushV(p2Bot, botNormal); pushV(p1Bot, botNormal);

        glm::vec3 sideDir = glm::normalize(p1Offset + p2Offset);
        glm::vec3 sideNormal = transformNormal(sideDir);

        pushV(p1Bot, sideNormal); pushV(p2Bot, sideNormal); pushV(p2Top, sideNormal);
        pushV(p1Bot, sideNormal); pushV(p2Top, sideNormal); pushV(p1Top, sideNormal);
    }
}

Drone::Drone()
    : position(0.0f, 2.0f, 0.0f),  
      rotation(0.0f),              
      VAO(0),                      
      VBO(0),                      
      shaderProgram(0),            
      bufferInitialised(false),    
      vertexCount(0)               
{
    shaderProgram = LoadShaders("shaders/drone/vertex.glsl", "shaders/drone/fragment.glsl");
    build();
}

Drone::~Drone(){
    if (bufferInitialised){
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
    }
    if (shaderProgram) glDeleteProgram(shaderProgram);
}

void Drone::pushBox(const glm::vec3& mn, const glm::vec3& mx){
    pushRotatedBox(vertexData, mn, mx, 0.0f); // Re-used helper function for standard boxes
}

void Drone::build(){
    vertexData.clear();

    float bodyHalf = 0.35f;
    pushBox(glm::vec3(-bodyHalf, -bodyHalf * 0.4f, -bodyHalf), glm::vec3(bodyHalf, bodyHalf * 0.4f, bodyHalf));

    float armLen = 0.9f;
    float armHalf = 0.06f;
    float cornerDist = bodyHalf * 1.41421356f; 

    glm::vec3 armMin(cornerDist - 0.05f, 0.0f, -armHalf);
    glm::vec3 armMax(cornerDist + armLen, bodyHalf * 0.4f, armHalf);

    pushRotatedBox(vertexData, armMin, armMax, 45.0f);  
    pushRotatedBox(vertexData, armMin, armMax, 135.0f); 
    pushRotatedBox(vertexData, armMin, armMax, 225.0f); 
    pushRotatedBox(vertexData, armMin, armMax, 315.0f); 

    float motorRadius = 0.08f;
    float motorHeight = 0.12f;
    int motorSegments = 16; 
    
    float motorCenterDist = cornerDist + armLen - 0.05f; 
    glm::vec3 motorBaseCenter(motorCenterDist, bodyHalf * 0.4f, 0.0f);

    pushRotatedCylinder(vertexData, motorBaseCenter, motorRadius, motorHeight, motorSegments, 45.0f);
    pushRotatedCylinder(vertexData, motorBaseCenter, motorRadius, motorHeight, motorSegments, 135.0f);
    pushRotatedCylinder(vertexData, motorBaseCenter, motorRadius, motorHeight, motorSegments, 225.0f);
    pushRotatedCylinder(vertexData, motorBaseCenter, motorRadius, motorHeight, motorSegments, 315.0f);

    float propRadius = 0.35f;
    float propThickness = 0.015f;
    float propWidth = 0.04f;
    float propCenterY = (bodyHalf * 0.4f) + motorHeight + propThickness; 

    glm::vec3 prop1Min(motorCenterDist - propRadius, propCenterY - propThickness, -propWidth);
    glm::vec3 prop1Max(motorCenterDist + propRadius, propCenterY + propThickness, propWidth);

    glm::vec3 prop2Min(motorCenterDist - propWidth, propCenterY - propThickness, -propRadius);
    glm::vec3 prop2Max(motorCenterDist + propWidth, propCenterY + propThickness, propRadius);

    pushRotatedBox(vertexData, prop1Min, prop1Max, 45.0f); pushRotatedBox(vertexData, prop2Min, prop2Max, 45.0f);
    pushRotatedBox(vertexData, prop1Min, prop1Max, 135.0f); pushRotatedBox(vertexData, prop2Min, prop2Max, 135.0f);
    pushRotatedBox(vertexData, prop1Min, prop1Max, 225.0f); pushRotatedBox(vertexData, prop2Min, prop2Max, 225.0f);
    pushRotatedBox(vertexData, prop1Min, prop1Max, 315.0f); pushRotatedBox(vertexData, prop2Min, prop2Max, 315.0f);

    initBuffer();
}

void Drone::initBuffer(){
    if (vertexData.empty()) return;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), vertexData.data(), GL_STATIC_DRAW);

    const GLsizei stride = 6 * sizeof(float);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));

    glBindVertexArray(0);
    bufferInitialised = true;
    vertexCount = (int)(vertexData.size() / 6);
}



glm::mat4 Drone::getModelMatrix() const {
    glm::mat4 model(1.0f);
    // 1. Move to position
    model = glm::translate(model, position);
    // 2. Apply rotations (yaw, pitch, roll)
    model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f)); // Yaw
    model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f)); // Pitch
    model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f)); // Roll
    return model;
}

glm::vec3 Drone::getCameraPosition() const {
    // Mount the camera on the front of the drone (assuming -Z is forward)
    glm::vec3 cameraOffset(0.0f, 0.0f, -0.35f); 
    glm::mat4 model = getModelMatrix();
    return glm::vec3(model * glm::vec4(cameraOffset, 1.0f));
}

glm::mat4 Drone::getCameraViewMatrix() const {
    glm::vec3 camPos = getCameraPosition();
    glm::mat4 model = getModelMatrix();
    
    // Calculate where "Forward" and "Up" are after the drone rotates
    // Using W=0.0f so translation is ignored, only rotation applies to the direction vectors
    glm::vec3 camForward = glm::normalize(glm::vec3(model * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f)));
    glm::vec3 camUp      = glm::normalize(glm::vec3(model * glm::vec4(0.0f, 1.0f,  0.0f, 0.0f)));

    return glm::lookAt(camPos, camPos + camForward, camUp);
}

glm::mat4 Drone::getCameraProjectionMatrix(float aspectRatio) const {
    // Standard perspective projection: 45 Degree Field of View
    // Near clip at 0.1, Far clip at 100.0 units
    return glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);
}

void Drone::render(const float* view4x4, const float* proj4x4, const glm::vec3& cameraPos, const DirectionalLight& light){
    if (!bufferInitialised) return;

    glUseProgram(shaderProgram);

    // Swap hard-coded `glm::translate` for our new getModelMatrix()
    glm::mat4 model = getModelMatrix(); 
    
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"),      1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"),       1, GL_FALSE, view4x4);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, proj4x4);

    glUniform3fv(glGetUniformLocation(shaderProgram, "lightDirection"), 1, glm::value_ptr(light.direction));
    glUniform3fv(glGetUniformLocation(shaderProgram, "lightColour"), 1, glm::value_ptr(light.colour));
    glUniform3fv(glGetUniformLocation(shaderProgram, "viewPos"), 1, glm::value_ptr(cameraPos));

    glm::vec3 col(0.9f, 0.9f, 0.95f);
    glUniform3fv(glGetUniformLocation(shaderProgram, "objectColour"), 1, glm::value_ptr(col));

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    glBindVertexArray(0);
}