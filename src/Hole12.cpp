#include "Hole12.h"

void Hole12::design(Terrain& terrain) {
    // Call base design if needed
    Hole::design(terrain);

    // Load textures
    GLuint grassTexture = loadBMPTexture("assets/terrain/grass.bmp");
    GLuint stoneTexture = loadBMPTexture("assets/terrain/stone.bmp");
    GLuint concreteTexture = loadBMPTexture("assets/terrain/concrete.bmp");
    GLuint woodTexture = loadBMPTexture("assets/terrain/wood.bmp");
    GLuint sandTexture = loadBMPTexture("assets/terrain/sand.bmp"); // Loaded sand for the pit texture
    if(grassTexture == 0 || stoneTexture == 0 || concreteTexture == 0 || woodTexture == 0 || sandTexture == 0) {
        return;
    }

    // =========================================================================
    // --- REPRODUCE HOLE13 PATH ENTRANCE COORDINATES ---
    // =========================================================================
    float targetX     = getTee().x + 20.0f;
    float targetZ     = getTee().z - 20.0f;
    float moundRadius = 7.0f;
    float blockWidth  = 2.5f;
    float pathRadius  = moundRadius + (blockWidth / 2.0f) + 0.5f; 

    // First segment of Hole13's concrete arc
    float pathEntranceX = targetX + cosf((float)M_PI / 2.0f) * pathRadius; 
    float pathEntranceZ = targetZ + sinf((float)M_PI / 2.0f) * pathRadius; 

    // =========================================================================
    // --- APPROACH PLATFORM SETUP ---
    // =========================================================================
    float wideWidth    = 11.0f;
    float wideLength   = 17.0f;
    float wideHeight   = 0.2f;
    float wideRounding = 0.15f;
    float platformSurfaceY = getTee().y + 0.1f;

    std::vector<float> widePlatformData = RenderObject::createRoundedBox(
        wideWidth, wideHeight, wideLength, wideRounding, 8
    );

    // Place it directly in front of the path entrance
    float h12PlatformX = pathEntranceX;
    float h12PlatformZ = pathEntranceZ + (wideLength / 2.0f) + 0.9f; 

    // Render the main grass platform base
    RenderObject* approachPlatform = new RenderObject(widePlatformData, grassTexture, 0);
    approachPlatform->setPosition(glm::vec3(h12PlatformX, platformSurfaceY, h12PlatformZ));
    approachPlatform->setRotation(glm::vec3(0.0f, 0.0f, 0.0f));
    addObject(approachPlatform);

    // =========================================================================
    // --- EMBEDDED PIT DEPRESSION ---
    // =========================================================================
    float pitRadius = 2.5f;
    float pitRimWidth = 0.3f;

    // 1. Sand pit base — sunk into the platform surface
    std::vector<float> pitData = RenderObject::createCylinder(24);
    RenderObject* visualPit = new RenderObject(pitData, sandTexture, sandTexture);
    visualPit->setScale(glm::vec3(pitRadius, wideHeight, pitRadius));

    // Sink it so the top face is slightly below platform surface
    float pitY = platformSurfaceY - wideHeight * 0.5f;
    visualPit->setPosition(glm::vec3(h12PlatformX, pitY, h12PlatformZ));
    addObject(visualPit);

    // 2. Stone rim ring around the pit edge
    std::vector<float> rimData = RenderObject::createCylinder(24);
    RenderObject* pitRim = new RenderObject(pitData, stoneTexture, 0);
    pitRim->setScale(glm::vec3(pitRadius + pitRimWidth, wideHeight * 0.3f, pitRadius + pitRimWidth));

    // Sits on top of platform surface as a raised curb
    float rimY = platformSurfaceY + (wideHeight * 0.3f) * 0.5f;
    pitRim->setPosition(glm::vec3(h12PlatformX, rimY, h12PlatformZ));
    addObject(pitRim);

    // 3. Grass cap to fill the ring gap (slightly smaller than rim, sits on surface)
    std::vector<float> capData = RenderObject::createCylinder(24);
    RenderObject* rimCap = new RenderObject(capData, grassTexture, 0);
    rimCap->setScale(glm::vec3(pitRadius + pitRimWidth, wideHeight * 0.31f, pitRadius + pitRimWidth));
    rimCap->setPosition(glm::vec3(h12PlatformX, rimY, h12PlatformZ));
    addObject(rimCap);

    // =========================================================================
    // --- MOUND SETUP ---
    // =========================================================================
    float h12MoundRadius = 6.0f;
    float h12MoundHeight = 0.7f;

    float moundCenterX = h12PlatformX + (wideWidth / 2.0f) - 1.2f; // shifted toward right corner
    float moundCenterZ = h12PlatformZ + (wideLength / 2.0f);

    terrain.addMound(
        moundCenterX,
        moundCenterZ,
        h12MoundRadius,
        h12MoundHeight
    );

    // =========================================================================
    // --- CYLINDER SETUP ---
    // =========================================================================
    float h12CylRadius = 2.5f;
    float h12CylHeight = 0.3f;

    std::vector<float> h12CylData = RenderObject::createCylinder(32);
    RenderObject* h12Cylinder = new RenderObject(h12CylData, grassTexture, grassTexture);
    h12Cylinder->setScale(glm::vec3(h12CylRadius, h12CylHeight / 2.0f, h12CylRadius));

    glm::vec3 h12CylPos = glm::vec3(
        h12PlatformX - (wideWidth / 2.0f) + h12CylRadius,
        platformSurfaceY,
        h12PlatformZ + (wideLength / 2.0f) - 1.5f  // pull back by one radius
    );
    h12Cylinder->setPosition(h12CylPos);
    addObject(h12Cylinder);

    // =========================================================================
    // --- WAVE WOODEN BEAMS ON CYLINDER OUTER SECTION ---
    // =========================================================================
    float cylBeamThickness = 0.3f; 
    float cylBeamHeight    = 2.0f; 
    float cylBeamWidth     = 0.3f;
    float cylBeamRounding  = 0.02f;

    std::vector<float> cylBeamData = RenderObject::createRoundedBox(cylBeamWidth, cylBeamHeight, cylBeamThickness, cylBeamRounding, 4);

    int beamsOnCylinder = 12; 
    float placementRadius = h12CylRadius + (cylBeamWidth / 2.0f) + 0.05f; 

    float cylSurfaceY = h12CylPos.y + (h12CylHeight / 2.0f);
    float cylBeamBaseY = cylSurfaceY - (cylBeamHeight / 2.0f) + 0.8f;

    float cylStartAngle = (float)M_PI;
    float cylEndAngle   = (float)M_PI / 2.0f;

    for (int i = 0; i < beamsOnCylinder; ++i) {
        float t = (float)i / (beamsOnCylinder - 1);
        float angle = glm::mix(cylStartAngle, cylEndAngle, t);

        float beamX = h12CylPos.x + cosf(angle) * placementRadius;
        float beamZ = h12CylPos.z + sinf(angle) * placementRadius;

        float waveScale = sinf(t * (float)M_PI); 
        float maxWaveHeightBonus = 1.0f; 
        float dynamicBeamY = cylBeamBaseY + (waveScale * maxWaveHeightBonus);

        RenderObject* beam = new RenderObject(cylBeamData, woodTexture, 0);
        beam->setPosition(glm::vec3(beamX, dynamicBeamY, beamZ));

        float tangentRotationY = -glm::degrees(angle);
        float randomYaw = static_cast<float>(rand() % 12 - 6);
        beam->setRotation(glm::vec3(0.0f, tangentRotationY + randomYaw, 0.0f));

        addObject(beam);
    }

    // =========================================================================
    // --- GOLF HOLE, POLE, AND FLAG ON CYLINDER (NEAR BEAMS) ---
    // =========================================================================
    float pinAngle = glm::mix(cylStartAngle, cylEndAngle, 0.5f); 
    float targetOffsetRadius = h12CylRadius * 0.65f;            

    glm::vec3 cylinderPinPos = glm::vec3(
        h12CylPos.x + cosf(pinAngle) * targetOffsetRadius,
        cylSurfaceY,
        h12CylPos.z + sinf(pinAngle) * targetOffsetRadius
    );

    // 1. Golf Hole (Cup) sunk into the Cylinder Surface
    float holeRadius = 0.15f;
    float holeDepth  = 1.2f;

    std::vector<float> holeData = RenderObject::createCylinder(16);
    RenderObject* golfHole = new RenderObject(holeData, stoneTexture, 0);
    golfHole->setScale(glm::vec3(holeRadius, holeDepth, holeRadius));
    golfHole->setPosition(glm::vec3(cylinderPinPos.x, cylinderPinPos.y - (holeDepth * 0.95f), cylinderPinPos.z));
    addObject(golfHole);

    // 2. Flag Pole
    float poleRadius = 0.02f;
    float poleHeight = 1.2f;

    std::vector<float> poleData = RenderObject::createCylinder(8);
    RenderObject* flagPole = new RenderObject(poleData, stoneTexture, 0);
    flagPole->setScale(glm::vec3(poleRadius, poleHeight, poleRadius));
    flagPole->setPosition(cylinderPinPos);
    addObject(flagPole);

    // 3. Triangular Flag
    float flagBottomWidth = 0.5f; 
    float flagTopWidth    = 0.0f; 
    float flagLength      = 0.4f; 
    float flagThickness   = 0.01f;

    std::vector<float> flagData = RenderObject::createTrapezoid(flagBottomWidth, flagTopWidth, flagLength, flagThickness);
    RenderObject* flag = new RenderObject(flagData, stoneTexture, stoneTexture);
    flag->setRotation(glm::vec3(0.0f, 0.0f, -90.0f));

    glm::vec3 flagPos = glm::vec3(
        cylinderPinPos.x + (flagLength / 2.0f),
        cylinderPinPos.y + poleHeight - (flagBottomWidth / 2.0f), 
        cylinderPinPos.z
    );
    flag->setPosition(flagPos);
    addObject(flag);

    // =========================================================================
    // --- NEW: ROCKS ALONG THE RIGHT-HAND EDGE OF THE APPROACH PLATFORM ---
    // =========================================================================
    int rocksOnRightEdge = 20;
    float platformRightEdgeX = h12PlatformX + (wideWidth / 2.0f) - 0.2f; // Inward slightly from absolute border
    float rightMinZ = h12PlatformZ - (wideLength / 2.0f);               // Front right corner
    float rightMaxZ = h12PlatformZ + (wideLength / 2.0f);               // Back right corner

    for (int i = 0; i < rocksOnRightEdge; ++i) {
        float t = (float)i / (rocksOnRightEdge - 1);
        float rockZ = glm::mix(rightMinZ, rightMaxZ, t);

        // Generate uniquely sized rock profiles
        float rockRadius = 0.5f + (static_cast<float>(rand()) / RAND_MAX) * 0.18f;
        std::vector<float> rockData = RenderObject::createSphere(4, 5);
        for (size_t j = 0; j < rockData.size(); j += 8) {
            rockData[j + 0] *= rockRadius;
            rockData[j + 1] *= rockRadius;
            rockData[j + 2] *= rockRadius;
        }

        RenderObject* rock = new RenderObject(rockData, stoneTexture, 0);
        // Placed cleanly on top of the approach surface height
        rock->setPosition(glm::vec3(platformRightEdgeX, platformSurfaceY, rockZ));
        
        // Add subtle variations to rotation
        float randomYaw = static_cast<float>(rand() % 360);
        rock->setRotation(glm::vec3(0.0f, randomYaw, 0.0f));
        addObject(rock);
    }

    // =========================================================================
    // --- CONCRETE PATH RUNNING ALONG THE OUTSIDE OF THE LEFT EDGE ---
    // =========================================================================
    float extBlockWidth    = 2.5f; 
    float extBlockLength   = 3.0f; 
    float extBlockHeight   = 0.2f;
    float extBlockRounding = 0.05f;

    std::vector<float> extPathData = RenderObject::createRoundedBox(
        extBlockWidth, extBlockHeight, extBlockLength, extBlockRounding, 4
    );

    float pathLeftEdgeX = h12PlatformX - (wideWidth / 2.0f) - (extBlockWidth / 2.0f);
    float startZ = pathEntranceZ + 0.2f;
    float endZ   = h12CylPos.z - h12CylRadius;

    int extensionSegments = 12; 

    for (int i = 0; i < extensionSegments; ++i) {
        float t = (float)i / (extensionSegments - 1);
        float segmentZ = glm::mix(startZ, endZ, t);

        RenderObject* extSegment = new RenderObject(extPathData, concreteTexture, 0);
        extSegment->setPosition(glm::vec3(pathLeftEdgeX, platformSurfaceY + 0.01f, segmentZ));
        extSegment->setRotation(glm::vec3(0.0f, 0.0f, 0.0f)); 

        addObject(extSegment);
    }

    // =========================================================================
    // --- 90-DEGREE PERPENDICULAR CONNECTING JOINT LINK ---
    // =========================================================================
    std::vector<float> horizontalPathData = RenderObject::createRoundedBox(
        2.0f, extBlockHeight, extBlockWidth, extBlockRounding, 4
    );

    float horizStartX = pathLeftEdgeX + (extBlockWidth / 2.0f);
    float horizEndX   = pathEntranceX;
    float horizZ      = pathEntranceZ; 

    int connectorSegments = 10;

    for (int i = 0; i < connectorSegments; ++i) {
        float t = (float)i / (connectorSegments - 1);
        float segmentX = glm::mix(horizStartX, horizEndX, t);

        RenderObject* horizSegment = new RenderObject(horizontalPathData, concreteTexture, 0);
        horizSegment->setPosition(glm::vec3(segmentX, platformSurfaceY + 0.012f, horizZ));
        horizSegment->setRotation(glm::vec3(0.0f, 0.0f, 0.0f));

        addObject(horizSegment);
    }
}