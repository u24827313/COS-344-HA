#include "Hole10.h"
#include "Terrain.h"
#include "RenderObject.h"
#include "TextureLoader.h"
#include <cmath>

void Hole10::design(Terrain &terrain)
{
    Hole::design(terrain);
    
    // Load textures
    GLuint grassTexture = loadBMPTexture("assets/terrain/grass.bmp");
    GLuint stoneTexture = loadBMPTexture("assets/terrain/stone.bmp");
    // Make sure you have a concrete texture file path in your assets
    GLuint concreteTexture = loadBMPTexture("assets/terrain/concrete.bmp"); 
    GLuint woodTexture = loadBMPTexture("assets/terrain/wood.bmp");
    
    if (grassTexture == 0 || stoneTexture == 0 || concreteTexture == 0) {
        return;
    }

    // Main platform dimensions
    float platformWidth = 4.0f;
    float platformDepth = 16.0f;
    float platformHeight = 1.5f;
    float raisedHeightOffset = 0.1f; 

    float surfaceY = getTee().y + raisedHeightOffset;

    float halfWidth = platformWidth / 2.0f;
    float halfDepth = platformDepth / 2.0f;
    float edgeOffset = 0.4f;

   // --- GOLF HOLE (CUP) ---
    // --- POSITION TUNING SLIDER ---
    // Change the Z offset to a negative value to move it closer to the mound (-Z).
    // Try -1.5f or adjust it to find the perfect distance!
    float pinMoundProximityZ = -7.5f; 

    glm::vec3 pinPos = glm::vec3(
        getTee().x - pinMoundProximityZ,   // Keep the X position slightly right of center
        getTee().y + 0.0f,
        getTee().z + 0.5f // Move it towards the mound side (-Z)
    );
    float holeRadius = 0.15f;
    float holeDepth = 0.2f;

    std::vector<float> holeData = RenderObject::createCylinder(16);
    RenderObject* golfHole = new RenderObject(holeData, stoneTexture, 0);
    golfHole->setScale(glm::vec3(holeRadius, holeDepth, holeRadius));

    golfHole->setPosition(glm::vec3(pinPos.x, surfaceY - (holeDepth * 0.95f), pinPos.z));
    addObject(golfHole);

    // --- FLAG POLE ---
    float poleRadius = 0.02f;
    float poleHeight = 1.2f;

    std::vector<float> poleData = RenderObject::createCylinder(8);
    RenderObject* flagPole = new RenderObject(poleData, concreteTexture, 0);
    flagPole->setScale(glm::vec3(poleRadius, poleHeight, poleRadius));

    glm::vec3 polePos = glm::vec3(
        pinPos.x + 4.0f,
        pinPos.y, // automatically follows the new pinPos
        pinPos.z - 20.0f
    );
    flagPole->setPosition(polePos);
    addObject(flagPole);

    // --- NEW TRIANGULAR FLAG USING TRAPEZOID ---
    float flagBottomWidth = 0.5f; // The attachment edge height against the pole
    float flagTopWidth    = 0.0f; // Squeezing the top to zero width turns it into a triangle!
    float flagLength      = 0.4f; // How far out the flag waves horizontally
    float flagThickness   = 0.01f;

    // Call your new engine utility function
    std::vector<float> flagData = RenderObject::createTrapezoid(flagBottomWidth, flagTopWidth, flagLength, flagThickness);
    RenderObject* flag = new RenderObject(flagData, concreteTexture, concreteTexture);

    // Because createTrapezoid generates the shape vertically oriented along its height axis, 
    // roll it 90 degrees on the Z-axis to let it extend sideways out from the flagpole
    flag->setRotation(glm::vec3(0.0f, 0.0f, -90.0f));

    glm::vec3 flagPos = glm::vec3(
        pinPos.x + flagLength / 2.0f + 4.0f,
        pinPos.y + poleHeight - (flagBottomWidth / 2.0f) - raisedHeightOffset, 
        pinPos.z - 20.0f
    );
    flag->setPosition(flagPos);
    addObject(flag);

    // --- MOUND DIVIDER (front -Z side, away from rocks/wood/concrete) ---
    float moundRadius = 2.0f;
    float moundHeight = 0.5f;

    terrain.addMound(
        getTee().x - (platformWidth / 2.0f) - 2.0f,
        getTee().z - (platformDepth / 2.0f) - 2.0f,  // always flush to the edge
        moundRadius,
        moundHeight
    );

    float scale = 0.65f;

    std::vector<float> circleData = RenderObject::createCylinder(64);
    RenderObject* circularMound = new RenderObject(circleData, grassTexture, grassTexture);
    circularMound->setScale(glm::vec3(7.8f * scale, 0.85f, 7.8f * scale));
    circularMound->setPosition(glm::vec3(
        getTee().x, 
        getTee().y - 0.72f, 
        getTee().z - 7.0f * scale
    ));

    for (int i = 0; i < 12; ++i) {
        float z = getTee().z - 8.0f - i * 2.1f * scale;
        float rockSize = (0.32f + (rand() % 55)/170.0f) * scale;

        // Left side
        auto* rockL = new RenderObject(RenderObject::createSphere(5, 6), stoneTexture, 0);
        rockL->setScale(glm::vec3(rockSize));
        rockL->setPosition(glm::vec3(getTee().x - 4.2f * scale, surfaceY - rockSize*0.35f, z));
        addObject(rockL);

        // Right side
        auto* rockR = new RenderObject(RenderObject::createSphere(5, 6), stoneTexture, 0);
        rockR->setScale(glm::vec3(rockSize * 0.9f));
        rockR->setPosition(glm::vec3(getTee().x + 4.2f * scale, surfaceY - rockSize*0.35f, z));
        addObject(rockR);
    }

    // === ROCK RING AROUND CIRCLE - OPEN AT THE BOTTOM ===
    for (int i = 0; i < 48; ++i) {
        float angle = i * 7.5f;

        // Skip rocks in the bottom sector (entrance)
        if (angle > 60.0f && angle < 120.0f) {
            continue;
        }

        float radius = 8.05f * scale;

        glm::vec3 offset(
            cosf(glm::radians(angle)) * radius,
            0.0f,
            sinf(glm::radians(angle)) * radius
        );

        float rockSize = 0.27f + (rand() % 50)/160.0f;

        auto* rock = new RenderObject(RenderObject::createSphere(5, 6), stoneTexture, 0);
        rock->setScale(glm::vec3(rockSize));

        glm::vec3 rockPos = circularMound->getPosition() + offset;
        rockPos.y = surfaceY - rockSize * 0.35f;

        rock->setPosition(rockPos);
        addObject(rock);
    }
}