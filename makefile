CXX      = g++
CXXFLAGS = -g -Wall
LDFLAGS  = -lglfw3 -lGLEW -pthread -ldl -lGL

SOURCES = main.cpp shader.cpp SkyBox.cpp Terrain.cpp
TARGET  = main2

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) $(SOURCES) $(LDFLAGS) -o $(TARGET)

clean:
	rm -f $(TARGET)