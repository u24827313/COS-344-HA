// Hole.h  — one shared file, written once
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <thread>
#include <random>
#include <chrono> 

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>


class Hole {
public:
    virtual void init() = 0;
    virtual void draw(GLuint shader) const = 0;
    virtual bool checkCollision(glm::vec3 pos, float radius) const = 0;
    virtual float getHeightAt(float x, float z) const = 0;
};