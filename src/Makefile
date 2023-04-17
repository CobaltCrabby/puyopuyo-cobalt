.PHONY: run
.PHONY: clean

main.exec: $(patsubst %.cpp, %.o, $(wildcard *.cpp))
	g++ $^ -o $@ -lGL -lGLU -lglfw3 -lX11 -lXxf86vm -lXrandr -lpthread -lXi -ldl -lXinerama -lXcursor

%.o: %.cpp *.h
	g++ -c -g $<

main.o: main.cpp grid.h
	g++ -c -g $<

run: main.exec
	valgrind --log-file=cobalt.rpt --track-origins=yes ./main.exec

clean:
	rm -f main.exec *.o