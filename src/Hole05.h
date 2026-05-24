#ifndef HOLE05_H
#define HOLE05_H
#include "Hole.h"
#include "Terrain.h"
#include <glm/glm.hpp>

class Hole05 : public Hole {
    public:
        Hole05(int number, const glm::vec3& tee, const glm::vec3& pin) : Hole(number, tee, pin) {}
        void design(Terrain& terrain) override;
        ~Hole05() = default;
    private:

    
};
#endif // HOLE05_H