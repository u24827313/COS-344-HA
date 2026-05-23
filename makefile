CXX      = g++
CXXFLAGS = -g -Wall
LDFLAGS  = -lglfw -lGLEW -pthread -ldl -lGL

SOURCES = main.cpp shader.cpp SkyBox.cpp Terrain.cpp Drone.cpp
TARGET  = main2

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) $(SOURCES) $(LDFLAGS) -o $(TARGET)

clean:
	rm -f $(TARGET)