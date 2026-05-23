#include "TextureLoader.h"

#include <cstdio>
#include <cstdlib>
#include <iostream>

//load a BMP into an RGB byte buffer.
static unsigned char* loadBMP(const char* path, int& width, int& height) {
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

GLuint loadBMPTexture(const char* path) {
    int w, h;
    unsigned char* data = loadBMP(path, w, h);
    if (!data) return 0;

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
    std::cout << "Texture " << path << " loaded\n";
    return tex;
}