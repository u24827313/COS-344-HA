#ifndef HOLE11_H
#define HOLE11_H
#include "Hole.h"
#include "Terrain.h"
#include "RenderObject.h"
#include "TextureLoader.h"

class Hole11 : public Hole {
    public:
        Hole11(int number, const glm::vec3& tee, const glm::vec3& pin) : Hole(number, tee, pin) {}
        void design(Terrain& terrain) override;
        ~Hole11() = default;
    private:

    
};
#endif // HOLE11_H