#pragma once
#include <vector>
#include <glm/glm.hpp>
#include "Terrain.h"


class Drone {
public:
    Drone();
    ~Drone();

    void build();
    void render(const float* view4x4, const float* proj4x4, const glm::vec3& cameraPos, const DirectionalLight& light);

    // --- NEW: Transform and Camera properties ---
    glm::vec3 position; // Drone's position in the world
    glm::vec3 rotation; // Euler angles: X = Pitch, Y = Yaw, Z = Roll (in degrees)

    glm::mat4 getModelMatrix() const;
    glm::vec3 getCameraPosition() const;
    
    // Use these in your main render loop to act as the viewport!
    glm::mat4 getCameraViewMatrix() const;
    glm::mat4 getCameraProjectionMatrix(float aspectRatio) const;

private:
    unsigned int VAO, VBO, shaderProgram;
    bool bufferInitialised;
    int vertexCount;
    std::vector<float> vertexData;

    void pushBox(const glm::vec3& mn, const glm::vec3& mx);
    void initBuffer();
};