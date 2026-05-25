#include "Hole17.h"
#include "Terrain.h"
#include "RenderObject.h"
#include "TextureLoader.h"
#include <cmath>

void Hole17::design(Terrain& terrain) {
    Hole::design(terrain);

    GLuint grassTexture    = loadBMPTexture("assets/terrain/grass.bmp");
    GLuint stoneTexture    = loadBMPTexture("assets/terrain/stone.bmp");
    GLuint concreteTexture = loadBMPTexture("assets/terrain/concrete.bmp");
    GLuint sandTexture     = loadBMPTexture("assets/terrain/sand.bmp");      // for the shared bunker
    GLuint woodTexture     = loadBMPTexture("assets/terrain/wood.bmp");
    GLuint walkwayTexture     = loadBMPTexture("assets/holes/concrete.bmp");
    if (grassTexture == 0 || stoneTexture == 0 || concreteTexture == 0 || sandTexture == 0) {
        return;
    }

    float greenHeight        = 0.3f;
    float raisedHeightOffset = 0.1f;
    float surfaceY           = getTee().y + raisedHeightOffset;

    
    // GREEN — large rounded shape on the left, built from overlapping cylinders
    
    // ---- MAIN cylinder (large center of the green) ----
    float mainRadius = 3.5f;
    std::vector<float> mainData = RenderObject::createClosedCylinder(32);
    for (size_t j = 0; j < mainData.size(); j += 8) {
        mainData[j + 0] *= mainRadius;
        mainData[j + 1] *= greenHeight;
        mainData[j + 2] *= mainRadius;

        float nx = mainData[j + 3];
        float nz = mainData[j + 5];
        float len = sqrtf(nx * nx + nz * nz);
        if (len > 0.0f) {
            mainData[j + 3] = nx / len;
            mainData[j + 5] = nz / len;
        }
    }

    RenderObject* mainGreen = new RenderObject(mainData, grassTexture, 0);
    glm::vec3 mainPos = glm::vec3(
        getTee().x,
        getTee().y - (greenHeight / 2.0f),
        getTee().z
    );
    mainGreen->setPosition(mainPos);
    addObject(mainGreen);

    // ---- LOWER BULGE (south-east extension) ----
    float lowerRadius = 2.5f;
    std::vector<float> lowerData = RenderObject::createClosedCylinder(32);
    for (size_t j = 0; j < lowerData.size(); j += 8) {
        lowerData[j + 0] *= lowerRadius;
        lowerData[j + 1] *= greenHeight;
        lowerData[j + 2] *= lowerRadius;

        float nx = lowerData[j + 3];
        float nz = lowerData[j + 5];
        float len = sqrtf(nx * nx + nz * nz);
        if (len > 0.0f) {
            lowerData[j + 3] = nx / len;
            lowerData[j + 5] = nz / len;
        }
    }

    RenderObject* lowerBulge = new RenderObject(lowerData, grassTexture, 0);
    glm::vec3 lowerPos = glm::vec3(
        getTee().x + 1.5f,
        getTee().y - (greenHeight / 2.0f),
        getTee().z - 3.0f
    );
    lowerBulge->setPosition(lowerPos);
    addObject(lowerBulge);

    // ---- UPPER BULGE (north extension, where the flag sits) ----
    float upperRadius = 2.2f;
    std::vector<float> upperData = RenderObject::createClosedCylinder(32);
    for (size_t j = 0; j < upperData.size(); j += 8) {
        upperData[j + 0] *= upperRadius;
        upperData[j + 1] *= greenHeight;
        upperData[j + 2] *= upperRadius;

        float nx = upperData[j + 3];
        float nz = upperData[j + 5];
        float len = sqrtf(nx * nx + nz * nz);
        if (len > 0.0f) {
            upperData[j + 3] = nx / len;
            upperData[j + 5] = nz / len;
        }
    }

    RenderObject* upperBulge = new RenderObject(upperData, grassTexture, 0);
    glm::vec3 upperPos = glm::vec3(
        getTee().x - 0.5f,
        getTee().y - (greenHeight / 2.0f),
        getTee().z + 3.5f
    );
    upperBulge->setPosition(upperPos);
    addObject(upperBulge);

    
    // SAND BUNKER

    float bunkerWidth  = 3.0f;
    float bunkerDepth  = 1.8f;
    float bunkerHeight = 0.35f;
    std::vector<float> bunkerData = RenderObject::createRoundedBox(
        bunkerWidth, bunkerHeight, bunkerDepth, 0.4f, 8
    );
    RenderObject* bunker = new RenderObject(bunkerData, walkwayTexture, 0);
    bunker->setPosition(glm::vec3(
        getTee().x + 2.5f,
        surfaceY + (bunkerHeight / 2.0f) - 0.02f,
        getTee().z - 0.5f
    ));
    bunker->setRotation(glm::vec3(180.0f, 0.0f, 0.0f));
    addObject(bunker);

    // ROCK BORDER 

    auto isInsideAnyGreen = [&](const glm::vec3& pos) {
        float dx, dz;
        dx = pos.x - mainPos.x;   dz = pos.z - mainPos.z;
        if (sqrtf(dx*dx + dz*dz) < mainRadius - 0.1f) return true;
        dx = pos.x - lowerPos.x;  dz = pos.z - lowerPos.z;
        if (sqrtf(dx*dx + dz*dz) < lowerRadius - 0.1f) return true;
        dx = pos.x - upperPos.x;  dz = pos.z - upperPos.z;
        if (sqrtf(dx*dx + dz*dz) < upperRadius - 0.1f) return true;
        
        // NEW: also skip if inside the bunker area
        glm::vec3 bunkerCenter(getTee().x + 2.5f, 0.0f, getTee().z - 0.5f);
        float bunkerHalfW = 2.5f / 2.0f + 0.3f;   // half width + a small margin
        float bunkerHalfD = 1.8f / 2.0f + 0.3f;
        if (fabsf(pos.x - bunkerCenter.x) < bunkerHalfW &&
            fabsf(pos.z - bunkerCenter.z) < bunkerHalfD) return true;
        
        return false;
    };

    auto addCylinderRocks = [&](const glm::vec3& center, float orbitRadius, int count) {
        for (int i = 0; i < count; ++i) {
            float arcAngle = (float)i / count * 2.0f * (float)M_PI;
            glm::vec3 offset = glm::vec3(cosf(arcAngle), 0.0f, sinf(arcAngle)) * orbitRadius;
            glm::vec3 rockPos = center + offset;

            if (isInsideAnyGreen(rockPos)) continue;  // skip rocks in shared areas

            float rockRadius = 0.35f + (static_cast<float>(rand()) / RAND_MAX) * 0.25f;
            std::vector<float> rockData = RenderObject::createSphere(4, 5);
            for (size_t j = 0; j < rockData.size(); j += 8) {
                rockData[j + 0] *= rockRadius;
                rockData[j + 1] *= rockRadius;
                rockData[j + 2] *= rockRadius;
            }

            RenderObject* rock = new RenderObject(rockData, stoneTexture, 0);
            rock->setPosition(glm::vec3(
                rockPos.x,
                surfaceY - (rockRadius * 0.15f),
                rockPos.z
            ));
            addObject(rock);
        }
    };

    addCylinderRocks(mainPos,  mainRadius + 0.2f,  18);
    addCylinderRocks(lowerPos, lowerRadius + 0.2f, 12);
    addCylinderRocks(upperPos, upperRadius + 0.2f, 12);

    // FLAG ASSEMBLY 
   
    float pinSurfaceY = upperPos.y + greenHeight;

    // Cup
    float holeRadius = 0.15f;
    float holeDepth  = 0.2f;
    std::vector<float> cupData = RenderObject::createCylinder(16);
    RenderObject* golfHole = new RenderObject(cupData, stoneTexture, 0);
    golfHole->setScale(glm::vec3(holeRadius, holeDepth, holeRadius));
    golfHole->setPosition(glm::vec3(
        upperPos.x,
        pinSurfaceY - (holeDepth * 0.95f),
        upperPos.z
    ));
    addObject(golfHole);

    // Flagpole
    float poleRadius = 0.02f;
    float poleHeight = 1.2f;
    std::vector<float> poleData = RenderObject::createCylinder(8);
    RenderObject* flagPole = new RenderObject(poleData, concreteTexture, 0);
    flagPole->setScale(glm::vec3(poleRadius, poleHeight, poleRadius));
    flagPole->setPosition(glm::vec3(upperPos.x, pinSurfaceY, upperPos.z));
    addObject(flagPole);

    // Triangular flag
    float flagBottomWidth = 0.5f;
    float flagTopWidth    = 0.0f;
    float flagLength      = 0.4f;
    float flagThickness   = 0.01f;

    std::vector<float> flagData = RenderObject::createTrapezoid(
        flagBottomWidth, flagTopWidth, flagLength, flagThickness
    );
    RenderObject* flag = new RenderObject(flagData, concreteTexture, 0);
    flag->setRotation(glm::vec3(0.0f, 0.0f, -90.0f));
    flag->setPosition(glm::vec3(
        upperPos.x + flagLength / 2.0f,
        pinSurfaceY + poleHeight - (flagBottomWidth / 2.0f) - raisedHeightOffset,
        upperPos.z
    ));
    addObject(flag);
}