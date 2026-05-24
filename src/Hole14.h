#ifndef HOLE14_H
#define HOLE14_H
#include "Hole.h"
#include "Terrain.h"
#include "RenderObject.h"
#include "TextureLoader.h"

class Hole14 : public Hole {
    public:
        Hole14(int number, const glm::vec3& tee, const glm::vec3& pin) : Hole(number, tee, pin) {}
        void design(Terrain& terrain) override;
        ~Hole14() = default;
    private:

};
#endif // HOLE14_H