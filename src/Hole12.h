#ifndef HOLE12_H
#define HOLE12_H
#include "Hole.h"
#include "Terrain.h"
#include "RenderObject.h"
#include "TextureLoader.h"

class Hole12 : public Hole {
    public:
        Hole12(int number, const glm::vec3& tee, const glm::vec3& pin) : Hole(number, tee, pin) {}
        void design(Terrain& terrain) override;
        ~Hole12() = default;
    private:

    
};
#endif // HOLE12_H