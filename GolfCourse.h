#ifndef GOLFCOURSE_H
#define GOLFCOURSE_H

#include <vector>
#include <memory>
#include "Terrain.h"
#include "Hole.h"
#include "Light.h"

class GolfCourse {
    private:
        Terrain terrain;
        std::vector<std::unique_ptr<Hole>> holes;
        int currentHole;
    public:
        
        // Pass terrain texture paths through.
        GolfCourse(const std::string& grassPath,
                   const std::string& sandPath,
                   const std::string& rockPath,
                   const std::string& concretePath,
                   const std::string& woodPath);

        // Add a hole; the course takes ownership.
        // The hole's design() is called immediately, applying its features to the terrain.
        void addHole(std::unique_ptr<Hole> hole);

        // Call once after all holes are added.
        void build();

        void render(const float* view,
                    const float* projection,
                    const glm::vec3& cameraPos,
                    const DirectionalLight& light);

        // Accessors
        Terrain&       getTerrain()         { return terrain; }
        const Terrain& getTerrain() const   { return terrain; }
        int            getHoleCount() const { return (int)holes.size(); }
        Hole&          getHole(int i)       { return *holes[i]; }
        const Hole&    getHole(int i) const { return *holes[i]; }

        int  getCurrentHoleIndex() const  { return currentHole; }
        void setCurrentHole(int i)        { currentHole = i; }
        Hole& getCurrentHole()            { return *holes[currentHole]; }
};

#endif