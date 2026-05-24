#include "Hole01.h"
#include "Terrain.h"
#include "RenderObject.h"
#include "TextureLoader.h"

void Hole01::design(Terrain& terrain) {
    // Call base design if needed
    Hole::design(terrain);
    
    // Load textures
    GLuint grassTexture = loadBMPTexture("assets/terrain/grass.bmp");
    GLuint stoneTexture = loadBMPTexture("assets/terrain/stone.bmp");
    // Make sure you have a concrete texture file path in your assets
    GLuint concreteTexture = loadBMPTexture("assets/terrain/concrete.bmp"); 
    GLuint woodTexture = loadBMPTexture("assets/terrain/wood.bmp");
    
    if (grassTexture == 0 || stoneTexture == 0 || concreteTexture == 0) {
        return;
    }

    // Main platform dimensions
    float platformWidth = 8.0f;
    float platformDepth = 8.0f;
    float platformHeight = 1.5f;
    float raisedHeightOffset = 0.1f; 

    // Create the main cuboid platform
    std::vector<float> grassBoxData = RenderObject::createRoundedBox(8.0f, 1.5f, 8.0f, 0.4f, 16);
    RenderObject* grassBox = new RenderObject(grassBoxData, grassTexture, 0);
    
    glm::vec3 basePos = glm::vec3(getTee().x, getTee().y - (platformHeight / 2.0f) + raisedHeightOffset, getTee().z);
    grassBox->setPosition(basePos);
    addObject(grassBox);

    float surfaceY = getTee().y + raisedHeightOffset;

    // --- CONFIGURING THE STRAIGHT LINE ROW OF ROCKS ---
    // --- CONFIGURING THE STRAIGHT LINE ROW OF ROCKS ---
    int numRocks = 9; // INCREASED: More rocks along the edge
    float halfWidth = platformWidth / 2.0f;
    float halfDepth = platformDepth / 2.0f;
    float edgeOffset = 0.4f; 

    for (int i = 0; i < numRocks; ++i) {
        float pct = -1.0f + 2.0f * (static_cast<float>(i) / (numRocks - 1));
        
        // OPTION A: Clean row along the RIGHT side (+X side)
        float offsetX = halfWidth - edgeOffset;
        float offsetZ = pct * (halfDepth - edgeOffset);

        // Randomize rock radius size
        float rockRadius = 0.35f + (static_cast<float>(rand()) / RAND_MAX) * 0.3f;

        // --- JAGGED ROCKS FIX ---
        // Reduced parallels to 4 and meridians to 5. 
        // This strips away vertex precision to give it a sharp, low-poly faceted rock appearance.
        std::vector<float> rockSphereData = RenderObject::createSphere(4, 5); 

        for (size_t j = 0; j < rockSphereData.size(); j += 8) {
            rockSphereData[j + 0] *= rockRadius;
            rockSphereData[j + 1] *= rockRadius;
            rockSphereData[j + 2] *= rockRadius;
        }

        RenderObject* rock = new RenderObject(rockSphereData, stoneTexture, 0);

        glm::vec3 rockPos = glm::vec3(
            getTee().x + offsetX,
            surfaceY - (rockRadius * 0.15f), 
            getTee().z + offsetZ
        );

        rock->setPosition(rockPos);
        addObject(rock);
    }

    // --- CONCRETE PATHWAY (PART 1: MAIN STRAIGHT ROAD) ---
    float pathWidth = 2.3f;
    float pathHeight = 0.15f;
    float pathDepth = 8.0f; // Main length running along Z
    float pathRounding = 0.03f;

    std::vector<float> pathData = RenderObject::createRoundedBox(pathWidth, pathHeight, pathDepth, pathRounding, 8);
    RenderObject* concretePath = new RenderObject(pathData, concreteTexture, 0);

    // Push it outward to the left edge of the platform
    float pathOutwardOffset = 0.8f; 
    glm::vec3 pathPos = glm::vec3(
        getTee().x - halfWidth - pathOutwardOffset, 
        surfaceY + (pathHeight / 2.0f) - 0.15f, // slightly sunk to look flush
        getTee().z
    );
    concretePath->setPosition(pathPos);
    addObject(concretePath);

    // --- CONCRETE PATHWAY (PART 2: CURVED LEFT TURN) ---
    int turnSegments = 8;            // more segments = smoother curve
    float totalCurveAngle = 45.0f;    // degrees of bend

    float turnSegmentLength = 1.0f;       // advance distance (keep this)
    float turnSegmentRender = 1.2f;       // render size slightly larger (add this)

    // Use render length for the mesh
    std::vector<float> turnSegmentData = RenderObject::createRoundedBox(
        pathWidth, pathHeight, turnSegmentRender, 0.0f, 8
    );

   
    // Start the curve at the front end of the straight path
    glm::vec3 segmentCenter = glm::vec3(
        pathPos.x,
        pathPos.y,
        pathPos.z - (pathDepth / 2.0f) - (turnSegmentLength / 2.0f)
    );

    float startAngle = 180.0f; // path is initially directed toward -Z
    float angleStep = totalCurveAngle / (turnSegments - 1);

    for (int i = 0; i < turnSegments; ++i) {
        // --- FIXED: Changed from '-' to '+' to rotate counter-clockwise (Left Turn) ---
        float angle = startAngle + i * angleStep; 
        
        RenderObject* seg = new RenderObject(turnSegmentData, concreteTexture, 0);
        seg->setRotation(glm::vec3(0.0f, angle, 0.0f));
        seg->setPosition(segmentCenter);
        addObject(seg);

        // Calculate the forward direction for the current angle rotation
        glm::vec3 forward = glm::vec3(
            sinf(glm::radians(angle)),
            0.0f,
            cosf(glm::radians(angle))
        );
        
        // Move the anchor point out for the next segment to snap to
        segmentCenter += forward * turnSegmentLength;
    }


   // --- GOLF HOLE (CUP) ---
    // --- POSITION TUNING SLIDER ---
    // Change the Z offset to a negative value to move it closer to the mound (-Z).
    // Try -1.5f or adjust it to find the perfect distance!
    float pinMoundProximityZ = -1.5f; 

    glm::vec3 pinPos = glm::vec3(
        getTee().x + 1.5f,   // Keep the X position slightly right of center
        getTee().y,
        getTee().z + pinMoundProximityZ // Move it towards the mound side (-Z)
    );
    float holeRadius = 0.15f;
    float holeDepth = 0.2f;

    std::vector<float> holeData = RenderObject::createCylinder(16);
    RenderObject* golfHole = new RenderObject(holeData, stoneTexture, 0);
    golfHole->setScale(glm::vec3(holeRadius, holeDepth, holeRadius));

    golfHole->setPosition(glm::vec3(pinPos.x, surfaceY - (holeDepth * 0.95f), pinPos.z));
    addObject(golfHole);

    // --- FLAG POLE ---
    float poleRadius = 0.02f;
    float poleHeight = 1.2f;

    std::vector<float> poleData = RenderObject::createCylinder(8);
    RenderObject* flagPole = new RenderObject(poleData, concreteTexture, 0);
    flagPole->setScale(glm::vec3(poleRadius, poleHeight, poleRadius));

    glm::vec3 polePos = glm::vec3(
        pinPos.x,
        surfaceY, // automatically follows the new pinPos
        pinPos.z
    );
    flagPole->setPosition(polePos);
    addObject(flagPole);

    // --- NEW TRIANGULAR FLAG USING TRAPEZOID ---
    float flagBottomWidth = 0.5f; // The attachment edge height against the pole
    float flagTopWidth    = 0.0f; // Squeezing the top to zero width turns it into a triangle!
    float flagLength      = 0.4f; // How far out the flag waves horizontally
    float flagThickness   = 0.01f;

    // Call your new engine utility function
    std::vector<float> flagData = RenderObject::createTrapezoid(flagBottomWidth, flagTopWidth, flagLength, flagThickness);
    RenderObject* flag = new RenderObject(flagData, concreteTexture, concreteTexture);

    // Because createTrapezoid generates the shape vertically oriented along its height axis, 
    // roll it 90 degrees on the Z-axis to let it extend sideways out from the flagpole
    flag->setRotation(glm::vec3(0.0f, 0.0f, -90.0f));

    glm::vec3 flagPos = glm::vec3(
        pinPos.x + flagLength / 2.0f,
        surfaceY + poleHeight - (flagBottomWidth / 2.0f) - raisedHeightOffset, 
        pinPos.z
    );
    flag->setPosition(flagPos);
    addObject(flag);

    // I need to add a wooden border
    // Let's make it an elongated beam. 
    // If it runs along the Back edge (+Z), it needs to stretch along the X axis (width).
    float borderWidth  = 7.0f;  // Long enough to span the side
    float borderHeight = 0.2f; // Tall enough to act as a curb/lip
    float borderDepth  = 0.3f;  // Thick enough to be visible
    float borderRound  = 0.04f; 

    std::vector<float> borderData = RenderObject::createRoundedBox(borderWidth, borderHeight, borderDepth, borderRound, 8);
    RenderObject* border = new RenderObject(borderData, woodTexture, 0);

    // Toggle how far out or close to the edge the border sits
    // Increse to move further inwards
    //Decrease to move further outwards (away from the center grass)
    float borderOutwardOffset = 0.2f; 

    // Sinking it slightly into the platform so it sits cleanly on the grass surface
    float borderYPos = surfaceY + (borderHeight / 2.0f) - 0.2f; 


    glm::vec3 borderPos = glm::vec3(
        getTee().x, 
        borderYPos, 
        getTee().z + halfDepth - borderOutwardOffset
    );

    border->setPosition(borderPos);
    addObject(border);

    // --- MOUND DIVIDER (front -Z side, away from rocks/wood/concrete) ---
    float moundRadius = 3.0f;
    float moundHeight = 1.0f;

    terrain.addMound(
        getTee().x,
        getTee().z - (platformDepth / 2.0f) - 3.0f,  // always flush to the edge
        moundRadius,
        moundHeight
    );

// --- ROCKS ON FAR SIDE OF MOUND ---
int numMoundRocks = 7;
float moundCenterX = getTee().x;
float moundCenterZ = getTee().z - (platformDepth / 2.0f) - 3.0f;

for (int i = 0; i < numMoundRocks; ++i) {
    float t = (float)i / (numMoundRocks - 1); // 0..1
    
    // Arc across the far half of the mound (180 degrees away from platform)
    float arcAngle = (float)M_PI + t * (float)M_PI; // pi to 2pi = far semicircle
    
    float rockRadius = 0.35f + (static_cast<float>(rand()) / RAND_MAX) * 0.3f;
    
    // Place rocks at the mound's perimeter on the far side
    float rockX = moundCenterX + cosf(arcAngle) * (moundRadius - 0.5f);
    float rockZ = moundCenterZ + sinf(arcAngle) * (moundRadius - 0.5f);
    
    // Sit them on top of the mound surface
    float rockY = surfaceY - (rockRadius * 0.15f);

    std::vector<float> rockData = RenderObject::createSphere(4, 5);
    for (size_t j = 0; j < rockData.size(); j += 8) {
        rockData[j + 0] *= rockRadius;
        rockData[j + 1] *= rockRadius;
        rockData[j + 2] *= rockRadius;
    }

    RenderObject* rock = new RenderObject(rockData, stoneTexture, 0);
    rock->setPosition(glm::vec3(rockX, rockY, rockZ));
    addObject(rock);
}

    

}