CXX      = g++
CXXFLAGS = -g -Wall
LDFLAGS  = -lglfw3 -lGLEW -pthread -ldl -lGL

SOURCES = main.cpp shader.cpp SkyBox.cpp Terrain.cpp Hole.cpp GolfCourse.cpp RenderObject.cpp TextureLoader.cpp
TARGET  = main3

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) $(SOURCES) $(LDFLAGS) -o $(TARGET)

clean:
	rm -f $(TARGET)