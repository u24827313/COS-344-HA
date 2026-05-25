#include "Hole13.h"

void Hole13::design(Terrain& terrain) {
    // Call base design if needed
    Hole::design(terrain);

    // Load textures
    GLuint grassTexture = loadBMPTexture("assets/terrain/grass.bmp");
    GLuint stoneTexture = loadBMPTexture("assets/terrain/stone.bmp");
    GLuint concreteTexture = loadBMPTexture("assets/terrain/concrete.bmp");
    GLuint woodTexture = loadBMPTexture("assets/terrain/wood.bmp");
    GLuint sandTexture = loadBMPTexture("assets/terrain/sand.bmp");
    if(grassTexture == 0 || stoneTexture == 0 || concreteTexture == 0 || woodTexture == 0 || sandTexture == 0) {
        return;
    }

    // =========================================================================
    // --- EXACT MOUND & BUNKER POSITION ---
    // =========================================================================
    float targetX = getTee().x + 20.0f;
    float targetZ = getTee().z - 20.0f;
    float moundRadius = 7.0f;
    float moundHeight = 0.5f;

    // Negative height = depression/pit in the terrain mesh
    terrain.addMound(targetX, targetZ, moundRadius, moundHeight);
    terrain.addBunker(targetX, targetZ, moundRadius, 1.5f);

    // =========================================================================
    // --- MATCHING RIGHT-ANGLE PLATFORMS ---
    // =========================================================================
    // Platform Dimensional Specs
    float pWidth  = 3.5f;   // How wide the walkways are
    float pLength = 10.0f;  // Length of Platform 1 ONLY
    float pHeight = 0.4f;   // Thickness of the platform blocks
    float pRounding = 0.1f; // Corner rounding
    float platformSurfaceY = getTee().y + 0.1f; 
    float moundOverlap = 2.0f;

    // Independent length variable for Platform 2
    float p2Length = 9.5f;  

    // Generate separate mesh layouts for each platform size
    std::vector<float> pData  = RenderObject::createRoundedBox(pWidth, pHeight, pLength, pRounding, 8);
    std::vector<float> p2Data = RenderObject::createRoundedBox(pWidth, pHeight, p2Length, pRounding, 8);

    // --- PLATFORM 1: THE CURRENT FRONT PLATFORM (-Z AXIS) ---
    RenderObject* platformZ = new RenderObject(pData, grassTexture, 0);
    
    // Anchored cleanly using targetX and targetZ
    glm::vec3 posZ = glm::vec3(
        targetX, 
        platformSurfaceY, 
        targetZ - moundRadius - (pLength / 2.0f) + moundOverlap
    );
    
    platformZ->setPosition(posZ);
    platformZ->setRotation(glm::vec3(0.0f, 0.0f, 0.0f)); 
    addObject(platformZ);


    // --- PLATFORM 2: THE NEW PERPENDICULAR RIGHT PLATFORM (+X AXIS) ---
    RenderObject* platformX = new RenderObject(p2Data, grassTexture, 0);
    
    // Calculate the center point of the front edge of Platform 1
    float platform1FrontZ = posZ.z - (pLength / 2.0f);

    // Anchored cleanly using targetX and targetZ
    glm::vec3 posX = glm::vec3(
        targetX + (p2Length / 2.0f) - (pWidth / 2.0f), 
        platformSurfaceY,
        platform1FrontZ + (pWidth / 2.0f)
    );
    
    platformX->setPosition(posX);
    platformX->setRotation(glm::vec3(0.0f, 90.0f, 0.0f)); 
    addObject(platformX);

    // =========================================================================
    // --- ROCKS ALONG THE INNER EDGES (90-DEGREE BEND) ---
    // =========================================================================
    int rocksOnPlatform1 = 10; 
    int rocksOnPlatform2 = 10; 
    float halfWidth = pWidth / 2.0f;

    // --- 1. ROCKS ALONG INNER EDGE OF PLATFORM 1 (Running along Z-axis) ---
    float p1InnerX = posZ.x + halfWidth; 
    float p1MinZ = posZ.z + (pLength / 2.0f) - moundOverlap;
    float p1MaxZ = posX.z + halfWidth; 

    for (int i = 0; i < rocksOnPlatform1; ++i) {
        float t = (float)i / (rocksOnPlatform1 - 1);
        float rockZ = glm::mix(p1MinZ, p1MaxZ, t);

        float rockRadius = 0.25f + (static_cast<float>(rand()) / RAND_MAX) * 0.2f;
        std::vector<float> rockData = RenderObject::createSphere(4, 5);
        for (size_t j = 0; j < rockData.size(); j += 8) {
            rockData[j + 0] *= rockRadius;
            rockData[j + 1] *= rockRadius;
            rockData[j + 2] *= rockRadius;
        }

        RenderObject* rock = new RenderObject(rockData, stoneTexture, 0);
        // Sits exactly on the right lip edge of Platform 1
        rock->setPosition(glm::vec3(p1InnerX, platformSurfaceY, rockZ));
        addObject(rock);
    }

    // --- 2. ROCKS ALONG INNER EDGE OF PLATFORM 2 (Running along X-axis) ---
    float p2InnerZ = posX.z + halfWidth; 
    float p2MinX = posZ.x + halfWidth;
    float p2MaxX = posX.x + (p2Length / 2.0f);

    for (int i = 0; i < rocksOnPlatform2; ++i) {
        float t = (float)i / (rocksOnPlatform2 - 1);
        float rockX = glm::mix(p2MinX, p2MaxX, t);

        if (i == 0) continue; 

        float rockRadius = 0.25f + (static_cast<float>(rand()) / RAND_MAX) * 0.2f;
        std::vector<float> rockData = RenderObject::createSphere(4, 5);
        for (size_t j = 0; j < rockData.size(); j += 8) {
            rockData[j + 0] *= rockRadius;
            rockData[j + 1] *= rockRadius;
            rockData[j + 2] *= rockRadius;
        }

        RenderObject* rock = new RenderObject(rockData, stoneTexture, 0);
        rock->setPosition(glm::vec3(rockX, platformSurfaceY, p2InnerZ));
        addObject(rock);
    }

    // =========================================================================
    // --- VERTICAL WOOD BEAMS ALONG THE OUTER EDGES ---
    // =========================================================================
    
    // Beam dimensional profile
    float beamThickness = 0.3f; 
    float beamHeight = 2.0f; // Total height of the post
    float beamWidth = 0.3f;
    float beamRounding = 0.02f;

    // Generate a single vertical post mesh layout
    std::vector<float> beamData = RenderObject::createRoundedBox(beamWidth, beamHeight, beamThickness, beamRounding, 4);


    // Calculate how deep the beams sink so they "poke out" of the ground gracefully
    // This leaves about 0.8 units of the beam sticking up above the platform surface
    float beamElevationY = platformSurfaceY - (beamHeight / 2.0f) + 0.8f; 

    // --- 1. WOOD BEAMS ALONG OUTER EDGE OF PLATFORM 1 (Running along Z-axis) ---
    // Outer edge is the -X side of the platform
    float p1OuterX = posZ.x - halfWidth - (beamWidth / 2.0f);
    
    float p1BeamMinZ = posZ.z + (pLength / 2.0f) - moundOverlap;
    float p1BeamMaxZ = posX.z - halfWidth; // Extends all the way to the absolute corner outer vertex

    // --- UPDATED: Increased beam count to get a smoother height curve profile ---
    int beamsOnPlatform1 = 15; 

    for (int i = 0; i < beamsOnPlatform1; ++i) {
        float t = (float)i / (beamsOnPlatform1 - 1);
        float beamZ = glm::mix(p1BeamMinZ, p1BeamMaxZ, t);

        // =========================================================================
        // --- WAVE HEIGHT MATHEMATICS ---
        // =========================================================================
        // t goes from 0.0 to 1.0. Multiplying by M_PI maps it from 0 to 180 degrees.
        // sinf() yields a perfect smooth mountain arch curve (0.0 -> 1.0 -> 0.0).
        float waveScale = sinf(t * (float)M_PI); 

        // Tweak parameters to adjust the peak amplitude of your wave effect
        float maxWaveHeightBonus = 1.2f; // The middle beam will rise this many units higher
        
        // Calculate the custom dynamic Y position for this specific post
        float dynamicBeamElevationY = beamElevationY + (waveScale * maxWaveHeightBonus);
        // =========================================================================

        RenderObject* beam = new RenderObject(beamData, woodTexture, 0);
        
        // --- CHANGED: Passing dynamicBeamElevationY instead of fixed beamElevationY ---
        beam->setPosition(glm::vec3(p1OuterX, dynamicBeamElevationY, beamZ));
        
        // Give them a tiny bit of random rotation on the Y-axis so they look like hand-placed wooden stakes
        float randomYaw = static_cast<float>(rand() % 15 - 7); 
        beam->setRotation(glm::vec3(0.0f, randomYaw, 0.0f));
        
        addObject(beam);
    }

    // --- 2. WOOD BEAMS ALONG OUTER EDGE OF PLATFORM 2 (Running along X-axis) ---
    // Outer edge is the -Z side of the platform due to the 90-degree turn
    float p2OuterZ = posX.z - halfWidth - (beamThickness / 2.0f);
    
    float p2BeamMinX = posZ.x - halfWidth;
    float p2BeamMaxX = posX.x + (p2Length / 2.0f);

    // --- UPDATED: Increased beam count to match the smooth curve resolution of Platform 1 ---
    int beamsOnPlatform2 = 20; 

    for (int i = 0; i < beamsOnPlatform2; ++i) {
        float t = (float)i / (beamsOnPlatform2 - 1);
        float beamX = glm::mix(p2BeamMinX, p2BeamMaxX, t);

        float waveScale = sinf(t * (float)M_PI); 
        float maxWaveHeightBonus = 1.2f; 
        float dynamicBeamElevationY = beamElevationY + (waveScale * maxWaveHeightBonus);

        RenderObject* beam = new RenderObject(beamData, woodTexture, 0);
        beam->setPosition(glm::vec3(beamX, dynamicBeamElevationY, p2OuterZ));
        
        float randomYaw = static_cast<float>(rand() % 15 - 7);
        beam->setRotation(glm::vec3(0.0f, 90.0f + randomYaw, 0.0f)); 
        addObject(beam);
    }

    // =========================================================================
    // --- GOLF HOLE, POLE, AND FLAG ON PLATFORM 2 ---
    // =========================================================================
    float flagOffsetX = (p2Length / 2.0f) * 0.75f; 
    glm::vec3 platform2GreenCenter = glm::vec3(
        posX.x + flagOffsetX,
        platformSurfaceY,
        posX.z
    );

    // 1. Golf Hole (Cup)
    float holeRadius = 0.15f;
    float holeDepth = 4.0f;

    std::vector<float> holeData = RenderObject::createCylinder(16);
    RenderObject* golfHole = new RenderObject(holeData, stoneTexture, 0);
    golfHole->setScale(glm::vec3(holeRadius, holeDepth, holeRadius));
    golfHole->setPosition(glm::vec3(platform2GreenCenter.x, platform2GreenCenter.y - (holeDepth * 0.95f), platform2GreenCenter.z));
    addObject(golfHole);

    // 2. Flag Pole
    float poleRadius = 0.02f;
    float poleHeight = 1.2f;

    std::vector<float> poleData = RenderObject::createCylinder(8);
    RenderObject* flagPole = new RenderObject(poleData, stoneTexture, 0);
    flagPole->setScale(glm::vec3(poleRadius, poleHeight, poleRadius));
    flagPole->setPosition(glm::vec3(platform2GreenCenter.x, platform2GreenCenter.y, platform2GreenCenter.z));
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
        platform2GreenCenter.x + (flagLength / 2.0f),
        platform2GreenCenter.y + poleHeight - (flagBottomWidth / 2.0f), 
        platform2GreenCenter.z
    );
    flag->setPosition(flagPos);
    addObject(flag);

    // =========================================================================
    // --- MIRRORED CURVED CONCRETE PATH (REVERSED CURVE DIRECTION) ---
    // =========================================================================
    float blockWidth  = 2.5f; 
    float blockLength = 3.0f; 
    float blockHeight = 0.2f; 
    float blockRounding = 0.05f;

    std::vector<float> pathBlockData = RenderObject::createRoundedBox(blockWidth, blockHeight, blockLength, blockRounding, 4);

    int pathSegments = 12; 
    float pathRadius = moundRadius + (blockWidth / 2.0f) + 0.5f; 

    float startAngle = (float)M_PI / 2.0f; 
    float endAngle   = 0.0f;         

    for (int i = 0; i < pathSegments; ++i) {
        float t = (float)i / (pathSegments - 1);
        float angle = glm::mix(startAngle, endAngle, t);

        float segmentX = targetX + cosf(angle) * pathRadius;
        float segmentZ = targetZ + sinf(angle) * pathRadius;

        RenderObject* pathSegment = new RenderObject(pathBlockData, concreteTexture, 0);
        pathSegment->setPosition(glm::vec3(segmentX, platformSurfaceY - 0.05f, segmentZ));

        float rotationY = -glm::degrees(angle); 
        pathSegment->setRotation(glm::vec3(0.0f, rotationY, 0.0f));

        addObject(pathSegment);
    }

    // =========================================================================
    // --- UPDATED: 4 EMBEDDED CONCRETE PATH SEGMENTS CROSSING THE MOUND ---
    // =========================================================================
    float moundBlockWidth    = 2.0f;
    float moundBlockLength   = 2.5f;
    float moundBlockHeight   = 0.1f;
    float moundBlockRounding = 0.05f;

    std::vector<float> moundPathData = RenderObject::createRoundedBox(
        moundBlockWidth, moundBlockHeight, moundBlockLength, moundBlockRounding, 4
    );

    // Reduced to 4 segment links
    int moundSegments = 5;
    
    // Sweeping across the span of the mound
    float moundStartZ = targetZ - moundRadius + 1.2f;
    float moundEndZ   = targetZ + moundRadius - 1.2f;

    for (int i = 0; i < moundSegments; ++i) {
        float t = (float)i / (moundSegments - 1);
        float segmentZ = glm::mix(moundStartZ, moundEndZ, t);
        float segmentX = targetX; 

        // Get accurate parabolic height profile matching the mound curve
        float dx = segmentX - targetX;
        float dz = segmentZ - targetZ;
        float dist = sqrtf(dx * dx + dz * dz);

        float heightOffset = 0.0f;
        if (dist < moundRadius) {
            float factor = dist / moundRadius;
            heightOffset = moundHeight * (1.0f - factor * factor);
        }

        RenderObject* moundSegment = new RenderObject(moundPathData, concreteTexture, 0);
        
        // --- CHANGED: Subtracted 0.08f to lower and blend the stones deep into the mound geometry ---
        float embeddedY = platformSurfaceY + heightOffset - 0.2f;

        moundSegment->setPosition(glm::vec3(segmentX, embeddedY, segmentZ));
        moundSegment->setRotation(glm::vec3(0.0f, 0.0f, 0.0f));

        addObject(moundSegment);
    }
}
