#include "GolfCourse.h"

GolfCourse::GolfCourse(const std::string& grassPath,
                       const std::string& sandPath,
                       const std::string& rockPath,
                       const std::string& concretePath,
                       const std::string& woodPath)
    : terrain(grassPath, sandPath, rockPath, concretePath, woodPath),
      currentHole(0)
{
}

void GolfCourse::addHole(std::unique_ptr<Hole> hole) {
    hole->design(terrain);          // each hole contributes its mounds/bunkers
    holes.push_back(std::move(hole));
}

void GolfCourse::build() {
    terrain.build();                
}

void GolfCourse::render(const float* view,
                        const float* projection,
                        const glm::vec3& cameraPos,
                        const DirectionalLight& light)
{
    terrain.render(view, projection, cameraPos, light);
    for (auto& hole : holes) {
        hole->render(view, projection, cameraPos, light);
    }
}