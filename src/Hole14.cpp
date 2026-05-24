#include "Hole14.h"
#include "Terrain.h"
#include "RenderObject.h"
#include "TextureLoader.h"
#include <cmath>
#include <iostream>

void Hole14::design(Terrain& terrain) {
    Hole::design(terrain);

    // Load textures
    GLuint grassTexture     = loadBMPTexture("assets/holes/grass.bmp");
    GLuint darkGrassTexture = loadBMPTexture("assets/holes/dirt.bmp");   
    GLuint stoneTexture     = loadBMPTexture("assets/holes/stone.bmp");
    GLuint concreteTexture  = loadBMPTexture("assets/holes/concrete.bmp");
    GLuint woodTexture      = loadBMPTexture("assets/holes/wood.bmp");
    GLuint cobblestoneTexture = loadBMPTexture("assets/holes/cobblestone.bmp");
    GLuint whiteFlag = loadBMPTexture("assets/holes/white.bmp");

    if (grassTexture == 0 || stoneTexture == 0 || concreteTexture == 0 || woodTexture == 0 || cobblestoneTexture == 0) {
        std::cerr << "[Hole 14] Texture loading failed:\n";
        std::cerr << "[Hole 14: grass] " << grassTexture << "\n";
        std::cerr << "[Hole 14: stone] " << stoneTexture << "\n";
        std::cerr << "[Hole 14: concrete] " << concreteTexture << "\n";
        std::cerr << "[Hole 14: wood] " << woodTexture << "\n";
        std::cerr << "[Hole 14: cobblestone] " << cobblestoneTexture << "\n";
        return;
    }

    float greenHeight        = 0.3f;
    float raisedHeightOffset = 0.1f;
    float surfaceY           = getTee().y + raisedHeightOffset;

    // =========================================================================
    // GREEN PLATFORM — oval/teardrop, wider at the bottom, narrower at the top.
    // Built from two stacked cylinders following the Hole03 cylinder pattern.
    // =========================================================================

    // ---- LOWER (wider) part of the green — a flat cylinder ----
    float lowerRadius = 3.0f;
    std::vector<float> lowerData = RenderObject::createCylinder(32);
    for (size_t j = 0; j < lowerData.size(); j += 8) {
        lowerData[j + 0] *= lowerRadius;
        lowerData[j + 1] *= greenHeight;
        lowerData[j + 2] *= lowerRadius;

        // Re-normalize lighting vectors after scaling (same as Hole03)
        float nx = lowerData[j + 3];
        float nz = lowerData[j + 5];
        float len = sqrtf(nx * nx + nz * nz);
        if (len > 0.0f) {
            lowerData[j + 3] = nx / len;
            lowerData[j + 5] = nz / len;
        }
    }

    RenderObject* lowerGreen = new RenderObject(lowerData, grassTexture, grassTexture);
    glm::vec3 lowerPos = glm::vec3(
        getTee().x,
        getTee().y - (greenHeight / 2.0f),
        getTee().z
    );
    lowerGreen->setPosition(lowerPos);
    lowerGreen->setRotation(glm::vec3(0.0f, 0.0f, 0.0f));
    addObject(lowerGreen);

    // ---- UPPER (narrower) part of the green — smaller cylinder, north of lower ----
    float upperRadius = 2.2f;
    std::vector<float> upperData = RenderObject::createCylinder(32);
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

    RenderObject* upperGreen = new RenderObject(upperData, grassTexture, grassTexture);
    glm::vec3 upperPos = glm::vec3(
        getTee().x,
        getTee().y - (greenHeight / 2.0f),
        getTee().z + lowerRadius + upperRadius - 1.5f   // overlap so the two read as one shape
    );
    upperGreen->setPosition(upperPos);
    upperGreen->setRotation(glm::vec3(0.0f, 0.0f, 0.0f));
    addObject(upperGreen);

    // ---- BRIDGING segment between the two cylinders (the narrow neck) ----
    float neckWidth      = 2.8f;
    float neckLength     = 2.5f;
    float neckRounding   = 0.4f;
    std::vector<float> neckData = RenderObject::createRoundedBox(
        neckWidth, greenHeight, neckLength, neckRounding, 8
    );
    RenderObject* neck = new RenderObject(neckData, grassTexture, grassTexture);
    neck->setPosition(glm::vec3(
        getTee().x,
        getTee().y - (greenHeight / 2.0f),
        (lowerPos.z + upperPos.z) * 0.5f
    ));
    addObject(neck);

    // =========================================================================
    // DARKER GRASS GUIDE PATH (the purple curve in the sketch)
    // A curved strip of darker grass weaving through the green, from the entry
    // at the bottom up around the right side to the upper green.
    // Built from rotated segments, same pattern as Hole03's curved track.
    // =========================================================================

    float guideSegLength = 1.0f;
    float guideSegWidth  = 1.5f;
    float guideHeight    = 0.06f;
    int   guideSegments  = 10;
    float guideTotalCurve = 90.0f;   

    std::vector<float> guideSegData = RenderObject::createRoundedBox(
        guideSegWidth, guideHeight, guideSegLength, 0.05f, 8
    );

    float guideAngle = 180.0f;       // start pointing -Z (into the green from the entry)
    float guideStep  = guideTotalCurve / (guideSegments - 1);

    glm::vec3 guidePos = glm::vec3(
        getTee().x + 3.9f,                 // slightly right of center
        surfaceY + (guideHeight / 2.0f) - 0.02f,
        getTee().z - lowerRadius + 0.3f    // start near the bottom edge
    );

    for (int i = 0; i < guideSegments; ++i) {
        RenderObject* seg = new RenderObject(guideSegData, darkGrassTexture, darkGrassTexture);
        seg->setPosition(guidePos);
        seg->setRotation(glm::vec3(0.0f, guideAngle - 180.0f, 0.0f));
        addObject(seg);

        glm::vec3 forward = glm::vec3(
            sinf(glm::radians(guideAngle)),
            0.0f,
            cosf(glm::radians(guideAngle))
        );
        // Walk forward but reverse direction (we want the path to go INTO the green, +Z)
        guidePos -= forward * guideSegLength;
        guideAngle += guideStep;
    }

    // =========================================================================
    // ENTRY at the bottom (the red blob at the bottom of the sketch)
    // A wooden ramp/platform leading into the green from below.
    // =========================================================================

    float entryWidth   = 10.5f;
    float entryHeight  = 0.15f;
    float entryDepth   = 1.5f;
    std::vector<float> entryData = RenderObject::createRoundedBox(
        entryWidth, entryHeight, entryDepth, 0.08f, 8
    );
    RenderObject* entry = new RenderObject(entryData, concreteTexture, 0);
    entry->setPosition(glm::vec3(
        getTee().x,
        surfaceY + (entryHeight / 2.0f) - 0.05f,
        getTee().z - lowerRadius - (entryDepth / 2.0f) + 0.4f
    ));
    addObject(entry);

    // =========================================================================
    // LEFT-SIDE FEATURE (the red blob on the left of the sketch)
    // A small rocky outcrop — a stone platform with a cluster of rocks on top.
    // =========================================================================

    /*float featureSize = 1.6f;
    std::vector<float> featureData = RenderObject::createRoundedBox(
        featureSize, 0.3f, featureSize, 0.2f, 8
    );
    RenderObject* feature = new RenderObject(featureData, stoneTexture, 0);
    glm::vec3 featurePos = glm::vec3(
        getTee().x - lowerRadius - 1.0f,
        surfaceY + 0.15f,
        getTee().z - 0.5f
    );
    feature->setPosition(featurePos);
    addObject(feature);

    // Decorative rocks on top of the outcrop
    int numFeatureRocks = 1;
    for (int i = 0; i < numFeatureRocks; ++i) {
        float angle = (float)i / numFeatureRocks * 2.0f * (float)M_PI;
        float rockRadius = 0.3f + (static_cast<float>(rand()) / RAND_MAX) * 0.15f;

        std::vector<float> rockData = RenderObject::createSphere(4, 5);
        for (size_t j = 0; j < rockData.size(); j += 8) {
            rockData[j + 0] *= rockRadius;
            rockData[j + 1] *= rockRadius;
            rockData[j + 2] *= rockRadius;
        }

        RenderObject* rock = new RenderObject(rockData, stoneTexture, 0);
        rock->setPosition(glm::vec3(
            featurePos.x + cosf(angle) * 0.4f,
            featurePos.y + 0.25f,
            featurePos.z + sinf(angle) * 0.4f
        ));
        addObject(rock);
    }*/

    // =========================================================================
    // ROCK BORDER — outlines both halves of the green (lower and upper)
    // Uses the same circular-rock pattern as Hole03's cylinder rocks.
    // =========================================================================

    int numLowerRocks = 14;
    float lowerOrbit  = lowerRadius + 0.2f;
    for (int i = 0; i < numLowerRocks; ++i) {
        // Skip rocks near the entry (bottom) and near the neck (top) so they don't block them
        float t = (float)i / numLowerRocks;
        float arcAngle = t * 2.0f * (float)M_PI;

        // Skip the bottom (entry side, ~270° or -PI/2) and top (neck, ~90° or PI/2)
        float deg = arcAngle * 180.0f / (float)M_PI;
        if ((deg > 250.0f && deg < 290.0f) || (deg > 70.0f && deg < 110.0f)) continue;

        glm::vec3 offset = glm::vec3(cosf(arcAngle), 0.0f, sinf(arcAngle)) * lowerOrbit;
        glm::vec3 rockPos = lowerPos + offset;

        float rockRadius = 0.35f + (static_cast<float>(rand()) / RAND_MAX) * 0.3f;
        std::vector<float> rockData = RenderObject::createSphere(4, 5);
        for (size_t j = 0; j < rockData.size(); j += 8) {
            rockData[j + 0] *= rockRadius;
            rockData[j + 1] *= rockRadius;
            rockData[j + 2] *= rockRadius;
        }

        RenderObject* rock = new RenderObject(rockData, stoneTexture, 0);
        rock->setPosition(glm::vec3(rockPos.x, surfaceY - (rockRadius * 0.15f), rockPos.z));
        addObject(rock);
    }

    int numUpperRocks = 15;
    float upperOrbit  = upperRadius + 0.2f;
    for (int i = 0; i < numUpperRocks; ++i) {
        float t = (float)i / numUpperRocks;
        float arcAngle = t * 2.0f * (float)M_PI;

        // Skip the bottom of the upper (where it connects to the neck)
        float deg = arcAngle * 180.0f / (float)M_PI;
        if (deg > 250.0f && deg < 290.0f) continue;

        glm::vec3 offset = glm::vec3(cosf(arcAngle), 0.0f, sinf(arcAngle)) * upperOrbit;
        glm::vec3 rockPos = upperPos + offset;

        float rockRadius = 0.3f + (static_cast<float>(rand()) / RAND_MAX) * 0.25f;
        std::vector<float> rockData = RenderObject::createSphere(4, 5);
        for (size_t j = 0; j < rockData.size(); j += 8) {
            rockData[j + 0] *= rockRadius;
            rockData[j + 1] *= rockRadius;
            rockData[j + 2] *= rockRadius;
        }

        RenderObject* rock = new RenderObject(rockData, stoneTexture, 0);
        rock->setPosition(glm::vec3(rockPos.x, surfaceY - (rockRadius * 0.15f), rockPos.z));
        addObject(rock);
    }

    float pinSurfaceY = upperPos.y + greenHeight;

    // Cup
    float holeRadius = 0.15f;
    float holeDepth  = 0.2f;
    std::vector<float> cupData = RenderObject::createCylinder(16);
    RenderObject* golfHole = new RenderObject(cupData, whiteFlag, 0);
    golfHole->setScale(glm::vec3(holeRadius, holeDepth, holeRadius));
    golfHole->setPosition(glm::vec3(
        getPin().x,
        pinSurfaceY - (holeDepth * 0.95f),
        getPin().z - 0.8f
    ));
    addObject(golfHole);

    // Flagpole
    float poleRadius = 0.02f;
    float poleHeight = 1.2f;
    std::vector<float> poleData = RenderObject::createCylinder(8);
    RenderObject* flagPole = new RenderObject(poleData, concreteTexture, 0);
    flagPole->setScale(glm::vec3(poleRadius, poleHeight, poleRadius));
    flagPole->setPosition(glm::vec3(getPin().x, pinSurfaceY, getPin().z - 0.8f));
    addObject(flagPole);

    // Triangular flag
    float flagBottomWidth = 0.5f;
    float flagTopWidth    = 0.0f;
    float flagLength      = 0.4f;
    float flagThickness   = 0.01f;

    std::vector<float> flagData = RenderObject::createTrapezoid(
        flagBottomWidth, flagTopWidth, flagLength, flagThickness
    );
    RenderObject* flag = new RenderObject(flagData, concreteTexture, whiteFlag);
    flag->setRotation(glm::vec3(0.0f, 0.0f, -90.0f));
    flag->setPosition(glm::vec3(
        getPin().x + flagLength / 2.0f,
        pinSurfaceY + poleHeight - (flagBottomWidth / 2.0f) - raisedHeightOffset,
        getPin().z - 0.8f
    ));
    addObject(flag);
}