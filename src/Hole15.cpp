#include "Hole15.h"
#include "Terrain.h"
#include "RenderObject.h"
#include "TextureLoader.h"
#include <cmath>

void Hole15::design(Terrain& terrain) {
    Hole::design(terrain);

    GLuint grassTexture    = loadBMPTexture("assets/terrain/grass.bmp");
    GLuint stoneTexture    = loadBMPTexture("assets/terrain/stone.bmp");
    GLuint concreteTexture = loadBMPTexture("assets/terrain/concrete.bmp");
    GLuint woodTexture     = loadBMPTexture("assets/terrain/wood.bmp");
    if (grassTexture == 0 || stoneTexture == 0 || concreteTexture == 0 || woodTexture == 0) {
        return;
    }

    float greenHeight        = 0.3f;
    float raisedHeightOffset = 0.1f;
    float surfaceY           = getTee().y + raisedHeightOffset;

    // ---- LEFT BULB (the lower-left rounded end, where the flag will sit) ----
    float leftBulbRadius = 2.2f;
    std::vector<float> leftBulbData = RenderObject::createClosedCylinder(32);
    for (size_t j = 0; j < leftBulbData.size(); j += 8) {
        leftBulbData[j + 0] *= leftBulbRadius;
        leftBulbData[j + 1] *= greenHeight;
        leftBulbData[j + 2] *= leftBulbRadius;

        float nx = leftBulbData[j + 3];
        float nz = leftBulbData[j + 5];
        float len = sqrtf(nx * nx + nz * nz);
        if (len > 0.0f) {
            leftBulbData[j + 3] = nx / len;
            leftBulbData[j + 5] = nz / len;
        }
    }

    RenderObject* leftBulb = new RenderObject(leftBulbData, grassTexture, 0);
    glm::vec3 leftBulbPos = glm::vec3(
        getTee().x,
        getTee().y - (greenHeight / 2.0f),
        getTee().z
    );
    leftBulb->setPosition(leftBulbPos);
    addObject(leftBulb);

    // ---- CURVED TRACK BODY — segments arcing from left bulb up to right bulb ----
    float bodyWidth      = 3.5f;
    float bodySegLength  = 1.0f;
    float bodyRounding   = 0.5f;
    int   bodySegments   = 6;
    float bodyCurveAngle = 50.0f;  // total angle the arc sweeps

    std::vector<float> bodyData = RenderObject::createRoundedBox(
        bodyWidth, greenHeight, bodySegLength, bodyRounding, 8
    );

    // Start at the right edge of the left bulb, pointing right (+X with slight +Z arc)
    float currentAngle = 90.0f;  // 90 = pointing +X
    float angleStep    = bodyCurveAngle / (bodySegments - 1);

    glm::vec3 bodyPos = leftBulbPos;
    bodyPos.x += leftBulbRadius + (bodySegLength / 2.0f) - 0.5f;  // slight overlap with bulb

    for (int i = 0; i < bodySegments; ++i) {
        RenderObject* seg = new RenderObject(bodyData, grassTexture, 0);
        seg->setPosition(bodyPos);
        seg->setRotation(glm::vec3(0.0f, currentAngle - 90.0f, 0.0f));
        addObject(seg);

        glm::vec3 forward = glm::vec3(
            sinf(glm::radians(currentAngle)),
            0.0f,
            cosf(glm::radians(currentAngle))
        );
        bodyPos += forward * bodySegLength;
        currentAngle -= angleStep;  // curve upward (-angle = curving toward +Z)
    }

    // ---- RIGHT BULB (the upper-right rounded end) ----
    float rightBulbRadius = 2.0f;
    std::vector<float> rightBulbData = RenderObject::createClosedCylinder(32);
    for (size_t j = 0; j < rightBulbData.size(); j += 8) {
        rightBulbData[j + 0] *= rightBulbRadius;
        rightBulbData[j + 1] *= greenHeight;
        rightBulbData[j + 2] *= rightBulbRadius;

        float nx = rightBulbData[j + 3];
        float nz = rightBulbData[j + 5];
        float len = sqrtf(nx * nx + nz * nz);
        if (len > 0.0f) {
            rightBulbData[j + 3] = nx / len;
            rightBulbData[j + 5] = nz / len;
        }
    }

    RenderObject* rightBulb = new RenderObject(rightBulbData, grassTexture, 0);
    // Position at the end of the curved body
    float finalAngleRad = glm::radians(currentAngle + angleStep);
    glm::vec3 rightBulbPos = bodyPos
        - glm::vec3(sinf(finalAngleRad), 0.0f, cosf(finalAngleRad)) * (bodySegLength * 0.5f)
        + glm::vec3(sinf(finalAngleRad), 0.0f, cosf(finalAngleRad)) * (rightBulbRadius - 0.5f);
    rightBulbPos.y = leftBulbPos.y;
    rightBulb->setPosition(rightBulbPos);
    addObject(rightBulb);

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

    addCylinderRocks(leftBulbPos, leftBulbRadius + 0.2f, 14);
    addCylinderRocks(rightBulbPos, rightBulbRadius + 0.2f, 12);

    float pinSurfaceY = leftBulbPos.y + greenHeight;

    // Cup
    float holeRadius = 0.15f;
    float holeDepth  = 0.2f;
    std::vector<float> cupData = RenderObject::createCylinder(16);
    RenderObject* golfHole = new RenderObject(cupData, stoneTexture, 0);
    golfHole->setScale(glm::vec3(holeRadius, holeDepth, holeRadius));
    golfHole->setPosition(glm::vec3(
        getPin().x,
        pinSurfaceY - (holeDepth * 0.95f),
        getPin().z
    ));
    addObject(golfHole);

    // Flagpole
    float poleRadius = 0.02f;
    float poleHeight = 1.2f;
    std::vector<float> poleData = RenderObject::createCylinder(8);
    RenderObject* flagPole = new RenderObject(poleData, concreteTexture, 0);
    flagPole->setScale(glm::vec3(poleRadius, poleHeight, poleRadius));
    flagPole->setPosition(glm::vec3(getPin().x, pinSurfaceY, getPin().z));
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
        getPin().x + flagLength / 2.0f,
        pinSurfaceY + poleHeight - (flagBottomWidth / 2.0f) - raisedHeightOffset,
        getPin().z
    ));
    addObject(flag);
}