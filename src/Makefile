.PHONY: run

main.exec: main.o
	g++ $< -o $@ -lGL -lGLU -lglfw3 -lX11 -lXxf86vm -lXrandr -lpthread -lXi -ldl -lXinerama -lXcursor

%.o: %.cpp
	g++ -c -g $<

run: main.exec
	valgrind --log-file=cobalt.rpt ./main.exec