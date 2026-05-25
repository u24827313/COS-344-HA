#ifndef HOLE17_H
#define HOLE17_H
#include "Hole.h"
#include "Terrain.h"
#include "RenderObject.h"
#include "TextureLoader.h"

class Hole17 : public Hole {
    public:
        Hole17(int number, const glm::vec3& tee, const glm::vec3& pin) : Hole(number, tee, pin) {}
        void design(Terrain& terrain) override;
        ~Hole17() = default;
};
#endif 