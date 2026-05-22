#include "Terrain.h"
#include "shader.hpp"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <cmath>

static const int MAX_BUNKERS = 32;

Terrain::Terrain(const std::string& grassPath, const std::string& sandPath, const std::string& rockPath, 
    const std::string& concretePath, const std::string& woodPath, int   gridSize, float worldSize){
        VAO = 0;
        VBO = 0;
        shaderProgram = 0;
        grassTexture = sandTexture = rockTexture = 0;
        concreteTexture = woodTexture = 0;
        bufferInitialised = false;
        this->gridSize = gridSize;
        this->worldSize = worldSize;
        vertexCount = 0;

        shaderProgram = LoadShaders("assets/terrain/terrain_vertex.glsl", "assets/terrain/terrain_fragment.glsl");
        loadTextures(grassPath, sandPath, rockPath, concretePath, woodPath);
}

Terrain::~Terrain(){
    clearBunkers();
    if(bufferInitialised){
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
    }
    glDeleteTextures(1, &grassTexture);
    glDeleteTextures(1, &sandTexture);
    glDeleteTextures(1, &rockTexture);
    glDeleteTextures(1, &concreteTexture);
    glDeleteTextures(1, &woodTexture);
    glDeleteProgram(shaderProgram);
}

void Terrain::addMound(float centerX, float centerZ, float radius, float height) {
    Mound m{ centerX, centerZ, radius, height };
    mounds.push_back(m);
}

void Terrain::addRaisedRect(float minX, float minZ, float maxX, float maxZ, float height) {
    RaisedRect r{ minX, minZ, maxX, maxZ, height };
    raisedRects.push_back(r);
}

void Terrain::build() {
    generateMesh(nullptr, 0, 0);
    bufferInitialised = true;
}

float Terrain::sampleDesignedHeight(float worldX, float worldZ) const{
    float y = 0.0f;
    for(const auto& m: mounds){
        float dx = worldX - m.cx;
        float dz = worldZ - m.cz;
        float d = sqrtf(dx * dx + dz * dz);

        if (d < m.radius){
            float t = d / m.radius;
            float falloff = 0.5f * (cosf(t * 3.14159265f) + 1.0f);
            y += m.height * falloff;
        }
    }

    // raise rectanges
    for (const auto& r : raisedRects) {
        if (worldX >= r.minX - 2.0f && worldX <= r.maxX + 2.0f &&
            worldZ >= r.minZ - 2.0f && worldZ <= r.maxZ + 2.0f) {
            // Smooth falloff at the edges
            float fadeX = 1.0f;
            float fadeZ = 1.0f;
            if (worldX < r.minX) fadeX = (worldX - (r.minX - 2.0f)) / 2.0f;
            if (worldX > r.maxX) fadeX = ((r.maxX + 2.0f) - worldX) / 2.0f;
            if (worldZ < r.minZ) fadeZ = (worldZ - (r.minZ - 2.0f)) / 2.0f;
            if (worldZ > r.maxZ) fadeZ = ((r.maxZ + 2.0f) - worldZ) / 2.0f;
            fadeX = std::max(0.0f, std::min(1.0f, fadeX));
            fadeZ = std::max(0.0f, std::min(1.0f, fadeZ));
            y += r.height * fadeX * fadeZ;
        }
    }
    return y;
}

