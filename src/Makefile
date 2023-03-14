.PHONY: run

main.exec: main.o
	g++ $< -o $@ -lGL -lGLU -lglfw3 -lX11 -lXxf86vm -lXrandr -lpthread -lXi -ldl -lXinerama -lXcursor

main.o: main.cpp grid.cpp line.cpp puyo.cpp
	g++ -c -g main.cpp

run: main.exec
	valgrind --log-file=cobalt.rpt ./main.exec