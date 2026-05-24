#ifndef HOLE03_H
#define HOLE03_H
#include "Hole.h"
#include "Terrain.h"
#include "RenderObject.h"
#include "TextureLoader.h"

class Hole03 : public Hole {
    public:
        Hole03(int number, const glm::vec3& tee, const glm::vec3& pin) : Hole(number, tee, pin) {}
        void design(Terrain& terrain) override;
        ~Hole03() = default;
    private:

    
};
#endif // HOLE03_H