#ifndef HOLE09_H
#define HOLE09_H
#include "Hole.h"

class Hole09 : public Hole {
public:
    Hole09(int number, const glm::vec3& tee, const glm::vec3& pin) 
        : Hole(number, tee, pin) {}
    void design(Terrain& terrain) override;
};
#endif