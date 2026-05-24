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

    // Negative height = depression/pit in the terrain mesh
    terrain.addMound(targetX, targetZ, moundRadius, 0.5f);
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
    
    // Rock placement configuration
    int rocksOnPlatform1 = 10; // Increased slightly for better gap coverage
    int rocksOnPlatform2 = 10; 
    
    // Nudge amount: Half the walkway width pulls the rocks perfectly to the edge skin
    float halfWidth = pWidth / 2.0f;

    // --- 1. ROCKS ALONG INNER EDGE OF PLATFORM 1 (Running along Z-axis) ---
    // Inner edge is the +X side of the platform
    float p1InnerX = posZ.x + halfWidth; 
    
    // Start near the mound edge and end exactly where Platform 2's inner edge clips it
    float p1MinZ = posZ.z + (pLength / 2.0f) - moundOverlap;
    float p1MaxZ = posX.z + halfWidth; // Aligns perfectly to the inner face of Platform 2

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
    // Inner edge is the +Z side of the platform due to the 90-degree rotation axis swap
    float p2InnerZ = posX.z + halfWidth; 
    
    // Start right at the intersection edge of Platform 1 and run out to the end of Platform 2
    float p2MinX = posZ.x + halfWidth;
    float p2MaxX = posX.x + (p2Length / 2.0f);

    for (int i = 0; i < rocksOnPlatform2; ++i) {
        float t = (float)i / (rocksOnPlatform2 - 1);
        float rockX = glm::mix(p2MinX, p2MaxX, t);

        // Skip the very first rock to prevent a messy double-stack right in the 90-degree joint vertex
        if (i == 0) continue; 

        float rockRadius = 0.25f + (static_cast<float>(rand()) / RAND_MAX) * 0.2f;
        std::vector<float> rockData = RenderObject::createSphere(4, 5);
        for (size_t j = 0; j < rockData.size(); j += 8) {
            rockData[j + 0] *= rockRadius;
            rockData[j + 1] *= rockRadius;
            rockData[j + 2] *= rockRadius;
        }

        RenderObject* rock = new RenderObject(rockData, stoneTexture, 0);
        // Sits exactly on the back lip edge of Platform 2
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

        // =========================================================================
        // --- WAVE HEIGHT MATHEMATICS ---
        // =========================================================================
        // Generates the matching smooth mountain arch curve (0.0 -> 1.0 -> 0.0)
        float waveScale = sinf(t * (float)M_PI); 

        // Matches the peak amplitude used on Platform 1 for visual consistency
        float maxWaveHeightBonus = 1.2f; 
        
        // Calculate the dynamic Y position for this post
        float dynamicBeamElevationY = beamElevationY + (waveScale * maxWaveHeightBonus);
        // =========================================================================

        RenderObject* beam = new RenderObject(beamData, woodTexture, 0);
        
        // --- CHANGED: Passing dynamicBeamElevationY instead of fixed beamElevationY ---
        beam->setPosition(glm::vec3(beamX, dynamicBeamElevationY, p2OuterZ));
        
        float randomYaw = static_cast<float>(rand() % 15 - 7);
        beam->setRotation(glm::vec3(0.0f, 90.0f + randomYaw, 0.0f)); // Kept square with orientation
        
        addObject(beam);
    }

    // =========================================================================
    // --- GOLF HOLE, POLE, AND FLAG ON PLATFORM 2 ---
    // =========================================================================
    
    // Calculate the target center near the far end of Platform 2
    // We take its center and slide it out by 75% of its half-length to avoid the very edge
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
    
    // Sink the cup slightly into the surface of Platform 2
    golfHole->setPosition(glm::vec3(platform2GreenCenter.x, platform2GreenCenter.y - (holeDepth * 0.95f), platform2GreenCenter.z));
    addObject(golfHole);

    // 2. Flag Pole
    float poleRadius = 0.02f;
    float poleHeight = 1.2f;

    std::vector<float> poleData = RenderObject::createCylinder(8);
    RenderObject* flagPole = new RenderObject(poleData, stoneTexture, 0);
    flagPole->setScale(glm::vec3(poleRadius, poleHeight, poleRadius));
    
    // Position it directly at the center of the cup
    glm::vec3 polePos = glm::vec3(platform2GreenCenter.x, platform2GreenCenter.y, platform2GreenCenter.z);
    flagPole->setPosition(polePos);
    addObject(flagPole);

    // 3. Triangular Flag
    float flagBottomWidth = 0.5f; 
    float flagTopWidth    = 0.0f; // Squeezing to zero turns the trapezoid into a triangle
    float flagLength      = 0.4f; 
    float flagThickness   = 0.01f;

    std::vector<float> flagData = RenderObject::createTrapezoid(flagBottomWidth, flagTopWidth, flagLength, flagThickness);
    
    // Switched to red stone/concrete texture if available, otherwise stoneTexture works fine
    RenderObject* flag = new RenderObject(flagData, stoneTexture, stoneTexture);

    // Rotate it 90 degrees on Z so it waves horizontally, and match platform orientation if necessary
    flag->setRotation(glm::vec3(0.0f, 0.0f, -90.0f));

    // Position it near the top of the flagpole
    // Note: Removed the extra trailing -raisedHeightOffset to keep it relative to platformSurfaceY
    glm::vec3 flagPos = glm::vec3(
        platform2GreenCenter.x + (flagLength / 2.0f),
        platform2GreenCenter.y + poleHeight - (flagBottomWidth / 2.0f), 
        platform2GreenCenter.z
    );
    flag->setPosition(flagPos);
    addObject(flag);
}
