#include "Hole11.h"
#include "Terrain.h"
#include "RenderObject.h"
#include "TextureLoader.h"
#include <cmath>

void Hole11::design(Terrain &terrain)
{
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
    float platformWidth = 4.0f;
    float platformDepth = 16.0f;
    float platformHeight = 1.5f;
    float raisedHeightOffset = 0.1f; 

    // Create the main cuboid platform
    std::vector<float> grassBoxData = RenderObject::createRoundedBox(16.0f, 1.5f, 6.0f, 1.0f, 16);
    RenderObject* grassBox = new RenderObject(grassBoxData, grassTexture, 0);
    
    glm::vec3 basePos = glm::vec3(getTee().x, getTee().y - (platformHeight / 2.0f) - 0.2f + raisedHeightOffset, getTee().z);
    grassBox->setPosition(basePos);
    addObject(grassBox);

    std::vector<float> grassBoxDataB = RenderObject::createRoundedBox(10.0f, 1.3f, 10.0f, 1.0f, 4);
    RenderObject* grassBoxB = new RenderObject(grassBoxDataB, grassTexture, 0);
    
    glm::vec3 basePosB = glm::vec3(getTee().x + 8.0, getTee().y - (platformHeight / 2.0f) + raisedHeightOffset, getTee().z - 2.0f);
    grassBoxB->setPosition(basePosB);
    addObject(grassBoxB);

    float surfaceY = getTee().y + raisedHeightOffset;

    float halfWidth = platformWidth / 2.0f;
    float halfDepth = platformDepth / 2.0f;
    float edgeOffset = 0.4f; 

    // --- CONCRETE PATHWAY (PART 1: MAIN STRAIGHT ROAD) ---
    float pathWidth = 8.0f;
    float pathHeight = 0.15f;
    float pathDepth = 2.3f; // Main length running along Z
    float pathRounding = 0.03f;

    std::vector<float> pathData = RenderObject::createRoundedBox(pathWidth, pathHeight, pathDepth, pathRounding, 8);
    RenderObject* concretePath = new RenderObject(pathData, concreteTexture, 0);

    // Push it outward to the left edge of the platform
    float pathOutwardOffset = 0.8f; 
    glm::vec3 pathPos = glm::vec3(
        getTee().x, 
        surfaceY + (pathHeight / 2.0f) - 0.15f, // slightly sunk to look flush
        getTee().z + halfWidth + pathOutwardOffset
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
        //addObject(seg);

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
    float pinMoundProximityZ = -7.5f; 

    glm::vec3 pinPos = glm::vec3(
        getTee().x - pinMoundProximityZ,   // Keep the X position slightly right of center
        getTee().y + 0.5f,
        getTee().z + 0.5f // Move it towards the mound side (-Z)
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
        pinPos.y, // automatically follows the new pinPos
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
        pinPos.y + poleHeight - (flagBottomWidth / 2.0f) - raisedHeightOffset, 
        pinPos.z
    );
    flag->setPosition(flagPos);
    addObject(flag);

    // --- MOUND DIVIDER (front -Z side, away from rocks/wood/concrete) ---
    float moundRadius = 2.0f;
    float moundHeight = 0.5f;

    terrain.addMound(
        getTee().x - (platformWidth / 2.0f) - 2.0f,
        getTee().z - (platformDepth / 2.0f) - 2.0f,  // always flush to the edge
        moundRadius,
        moundHeight
    );
}