void Terrain::addBunker(float cX, float cZ, float r, float edgefade){
    if((int) bunkers.size() >= MAX_BUNKERS){
        std::cerr << "[TERRAIN] Reached bunker limit (" << MAX_BUNKERS << " bunkers)\n";
        return;
    }
    Bunker b;
    b.cx = cX;
    b.cz = cZ;
    b.radius = r;
    b.edgeFade = edgefade;
    bunkers.push_back(b);
}
void Terrain::clearBunkers(){
    bunkers.clear();
}
unsigned char* Terrain::loadBMP(const char* path, int& width, int& height)
{
    FILE* f = fopen(path, "rb");
    if (!f) {
        std::cerr << "[Terrain] Cannot open: " << path << "\n";
        return nullptr;
    }

    unsigned char header[54];
    if (fread(header, 1, 54, f) != 54 || header[0] != 'B' || header[1] != 'M') {
        std::cerr << "[Terrain] Not a valid BMP: " << path << "\n";
        fclose(f);
        return nullptr;
    }

    int dataOffset = *(int*)&header[10];
    width  = *(int*)&header[18];
    height = *(int*)&header[22];
    short bpp = *(short*)&header[28];

    // Only support 24-bit and 32-bit BMP
    if (bpp != 24 && bpp != 32) {
        std::cerr << "[Terrain] Only 24-bit or 32-bit BMP supported:  " << path << "(" << bpp << " bpp)\n";
        fclose(f);
        return nullptr;
    }

    int bytesPerPixel = bpp / 8;

    // Row size is padded to 4-byte boundary
    int rowStride = (width * bytesPerPixel + 3) & ~3;
    int rawSize   = rowStride * height;

    unsigned char* raw = (unsigned char*)malloc(rawSize);
    if (!raw) {
        fprintf(stderr, "[Terrain] Out of memory\n");
        fclose(f);
        return nullptr;
    }

    fseek(f, dataOffset, SEEK_SET);
    if(fread(raw, 1, rawSize, f) != (size_t)rawSize){
        std::cerr << "[Terrain] Failed reading BMP data: " << path << "\n";

    free(raw);
    fclose(f);
    return nullptr;
    }
    fclose(f);

    // Output RGB buffer (always 3 bytes per pixel)
    unsigned char* pixels =
        (unsigned char*)malloc(width * height * 3);

    if (!pixels) {
        fprintf(stderr, "[Terrain] Out of memory\n");
        free(raw);
        return nullptr;
    }

    // Convert BGR(A) + flip vertically
    for (int row = 0; row < height; ++row) {

        unsigned char* src =
            raw + (height - 1 - row) * rowStride;

        unsigned char* dst =
            pixels + row * width * 3;

        for (int col = 0; col < width; ++col) {

            unsigned char* pixel =
                src + col * bytesPerPixel;

            // BMP is BGR or BGRA -> convert to RGB
            dst[col * 3 + 0] = pixel[2]; // R
            dst[col * 3 + 1] = pixel[1]; // G
            dst[col * 3 + 2] = pixel[0]; // B
        }
    }

    free(raw);
    return pixels;
}

GLuint Terrain::loadBMPTexture(const char* path){
    int w, h;
    unsigned char* data = loadBMP(path, w, h);

    if (!data){
        return 0;
    }

    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    free(data);
    return tex;
}


void Terrain::loadTextures(const std::string& grassPath, const std::string& sandPath, const std::string& rockPath, const std::string& concretePath, const std::string& woodPath){
    grassTexture = loadBMPTexture(grassPath.c_str());
    sandTexture = loadBMPTexture(sandPath.c_str());
    rockTexture = loadBMPTexture(rockPath.c_str());
    woodTexture = loadBMPTexture(woodPath.c_str());
    concreteTexture = loadBMPTexture(concretePath.c_str());
}

