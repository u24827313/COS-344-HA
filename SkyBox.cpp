#include "SkyBox.h"
#include "shader.hpp"


#include <cstdio>
#include <cstdlib>
#include <cstring>


static const float CUBE_VERTS[] = {
    // Back face  (-Z)
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,

    // Left face  (-X)
    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    // Right face (+X)
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
 
    // Front face (+Z)
    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,
 
    // Top face   (+Y)
    -1.0f,  1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,
 
    // Bottom face (-Y)
    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f,  1.0f
};


SkyBox::SkyBox(const std::vector<std::string>& faces){
    VAO = 0;
    VBO = 0;
    textureID = 0;
    bufferInitialised = false;

    shaderProgram = LoadShaders("assets/skybox/skybox_vertex.glsl", "assets/skybox/skybox_fragment.glsl");  // ADD THIS

    initBuffer();
    loadCubeMap(faces);
}

SkyBox::~SkyBox(){
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteTextures(1, &textureID);
    glDeleteProgram(shaderProgram);
}

void SkyBox::initBuffer(){
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
 
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(CUBE_VERTS), CUBE_VERTS, GL_STATIC_DRAW);
 
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
 
    glBindVertexArray(0);
    bufferInitialised = true;
}

unsigned char* SkyBox::loadBMP(const char* path, int& width, int& height)
{
    FILE* f = fopen(path, "rb");
    if (!f) {
        fprintf(stderr, "[Skybox] Cannot open: %s\n", path);
        return nullptr;
    }
 
    unsigned char header[54];
    if (fread(header, 1, 54, f) != 54 || header[0] != 'B' || header[1] != 'M') {
        fprintf(stderr, "[Skybox] Not a valid BMP: %s\n", path);
        fclose(f);
        return nullptr;
    }
 
    int   dataOffset = *(int*)  &header[10];
    width            = *(int*)  &header[18];
    height           = *(int*)  &header[22];
    short bpp        = *(short*)&header[28];
 
    if (bpp != 24) {
        fprintf(stderr, "[Skybox] Only 24-bit BMP supported: %s (%d bpp)\n", path, bpp);
        fclose(f);
        return nullptr;
    }
 
    // BMP row stride is padded to a multiple of 4 bytes
    int rowStride = (width * 3 + 3) & ~3;
    int rawSize   = rowStride * height;
 
    unsigned char* raw = (unsigned char*)malloc(rawSize);
    fseek(f, dataOffset, SEEK_SET);
    fread(raw, 1, rawSize, f);
    fclose(f);
 
    // BMP is stored BGR, bottom-row-first.  Convert to RGB, top-row-first.
    unsigned char* pixels = (unsigned char*)malloc(width * height * 3);
    for (int row = 0; row < height; ++row) {
        unsigned char* src = raw + (height - 1 - row) * rowStride;
        unsigned char* dst = pixels + row * width * 3;
        for (int col = 0; col < width; ++col) {
            dst[col * 3 + 0] = src[col * 3 + 2]; // R <- B
            dst[col * 3 + 1] = src[col * 3 + 1]; // G
            dst[col * 3 + 2] = src[col * 3 + 0]; // B <- R
        }
    }
    free(raw);
    return pixels;
}

void SkyBox::loadCubeMap(const std::vector<std::string>& faces)
{
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
 
    for (int i = 0; i < 6; ++i) {
        int w, h;
        unsigned char* data = loadBMP(faces[i].c_str(), w, h);
        if (data) {
            glTexImage2D(
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0, GL_RGB, w, h, 0,
                GL_RGB, GL_UNSIGNED_BYTE, data
            );
            free(data);
        } else {
            fprintf(stderr, "[Skybox] Skipping face %d: %s\n", i, faces[i].c_str());
        }
    }
 
    // Linear filtering and clamp on all 3 axes — prevents seams at cube edges
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
 
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void SkyBox::render(const float* view4x4, const float* proj4x4)
{
    if (!bufferInitialised) return;
 
    // Strip translation by zeroing column 3 of the view matrix.
    // Column-major layout:
    //   col0: [0..3]   col1: [4..7]   col2: [8..11]   col3: [12..15]
    float viewNoTrans[16] = {
        view4x4[0], view4x4[1], view4x4[2],  0.0f,  // col 0
        view4x4[4], view4x4[5], view4x4[6],  0.0f,  // col 1
        view4x4[8], view4x4[9], view4x4[10], 0.0f,  // col 2
        0.0f,       0.0f,       0.0f,         1.0f   // col 3
    };
 
    // Save depth state
    GLint    prevDepthFunc;
    GLboolean prevDepthMask;
    glGetIntegerv(GL_DEPTH_FUNC, &prevDepthFunc);
    glGetBooleanv(GL_DEPTH_WRITEMASK, &prevDepthMask);
 
    // Skybox fragments are written at depth 1.0 (via xyww in the vertex shader).
    // GL_LEQUAL lets them pass on pixels real geometry hasn't touched.
    // GL_FALSE on the depth mask means the skybox never writes into the depth
    // buffer, so it can never occlude geometry drawn after it.
    glDepthFunc(GL_LEQUAL);
    glDepthMask(GL_FALSE);
 
    glUseProgram(shaderProgram);
 
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"),       1, GL_FALSE, viewNoTrans);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, proj4x4);
    glUniform1i       (glGetUniformLocation(shaderProgram, "skybox"),     0);
 
    glBindVertexArray(VAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
 
    glDrawArrays(GL_TRIANGLES, 0, 36);
 
    // Restore depth state so subsequent draws are unaffected
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    glDepthMask(prevDepthMask);
    glDepthFunc(prevDepthFunc);
}
 