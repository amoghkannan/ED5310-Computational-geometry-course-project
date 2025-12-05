DEFAULTFLAGS=-std=c++17 -O3 -fopenmp -Lsrc/opengl -Isrc/opengl 
DEBUGFLAGS=-std=c++17 -pedantic -Wall -Wextra -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 -Winit-self -Wlogical-op -Wmissing-declarations -Wmissing-include-dirs -Wnoexcept -Woverloaded-virtual -Wredundant-decls -Wshadow -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=5 -Wswitch-default -Wundef -Wno-unused

n_bodies=4
domain_size=1.5
max_depth=9

build:
	g++ ${DEFAULTFLAGS} -c src/utils.cpp -o utils.o
	g++ ${DEFAULTFLAGS} -c src/eikonal.cpp -o eikonal.o
	g++ ${DEFAULTFLAGS} -c src/main.cpp -o main.o
	g++ ${DEFAULTFLAGS} -o sim.exe utils.o eikonal.o main.o 
	mv sim.exe bin/
	mv *.o Modules/

runplot:
	bin/sim.exe ${domain_size} ${max_depth} ${n_bodies}  #Domain size,maximum depth,number of bodies
	python3 ib_plotter.py ${n_bodies} #Number of bodies
	python3 quad_plotter.py ${n_bodies}
	rm -rf tree.dat

buildrunplot:
	g++ ${DEFAULTFLAGS} -c src/glad.c -o glad.o	
	g++ ${DEFAULTFLAGS} -c src/utils.cpp -o utils.o
	g++ ${DEFAULTFLAGS} -c src/eikonal.cpp -o eikonal.o
	g++ ${DEFAULTFLAGS} -c src/postprocessing.cpp -o postprocessing.o
	g++ ${DEFAULTFLAGS} -c src/main.cpp -o main.o
	g++ ${DEFAULTFLAGS} -o sim.exe main.o postprocessing.o eikonal.o utils.o glad.o -ldl -lglfw3 -lpthread
	mv sim.exe bin/
	mv *.o  Modules/
	bin/sim.exe ${domain_size} ${max_depth} ${n_bodies}  #Domain size,maximum depth,number of bodies
	#python3 ib_plotter.py ${n_bodies} #Number of bodies
	#python3 quad_plotter.py ${n_bodies}

debug:
	g++ ${DEBUGFLAGS} -c src/glad.c -o glad.o	
	g++ ${DEBUGFLAGS} -c src/utils.cpp -o utils.o
	g++ ${DEBUGFLAGS} -c src/eikonal.cpp -o eikonal.o
	g++ ${DEBUGFLAGS} -c src/postprocessing.cpp -o postprocessing.o
	g++ ${DEBUGFLAGS} -c src/main.cpp -o main.o
	g++ ${DEBUGFLAGS} -o sim.exe main.o postprocessing.o eikonal.o utils.o glad.o -ldl -lglfw3 -lpthread
	mv sim.exe bin/
	mv *.o  Modules/
	bin/sim.exe ${domain_size} ${max_depth} ${n_bodies}  #Domain size,maximum depth,number of bodies
