#ifndef HOLE15_H
#define HOLE15_H
#include "Hole.h"
#include "Terrain.h"
#include "RenderObject.h"
#include "TextureLoader.h"

class Hole15 : public Hole {
    public:
        Hole15(int number, const glm::vec3& tee, const glm::vec3& pin) : Hole(number, tee, pin) {}
        void design(Terrain& terrain) override;
        ~Hole15() = default;
};
#endif 