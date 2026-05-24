#include "Hole04.h"
#include "Terrain.h"
#include "RenderObject.h"
#include "TextureLoader.h"

void Hole04::design(Terrain &terrain)
{
    Hole::design(terrain);

    GLuint grassTexture = loadBMPTexture("assets/terrain/grass.bmp");
    GLuint stoneTexture = loadBMPTexture("assets/terrain/stone.bmp");

    if (grassTexture == 0)
        return;

    float platformHeight = 2.5f;
    float raisedOffset = 0.2f;

    float pathWidth = 6.0f;
    float segLength = 4.0f;
    float roundness = 0.25f;

    glm::vec3 startPos = getTee();

    // =========================================================
    // PATH
    // =========================================================

    std::vector<float> pathData =
        RenderObject::createRoundedBox(
            pathWidth,
            platformHeight,
            segLength,
            roundness,
            10);

    glm::vec3 pos = startPos;
    float angle = 180.0f;

    for (int i = 0; i < 8; i++)
    {
        RenderObject *seg =
            new RenderObject(pathData, grassTexture, 0);

        glm::vec3 forward(
            sinf(glm::radians(angle)),
            0.0f,
            cosf(glm::radians(angle)));

        glm::vec3 segPos = pos;
        segPos.y = getTee().y - (platformHeight * 0.5f) + raisedOffset;

        seg->setPosition(segPos);
        seg->setRotation(glm::vec3(0.0f, angle - 180.0f, 0.0f));
        addObject(seg);

        pos += forward * segLength;
        angle -= 1.8f;
    }

    for (int i = 0; i < 10; i++)
    {
        RenderObject *seg =
            new RenderObject(pathData, grassTexture, 0);

        glm::vec3 forward(
            sinf(glm::radians(angle)),
            0.0f,
            cosf(glm::radians(angle)));

        glm::vec3 segPos = pos;
        segPos.y = getTee().y - (platformHeight * 0.5f) + raisedOffset;

        seg->setPosition(segPos);
        seg->setRotation(glm::vec3(0.0f, angle - 180.0f, 0.0f));
        addObject(seg);

        pos += forward * segLength;
        angle += 6.0f;
    }

    // =========================================================
    // GREEN
    // =========================================================

    float greenRadius = 3.5f;

    std::vector<float> greenData =
        RenderObject::createCylinder(70);

    RenderObject *green =
        new RenderObject(greenData, grassTexture, grassTexture);

    green->setScale(glm::vec3(
        greenRadius,
        platformHeight * 0.5f,
        greenRadius));

    glm::vec3 greenPos = pos;

    greenPos.y = getTee().y - (platformHeight * 0.5f) + raisedOffset;

    green->setPosition(greenPos);
    addObject(green);

    // =========================================================
    // C-SHAPED HOLLOW HEDGE (WITH OPENING)
    // =========================================================

    float innerRadius = greenRadius + 0.8f;
    float outerRadius = greenRadius + 2.2f;
    float hedgeHeight = 2.8f;

    int segments = 140;

    float gapStart = M_PI;             // 180°
    float gapEnd = 3.0f * M_PI / 2.0f; // 270°// 90° opening

    std::vector<float> hedgeData;

    for (int i = 0; i < segments; i++)
    {
        float t1 = (float)i / segments * 2.0f * M_PI;
        float t2 = (float)(i + 1) / segments * 2.0f * M_PI;

        // skip opening
        if (t1 >= gapStart && t1 <= gapEnd)
            continue;

        float c1 = cosf(t1), s1 = sinf(t1);
        float c2 = cosf(t2), s2 = sinf(t2);

        glm::vec3 v0(outerRadius * c1, 0.0f, outerRadius * s1);
        glm::vec3 v1(outerRadius * c2, 0.0f, outerRadius * s2);
        glm::vec3 v2(innerRadius * c2, 0.0f, innerRadius * s2);
        glm::vec3 v3(innerRadius * c1, 0.0f, innerRadius * s1);

        glm::vec3 v4 = v0 + glm::vec3(0, hedgeHeight, 0);
        glm::vec3 v5 = v1 + glm::vec3(0, hedgeHeight, 0);
        glm::vec3 v6 = v2 + glm::vec3(0, hedgeHeight, 0);
        glm::vec3 v7 = v3 + glm::vec3(0, hedgeHeight, 0);

        auto push = [&](glm::vec3 v)
        {
            hedgeData.push_back(v.x);
            hedgeData.push_back(v.y);
            hedgeData.push_back(v.z);
            hedgeData.push_back(0);
            hedgeData.push_back(1);
            hedgeData.push_back(0);
            hedgeData.push_back(0);
            hedgeData.push_back(0);
        };

        // outer
        push(v0);
        push(v1);
        push(v5);
        push(v0);
        push(v5);
        push(v4);

        // inner
        push(v2);
        push(v3);
        push(v7);
        push(v2);
        push(v7);
        push(v6);

        // top
        push(v4);
        push(v5);
        push(v6);
        push(v4);
        push(v6);
        push(v7);
    }

    RenderObject *hedge =
        new RenderObject(hedgeData, grassTexture, grassTexture);

    hedge->setPosition(greenPos);
    addObject(hedge);

    // =========================================================
    // GARDEN WALLS AROUND GREEN
    // =========================================================
    float wallHeight = 2.8f;

    std::vector<float> wallData =
        RenderObject::createRoundedBox(
            outerRadius - innerRadius,
            wallHeight,
            1.0f,
            roundness,
            10);
        glm::vec3 wallPos = greenPos+ glm::vec3(0, 0, -(innerRadius + outerRadius) * 0.5f);
        wallPos.y = getTee().y - (platformHeight * 0.5f) + raisedOffset + (wallHeight * 0.5f);
        RenderObject *wall =
        new RenderObject(wallData, grassTexture, 0);
        wall->setPosition(wallPos);
        addObject(wall);

    // =========================================================
    // STRAIGHT ROAD FROM OPENING INTO GREEN
    // =========================================================

    float roadWidth = 62.0f;
    float roadLength = 10.0f;

    float roadAngle = 5.0f * M_PI / 4.0f; // 225° (LEFT opening)

    glm::vec3 roadDir(
        cosf(roadAngle),
        0.0f,
        sinf(roadAngle));

    glm::vec3 roadStart =
        greenPos + roadDir * (outerRadius + 2.0f);

    std::vector<float> roadData =
        RenderObject::createRoundedBox(
            roadWidth,
            platformHeight,
            roadLength,
            roundness,
            10);

    RenderObject *road =
        new RenderObject(roadData, grassTexture, 0);

    glm::vec3 roadPos = roadStart;
    roadPos.y = getTee().y - (platformHeight * 0.5f) + raisedOffset;

    road->setPosition(roadPos);

    road->setRotation(glm::vec3(
        0.0f,
        glm::degrees(roadAngle) - 90.0f,
        0.0f));

    addObject(road);



// =========================================================
// Hole 4 last section
// =========================================================
float broadWidth = 62.0f;
    float broadLength = 10.0f;

    float broadAngle = 5.0f * M_PI / 4.0f; // 225° (LEFT opening)

    glm::vec3 broadDir(
        cosf(broadAngle),
        0.0f,
        sinf(broadAngle));

    std::vector<float> broadData =
        RenderObject::createRoundedBox(
            roadWidth,
            platformHeight,
            roadLength,
            roundness,
            10);

    RenderObject *broad =
        new RenderObject(broadData, grassTexture, 0);

    glm::vec3 broadPos = roadStart;
    broadPos.y = getTee().y - (platformHeight * 0.5f) + raisedOffset;

    broad->setPosition(broadPos);

    broad->setRotation(glm::vec3(
        0.0f,
        glm::degrees(broadAngle) - 90.0f,
        0.0f));

    addObject(broad);

// =========================================================
// LEFT SIDE WALL ALONG STRAIGHT ROAD
// =========================================================

float lroadAngle = 5.0f * M_PI / 4.0f; // same as road
glm::vec3 lroadDir(cosf(lroadAngle), 0.0f, sinf(lroadAngle));

// perpendicular (LEFT side of road)
glm::vec3 leftDir(6*lroadDir.z, 0.0f, 2*lroadDir.x);

float lwallSegmentCount = 1;
float lwallSegmentLength = 1.0f;
float lwallHeight = 2.8f;
float lwallOffset = 2.8f; // distance from road center

std::vector<float> wallMesh =
    RenderObject::createRoundedBox(
        20.5f,        // thin wall thickness
        lwallHeight,
        lwallSegmentLength,
        roundness,
        10
    );

glm::vec3 start =
    greenPos - lroadDir * 2.0f; // start near opening, outside green

for (int i = 0; i < lwallSegmentCount; i++)
{
    glm::vec3 segmentPos =
        start + lroadDir * (i * lwallSegmentLength);

    // push to LEFT side of road
    segmentPos += leftDir * lwallOffset;

    segmentPos.y =
        getTee().y - (platformHeight * 0.5f)
        + raisedOffset
        + (wallHeight * 0.5f);

    RenderObject* wallSeg =
        new RenderObject(
            wallMesh,
            grassTexture,
            0
        );

    wallSeg->setPosition(segmentPos);

    wallSeg->setRotation(glm::vec3(
        0.0f,
        glm::degrees(lroadAngle) - 90.0f,
        0.0f
    ));

    addObject(wallSeg);
// =========================================================
// SPLIT FOR STRAIGHT ROAD
// =========================================================

float mroadAngle = 5.0f * M_PI / 4.0f; // same as road
glm::vec3 mroadDir(cosf(mroadAngle), 0.0f, sinf(mroadAngle));

// perpendicular (LEFT side of road)
glm::vec3 leftDir(1*lroadDir.z, 0.0f, 2*lroadDir.x);

float lwallSegmentCount = 1;
float lwallSegmentLength = 1.0f;
float lwallHeight = 1.8f;
float lwallOffset = 1.0f; // distance from road center

std::vector<float> wallMesh =
    RenderObject::createRoundedBox(
        20.5f,        // thin wall thickness
        lwallHeight,
        lwallSegmentLength,
        roundness,
        10
    );

glm::vec3 start =
    greenPos - lroadDir * 4.0f; // start near opening, outside green

for (int i = 0; i < lwallSegmentCount; i++)
{
    glm::vec3 segmentPos =
        start + lroadDir * (i * lwallSegmentLength);

    // push to LEFT side of road
    segmentPos += leftDir * lwallOffset;

    segmentPos.y =
        getTee().y - (platformHeight * 0.5f)
        + raisedOffset
        + (wallHeight * 0.5f);

    RenderObject* wallSeg =
        new RenderObject(
            wallMesh,
            grassTexture,
            0
        );

    wallSeg->setPosition(segmentPos);

    wallSeg->setRotation(glm::vec3(
        0.0f,
        glm::degrees(lroadAngle) - 90.0f,
        0.0f
    ));

    addObject(wallSeg);

}

    // =========================================================
    // CUP
    // =========================================================

    std::vector<float> cupData =
        RenderObject::createCylinder(20);

    RenderObject *cup =
        new RenderObject(cupData, stoneTexture, 0);

    cup->setScale(glm::vec3(0.3f, 0.2f, 0.3f));

    glm::vec3 cupPos = greenPos;
    cupPos.y += 0.15f;

    cup->setPosition(cupPos);
    addObject(cup);
}
}