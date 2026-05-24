#include "Hole12.h"

void Hole12::design(Terrain& terrain) {
    // Call base design if needed
    Hole::design(terrain);

    // Load textures
    GLuint grassTexture = loadBMPTexture("assets/terrain/grass.bmp");
    GLuint stoneTexture = loadBMPTexture("assets/terrain/stone.bmp");
    GLuint concreteTexture = loadBMPTexture("assets/terrain/concrete.bmp");
    GLuint woodTexture = loadBMPTexture("assets/terrain/wood.bmp");
    if(grassTexture == 0 || stoneTexture == 0 || concreteTexture == 0 || woodTexture == 0) {
        return;
    }

    
}