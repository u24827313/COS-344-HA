#ifndef OBSTACLEBUILDER_H
#define OBSTACLEBUILDER_H

#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include "RenderObject.h"
#include "Hole.h"

class ObjectBuilder {
    private:
        std::vector<RenderObject*> objects;
    public:
        ObjectBuilder();
        ~ObjectBuilder();

        void clone(ObjectBuilder* old);

        std::vector<RenderObject*> getObjects();
        void setObjects(std::vector<RenderObject*> objs);

        void setPosition(const glm::vec3& p);
        void setRotation(const glm::vec3& rDegrees);
        void setScale(const glm::vec3& s);
        void setScale(float uniform);

        void setTexture(GLuint t);
        void setShaderProgram(GLuint s);
        void render(const float* view, const float* projection, const glm::vec3& cameraPos, const DirectionalLight& light);
        void addObject(Hole* h);

        void makeBarn(GLuint texture_, GLuint shaderProgram_);
        void makeWallTower(GLuint texture_, GLuint shaderProgram_);
        void makeLoop(GLuint texture_, GLuint shaderProgram_);
        void makeBridge(GLuint texture_, GLuint shaderProgram_);
        void makeVolcano(GLuint texture_, GLuint shaderProgram_);
        void makeFlag(GLuint textureA_, GLuint textureB_, GLuint shaderProgram_);

        void makeMarker(GLuint texture_, GLuint shaderProgram_);
        void makePodium(GLuint texture_, GLuint shaderProgram_);
        void makeShade(GLuint texture_, GLuint shaderProgram_);
        void makeLamp(GLuint texture_, GLuint shaderProgram_);
        void makeSignpost(GLuint texture_, GLuint shaderProgram_);
};

#endif