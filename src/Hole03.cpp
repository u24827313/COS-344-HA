#include "Hole03.h"
#include "Terrain.h"
#include "RenderObject.h"
#include "TextureLoader.h"
#include <cmath>

void Hole03::design(Terrain& terrain) {
    // Call base design if needed
    Hole::design(terrain);

    // Load textures
    GLuint grassTexture = loadBMPTexture("assets/terrain/grass.bmp");
    GLuint stoneTexture = loadBMPTexture("assets/terrain/stone.bmp");
    GLuint concreteTexture = loadBMPTexture("assets/terrain/concrete.bmp");
    GLuint woodTexture = loadBMPTexture("assets/terrain/wood.bmp");
    if(grassTexture == 0 || stoneTexture == 0 || concreteTexture == 0 || woodTexture == 0) {
        return;
    }

    float greenRadius = 2.0f;
    float greenHeight = 0.3f;
    float raisedHeightOffset = 0.1f;
    float moundRadius = 2.0f; 

    // --- CYLINDER GREEN BASE CONFIGURATION ---
    std::vector<float> cylinderData = RenderObject::createCylinder(32);
    
    // Scale vertices directly so normal orientation vectors stay perfectly matched
    for (size_t j = 0; j < cylinderData.size(); j += 8) {
        cylinderData[j + 0] *= greenRadius; 
        cylinderData[j + 1] *= greenHeight;  
        cylinderData[j + 2] *= greenRadius; 

        // Re-normalize lighting vectors to fix the black rendering glitch natively
        float nx = cylinderData[j + 3];
        float nz = cylinderData[j + 5];
        float len = sqrtf(nx * nx + nz * nz);
        if (len > 0.0f) {
            cylinderData[j + 3] = nx / len;
            cylinderData[j + 5] = nz / len;
        }
    }

    RenderObject* greenCylinder = new RenderObject(cylinderData, grassTexture, grassTexture);

    float moundCenterZ = getTee().z - 4.0f - 3.0f;
    glm::vec3 cylinderPos = glm::vec3(
        getTee().x,
        getTee().y - (greenHeight / 2.0f),
        moundCenterZ - moundRadius - greenRadius  
    );

    greenCylinder->setPosition(cylinderPos);
    greenCylinder->setRotation(glm::vec3(0.0f, 0.0f, 0.0f));
    addObject(greenCylinder);

    // --- GENERATING THE CURVED HOLE03 PLATFORM (LEFT TURN) ---
    float h3PlatformWidth = 4.5f;
    float h3SegmentLength = 1.5f;      
    float h3SegmentRender = 1.85f;    
    
    // --- CHANGED: Reduced from 10 to 6 to significantly shorten the track length ---
    int h3TotalSegments = 7; 

    float h3TotalCurveAngle = 60.0f; 
    float h3PlatformRounding = 0.2f;

    std::vector<float> h3SegData = RenderObject::createRoundedBox(
        h3PlatformWidth, greenHeight, h3SegmentRender, h3PlatformRounding, 8
    );

    // Start pointing directly out the front of the cylinder (-Z direction)
    float currentAngle = 180.0f; 
    float angleStepH3 = h3TotalCurveAngle / (h3TotalSegments - 1);

    // Set anchor point starting precisely at the terminal front edge of the cylinder
    glm::vec3 segmentCenter = glm::vec3(
        cylinderPos.x,
        cylinderPos.y,
        cylinderPos.z - greenRadius - (h3SegmentLength / 2.0f)
    );

    // --- YOUR TUNING SLIDERS ---
    float initialGapCompensation = -1.4f; // Slides the whole curve forward/backward into the cylinder base
    float lateralShiftModifier = 0.0f;    // Slides the curve left/right relative to the path center

    glm::vec3 initialForward = glm::vec3(sinf(glm::radians(currentAngle)), 0.0f, cosf(glm::radians(currentAngle)));
    glm::vec3 initialRight = glm::vec3(-initialForward.z, 0.0f, initialForward.x);
    
    segmentCenter += (initialForward * initialGapCompensation) + (initialRight * lateralShiftModifier);

    // Render the left-curving track loop
    for (int i = 0; i < h3TotalSegments; ++i) {
        RenderObject* seg = new RenderObject(h3SegData, grassTexture, grassTexture);

        glm::vec3 segPos = segmentCenter;
        seg->setPosition(segPos);
        seg->setRotation(glm::vec3(0.0f, currentAngle - 180.0f, 0.0f));
        addObject(seg);

        // Advance tracker along the current segment's exit heading
        glm::vec3 forward = glm::vec3(
            sinf(glm::radians(currentAngle)),
            0.0f,
            cosf(glm::radians(currentAngle))
        );
        segmentCenter += forward * h3SegmentLength;
        
        // This will now progressively add positive degrees, arching the path to the left
        currentAngle += angleStepH3;
    }

    // --- ADDING A SECOND GRASSY CYLINDER TO THE END OF THE CURVED PLATFORM ---
    float endCylinderRadius = 4.0f / 2.0f;           // Matches half the platform width (2.0f)
    float endCylinderHeight = greenHeight;           // Keeps it the same thickness (0.3f)

    // 1. Generate and scale the ending cylinder's data
    std::vector<float> endCylinderData = RenderObject::createCylinder(32);
    for (size_t j = 0; j < endCylinderData.size(); j += 8) {
        endCylinderData[j + 0] *= endCylinderRadius;
        endCylinderData[j + 1] *= endCylinderHeight;
        endCylinderData[j + 2] *= endCylinderRadius;

        // Re-normalize lighting vectors to prevent it from rendering black
        float nx = endCylinderData[j + 3];
        float nz = endCylinderData[j + 5];
        float len = sqrtf(nx * nx + nz * nz);
        if (len > 0.0f) {
            endCylinderData[j + 3] = nx / len;
            endCylinderData[j + 5] = nz / len;
        }
    }

    RenderObject* endCylinder = new RenderObject(endCylinderData, grassTexture, grassTexture);

    // 2. Track the exact exit direction out of the last segment
    float finalExitAngle = currentAngle - angleStepH3;
    glm::vec3 exitForward = glm::vec3(
        sinf(glm::radians(finalExitAngle)),
        0.0f,
        cosf(glm::radians(finalExitAngle))
    );

    // --- CHANGED: Lowered from 2.0f to 0.5f so it fits perfectly on a shorter path arc ---
    float endCylinderInwardSlider = 2.0f; 

    // Calculate position: Shift backward relative to the final tracking point to overlap seamlessly
    glm::vec3 endCylinderPos = segmentCenter - (exitForward * (h3SegmentLength + endCylinderRadius - endCylinderInwardSlider));
    endCylinderPos.y = cylinderPos.y; // Match the exact height profile of your initial green platform

    endCylinder->setPosition(endCylinderPos);
    endCylinder->setRotation(glm::vec3(0.0f, 0.0f, 0.0f)); // Keep lighting calculations clean
    addObject(endCylinder);


    // --- FLAG ASSEMBLY ON THE FIRST CYLINDER GREEN ---
    float firstCylinderSurfaceY = cylinderPos.y + greenHeight;

    // 1. Golf Hole (Cup)
    float holeRadius = 0.15f;
    float holeDepth = 0.2f;

    std::vector<float> holeData = RenderObject::createCylinder(16);
    RenderObject* golfHole = new RenderObject(holeData, stoneTexture, 0);
    golfHole->setScale(glm::vec3(holeRadius, holeDepth, holeRadius));
    
    golfHole->setPosition(glm::vec3(cylinderPos.x, firstCylinderSurfaceY - (holeDepth * 0.95f), cylinderPos.z));
    addObject(golfHole);

    // 2. Flag Pole
    float poleRadius = 0.02f;
    float poleHeight = 1.2f;

    std::vector<float> poleData = RenderObject::createCylinder(8);
    RenderObject* flagPole = new RenderObject(poleData, concreteTexture, 0);
    flagPole->setScale(glm::vec3(poleRadius, poleHeight, poleRadius));
    
    glm::vec3 polePos = glm::vec3(cylinderPos.x, firstCylinderSurfaceY, cylinderPos.z);
    flagPole->setPosition(polePos);
    addObject(flagPole);

    // 3. Triangular Flag
    float flagBottomWidth = 0.5f; 
    float flagTopWidth    = 0.0f; 
    float flagLength      = 0.4f; 
    float flagThickness   = 0.01f;

    std::vector<float> flagData = RenderObject::createTrapezoid(flagBottomWidth, flagTopWidth, flagLength, flagThickness);
    RenderObject* flag = new RenderObject(flagData, concreteTexture, concreteTexture);

    flag->setRotation(glm::vec3(0.0f, 0.0f, -90.0f));

    glm::vec3 flagPos = glm::vec3(
        cylinderPos.x + flagLength / 2.0f,
        firstCylinderSurfaceY + poleHeight - (flagBottomWidth / 2.0f) - raisedHeightOffset, 
        cylinderPos.z
    );
    flag->setPosition(flagPos);
    addObject(flag);


    // --- ROCKS ALONG RIGHT SIDE OF CURVED PLATFORM ---
    float rockWalkAngle = 180.0f;
    float rockEdgeOffset = h3PlatformWidth / 2.0f + 0.1f;

    glm::vec3 rockWalkPos = glm::vec3(
        cylinderPos.x,
        cylinderPos.y,
        cylinderPos.z - greenRadius - (h3SegmentLength / 2.0f)
    );
    rockWalkPos += (glm::vec3(sinf(glm::radians(rockWalkAngle)), 0.0f, cosf(glm::radians(rockWalkAngle))) * initialGapCompensation);

    int numRocksPerSeg = 2;

    // Loop automatically uses updated h3TotalSegments
    for (int i = 0; i < h3TotalSegments; ++i) {
        float a = rockWalkAngle;
        glm::vec3 forward = glm::vec3(sinf(glm::radians(a)), 0.0f, cosf(glm::radians(a)));
        glm::vec3 left = glm::vec3(-forward.z, 0.0f, forward.x);

        for (int r = 0; r < numRocksPerSeg; ++r) {
            float pct = -0.5f + (float)r / (numRocksPerSeg - 1);
            glm::vec3 rockPos = rockWalkPos
                + forward * (pct * h3SegmentLength)
                + left   * rockEdgeOffset;

            float rockRadius = 0.35f + (static_cast<float>(rand()) / RAND_MAX) * 0.3f;

            std::vector<float> rockData = RenderObject::createSphere(4, 5);
            for (size_t j = 0; j < rockData.size(); j += 8) {
                rockData[j + 0] *= rockRadius;
                rockData[j + 1] *= rockRadius;
                rockData[j + 2] *= rockRadius;
            }

            RenderObject* rock = new RenderObject(rockData, stoneTexture, 0);
            rock->setPosition(glm::vec3(rockPos.x, getTee().y + raisedHeightOffset - (rockRadius * 0.15f), rockPos.z));
            addObject(rock);
        }

        rockWalkPos += forward * h3SegmentLength;
        rockWalkAngle += angleStepH3;
    }

    // --- ROCKS AROUND A QUARTER OF THE CYLINDER GREEN ---
    int numCylRocks = 4; 
    float cylRockOrbit = endCylinderRadius + 0.2f;
    
    // --- FIXED HERE: Changed angleStepH2 to angleStepH3 ---
    float finalExitAngle2 = 180.0f + h3TotalCurveAngle; 

    for (int i = 0; i < numCylRocks; ++i) {
        float t = (float)i / (numCylRocks - 1);
        
        // Use the tracked finalExitAngle variable we calculated earlier for the end cylinder!
        float arcAngle = glm::radians(finalExitAngle) + ((float)M_PI * 0.5f) * (-0.5f + t) - glm::radians(60.0f);

        glm::vec3 offset = glm::vec3(sinf(arcAngle), 0.0f, cosf(arcAngle)) * cylRockOrbit;
        glm::vec3 rockPos = endCylinderPos + offset; 

        float rockRadius = 0.35f + (static_cast<float>(rand()) / RAND_MAX) * 0.3f;

        std::vector<float> rockData = RenderObject::createSphere(4, 5);
        for (size_t j = 0; j < rockData.size(); j += 8) {
            rockData[j + 0] *= rockRadius;
            rockData[j + 1] *= rockRadius;
            rockData[j + 2] *= rockRadius;
        }

        RenderObject* rock = new RenderObject(rockData, stoneTexture, 0);
        rock->setPosition(glm::vec3(rockPos.x, getTee().y + raisedHeightOffset - (rockRadius * 0.15f), rockPos.z));
        addObject(rock);
    }

    // Connection Hole02 and Hole03 with a Curved Bridge Path
    // 1. Re-track Hole02's Green Cylinder Position
    float surfaceY = getTee().y + raisedHeightOffset; 

    float platformHeightH2 = 1.5f;
    float pathDepthH1 = 8.0f;
    float turnSegmentLengthH1 = 1.0f;
    int turnSegmentsH1 = 9;
    float totalCurveAngleH1 = 45.0f;
    float platformWidthH1 = 8.0f;
    float halfWidthH1 = platformWidthH1 / 2.0f;
    float pathOutwardOffset = 0.8f;

    glm::vec3 h2SegmentCenter = glm::vec3(
        getTee().x - halfWidthH1 - pathOutwardOffset,
        surfaceY, // Now safely defined!
        getTee().z - (pathDepthH1 / 2.0f) - (turnSegmentLengthH1 / 2.0f)
    );

    float startAngleH1 = 180.0f;
    float angleStepH1 = totalCurveAngleH1 / (turnSegmentsH1 - 1);
    float finalH1Angle = startAngleH1;

    for (int i = 0; i < turnSegmentsH1; ++i) {
        finalH1Angle = startAngleH1 + i * angleStepH1;
        glm::vec3 forward = glm::vec3(sinf(glm::radians(finalH1Angle)), 0.0f, cosf(glm::radians(finalH1Angle)));
        h2SegmentCenter += forward * turnSegmentLengthH1;
    }

    float h2PlatformWidth = 4.5f;
    float h2SegmentLength = 1.0f;
    int h2TotalSegments = 12;
    float h2TotalCurveAngle = -60.0f;
    float angleStepH2 = h2TotalCurveAngle / (h2TotalSegments - 1);
    float h2CurrentAngle = finalH1Angle;

    float initialGapCompensationH2 = -0.5f;
    glm::vec3 h2InitialForward = glm::vec3(sinf(glm::radians(h2CurrentAngle)), 0.0f, cosf(glm::radians(h2CurrentAngle)));
    h2SegmentCenter += h2InitialForward * initialGapCompensationH2;

    for (int i = 0; i < h2TotalSegments; ++i) {
        glm::vec3 forward = glm::vec3(sinf(glm::radians(h2CurrentAngle)), 0.0f, cosf(glm::radians(h2CurrentAngle)));
        h2SegmentCenter += forward * h2SegmentLength;
        h2CurrentAngle += angleStepH2;
    }

    float h2GreenRadius = h2PlatformWidth / 2.0f;
    float h2CylinderInwardSlider = 4.5f;
    glm::vec3 h2FinalExitDir = glm::vec3(sinf(glm::radians(h2CurrentAngle - angleStepH2)), 0.0f, cosf(glm::radians(h2CurrentAngle - angleStepH2)));
    
    // This is our absolute destination target center!
    glm::vec3 h2CylinderTargetPos = h2SegmentCenter + (h2FinalExitDir * (h2GreenRadius - h2CylinderInwardSlider));
    h2CylinderTargetPos.y = getTee().y - (platformHeightH2 / 2.0f) + raisedHeightOffset;


    // 2. Build the Curved Interpolation Path
    // Start node: endCylinderPos (Hole03 Exit)
    // End node: h2CylinderTargetPos (Hole02 Entrance)
    glm::vec3 bridgeStart = endCylinderPos;
    glm::vec3 bridgeEnd   = h2CylinderTargetPos;

    int bridgeSegments = 8; // Number of intermediate steps
    float bridgeWidth = 3.5f; // Sized slightly narrower than main paths for visual style
    float bridgeRounding = 0.1f;
    float bridgeSegmentLength = 1.6f;

    std::vector<float> bridgeSegData = RenderObject::createRoundedBox(
        bridgeWidth, greenHeight, bridgeSegmentLength, bridgeRounding, 8
    );

    // Generate path using a simple linear/bezier hybrid tracking system
    for (int i = 0; i < bridgeSegments; ++i) {
        float t = (float)i / (bridgeSegments - 1);
        
        // Linear interpolation from Start Green to End Green
        glm::vec3 currentBridgePos = glm::mix(bridgeStart, bridgeEnd, t);
        
        // Smooth out the height assignment dynamically
        currentBridgePos.y = cylinderPos.y; 

        RenderObject* bridgeSeg = new RenderObject(bridgeSegData, grassTexture, grassTexture);
        bridgeSeg->setPosition(currentBridgePos);

        // Calculate rotation heading to point towards the target cleanly
        glm::vec3 nextPos = glm::mix(bridgeStart, bridgeEnd, glm::min(t + 0.05f, 1.0f));
        glm::vec3 headingDirection = glm::normalize(nextPos - currentBridgePos);
        
        float bridgeAngleRadians = atan2f(headingDirection.x, headingDirection.z);
        float bridgeAngleDegrees = glm::degrees(bridgeAngleRadians);

        bridgeSeg->setRotation(glm::vec3(0.0f, bridgeAngleDegrees, 0.0f));
        addObject(bridgeSeg);
    }
}