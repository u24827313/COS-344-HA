#ifndef HOLE_H
#define HOLE_H

#include <vector>
#include <glm/glm.hpp>
#include "RenderObject.h"
#include "Light.h"

class Terrain;  
class Hole {
    private:
        int holeNumber;
        glm::vec3 teePosition;
        glm::vec3 pinPosition;

        
        std::vector<RenderObject*> objects;
        std::vector<int> bunkerIndices;

    public:
        Hole(int number, const glm::vec3& tee, const glm::vec3& pin);
        ~Hole();

        Hole(const Hole&) = delete;
        Hole& operator=(const Hole&) = delete;

        // Allow moving so std::vector<Hole> works.
        Hole(Hole&& other) noexcept;
        Hole& operator=(Hole&& other) noexcept;

        virtual void design(Terrain& terrain);

        void render(const float* view, const float* projection, const glm::vec3& cameraPos, const DirectionalLight& light);

        void addObject(RenderObject* obj);

        // Remember a bunker index returned from the Terrain.
        void linkBunker(int terrainBunkerIndex);

        
        int       getNumber() const{ 
            return holeNumber; 
        }
        glm::vec3 getTee() const{ 
            return teePosition; 
        }
        glm::vec3 getPin() const{ 
            return pinPosition; 
        }
        const std::vector<int>& getBunkerIndices() const { return bunkerIndices; }

        void setShader (GLuint shaderProgram);
};

#endif