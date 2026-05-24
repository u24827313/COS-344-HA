#ifndef HOLE13_H
#define HOLE13_H

#include "Hole.h"
#include "Terrain.h"
#include "RenderObject.h"
#include "TextureLoader.h"

class Hole13 : public Hole {
    public:
        Hole13(int number, const glm::vec3& tee, const glm::vec3& pin) : Hole(number, tee, pin) {}
        void design(Terrain& terrain) override;
        ~Hole13() = default;
    private:

    
};

#endif // HOLE13_H