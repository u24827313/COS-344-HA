files = shader.cpp SkyBox.cpp

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