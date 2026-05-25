#ifndef HOLE08_H
#define HOLE08_H
#include "Hole.h"

class Hole08 : public Hole {
public:
    Hole08(int number, const glm::vec3& tee, const glm::vec3& pin) 
        : Hole(number, tee, pin) {}
    void design(Terrain& terrain) override;
};
#endif