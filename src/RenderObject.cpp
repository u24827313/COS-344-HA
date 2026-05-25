#include "RenderObject.h"
#include "MathUtils.h"
#include "glm/glm.hpp"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

RenderObject::RenderObject(const std::vector<float>& vertexData, GLuint texture_, GLuint shaderProgram_)
    : VAO(0), VBO(0),
      vertexCount(0),
      texture(texture_),
      shaderProgram(shaderProgram_),
      position(0.0f),
      rotationEuler(0.0f),
      scale(1.0f),
      bufferInitialised(false)
{
    initBuffer(vertexData);
}

RenderObject::~RenderObject() {
    if (bufferInitialised) {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
    }
}



void RenderObject::initBuffer(const std::vector<float>& vertexData) {
    if (vertexData.empty()) return;

    // 8 floats per vertex
    vertexCount = (int)(vertexData.size() / 8);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER,
                 vertexData.size() * sizeof(float),
                 vertexData.data(),
                 GL_STATIC_DRAW);

    const GLsizei stride = 8 * sizeof(float);
    // position (location 0)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    // normal (location 1)
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    // uv (location 2)
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));

    glBindVertexArray(0);

    bufferInitialised = true;
}

glm::mat4 RenderObject::computeModelMatrix() const {
    glm::mat4 m = makeTranslate(position);
    m = m * makeRotateY(toRadians(rotationEuler.y));
    m = m * makeRotateX(toRadians(rotationEuler.x));
    m = m * makeRotateZ(toRadians(rotationEuler.z));
    m = m * makeScale(scale);
    return m;
}

