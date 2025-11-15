DEFAULTFLAGS=-std=c++17 -O3
DEBUGFLAGS=-std=c++17 -pedantic -Wall -Wextra -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 -Winit-self -Wlogical-op -Wmissing-declarations -Wmissing-include-dirs -Wnoexcept -Woverloaded-virtual -Wredundant-decls -Wshadow -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=5 -Wswitch-default -Wundef -Wno-unused 

build:
	g++ ${DEFAULTFLAGS} -c src/utils.cpp -o utils.o
	g++ ${DEFAULTFLAGS} -c src/eikonal.cpp -o eikonal.o
	g++ ${DEFAULTFLAGS} -c src/main.cpp -o main.o
	g++ ${DEFAULTFLAGS} -o sim.exe utils.o eikonal.o main.o 
	mv sim.exe bin/
	mv *.o Modules/

buildrunplot:
	g++ ${DEFAULTFLAGS} -c src/utils.cpp -o utils.o
	g++ ${DEFAULTFLAGS} -c src/eikonal.cpp -o eikonal.o
	g++ ${DEFAULTFLAGS} -c src/main.cpp -o main.o
	g++ ${DEFAULTFLAGS} -o sim.exe main.o eikonal.o utils.o
	mv sim.exe bin/
	mv *.o  Modules/
	bin/sim.exe 2.0 8 1  #Domain size,maximum depth,number of bodies
	python3 ib_plotter.py
	#python3 quad_plotter.py

debug:
	g++ ${DEBUGFLAGS} -c src/utils.cpp -o utils.o
	g++ ${DEBUGFLAGS} -c src/eikonal.cpp -o eikonal.o
	g++ ${DEBUGFLAGS} -c src/main.cpp -o main.o
	g++ ${DEBUGFLAGS} -o sim.exe main.o eikonal.o utils.o
	mv sim.exe bin/
	mv *.o Modules/
