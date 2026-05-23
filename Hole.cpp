#include "Hole.h"
#include "Terrain.h"

Hole::Hole(int number, int par_, const glm::vec3& tee, const glm::vec3& pin): holeNumber(number), par(par_), teePosition(tee), pinPosition(pin){
}

Hole::~Hole() {
    for (auto* obj : objects) {
        delete obj;
    }
}

Hole::Hole(Hole&& other) noexcept: holeNumber(other.holeNumber), par(other.par), teePosition(other.teePosition), pinPosition(other.pinPosition), objects(std::move(other.objects)), bunkerIndices(std::move(other.bunkerIndices)){
}

Hole& Hole::operator=(Hole&& other) noexcept {
    if (this != &other) {
        for (auto* obj : objects) delete obj;
        holeNumber    = other.holeNumber;
        par           = other.par;
        teePosition   = other.teePosition;
        pinPosition   = other.pinPosition;
        objects       = std::move(other.objects);
        bunkerIndices = std::move(other.bunkerIndices);
    }
    return *this;
}

void Hole::design(Terrain& terrain) {
    
}

void Hole::render(const float* view,const float* projection,const glm::vec3& cameraPos,const DirectionalLight& light){
    for (auto* obj : objects) {
        obj->render(view, projection, cameraPos, light );  
    }
    (void)view; (void)projection; (void)cameraPos; (void)light;  // suppress warnings
}

void Hole::addObject(RenderObject* obj) {
    objects.push_back(obj);
}

void Hole::linkBunker(int idx) {
    bunkerIndices.push_back(idx);
}