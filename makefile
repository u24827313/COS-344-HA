CXX      = g++
CXXFLAGS = -g -Wall
LDFLAGS  = -lglfw3 -lGLEW -pthread -ldl -lGL

SOURCES = main.cpp src/shader.cpp src/SkyBox.cpp src/Terrain.cpp src/Hole.cpp src/GolfCourse.cpp src/RenderObject.cpp src/TextureLoader.cpp src/drone.cpp src/Hole01.cpp src/Hole02.cpp src/Hole03.cpp src/Hole04.cpp src/Hole07.cpp src/Hole08.cpp src/Hole09.cpp src/Hole10.cpp src/Hole11.cpp src/Hole12.cpp src/Hole13.cpp src/Hole14.cpp src/Hole15.cpp src/Hole16.cpp src/Hole17.cpp src/Hole18.cpp src/ObjectBuilder.cpp
TARGET  = main

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) $(SOURCES) $(LDFLAGS) -o $(TARGET)

clean:
	rm -f $(TARGET)

all:
	make clean
	make
	./main