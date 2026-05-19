#ifndef SKYBOX_H
#define SKYBOX_H

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>

class SkyBox{
    private:
        GLuint textureID;
        GLuint VAO, VBO;
        GLuint shaderProgram;
        bool bufferInitialised;

        void initBuffer();
        void loadCubeMap(const std::vector<std::string>& faces);
        unsigned char* loadBMP(const char* path, int& width, int& height);

    public:
        SkyBox(const std::vector<std::string>& facePaths);
        ~SkyBox();
        void render(const float* view4x4, const float* proj4x4);
};

#endif