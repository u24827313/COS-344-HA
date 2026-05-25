#ifndef HOLE07_H
#define HOLE07_H
#include "Hole.h"

class Hole07 : public Hole {
public:
    Hole07(int number, const glm::vec3& tee, const glm::vec3& pin) 
        : Hole(number, tee, pin) {}
    void design(Terrain& terrain) override;
};
#endif