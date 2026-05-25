#ifndef HOLE10_H
#define HOLE10_H
#include "Hole.h"
#include "Terrain.h"
#include "RenderObject.h"
#include "TextureLoader.h"

class Hole10 : public Hole {
    public:
        Hole10(int number, const glm::vec3& tee, const glm::vec3& pin) : Hole(number, tee, pin) {}
        void design(Terrain& terrain) override;
        ~Hole10() = default;
    private:

    
};
#endif // HOLE10_H