void RenderObject::render(const float* view4x4,
                          const float* proj4x4,
                          const glm::vec3& cameraPos,
                          const DirectionalLight& light)
{
    if (!bufferInitialised || shaderProgram == 0) return;

    glUseProgram(shaderProgram);

    // Matrices
    glm::mat4 model = computeModelMatrix();
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"),      1, GL_FALSE, &model[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"),       1, GL_FALSE, view4x4);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, proj4x4);

    // Lighting
    glUniform3fv(glGetUniformLocation(shaderProgram, "lightDirection"), 1, &light.direction[0]);
    glUniform3fv(glGetUniformLocation(shaderProgram, "lightColour"),    1, &light.colour[0]);
    glUniform1f (glGetUniformLocation(shaderProgram, "lightAmbient"),       light.ambient);
    glUniform3fv(glGetUniformLocation(shaderProgram, "viewPos"),        1, &cameraPos[0]);

    // Texture
    if (texture != 0) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glUniform1i(glGetUniformLocation(shaderProgram, "diffuseTex"), 0);
    }

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    glBindVertexArray(0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

std::vector<float> RenderObject::createQuad() {
    
    return {
        // x      y     z     nx   ny   nz    u    v
        -0.5f, 0.0f, -0.5f,   0.0f, 1.0f, 0.0f,  0.0f, 0.0f,
         0.5f, 0.0f, -0.5f,   0.0f, 1.0f, 0.0f,  1.0f, 0.0f,
         0.5f, 0.0f,  0.5f,   0.0f, 1.0f, 0.0f,  1.0f, 1.0f,

         0.5f, 0.0f,  0.5f,   0.0f, 1.0f, 0.0f,  1.0f, 1.0f,
        -0.5f, 0.0f,  0.5f,   0.0f, 1.0f, 0.0f,  0.0f, 1.0f,
        -0.5f, 0.0f, -0.5f,   0.0f, 1.0f, 0.0f,  0.0f, 0.0f,
    };
}

std::vector<float> RenderObject::createSphere(int parralels, int meridians) {
    std::vector<float> data;

    auto pointAt = [&](int lat, int lon, float& x, float& y, float& z,
                       float& u, float& v) {
        float theta = (float)lat / parralels * (float)M_PI;        // 0..pi
        float phi   = (float)lon / meridians * 2.0f * (float)M_PI; // 0..2pi
        float sinT = sinf(theta), cosT = cosf(theta);
        float sinP = sinf(phi),   cosP = cosf(phi);
        x = sinT * cosP;
        y = cosT;
        z = sinT * sinP;
        u = (float)lon / meridians;
        v = (float)lat / parralels;
    };

    auto push = [&](float x, float y, float z, float u, float v) {
        data.push_back(x); data.push_back(y); data.push_back(z);
        data.push_back(x); data.push_back(y); data.push_back(z);  // normal = position for unit sphere
        data.push_back(u); data.push_back(v);
    };

    for (int lat = 0; lat < parralels; ++lat) {
        for (int lon = 0; lon < meridians; ++lon) {
            float x1, y1, z1, u1, v1;  pointAt(lat,     lon,     x1, y1, z1, u1, v1);
            float x2, y2, z2, u2, v2;  pointAt(lat + 1, lon,     x2, y2, z2, u2, v2);
            float x3, y3, z3, u3, v3;  pointAt(lat,     lon + 1, x3, y3, z3, u3, v3);
            float x4, y4, z4, u4, v4;  pointAt(lat + 1, lon + 1, x4, y4, z4, u4, v4);

            // Triangle 1: 1, 2, 3
            push(x1, y1, z1, u1, v1);
            push(x2, y2, z2, u2, v2);
            push(x3, y3, z3, u3, v3);
            // Triangle 2: 3, 2, 4
            push(x3, y3, z3, u3, v3);
            push(x2, y2, z2, u2, v2);
            push(x4, y4, z4, u4, v4);
        }
    }
    return data;
}

std::vector<float> RenderObject::createCylinder(int segments) {
    std::vector<float> data;
    // Build the side as `segments` quads around the Y axis.
    for (int i = 0; i < segments; ++i) {
        float a0 = (float)i       / segments * 2.0f * (float)M_PI;
        float a1 = (float)(i + 1) / segments * 2.0f * (float)M_PI;
        float c0 = cosf(a0), s0 = sinf(a0);
        float c1 = cosf(a1), s1 = sinf(a1);
        float u0 = (float)i       / segments;
        float u1 = (float)(i + 1) / segments;

        // Bottom-left, bottom-right, top-right, top-left
        // Position: (c, y, s)  Normal: (c, 0, s)  UV: (u, v)
        float bl[8] = { c0, 0, s0,  c0, 0, s0,  u0, 0 };
        float br[8] = { c1, 0, s1,  c1, 0, s1,  u1, 0 };
        float tr[8] = { c1, 1, s1,  c1, 0, s1,  u1, 1 };
        float tl[8] = { c0, 1, s0,  c0, 0, s0,  u0, 1 };

        auto pushVert = [&](const float* v) {
            for (int k = 0; k < 8; ++k) data.push_back(v[k]);
        };

        pushVert(bl); pushVert(br); pushVert(tr);
        pushVert(tr); pushVert(tl); pushVert(bl);
    }
    return data;
}

std::vector<float> RenderObject::createBox(float width, float height, float depth) {
    std::vector<float> data;
    
    float w = width * 0.5f;
    float h = height * 0.5f;
    float d = depth * 0.5f;

    auto pushFace = [&](
        float x1, float y1, float z1,
        float x2, float y2, float z2,
        float x3, float y3, float z3,
        float x4, float y4, float z4,
        float nx, float ny, float nz) {
        
        auto push = [&](float x, float y, float z, float u, float v) {
            data.push_back(x); data.push_back(y); data.push_back(z);
            data.push_back(nx); data.push_back(ny); data.push_back(nz);
            data.push_back(u); data.push_back(v);
        };

        // Triangle 1: p1, p2, p3
        push(x1, y1, z1, 0.0f, 1.0f);
        push(x2, y2, z2, 1.0f, 1.0f);
        push(x3, y3, z3, 1.0f, 0.0f);
        
        // Triangle 2: p1, p3, p4
        push(x1, y1, z1, 0.0f, 1.0f);
        push(x3, y3, z3, 1.0f, 0.0f);
        push(x4, y4, z4, 0.0f, 0.0f);
    };

    // Front face (+Z)
    pushFace(-w, -h,  d,   w, -h,  d,   w,  h,  d,  -w,  h,  d,   0,0,1);
    // Back face (-Z)
    pushFace( w, -h, -d,  -w, -h, -d,  -w,  h, -d,   w,  h, -d,   0,0,-1);
    // Top face (+Y)
    pushFace(-w,  h, -d,   w,  h, -d,   w,  h,  d,  -w,  h,  d,   0,1,0);
    // Bottom face (-Y)
    pushFace(-w, -h,  d,   w, -h,  d,   w, -h, -d,  -w, -h, -d,   0,-1,0);
    // Right face (+X)
    pushFace( w, -h, -d,   w, -h,  d,   w,  h,  d,   w,  h, -d,   1,0,0);
    // Left face (-X)
    pushFace(-w, -h,  d,  -w, -h, -d,  -w,  h, -d,  -w,  h,  d,  -1,0,0);

    return data;
}

std::vector<float> RenderObject::createRoundedBox(float width, float height, float depth, float radius, int subdivisions) {
    std::vector<float> data;

    float hw = width / 2.0f;
    float hh = height / 2.0f;
    float hd = depth / 2.0f;

    auto pushVert = [&](float x, float y, float z, float nx, float ny, float nz, float u, float v) {
        data.push_back(x);  data.push_back(y);  data.push_back(z);  
        data.push_back(nx); data.push_back(ny); data.push_back(nz); 
        data.push_back(u);  data.push_back(v);                      
    };

    for (int face = 0; face < 6; ++face) {
        for (int i = 0; i < subdivisions; ++i) {
            for (int j = 0; j < subdivisions; ++j) {
                
                float u0 = (float)i / subdivisions;
                float v0 = (float)j / subdivisions;
                float u1 = (float)(i + 1) / subdivisions;
                float v1 = (float)(j + 1) / subdivisions;

                float x[4], y[4], z[4];
                float nx[4], ny[4], nz[4]; // Array to store individual vertex normals

                auto getFacePoint = [&](float u, float v, float& outX, float& outY, float& outZ, float& outNX, float& outNY, float& outNZ) {
                    float a = -1.0f + 2.0f * u;
                    float b = -1.0f + 2.0f * v;

                    if (face == 0) { outX =  hw * a; outY =  hh * b; outZ =  hd;     } // Front
                    if (face == 1) { outX =  hw * a; outY =  hh * b; outZ = -hd;     } // Back
                    if (face == 2) { outX =  hw * a; outY =  hh;     outZ =  hd * b; } // Top
                    if (face == 3) { outX =  hw * a; outY = -hh;     outZ =  hd * b; } // Bottom
                    if (face == 4) { outX =  hw;     outY =  hh * a; outZ =  hd * b; } // Right
                    if (face == 5) { outX = -hw;     outY =  hh * a; outZ =  hd * b; } // Left

                    // Find inner rigid boundary box coordinates 
                    float cx = fmaxf(-hw + radius, fminf(outX, hw - radius));
                    float cy = fmaxf(-hh + radius, fminf(outY, hh - radius));
                    float cz = fmaxf(-hd + radius, fminf(outZ, hd - radius));

                    // Direction vector tracking outward offset from inner boundary
                    float dx = outX - cx;
                    float dy = outY - cy;
                    float dz = outZ - cz;
                    float len = sqrtf(dx * dx + dy * dy + dz * dz);

                    if (len > 0.001f) {
                        // Mathematically perfect Outward Normal vector
                        outNX = dx / len;
                        outNY = dy / len;
                        outNZ = dz / len;

                        // Final physical vertex position surface displacement
                        outX = cx + outNX * radius;
                        outY = cy + outNY * radius;
                        outZ = cz + outNZ * radius;
                    } else {
                        // Fallback fallback normal for completely flat center points of the faces
                        if (face == 0) { outNX = 0.0f;  outNY = 0.0f;  outNZ = 1.0f;  }
                        if (face == 1) { outNX = 0.0f;  outNY = 0.0f;  outNZ = -1.0f; }
                        if (face == 2) { outNX = 0.0f;  outNY = 1.0f;  outNZ = 0.0f;  }
                        if (face == 3) { outNX = 0.0f;  outNY = -1.0f; outNZ = 0.0f;  }
                        if (face == 4) { outNX = 1.0f;  outNY = 0.0f;  outNZ = 0.0f;  }
                        if (face == 5) { outNX = -1.0f; outNY = 0.0f;  outNZ = 0.0f;  }
                    }
                };

                // Sample positions AND exact outward normals for all 4 corners of the grid cell
                getFacePoint(u0, v0, x[0], y[0], z[0], nx[0], ny[0], nz[0]);
                getFacePoint(u1, v0, x[1], y[1], z[1], nx[1], ny[1], nz[1]);
                getFacePoint(u1, v1, x[2], y[2], z[2], nx[2], ny[2], nz[2]);
                getFacePoint(u0, v1, x[3], y[3], z[3], nx[3], ny[3], nz[3]);

                // Triangle 1 (using precise matching vertex normals instead of shared cross-products)
                pushVert(x[0], y[0], z[0], nx[0], ny[0], nz[0], u0, v0);
                pushVert(x[1], y[1], z[1], nx[1], ny[1], nz[1], u1, v0);
                pushVert(x[2], y[2], z[2], nx[2], ny[2], nz[2], u1, v1);

                // Triangle 2
                pushVert(x[0], y[0], z[0], nx[0], ny[0], nz[0], u0, v0);
                pushVert(x[2], y[2], z[2], nx[2], ny[2], nz[2], u1, v1);
                pushVert(x[3], y[3], z[3], nx[3], ny[3], nz[3], u0, v1);
            }
        }
    }
    return data;
}

std::vector<float> RenderObject::createCone(int segments)
{
    std::vector<float> data;
    // Build the side as `segments` quads around the Y axis.
    for (int i = 0; i < segments; ++i) {
        float a0 = (float)i       / segments * 2.0f * (float)M_PI;
        float a1 = (float)(i + 1) / segments * 2.0f * (float)M_PI;
        float c0 = cosf(a0), s0 = sinf(a0);
        float c1 = cosf(a1), s1 = sinf(a1);
        float u0 = (float)i       / segments;
        float u1 = (float)(i + 1) / segments;

        // Bottom-left, bottom-right, top-right, top-left
        // Position: (c, y, s)  Normal: (c, 0, s)  UV: (u, v)
        float bl[8] = { c0, 0, s0,  c0, 0, s0,  u0, 0 };
        float br[8] = { c1, 0, s1,  c1, 0, s1,  u1, 0 };
        float tr[8] = { 0 , 1, 0 ,  0 , 1, 0 ,  u1, 1 };

        auto pushVert = [&](const float* v) {
            for (int k = 0; k < 8; ++k) data.push_back(v[k]);
        };

        pushVert(bl); pushVert(br); pushVert(tr);
    }
    return data;
}

std::vector<float> RenderObject::createTrapezoid(float bottomWidth, float topWidth, float height, float thickness) {
    std::vector<float> vertices;

    float hwBottom = bottomWidth / 2.0f;
    float hwTop = topWidth / 2.0f;
    float hh = height / 2.0f;
    float ht = thickness / 2.0f;

    // Front Face Vertices (Z = +ht)
    // Top-Left, Top-Right, Bottom-Right, Bottom-Left
    float fTL[3] = { -hwTop,    hh,  ht };
    float fTR[3] = {  hwTop,    hh,  ht };
    float fBR[3] = {  hwBottom, -hh,  ht };
    float fBL[3] = { -hwBottom, -hh,  ht };

    // Back Face Vertices (Z = -ht)
    float bTL[3] = { -hwTop,    hh, -ht };
    float bTR[3] = {  hwTop,    hh, -ht };
    float bBR[3] = {  hwBottom, -hh, -ht };
    float bBL[3] = { -hwBottom, -hh, -ht };

    // Helper lambda to easily pack a quad face vertex format: X, Y, Z, U, V, NX, NY, NZ
    // Helper lambda to safely append a quad face vertex format: X, Y, Z, U, V, NX, NY, NZ
    auto addQuad = [&](float p1[3], float p2[3], float p3[3], float p4[3], float nx, float ny, float nz) {
        // Triangle 1
        // Vertex 1 (p1)
        vertices.push_back(p1[0]); vertices.push_back(p1[1]); vertices.push_back(p1[2]);
        vertices.push_back(0.0f);  vertices.push_back(1.0f);
        vertices.push_back(nx);    vertices.push_back(ny);    vertices.push_back(nz);

        // Vertex 2 (p4)
        vertices.push_back(p4[0]); vertices.push_back(p4[1]); vertices.push_back(p4[2]);
        vertices.push_back(0.0f);  vertices.push_back(0.0f);
        vertices.push_back(nx);    vertices.push_back(ny);    vertices.push_back(nz);

        // Vertex 3 (p2)
        vertices.push_back(p2[0]); vertices.push_back(p2[1]); vertices.push_back(p2[2]);
        vertices.push_back(1.0f);  vertices.push_back(1.0f);
        vertices.push_back(nx);    vertices.push_back(ny);    vertices.push_back(nz);


        // Triangle 2
        // Vertex 1 (p2)
        vertices.push_back(p2[0]); vertices.push_back(p2[1]); vertices.push_back(p2[2]);
        vertices.push_back(1.0f);  vertices.push_back(1.0f);
        vertices.push_back(nx);    vertices.push_back(ny);    vertices.push_back(nz);

        // Vertex 2 (p4)
        vertices.push_back(p4[0]); vertices.push_back(p4[1]); vertices.push_back(p4[2]);
        vertices.push_back(0.0f);  vertices.push_back(0.0f);
        vertices.push_back(nx);    vertices.push_back(ny);    vertices.push_back(nz);

        // Vertex 3 (p3)
        vertices.push_back(p3[0]); vertices.push_back(p3[1]); vertices.push_back(p3[2]);
        vertices.push_back(1.0f);  vertices.push_back(0.0f);
        vertices.push_back(nx);    vertices.push_back(ny);    vertices.push_back(nz);
    };

    // 1. Front face
    addQuad(fTL, fTR, fBR, fBL, 0.0f, 0.0f, 1.0f);
    // 2. Back face
    addQuad(bTR, bTL, bBL, bBR, 0.0f, 0.0f, -1.0f);
    // 3. Top face
    addQuad(bTL, bTR, fTR, fTL, 0.0f, 1.0f, 0.0f);
    // 4. Bottom face
    addQuad(fBL, fBR, bBR, bBL, 0.0f, -1.0f, 0.0f);

    // 5. Left face slope normal calculation
    glm::vec3 leftEdge = glm::vec3(fTL[0] - fBL[0], fTL[1] - fBL[1], 0.0f);
    glm::vec3 leftNormal = glm::normalize(glm::vec3(-leftEdge.y, leftEdge.x, 0.0f));
    addQuad(bTL, fTL, fBL, bBL, leftNormal.x, leftNormal.y, 0.0f);

    // 6. Right face slope normal calculation
    glm::vec3 rightEdge = glm::vec3(fTR[0] - fBR[0], fTR[1] - fBR[1], 0.0f);
    glm::vec3 rightNormal = glm::normalize(glm::vec3(rightEdge.y, -rightEdge.x, 0.0f));
    addQuad(fTR, bTR, bBR, fBR, rightNormal.x, rightNormal.y, 0.0f);

    return vertices;
}

std::vector<float> RenderObject::createClosedCylinder(int segments) {
    std::vector<float> data;

    auto pushVert = [&](const float* v) {
        for (int k = 0; k < 8; ++k) data.push_back(v[k]);
    };

    // -------- SIDE WALL --------
    for (int i = 0; i < segments; ++i) {
        float a0 = (float)i       / segments * 2.0f * (float)M_PI;
        float a1 = (float)(i + 1) / segments * 2.0f * (float)M_PI;
        float c0 = cosf(a0), s0 = sinf(a0);
        float c1 = cosf(a1), s1 = sinf(a1);
        float u0 = (float)i       / segments;
        float u1 = (float)(i + 1) / segments;

        float bl[8] = { c0, 0, s0,  c0, 0, s0,  u0, 0 };
        float br[8] = { c1, 0, s1,  c1, 0, s1,  u1, 0 };
        float tr[8] = { c1, 1, s1,  c1, 0, s1,  u1, 1 };
        float tl[8] = { c0, 1, s0,  c0, 0, s0,  u0, 1 };

        pushVert(bl); pushVert(br); pushVert(tr);
        pushVert(tr); pushVert(tl); pushVert(bl);
    }

    // -------- TOP CAP (normal pointing +Y) --------
    for (int i = 0; i < segments; ++i) {
        float a0 = (float)i       / segments * 2.0f * (float)M_PI;
        float a1 = (float)(i + 1) / segments * 2.0f * (float)M_PI;
        float c0 = cosf(a0), s0 = sinf(a0);
        float c1 = cosf(a1), s1 = sinf(a1);

        // Center vertex of the top disc
        float center[8]  = { 0, 1, 0,   0, 1, 0,   0.5f, 0.5f };
        // Edge vertex 1
        float edge0[8]   = { c0, 1, s0,  0, 1, 0,   0.5f + c0 * 0.5f, 0.5f + s0 * 0.5f };
        // Edge vertex 2
        float edge1[8]   = { c1, 1, s1,  0, 1, 0,   0.5f + c1 * 0.5f, 0.5f + s1 * 0.5f };

        // Wind counter-clockwise when viewed from above (so normal faces up)
        pushVert(center); pushVert(edge0); pushVert(edge1);
    }

    // -------- BOTTOM CAP (normal pointing -Y) --------
    for (int i = 0; i < segments; ++i) {
        float a0 = (float)i       / segments * 2.0f * (float)M_PI;
        float a1 = (float)(i + 1) / segments * 2.0f * (float)M_PI;
        float c0 = cosf(a0), s0 = sinf(a0);
        float c1 = cosf(a1), s1 = sinf(a1);

        float center[8] = { 0, 0, 0,    0, -1, 0,   0.5f, 0.5f };
        float edge0[8]  = { c0, 0, s0,  0, -1, 0,   0.5f + c0 * 0.5f, 0.5f + s0 * 0.5f };
        float edge1[8]  = { c1, 0, s1,  0, -1, 0,   0.5f + c1 * 0.5f, 0.5f + s1 * 0.5f };

        // Wind opposite direction so normal faces down
        pushVert(center); pushVert(edge1); pushVert(edge0);
    }

    return data;
}