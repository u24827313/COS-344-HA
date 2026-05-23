#ifndef LIGHT_H
#define LIGHT_H

#include <glm/glm.hpp>

struct DirectionalLight {
    glm::vec3 direction;   // normalized
    glm::vec3 colour;      // 
    float     ambient;     // 0.0 - 1.0

    DirectionalLight()
        : direction(glm::normalize(glm::vec3(-0.3f, -1.0f, -0.2f))),
          colour(1.0f, 0.95f, 0.85f),
          ambient(0.25f) {}
};

struct LightingState {
    DirectionalLight sun;
    glm::vec3        skyTint;     // multiplied into the skybox colour
    
    LightingState()
        : skyTint(1.0f, 1.0f, 1.0f) {}
};

#endif