#include "Hole03.h"

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
    float h3SegmentLength = 1.5f;    // reducing this makes them more compact  
    float h3SegmentRender = 1.85f;    
    int h3TotalSegments = 10;

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
    float endCylinderRadius = 4.0f / 2.0f; // Matches half the platform width (2.0f)
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
    // We subtract one angle step because the loop increments it one extra time at the very end
    float finalExitAngle = currentAngle - angleStepH3;
    glm::vec3 exitForward = glm::vec3(
        sinf(glm::radians(finalExitAngle)),
        0.0f,
        cosf(glm::radians(finalExitAngle))
    );

    // --- GAP CLOSING TUNING SLIDER ---
    // Increase this value to push the cylinder deeper inside the final runway block.
    // Decrease it if it floats away or overlaps too aggressively.
    float endCylinderInwardSlider = 2.0f; 

    // Calculate position: Shift backward relative to the final tracking point to overlap seamlessly
    glm::vec3 endCylinderPos = segmentCenter - (exitForward * (h3SegmentLength + endCylinderRadius - endCylinderInwardSlider));
    endCylinderPos.y = cylinderPos.y; // Match the exact height profile of your initial green platform

    endCylinder->setPosition(endCylinderPos);
    endCylinder->setRotation(glm::vec3(0.0f, 0.0f, 0.0f)); // Keep lighting calculations clean

    addObject(endCylinder);

    // --- ROCKS ALONG RIGHT SIDE OF CURVED PLATFORM ---
float rockWalkAngle = 180.0f;
float rockEdgeOffset = h3PlatformWidth / 2.0f + 0.1f;

// Reset to the same start point as the curve
glm::vec3 rockWalkPos = glm::vec3(
    cylinderPos.x,
    cylinderPos.y,
    cylinderPos.z - greenRadius - (h3SegmentLength / 2.0f)
);
rockWalkPos += (glm::vec3(sinf(glm::radians(rockWalkAngle)), 0.0f, cosf(glm::radians(rockWalkAngle))) * initialGapCompensation);

int numRocksPerSeg = 2;

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

// --- ROCKS AROUND RIGHT HALF OF END CYLINDER ---
int numCylRocks = 8;
float cylRockOrbit = endCylinderRadius + 0.2f;
float finalExitAngle2 = 180.0f + h3TotalCurveAngle; // exit heading

for (int i = 0; i < numCylRocks; ++i) {
    float t = (float)i / (numCylRocks - 1);
    float arcAngle = glm::radians(finalExitAngle2) + (float)M_PI * (-0.5f + t);

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
}