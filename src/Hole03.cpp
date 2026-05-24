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
    float surfaceY = getTee().y + raisedHeightOffset; // add this



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

    // --- CONCRETE BRIDGE: Hole02 Cylinder -> Hole03 End Cylinder ---

// Re-track Hole02's cylinder position
float platformHeightH2 = 1.5f;
float pathDepthH1b = 8.0f;
float turnSegmentLengthH1b = 1.0f;
int turnSegmentsH1b = 9;
float totalCurveAngleH1b = 45.0f;
float platformWidthH1b = 8.0f;
float halfWidthH1b = platformWidthH1b / 2.0f;
float pathOutwardOffsetb = 0.8f;



glm::vec3 h2SegCenter = glm::vec3(
    getTee().x - halfWidthH1b - pathOutwardOffsetb,
    surfaceY,
    getTee().z - (pathDepthH1b / 2.0f) - (turnSegmentLengthH1b / 2.0f)
);

float startAngleH1b = 180.0f;
float angleStepH1b = totalCurveAngleH1b / (turnSegmentsH1b - 1);
float finalH1Angleb = startAngleH1b;

for (int i = 0; i < turnSegmentsH1b; ++i) {
    finalH1Angleb = startAngleH1b + i * angleStepH1b;
    glm::vec3 fwd = glm::vec3(sinf(glm::radians(finalH1Angleb)), 0.0f, cosf(glm::radians(finalH1Angleb)));
    h2SegCenter += fwd * turnSegmentLengthH1b;
}

float h2PlatformWidthb  = 4.5f;
float h2SegmentLengthb  = 1.0f;
int   h2TotalSegmentsb  = 12;
float h2TotalCurveAngleb = -60.0f;
float angleStepH2b = h2TotalCurveAngleb / (h2TotalSegmentsb - 1);
float h2CurrentAngleb = finalH1Angleb;

glm::vec3 h2InitFwd = glm::vec3(sinf(glm::radians(h2CurrentAngleb)), 0.0f, cosf(glm::radians(h2CurrentAngleb)));
h2SegCenter += h2InitFwd * (-0.5f);

for (int i = 0; i < h2TotalSegmentsb; ++i) {
    glm::vec3 fwd = glm::vec3(sinf(glm::radians(h2CurrentAngleb)), 0.0f, cosf(glm::radians(h2CurrentAngleb)));
    h2SegCenter += fwd * h2SegmentLengthb;
    h2CurrentAngleb += angleStepH2b;
}

float h2GreenRadiusb = h2PlatformWidthb / 2.0f;
float h2CylInwardSlider = 2.5f;
glm::vec3 h2ExitDir = glm::vec3(
    sinf(glm::radians(h2CurrentAngleb - angleStepH2b)),
    0.0f,
    cosf(glm::radians(h2CurrentAngleb - angleStepH2b))
);

glm::vec3 h2CylPos = h2SegCenter + (h2ExitDir * (h2GreenRadiusb - h2CylInwardSlider));
h2CylPos.y = getTee().y - (platformHeightH2 / 2.0f) + raisedHeightOffset;

// Bridge start = Hole02 cylinder, bridge end = Hole03 end cylinder
glm::vec3 bridgeStart = h2CylPos;
glm::vec3 bridgeEnd   = endCylinderPos;

// Midpoint control for the curve (pulls the path into a smooth arc)
glm::vec3 bridgeMid = glm::vec3(
    (bridgeStart.x + bridgeEnd.x) / 2.0f - 3.0f, // pull left for natural curve
    cylinderPos.y,
    (bridgeStart.z + bridgeEnd.z) / 2.0f
);

int   bridgeSegments      = 12;
float bridgeWidth         = 2.3f;
float bridgeRounding      = 0.0f;  // flat like the concrete path in Hole01
float bridgeSegmentLength = 1.4f;
float bridgeSegmentRender = 1.6f;  // overlap to hide seams

std::vector<float> bridgeSegData = RenderObject::createRoundedBox(
    bridgeWidth, greenHeight, bridgeSegmentRender, bridgeRounding, 8
);

for (int i = 0; i < bridgeSegments; ++i) {
    float t = (float)i / (bridgeSegments - 1);

    // Quadratic bezier: gives a proper curve rather than a straight line
    float u = 1.0f - t;
    glm::vec3 pos = (u * u) * bridgeStart
                  + (2.0f * u * t) * bridgeMid
                  + (t * t) * bridgeEnd;
    pos.y = cylinderPos.y;

    // Heading: sample slightly ahead on the curve for rotation
    float t2 = glm::min(t + 0.01f, 1.0f);
    float u2 = 1.0f - t2;
    glm::vec3 nextPos = (u2 * u2) * bridgeStart
                      + (2.0f * u2 * t2) * bridgeMid
                      + (t2 * t2) * bridgeEnd;

    glm::vec3 heading = glm::normalize(nextPos - pos);
    float angle = glm::degrees(atan2f(heading.x, heading.z));

    RenderObject* bridgeSeg = new RenderObject(bridgeSegData, concreteTexture, 0);
    bridgeSeg->setPosition(pos);
    bridgeSeg->setRotation(glm::vec3(0.0f, angle, 0.0f));
    addObject(bridgeSeg);
}

    
}