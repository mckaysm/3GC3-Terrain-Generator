all: Terrain
	
Terrain:  Terrain.cpp math2D.cpp
	g++ -o Terrain.x Terrain.cpp math2D.cpp -g -Wall -std=c++11 -lGL -lGLU -lglut

clean:
	rm Terrain.x 