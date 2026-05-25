#ifndef HOLE18_H
#define HOLE18_H
#include "Hole.h"
#include "Terrain.h"
#include "RenderObject.h"
#include "TextureLoader.h"

class Hole18 : public Hole {
    public:
        Hole18(int number, const glm::vec3& tee, const glm::vec3& pin) : Hole(number, tee, pin) {}
        void design(Terrain& terrain) override;
        ~Hole18() = default;
};
#endif // HOLE18_H