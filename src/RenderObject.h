#ifndef RENDEROBJECT_H
#define RENDEROBJECT_H

#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include "Light.h"

class RenderObject {
    private:
        GLuint VAO, VBO;
        int    vertexCount;

        GLuint texture;       
        GLuint shaderProgram; 

        glm::vec3 position;
        glm::vec3 rotationEuler;
        glm::vec3 scale;

        bool bufferInitialised;

        void initBuffer(const std::vector<float>& vertexData);
        glm::mat4 computeModelMatrix() const;

    public:
        
        RenderObject(const std::vector<float>& vertexData, GLuint texture, GLuint shaderProgram);
        ~RenderObject();

        RenderObject(const RenderObject&) = delete;
        RenderObject& operator=(const RenderObject&) = delete;

        // Transform setters
        void setPosition(const glm::vec3& p) { position = p; }
        void setRotation(const glm::vec3& rDegrees) { rotationEuler = rDegrees; }
        void setScale(const glm::vec3& s) { scale = s; }
        void setScale(float uniform) { scale = glm::vec3(uniform); }

        glm::vec3 getPosition() const { return position; }
        glm::vec3 getRotation() const { return rotationEuler; }
        glm::vec3 getScale() const    { return scale; }

        // Texture/shader swappers (rarely needed but useful)
        void setTexture(GLuint t)        { texture = t; }
        void setShaderProgram(GLuint s)  { shaderProgram = s; }

        void render(const float* view, const float* projection, const glm::vec3& cameraPos, const DirectionalLight& light);

        // ----- Factory methods for common shapes -----
        static std::vector<float> createQuad();

        static std::vector<float> createSphere(int parallels = 16, int meridians = 16);
        static std::vector<float> createCylinder(int segments = 16);
        static std::vector<float> createBox(float width = 1.0f, float height = 1.0f, float depth = 1.0f);
        static std::vector<float> createRoundedBox(float width = 1.0f, float height = 1.0f, float depth = 1.0f, float radius = 0.1f, int segments = 8);
        static std::vector<float> createCone(int segments = 16);
        static std::vector<float> createTrapezoid(float bottomWidth, float topWidth, float height, float thickness);
        static std::vector<float> createClosedCylinder(int segments) ;
};

#endif