void Terrain::generateMesh(const unsigned char* heightData, int hmW, int hmH)
{
    
    const int   N = gridSize;
    const float step = worldSize / (N - 1);
    const float halfWorld = worldSize * 0.5f;


    heights.assign(N * N, 0.0f);
    normals.assign(N * N, glm::vec3(0.0f, 1.0f, 0.0f));

    // Build the unique vertex grid  for normal calculation)
    std::vector<glm::vec3> positions(N * N);
    for (int z = 0; z < N; ++z) {
        for (int x = 0; x < N; ++x) {
            float worldX = -halfWorld + x * step;
            float worldZ = -halfWorld + z * step;

            float y = sampleDesignedHeight(worldX, worldZ);

            int idx = z * N + x;
            positions[idx] = glm::vec3(worldX, y, worldZ);
            heights[idx] = y;
        }
    }

    // Compute vertex normals from surrounding triangles
    std::fill(normals.begin(), normals.end(), glm::vec3(0.0f));
    for (int z = 0; z < N - 1; ++z) {
        for (int x = 0; x < N - 1; ++x) {
            int tl = z * N + x;
            int tr = z * N + (x + 1);
            int bl = (z + 1) * N + x;
            int br = (z + 1) * N + (x + 1);

            glm::vec3 n1 = glm::normalize(glm::cross( positions[bl] - positions[tl], positions[tr] - positions[tl]));
            glm::vec3 n2 = glm::normalize(glm::cross( positions[bl] - positions[tr], positions[br] - positions[tr]));

            normals[tl] += n1;
            normals[bl] += n1;
            normals[tr] += n1;

            normals[tr] += n2;
            normals[bl] += n2;
            normals[br] += n2;
        }
    }
    for (auto& n : normals) n = glm::normalize(n);

    // 6 vertices per cell, no sharing.
    // Each vertex is position(3) + normal(3) + uv(2) = 8 floats.
    std::vector<float> vertexData;
    vertexData.reserve((N - 1) * (N - 1) * 6 * 8);

    auto pushVertex = [&](int x, int z) {
        int idx = z * N + x;
        const glm::vec3& p = positions[idx];
        const glm::vec3& n = normals[idx];
        float u = (float)x / (N - 1);
        float v = (float)z / (N - 1);
        vertexData.push_back(p.x);
        vertexData.push_back(p.y);
        vertexData.push_back(p.z);
        vertexData.push_back(n.x);
        vertexData.push_back(n.y);
        vertexData.push_back(n.z);
        vertexData.push_back(u);
        vertexData.push_back(v);
    };

    for (int z = 0; z < N - 1; ++z) {
        for (int x = 0; x < N - 1; ++x) {
            // Triangle 1: top-left, bottom-left, top-right
            pushVertex(x,     z);
            pushVertex(x,     z + 1);
            pushVertex(x + 1, z);
            // Triangle 2: top-right, bottom-left, bottom-right
            pushVertex(x + 1, z);
            pushVertex(x,     z + 1);
            pushVertex(x + 1, z + 1);
        }
    }

    vertexCount = (int)(vertexData.size() / 8);


    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER,vertexData.size() * sizeof(float),vertexData.data(),GL_STATIC_DRAW);

    const GLsizei stride = 8 * sizeof(float);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));

    glBindVertexArray(0);
}


void Terrain::render(const float* view4x4, const float* proj4x4, const glm::vec3& cameraPos, const DirectionalLight& light){
    if (!bufferInitialised) return;

    glUseProgram(shaderProgram);

    // Matrices
    glm::mat4 model(1.0f);  
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"),      1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"),       1, GL_FALSE, view4x4);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, proj4x4);

    // Lighting
    glUniform3fv(glGetUniformLocation(shaderProgram, "lightDirection"), 1, glm::value_ptr(light.direction));
    glUniform3fv(glGetUniformLocation(shaderProgram, "lightColour"), 1, glm::value_ptr(light.colour));
    glUniform1f (glGetUniformLocation(shaderProgram, "lightAmbient"), light.ambient);
    glUniform3fv(glGetUniformLocation(shaderProgram, "viewPos"), 1, glm::value_ptr(cameraPos)); 
    glUniform1f(glGetUniformLocation(shaderProgram, "tileFactor"), 50.0f);

    // Assign textures to texture units
    glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, grassTexture);
    glActiveTexture(GL_TEXTURE1); glBindTexture(GL_TEXTURE_2D, sandTexture);
    glActiveTexture(GL_TEXTURE2); glBindTexture(GL_TEXTURE_2D, rockTexture);
    glActiveTexture(GL_TEXTURE3); glBindTexture(GL_TEXTURE_2D, concreteTexture);
    glActiveTexture(GL_TEXTURE4); glBindTexture(GL_TEXTURE_2D, woodTexture);
    

    glUniform1i(glGetUniformLocation(shaderProgram, "grassTex"),    0);
    glUniform1i(glGetUniformLocation(shaderProgram, "sandTex"),     1);
    glUniform1i(glGetUniformLocation(shaderProgram, "rockTex"),     2);
    glUniform1i(glGetUniformLocation(shaderProgram, "concreteTex"), 3);
    glUniform1i(glGetUniformLocation(shaderProgram, "woodTex"),     4);
    

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    glBindVertexArray(0);

    
    glUniform1f(glGetUniformLocation(shaderProgram, "worldHalfSize"), worldSize * 0.5f);

    
    int n = (int)bunkers.size();
    if (n > 32) n = 32;
    glUniform1i(glGetUniformLocation(shaderProgram, "numBunkers"), n);


    std::vector<float> packed(32 * 4, 0.0f);
    for (int i = 0; i < n; ++i) {
        packed[i*4 + 0] = bunkers[i].cx;
        packed[i*4 + 1] = bunkers[i].cz;
        packed[i*4 + 2] = bunkers[i].radius;
        packed[i*4 + 3] = bunkers[i].edgeFade;
    }
    glUniform4fv(glGetUniformLocation(shaderProgram, "bunkers"), 32, packed.data());

    // reset texture bindingds
    for (int i = 0; i < 6; ++i) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    glActiveTexture(GL_TEXTURE0);
}


