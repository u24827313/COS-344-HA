#include "Hole16.h"
#include "Terrain.h"
#include "RenderObject.h"
#include "TextureLoader.h"
#include <cmath>

void Hole16::design(Terrain& terrain) {
    Hole::design(terrain);

    GLuint grassTexture    = loadBMPTexture("assets/holes/grass.bmp");
    GLuint stoneTexture    = loadBMPTexture("assets/holes/stone.bmp");
    GLuint concreteTexture = loadBMPTexture("assets/holes/concrete.bmp");
    GLuint woodTexture     = loadBMPTexture("assets/holes/wood.bmp");
    if (grassTexture == 0 || stoneTexture == 0 || concreteTexture == 0 || woodTexture == 0) {
        return;
    }

    float greenHeight        = 0.3f;
    float raisedHeightOffset = 0.1f;
    float surfaceY           = getTee().y + raisedHeightOffset;

    // ---- CENTRAL HUB ----
    float centralRadius = 2.5f;
    std::vector<float> centralData = RenderObject::createClosedCylinder(32);
    for (size_t j = 0; j < centralData.size(); j += 8) {
        centralData[j + 0] *= centralRadius;
        centralData[j + 1] *= greenHeight;
        centralData[j + 2] *= centralRadius;

        float nx = centralData[j + 3];
        float nz = centralData[j + 5];
        float len = sqrtf(nx * nx + nz * nz);
        if (len > 0.0f) {
            centralData[j + 3] = nx / len;
            centralData[j + 5] = nz / len;
        }
    }

    RenderObject* central = new RenderObject(centralData, grassTexture, 0);
    glm::vec3 centralPos = glm::vec3(
        getTee().x,
        getTee().y - (greenHeight / 2.0f),
        getTee().z
    );
    central->setPosition(centralPos);
    addObject(central);

    // ---- LEFT ARM (extends to -X) ----
    float armWidth = 3.0f;
    float armLength = 3.5f;
    std::vector<float> leftArmData = RenderObject::createRoundedBox(
        armLength, greenHeight, armWidth, 0.4f, 12
    );
    RenderObject* leftArm = new RenderObject(leftArmData, grassTexture, 0);
    leftArm->setPosition(glm::vec3(
        centralPos.x - centralRadius - (armLength / 2.0f) + 0.5f,
        centralPos.y,
        centralPos.z
    ));
    addObject(leftArm);

    // ---- UPPER ARM (extends to +Z) ----
    float upperArmWidth  = 2.8f;
    float upperArmLength = 3.5f;
    std::vector<float> upperArmData = RenderObject::createRoundedBox(
        upperArmWidth, greenHeight, upperArmLength, 0.4f, 12
    );
    RenderObject* upperArm = new RenderObject(upperArmData, grassTexture, 0);
    upperArm->setPosition(glm::vec3(
        centralPos.x - 0.3f,
        centralPos.y,
        centralPos.z + centralRadius + (upperArmLength / 2.0f) - 0.3f
    ));
    addObject(upperArm);

    // ---- UPPER-RIGHT BULB (where the flag goes) ----
    float upperRightRadius = 2.2f;
    std::vector<float> upperRightData = RenderObject::createClosedCylinder(32);
    for (size_t j = 0; j < upperRightData.size(); j += 8) {
        upperRightData[j + 0] *= upperRightRadius;
        upperRightData[j + 1] *= greenHeight;
        upperRightData[j + 2] *= upperRightRadius;

        float nx = upperRightData[j + 3];
        float nz = upperRightData[j + 5];
        float len = sqrtf(nx * nx + nz * nz);
        if (len > 0.0f) {
            upperRightData[j + 3] = nx / len;
            upperRightData[j + 5] = nz / len;
        }
    }

    RenderObject* upperRight = new RenderObject(upperRightData, grassTexture, 0);
    glm::vec3 upperRightPos = glm::vec3(
        centralPos.x + 2.0f,
        centralPos.y,
        centralPos.z + 4.5f
    );
    upperRight->setPosition(upperRightPos);
    addObject(upperRight);

    // ---- CONNECTING ARM from upper arm to upper-right bulb ----
    float connWidth = 2.5f;
    float connLength = 2.5f;
    std::vector<float> connData = RenderObject::createRoundedBox(
        connLength, greenHeight, connWidth, 0.4f, 12
    );
    RenderObject* connArm = new RenderObject(connData, grassTexture, 0);
    connArm->setRotation(glm::vec3(0.0f, -30.0f, 0.0f));
    connArm->setPosition(glm::vec3(
        centralPos.x + 1.0f,
        centralPos.y,
        centralPos.z + 4.0f
    ));
    addObject(connArm);

    // ---- LOWER-LEFT EXTENSION (the small bulge in the sketch) ----
    float lowerBulgeRadius = 1.5f;
    std::vector<float> lowerBulgeData = RenderObject::createClosedCylinder(32);
    for (size_t j = 0; j < lowerBulgeData.size(); j += 8) {
        lowerBulgeData[j + 0] *= lowerBulgeRadius;
        lowerBulgeData[j + 1] *= greenHeight;
        lowerBulgeData[j + 2] *= lowerBulgeRadius;

        float nx = lowerBulgeData[j + 3];
        float nz = lowerBulgeData[j + 5];
        float len = sqrtf(nx * nx + nz * nz);
        if (len > 0.0f) {
            lowerBulgeData[j + 3] = nx / len;
            lowerBulgeData[j + 5] = nz / len;
        }
    }

    RenderObject* lowerBulge = new RenderObject(lowerBulgeData, grassTexture, 0);
    glm::vec3 lowerBulgePos = glm::vec3(
        centralPos.x - 1.5f,
        centralPos.y,
        centralPos.z - 3.5f
    );
    lowerBulge->setPosition(lowerBulgePos);
    addObject(lowerBulge);


    auto addCylinderRocks = [&](const glm::vec3& center, float orbitRadius, int count) {
        for (int i = 0; i < count; ++i) {
            float arcAngle = (float)i / count * 2.0f * (float)M_PI;
            glm::vec3 offset = glm::vec3(cosf(arcAngle), 0.0f, sinf(arcAngle)) * orbitRadius;
            glm::vec3 rockPos = center + offset;

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

    addCylinderRocks(centralPos, centralRadius + 0.2f, 16);
    addCylinderRocks(upperRightPos, upperRightRadius + 0.2f, 12);
    addCylinderRocks(lowerBulgePos, lowerBulgeRadius + 0.2f, 8);

    float pinSurfaceY = upperRightPos.y + greenHeight;

    // Cup
    float holeRadius = 0.15f;
    float holeDepth  = 0.2f;
    std::vector<float> cupData = RenderObject::createCylinder(16);
    RenderObject* golfHole = new RenderObject(cupData, stoneTexture, 0);
    golfHole->setScale(glm::vec3(holeRadius, holeDepth, holeRadius));
    golfHole->setPosition(glm::vec3(
        upperRightPos.x,
        pinSurfaceY - (holeDepth * 0.95f),
        upperRightPos.z
    ));
    addObject(golfHole);

    // Flagpole
    float poleRadius = 0.02f;
    float poleHeight = 1.2f;
    std::vector<float> poleData = RenderObject::createCylinder(8);
    RenderObject* flagPole = new RenderObject(poleData, concreteTexture, 0);
    flagPole->setScale(glm::vec3(poleRadius, poleHeight, poleRadius));
    flagPole->setPosition(glm::vec3(upperRightPos.x, pinSurfaceY, upperRightPos.z));
    addObject(flagPole);

    // Triangular flag
    float flagBottomWidth = 0.5f;
    float flagTopWidth    = 0.0f;
    float flagLength      = 0.4f;
    float flagThickness   = 0.01f;

    std::vector<float> flagData = RenderObject::createTrapezoid(
        flagBottomWidth, flagTopWidth, flagLength, flagThickness
    );
    RenderObject* flag = new RenderObject(flagData, concreteTexture, concreteTexture);
    flag->setRotation(glm::vec3(0.0f, 0.0f, -90.0f));
    flag->setPosition(glm::vec3(
        upperRightPos.x + flagLength / 2.0f,
        pinSurfaceY + poleHeight - (flagBottomWidth / 2.0f) - raisedHeightOffset,
        upperRightPos.z
    ));
    addObject(flag);
}