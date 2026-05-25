#ifndef HOLE16_H
#define HOLE16_H
#include "Hole.h"
#include "Terrain.h"
#include "RenderObject.h"
#include "TextureLoader.h"

class Hole16 : public Hole {
    public:
        Hole16(int number, const glm::vec3& tee, const glm::vec3& pin) : Hole(number, tee, pin) {}
        void design(Terrain& terrain) override;
        ~Hole16() = default;
};
#endif 