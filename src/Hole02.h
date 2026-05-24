#ifndef HOLE02_H
#define HOLE02_H
#include "Hole.h"
#include "Terrain.h"
#include <glm/glm.hpp>

class Hole02 : public Hole {
    public:
        Hole02(int number, const glm::vec3& tee, const glm::vec3& pin) : Hole(number, tee, pin) {}
        void design(Terrain& terrain) override;
        ~Hole02() = default;
    private:

    
};
#endif // HOLE02_H
