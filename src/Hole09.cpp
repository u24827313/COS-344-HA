#include "Hole09.h"
#include "Terrain.h"
#include "RenderObject.h"
#include "TextureLoader.h"
#include <glm/gtc/matrix_transform.hpp>

void Hole09::design(Terrain& terrain) {
    Hole::design(terrain);

    GLuint grassTex = loadBMPTexture("assets/terrain/grass.bmp");
    GLuint stoneTex = loadBMPTexture("assets/terrain/stone.bmp");
    GLuint concreteTex = loadBMPTexture("assets/terrain/concrete.bmp");
    if (!grassTex || !stoneTex || !concreteTex) return;

    float scale = 1.5f;
    float surfaceY = getTee().y + 0.1f;
    float rotationDeg = -20.0f;
    float cosR = cosf(glm::radians(rotationDeg));
    float sinR = sinf(glm::radians(rotationDeg));

    // Transform local (relative to tee) to world (for positions not relative to rectangle)
    auto rotatePoint = [&](float localX, float localZ) -> glm::vec3 {
        float worldX = localX * cosR - localZ * sinR;
        float worldZ = localX * sinR + localZ * cosR;
        return glm::vec3(getTee().x + worldX, surfaceY, getTee().z + worldZ);
    };

    // --- 1. Rectangular fairway ---
    float rectWidth  = 4.5f * scale;
    float rectLength = 12.0f * scale;
    float rectHeight = 0.2f;
    float rectCentreZ = -rectLength / 2.0f;          // local Z of rectangle centre
    glm::vec3 rectPos = rotatePoint(0.0f, rectCentreZ);
    rectPos.y = getTee().y - rectHeight/2.0f;        // centre Y of rectangle

    std::vector<float> rectData = RenderObject::createRoundedBox(rectWidth, rectHeight, rectLength, 0.3f, 12);
    RenderObject* fairway = new RenderObject(rectData, grassTex, 0);
    fairway->setPosition(rectPos);
    fairway->setRotation(glm::vec3(0.0f, rotationDeg, 0.0f));
    addObject(fairway);

    // --- 2. Circular green on the side (lowered) ---
    float moundRadius = 2.8f * scale;
    float moundHeight = 0.5f * scale;
    float moundLower = 0.25f;
    float moundLocalX = rectWidth/2.0f + moundRadius * 0.7f;
    float moundLocalZ = -rectLength/2.0f;             // centre of rectangle length
    glm::vec3 moundPos = rotatePoint(moundLocalX, moundLocalZ);
    moundPos.y = getTee().y - moundHeight/2.0f + 0.1f - moundLower;

    std::vector<float> moundData = RenderObject::createCylinder(48);
    RenderObject* green = new RenderObject(moundData, grassTex, 0);
    green->setScale(glm::vec3(moundRadius, moundHeight, moundRadius));
    green->setPosition(moundPos);
    addObject(green);

    terrain.addMound(moundPos.x, moundPos.z, (moundRadius + 0.8f) * scale, moundHeight);

    // --- 3. Flag on the green ---
    glm::vec3 flagPos = moundPos;
    flagPos.y += moundHeight;

    float cupScale = 0.18f * scale;
    std::vector<float> cupData = RenderObject::createCylinder(16);
    RenderObject* cup = new RenderObject(cupData, stoneTex, 0);
    cup->setScale(glm::vec3(cupScale, cupScale * 1.2f, cupScale));
    cup->setPosition(glm::vec3(flagPos.x, flagPos.y - 0.15f, flagPos.z));
    addObject(cup);

    float poleRadius = 0.04f * scale;
    float poleHeight = 1.8f * scale;
    std::vector<float> poleData = RenderObject::createCylinder(8);
    RenderObject* pole = new RenderObject(poleData, concreteTex, 0);
    pole->setScale(glm::vec3(poleRadius, poleHeight, poleRadius));
    pole->setPosition(flagPos);
    addObject(pole);

    float flagBottom = 0.55f * scale;
    float flagLength = 0.45f * scale;
    std::vector<float> flagData = RenderObject::createTrapezoid(flagBottom, 0.0f, flagLength, 0.02f);
    RenderObject* flag = new RenderObject(flagData, concreteTex, concreteTex);
    flag->setRotation(glm::vec3(0.0f, 0.0f, -90.0f));
    flag->setPosition(glm::vec3(flagPos.x + 0.28f * scale, flagPos.y + 1.4f * scale, flagPos.z));
    addObject(flag);

    // --- 4. Rocks around the green (at ground level) ---
    int numGreenRocks = 12;
    for (int i = 0; i < numGreenRocks; ++i) {
        float angle = i * (360.0f / numGreenRocks);
        float rad = glm::radians(angle);
        float rockX = moundLocalX + (moundRadius + 0.4f * scale) * cosf(rad);
        float rockZ = moundLocalZ + (moundRadius + 0.4f * scale) * sinf(rad);
        glm::vec3 rockPos = rotatePoint(rockX, rockZ);
        rockPos.y = surfaceY;

        float rockSize = 0.35f * scale;
        auto* rock = new RenderObject(RenderObject::createSphere(5, 6), stoneTex, 0);
        rock->setScale(glm::vec3(rockSize));
        rock->setPosition(rockPos);
        addObject(rock);
    }
}