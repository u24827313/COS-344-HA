CXX      = g++
CXXFLAGS = -g -Wall
LDFLAGS  = -lglfw3 -lGLEW -pthread -ldl -lGL

SOURCES = main.cpp src/shader.cpp src/SkyBox.cpp src/Terrain.cpp src/Hole.cpp src/GolfCourse.cpp src/RenderObject.cpp src/TextureLoader.cpp
TARGET  = main

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) $(SOURCES) $(LDFLAGS) -o $(TARGET)

clean:
	rm -f $(TARGET)

all:
	make clean
	make
	./main