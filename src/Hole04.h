#ifndef HOLE04_H
#define HOLE04_H
#include "Hole.h"
#include "Terrain.h"
#include <glm/glm.hpp>

class Hole04 : public Hole {
    public:
        Hole04(int number, const glm::vec3& tee, const glm::vec3& pin) : Hole(number, tee, pin) {}
        void design(Terrain& terrain) override;
        ~Hole04() = default;
    private:

    
};
#endif // HOLE04_H