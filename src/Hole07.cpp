#include "Hole07.h"
#include "Terrain.h"
#include "RenderObject.h"
#include "TextureLoader.h"

void Hole07::design(Terrain& terrain) {
    Hole::design(terrain);

    GLuint grassTexture = loadBMPTexture("assets/terrain/grass.bmp");
    GLuint stoneTexture = loadBMPTexture("assets/terrain/stone.bmp");
    GLuint concreteTexture = loadBMPTexture("assets/terrain/concrete.bmp");

    float surfaceY = getTee().y + 0.1f;
    
    float scale = 0.65f;        // ← Scale factor (smaller = less than 1.0)

    // === SMALLER RECTANGLE FAIRWAY ===
    std::vector<float> rectData = RenderObject::createRoundedBox(
        5.0f * scale, 
        1.1f, 
        32.0f * scale, 
        0.4f, 12
    );
    RenderObject* longRect = new RenderObject(rectData, grassTexture, grassTexture);
    longRect->setPosition(glm::vec3(
        getTee().x, 
        getTee().y - 0.55f, 
        getTee().z - 12.0f * scale
    ));
    addObject(longRect);

    // === SMALLER CIRCULAR MOUND ===
    std::vector<float> circleData = RenderObject::createCylinder(64);
    RenderObject* circularMound = new RenderObject(circleData, grassTexture, grassTexture);
    circularMound->setScale(glm::vec3(7.8f * scale, 0.85f, 7.8f * scale));
    circularMound->setPosition(glm::vec3(
        getTee().x, 
        getTee().y - 0.72f, 
        getTee().z - 29.0f * scale
    ));
    addObject(circularMound);

    terrain.addMound(
        getTee().x, 
        getTee().z - 29.0f * scale, 
        9.5f * scale, 
        1.6f
    );

    // === FLAG ===
    glm::vec3 pinPos = glm::vec3(
        getTee().x, 
        surfaceY + 0.65f, 
        getTee().z - 29.0f * scale
    );

    std::vector<float> cupData = RenderObject::createCylinder(16);
    RenderObject* cup = new RenderObject(cupData, stoneTexture, 0);
    cup->setScale(glm::vec3(0.18f, 0.25f, 0.18f));
    cup->setPosition(glm::vec3(pinPos.x, pinPos.y - 0.22f, pinPos.z));
    addObject(cup);

    std::vector<float> poleData = RenderObject::createCylinder(8);
    RenderObject* pole = new RenderObject(poleData, concreteTexture, 0);
    pole->setScale(glm::vec3(0.03f, 1.8f, 0.03f));
    pole->setPosition(pinPos);
    addObject(pole);

    std::vector<float> flagData = RenderObject::createTrapezoid(0.55f, 0.0f, 0.45f, 0.02f);
    RenderObject* flag = new RenderObject(flagData, concreteTexture, concreteTexture);
    flag->setRotation(glm::vec3(0.0f, 0.0f, -90.0f));
    flag->setPosition(glm::vec3(pinPos.x + 0.28f, pinPos.y + 1.4f, pinPos.z));
    addObject(flag);

    // === ROCKS ALONG RECTANGLE SIDES ===
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