<<<<<<< Updated upstream
files = shader.cpp SkyBox.cpp
=======
CXX      = g++
CXXFLAGS = -g -Wall
LDFLAGS  = -lglfw3 -lGLEW -pthread -ldl -lGL

SOURCES = main.cpp src/shader.cpp src/SkyBox.cpp src/Terrain.cpp src/Hole.cpp src/GolfCourse.cpp src/RenderObject.cpp src/TextureLoader.cpp src/drone.cpp src/Hole01.cpp src/Hole02.cpp src/Hole03.cpp src/Hole07.cpp src/Hole08.cpp src/Hole09.cpp src/ObjectBuilder.cpp
TARGET  = main

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) $(SOURCES) $(LDFLAGS) -o $(TARGET)
>>>>>>> Stashed changes

SRC = main.cpp 
main: main.cpp
	g++ -g main.cpp $(files) -lglfw3 -lGLEW -pthread -ldl -lGL -o main
clean:
	rm -f *.o main

run:
	./main

all:
	make clean
	make
	make run