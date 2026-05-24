#ifndef HOLE01_H
#define HOLE01_H

#include <glm/glm.hpp>
#include "Hole.h"

class Hole01 : public Hole {
    public:
    Hole01(int number, const glm::vec3& tee, const glm::vec3& pin) : Hole::Hole(number, tee, pin) {};
    ~Hole01() = default;
    virtual void design(Terrain& terrain);

    private:


};

#endif