#include "Hole.h"

class Hole1 : public Hole {
public:
    void init() override;
    void draw(GLuint shader) const override;
    bool checkCollision(glm::vec3 pos, float radius) const override;
    float getHeightAt(float x, float z) const override;
private:
    float sampleHeightMap(float x, float z); //used inside getHeightAt()
};