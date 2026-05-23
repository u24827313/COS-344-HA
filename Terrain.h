#ifndef TERRAIN_H
#define TERRAIN_H

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include "Light.h"

struct BunkerRegion {
    glm::vec2 center;
    float innerRadius;
    float outerRadius;
};

struct Bunker{
    float cx;
    float cz;
    float radius;
    float edgeFade;
};

class Terrain {
    private:
        GLuint VAO, VBO;
        GLuint shaderProgram;

        // Texture handles
        GLuint grassTexture;
        GLuint sandTexture;
        GLuint rockTexture;
        GLuint concreteTexture;
        GLuint woodTexture;
        

        bool bufferInitialised;

       
        int   gridSize;        
        float worldSize;       
        int   vertexCount;      

        struct Mound      { float cx, cz, radius, height; };
        struct RaisedRect { float minX, minZ, maxX, maxZ, height; };
        std::vector<Mound>      mounds;
        std::vector<RaisedRect> raisedRects;
       
        
        std::vector<float> heights;       
        std::vector<glm::vec3> normals;   

        std::vector<Bunker> bunkers;
        
        void loadTextures(const std::string& grassPath, const std::string& sandPath, const std::string& rockPath, const std::string& concretePath, const std::string& woodPath);
        GLuint loadBMPTexture(const char* path);
        unsigned char* loadBMP(const char* path, int& width, int& height);

        void generateMesh(const unsigned char* heightData, int hmW, int hmH);
        float sampleDesignedHeight(float worldX, float worldZ) const;


    public:
        Terrain(const std::string& grassPath, const std::string& sandPath, const std::string& rockPath, const std::string& concretePath, 
            const std::string& woodPath, int gridSize  = 200, float worldSize = 500.0f);

        ~Terrain();

        void addMound(float centerX, float centerZ, float radius, float height);
        void addRaisedRect(float minX, float minZ, float maxX, float maxZ, float height);
        void addBunker(float cX, float cZ, float r, float edgefade = 3.0f);
        void clearBunkers();
        void build();
        void render(const float* view4x4, const float* proj4x4, const glm::vec3& cameraPos, const DirectionalLight& light);
        float     getHeightAt(float x, float z) const;
        glm::vec3 getNormalAt(float x, float z) const;
};

#endif