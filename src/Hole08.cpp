// Hole08.cpp – Oval at tee end, bigger higher rocks around it
#include "Hole08.h"
#include "Terrain.h"
#include "RenderObject.h"
#include "TextureLoader.h"

void Hole08::design(Terrain& terrain) {
    Hole::design(terrain);

    GLuint grassTexture = loadBMPTexture("assets/terrain/grass.bmp");
    GLuint stoneTexture = loadBMPTexture("assets/terrain/stone.bmp");
    GLuint concreteTexture = loadBMPTexture("assets/terrain/concrete.bmp");

    float surfaceY = getTee().y + 0.1f;
    float scale = 0.65f;

    // --- OVAL GREEN (two overlapping circles) AT THE TEE END ---
    std::vector<float> circleData = RenderObject::createCylinder(64);
    float lateralOffset = 2.0f;
    float radiusX = 6.5f * scale;
    float radiusZ = 5.0f * scale;
    float yOffset = -0.75f;

    // Left part
    RenderObject* leftMound = new RenderObject(circleData, grassTexture, grassTexture);
    leftMound->setScale(glm::vec3(radiusX, 0.75f, radiusZ));
    leftMound->setPosition(glm::vec3(getTee().x - lateralOffset, getTee().y + yOffset, getTee().z));
    addObject(leftMound);

    // Right part
    RenderObject* rightMound = new RenderObject(circleData, grassTexture, grassTexture);
    rightMound->setScale(glm::vec3(radiusX, 0.75f, radiusZ));
    rightMound->setPosition(glm::vec3(getTee().x + lateralOffset, getTee().y + yOffset, getTee().z));
    addObject(rightMound);

    terrain.addMound(getTee().x, getTee().z, 9.5f * scale, 1.5f);

    // --- FLAG ON THE OVAL ---
    glm::vec3 pinPos = glm::vec3(getTee().x, surfaceY + 0.65f, getTee().z);

    std::vector<float> cupData = RenderObject::createCylinder(16);
    RenderObject* cup = new RenderObject(cupData, stoneTexture, 0);
    cup->setScale(glm::vec3(0.18f, 0.25f, 0.18f));
    cup->setPosition(glm::vec3(pinPos.x, pinPos.y - 0.22f, pinPos.z));
    addObject(cup);

    std::vector<float> poleData = RenderObject::createCylinder(8);
    RenderObject* pole = new RenderObject(poleData, concreteTexture, 0);
    pole->setScale(glm::vec3(0.03f, 1.9f, 0.03f));
    pole->setPosition(pinPos);
    addObject(pole);

    std::vector<float> flagData = RenderObject::createTrapezoid(0.55f, 0.0f, 0.45f, 0.02f);
    RenderObject* flag = new RenderObject(flagData, concreteTexture, concreteTexture);
    flag->setRotation(glm::vec3(0.0f, 0.0f, -90.0f));
    flag->setPosition(glm::vec3(pinPos.x + 0.28f, pinPos.y + 1.45f, pinPos.z));
    addObject(flag);

    // --- RECTANGLE FAIRWAY (extending behind the oval) ---
    std::vector<float> rectData = RenderObject::createRoundedBox(
        4.2f * scale, 1.1f, 36.0f * scale, 0.4f, 12
    );
    RenderObject* longRect = new RenderObject(rectData, grassTexture, grassTexture);
    longRect->setPosition(glm::vec3(
        getTee().x, getTee().y - 0.55f, getTee().z - (18.0f * scale)
    ));
    addObject(longRect);

    // --- ROCKS ALONG RECTANGLE SIDES (unchanged) ---
    for (int i = 0; i < 14; ++i) {
        float z = getTee().z - 4.0f - i * 2.0f * scale;
        float rockSize = (0.30f + (rand() % 50)/160.0f) * scale;
        auto* rockL = new RenderObject(RenderObject::createSphere(5, 6), stoneTexture, 0);
        rockL->setScale(glm::vec3(rockSize));
        rockL->setPosition(glm::vec3(getTee().x - 3.6f * scale, surfaceY - rockSize*0.35f, z));
        addObject(rockL);

        auto* rockR = new RenderObject(RenderObject::createSphere(5, 6), stoneTexture, 0);
        rockR->setScale(glm::vec3(rockSize * 0.9f));
        rockR->setPosition(glm::vec3(getTee().x + 3.6f * scale, surfaceY - rockSize*0.35f, z));
        addObject(rockR);
    }

    // --- BIGGER, HIGHER ROCKS AROUND THE OVAL GREEN ---
    int numRocks = 48;
    // Expanded ring radius to fit larger rocks
    float ringRadiusX = 7.6f * scale;
    float ringRadiusZ = 6.4f * scale;

    for (int i = 0; i < numRocks; ++i) {
        float angle = i * (360.0f / numRocks);
        // Leave an opening on the back side (where fairway attaches)
        if (angle > 240.0f && angle < 300.0f) {
            continue;
        }

        float rad = glm::radians(angle);
        glm::vec3 offset(
            cosf(rad) * ringRadiusX,
            0.0f,
            sinf(rad) * ringRadiusZ
        );

        // Bigger rocks – nearly double the previous size
        float rockSize = 0.65f * scale; 
        auto* rock = new RenderObject(RenderObject::createSphere(5, 6), stoneTexture, 0);
        rock->setScale(glm::vec3(rockSize));

        // Position rocks HIGHER – sitting on top of the grass, not buried
        glm::vec3 rockPos = glm::vec3(
            getTee().x,
            surfaceY + rockSize * 0.1f,
            getTee().z
        ) + offset;

        rock->setPosition(rockPos);
        addObject(rock);
    }
}