#include "Hole02.h"
#include "Terrain.h"
#include "RenderObject.h"
#include "TextureLoader.h"

void Hole02::design(Terrain& terrain) {
    // Call base design if needed
    Hole::design(terrain);

    // 1. Load textures
    GLuint grassTexture = loadBMPTexture("assets/terrain/grass.bmp");
    if (grassTexture == 0) return;

    // Platform basic structural dimensions
    float platformHeight = 1.5f;
    float raisedHeightOffset = 0.1f;
    float surfaceY = getTee().y + raisedHeightOffset;

    // Hole01's original path dimensions (used strictly to find our starting seam)
    float pathDepthH1 = 8.0f;
    float turnSegmentLengthH1 = 1.0f;
    int turnSegmentsH1 = 9;
    float totalCurveAngleH1 = 45.0f;

    // --- RE-TRACKING THE HOLE01 END POINT ---
    glm::vec3 hole1Tee = getTee(); 
    float platformWidthH1 = 8.0f;
    float halfWidthH1 = platformWidthH1 / 2.0f;
    float pathOutwardOffset = 0.8f;

    glm::vec3 segmentCenter = glm::vec3(
        hole1Tee.x - halfWidthH1 - pathOutwardOffset,
        surfaceY,
        hole1Tee.z - (pathDepthH1 / 2.0f) - (turnSegmentLengthH1 / 2.0f)
    );

    float startAngleH1 = 180.0f;
    float angleStepH1 = totalCurveAngleH1 / (turnSegmentsH1 - 1);
    float finalH1Angle = startAngleH1;

    // Find exactly where Hole01's curve finishes
    for (int i = 0; i < turnSegmentsH1; ++i) {
        finalH1Angle = startAngleH1 + i * angleStepH1;
        glm::vec3 forward = glm::vec3(
            sinf(glm::radians(finalH1Angle)),
            0.0f,
            cosf(glm::radians(finalH1Angle))
        );
        segmentCenter += forward * turnSegmentLengthH1;
    }

    // --- GENERATING THE CURVED HOLE02 PLATFORM ---
    float h2PlatformWidth = 4.5f;        // UPDATED: Increased width to 4.5f
    float h2SegmentLength = 1.0f;        
    int h2TotalSegments = 12;            
    float h2TotalCurveAngle = -60.0f;    
    float h2PlatformRounding = 0.2f;
    float h2SegmentRender = 1.75f; 

    std::vector<float> h2SegData = RenderObject::createRoundedBox(
    h2PlatformWidth, platformHeight, h2SegmentRender, // use render length here
    h2PlatformRounding, 8
);
    float currentAngle = finalH1Angle; 
    float angleStepH2 = h2TotalCurveAngle / (h2TotalSegments - 1);

    float initialGapCompensation = -0.5f; 
    glm::vec3 initialForward = glm::vec3(sinf(glm::radians(currentAngle)), 0.0f, cosf(glm::radians(currentAngle)));
    segmentCenter += initialForward * initialGapCompensation;

    for (int i = 0; i < h2TotalSegments; ++i) {
        float renderAngle = currentAngle;
        
        RenderObject* seg = new RenderObject(h2SegData, grassTexture, 0);
        
        glm::vec3 segPos = segmentCenter;
        segPos.y = getTee().y - (platformHeight / 2.0f) + raisedHeightOffset;
        
        seg->setPosition(segPos);
        seg->setRotation(glm::vec3(0.0f, renderAngle - 180.0f, 0.0f));
        addObject(seg);

        glm::vec3 forward = glm::vec3(
            sinf(glm::radians(currentAngle)),
            0.0f,
            cosf(glm::radians(currentAngle))
        );
        segmentCenter += forward * h2SegmentLength;
        currentAngle += angleStepH2;
    }

    // --- ADDING THE GRASSY CYLINDER GREEN TO THE END (WITH INDEPEDENT SLIDER) ---
    float greenRadius = h2PlatformWidth/2; // 4.5f
    float greenDepth = platformHeight/2;   

    // 1. Generate and scale cylinder vertices
    std::vector<float> cylinderData = RenderObject::createCylinder(32);
    RenderObject* greenCylinder = new RenderObject(cylinderData, grassTexture, grassTexture);

    // 2. Apply scale via transform instead
    greenCylinder->setScale(glm::vec3(greenRadius, greenDepth, greenRadius));
    
    // 2. Direction vector matching the exit heading of the final segment
    glm::vec3 finalExitDirection = glm::vec3(
        sinf(glm::radians(currentAngle - angleStepH2)), 
        0.0f,
        cosf(glm::radians(currentAngle - angleStepH2))
    );

    // --- YOUR INWARD POSITION SLIDER ---
    // Increase this number to pull the cylinder deeper inside the current platform.
    // Decrease it (closer to 0.0f) to push it further outward into space.
    float cylinderInwardSlider = 2.5f; 

    // Calculate position: Start from the last segment edge, then offset it safely
    glm::vec3 cylinderPos = segmentCenter + (finalExitDirection * (greenRadius - cylinderInwardSlider));
    cylinderPos.y = getTee().y - (platformHeight / 2.0f) + raisedHeightOffset;

    greenCylinder->setPosition(cylinderPos);
    
    // Reset rotation to flat zero to ensure normal lighting alignments match up cleanly
    greenCylinder->setRotation(glm::vec3(0.0f, 0.0f, 0.0f));
    
    addObject(greenCylinder);

    // --- ROCKS ALONG LEFT SIDE OF CURVED PLATFORM + CYLINDER ---
    GLuint stoneTexture = loadBMPTexture("assets/terrain/stone.bmp");
    if (stoneTexture == 0) return;

    int numRocksPerSeg = 3;
    float rockEdgeOffset = (h2PlatformWidth / 2.0f) + 0.1f; // just outside left edge

    // Reset to re-walk the same curve
    glm::vec3 rockSegCenter = segmentCenter; // segmentCenter is already past the loop, so re-walk
    float rockAngle = finalH1Angle;

    // Re-walk from the beginning to place rocks
    glm::vec3 rockWalkPos = glm::vec3(
        hole1Tee.x - halfWidthH1 - pathOutwardOffset,
        surfaceY,
        hole1Tee.z - (pathDepthH1 / 2.0f) - (turnSegmentLengthH1 / 2.0f)
    );
    for (int i = 0; i < turnSegmentsH1; ++i) {
        float a = startAngleH1 + i * angleStepH1;
        rockWalkPos += glm::vec3(sinf(glm::radians(a)), 0.0f, cosf(glm::radians(a))) * turnSegmentLengthH1;
    }
    rockWalkPos += initialForward * initialGapCompensation;

    for (int i = 0; i < h2TotalSegments; ++i) {
        float a = finalH1Angle + i * angleStepH2;

        glm::vec3 forward = glm::vec3(sinf(glm::radians(a)), 0.0f, cosf(glm::radians(a)));

        // Left perpendicular (rotate forward -90 degrees around Y)
        glm::vec3 right = glm::vec3(forward.z, 0.0f, -forward.x);

        for (int r = 0; r < numRocksPerSeg; ++r) {
            float pct = -0.5f + (float)r / (numRocksPerSeg - 1); // spread along segment length
            glm::vec3 rockPos = rockWalkPos
                + forward * (pct * h2SegmentLength)
                + right   * rockEdgeOffset;
            rockPos.y = getTee().y + raisedHeightOffset;

            float rockRadius = 0.35f + (static_cast<float>(rand()) / RAND_MAX) * 0.3f;

            std::vector<float> rockData = RenderObject::createSphere(4, 5);
            for (size_t j = 0; j < rockData.size(); j += 8) {
                rockData[j + 0] *= rockRadius;
                rockData[j + 1] *= rockRadius;
                rockData[j + 2] *= rockRadius;
            }

            RenderObject* rock = new RenderObject(rockData, stoneTexture, 0);
            rock->setPosition(glm::vec3(rockPos.x, rockPos.y - (rockRadius * 0.15f), rockPos.z));
            addObject(rock);
        }

        rockWalkPos += forward * h2SegmentLength;
    }

    // --- ROCKS AROUND A QUARTER OF THE CYLINDER GREEN ---
    int numCylRocks = 4; // Reduced count since the arc is tighter
    float cylRockRadius = greenRadius + 0.2f; 

    for (int i = 0; i < numCylRocks; ++i) {
        float t = (float)i / (numCylRocks - 1); 
        
        // --- UPDATED: Multiplied by 0.5f * M_PI to shrink the arc from 180 to 90 degrees ---
        // This places them strictly on the outer-left quadrant, leaving the right/back open
        float arcAngle = glm::radians(currentAngle - angleStepH2) + ((float)M_PI * 0.5f) * (-0.5f + t) + glm::radians(45.0f);

        glm::vec3 offset = glm::vec3(sinf(arcAngle), 0.0f, cosf(arcAngle)) * cylRockRadius;
        glm::vec3 rockPos = greenCylinder->getPosition() + offset;
        rockPos.y = getTee().y + raisedHeightOffset;

        float rockRadius = 0.35f + (static_cast<float>(rand()) / RAND_MAX) * 0.3f;

        std::vector<float> rockData = RenderObject::createSphere(4, 5);
        for (size_t j = 0; j < rockData.size(); j += 8) {
            rockData[j + 0] *= rockRadius;
            rockData[j + 1] *= rockRadius;
            rockData[j + 2] *= rockRadius;
        }

        RenderObject* rock = new RenderObject(rockData, stoneTexture, 0);
        rock->setPosition(glm::vec3(rockPos.x, rockPos.y - (rockRadius * 0.15f), rockPos.z));
        addObject(rock);
    }
// --- FLAG ASSEMBLY ON THE CYLINDER GREEN ---
    // Anchor everything to the exact center of the cylinder green surface
    glm::vec3 cylinderCenter = greenCylinder->getPosition();
    
    // Calculate the top surface of the scaled cylinder green
    float greenSurfaceY = cylinderCenter.y + greenDepth; 

    // 1. Golf Hole (Cup)
    float holeRadius = 0.15f;
    float holeDepth = 0.2f;

    std::vector<float> holeData = RenderObject::createCylinder(16);
    RenderObject* golfHole = new RenderObject(holeData, stoneTexture, 0);
    golfHole->setScale(glm::vec3(holeRadius, holeDepth, holeRadius));
    
    // Sink the cup slightly into the surface
    golfHole->setPosition(glm::vec3(cylinderCenter.x, greenSurfaceY - (holeDepth * 0.95f), cylinderCenter.z));
    addObject(golfHole);

    // 2. Flag Pole
    float poleRadius = 0.02f;
    float poleHeight = 1.2f;

    std::vector<float> poleData = RenderObject::createCylinder(8);
    RenderObject* flagPole = new RenderObject(poleData, stoneTexture, 0);
    flagPole->setScale(glm::vec3(poleRadius, poleHeight, poleRadius));
    
    // Position it directly at the center of the cup
    glm::vec3 polePos = glm::vec3(cylinderCenter.x, greenSurfaceY, cylinderCenter.z);
    flagPole->setPosition(polePos);
    addObject(flagPole);

    // 3. Triangular Flag (Using the custom createTrapezoid from Hole03)
    float flagBottomWidth = 0.5f; 
    float flagTopWidth    = 0.0f; // Squeezing to zero turns the trapezoid into a triangle
    float flagLength      = 0.4f; 
    float flagThickness   = 0.01f;

    std::vector<float> flagData = RenderObject::createTrapezoid(flagBottomWidth, flagTopWidth, flagLength, flagThickness);
    RenderObject* flag = new RenderObject(flagData, stoneTexture, stoneTexture);

    // Rotate it 90 degrees on Z so it waves horizontally out from the pole
    flag->setRotation(glm::vec3(0.0f, 0.0f, -90.0f));

    // Position it near the top of the flagpole
    glm::vec3 flagPos = glm::vec3(
        cylinderCenter.x + flagLength / 2.0f,
        greenSurfaceY + poleHeight - (flagBottomWidth / 2.0f) - raisedHeightOffset, 
        cylinderCenter.z
    );
    flag->setPosition(flagPos);
    addObject(flag);
}