float Terrain::getHeightAt(float x, float z) const
{
    if (heights.empty()) return 0.0f;

    const int N = gridSize;
    const float halfWorld = worldSize * 0.5f;
    const float step = worldSize / (N - 1);

    // Convert world (x,z) -> grid coordinates (fractional)
    float gx = (x + halfWorld) / step;
    float gz = (z + halfWorld) / step;

    // Out of bounds -> clamp
    if (gx < 0) gx = 0;  
    if (gx > N - 1) gx = (float)(N - 1);
    if (gz < 0) gz = 0;  
    if (gz > N - 1) gz = (float)(N - 1);

    int x0 = (int)floorf(gx);
    int z0 = (int)floorf(gz);
    int x1 = (x0 + 1 < N) ? x0 + 1 : x0;
    int z1 = (z0 + 1 < N) ? z0 + 1 : z0;

    float fx = gx - x0;
    float fz = gz - z0;

    
    float h00 = heights[z0 * N + x0];
    float h10 = heights[z0 * N + x1];
    float h01 = heights[z1 * N + x0];
    float h11 = heights[z1 * N + x1];

    float h0 = h00 * (1 - fx) + h10 * fx;
    float h1 = h01 * (1 - fx) + h11 * fx;
    return h0 * (1 - fz) + h1 * fz;
}

glm::vec3 Terrain::getNormalAt(float x, float z) const{
    if (normals.empty()) return glm::vec3(0, 1, 0);

    const int N = gridSize;
    const float halfWorld = worldSize * 0.5f;
    const float step = worldSize / (N - 1);

    float gx = (x + halfWorld) / step;
    float gz = (z + halfWorld) / step;

    if (gx < 0) gx = 0;  
    if (gx > N - 1) gx = (float)(N - 1);
    if (gz < 0) gz = 0;  
    if (gz > N - 1) gz = (float)(N - 1);

    int x0 = (int)floorf(gx);
    int z0 = (int)floorf(gz);
    int x1 = (x0 + 1 < N) ? x0 + 1 : x0;
    int z1 = (z0 + 1 < N) ? z0 + 1 : z0;

    float fx = gx - x0;
    float fz = gz - z0;

    glm::vec3 n00 = normals[z0 * N + x0];
    glm::vec3 n10 = normals[z0 * N + x1];
    glm::vec3 n01 = normals[z1 * N + x0];
    glm::vec3 n11 = normals[z1 * N + x1];

    glm::vec3 n0 = glm::mix(n00, n10, fx);
    glm::vec3 n1 = glm::mix(n01, n11, fx);
    return glm::normalize(glm::mix(n0, n1, fz));
}