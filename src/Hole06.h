#ifndef HOLE06_H
#define HOLE06_H
#include "Hole.h"
#include "Terrain.h"
#include <glm/glm.hpp>

class Hole06 : public Hole {
    public:
        Hole06(int number, const glm::vec3& tee, const glm::vec3& pin) : Hole(number, tee, pin) {}
        void design(Terrain& terrain) override;
        ~Hole06() = default;
    private:

    
};
#endif // HOLE06_H