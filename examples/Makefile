DEBUG = -g
FCFLAGS = $(DEBUG) -fPIC -std=legacy -ftree-vectorize -msse2 -ftree-vectorizer-verbose=0 -ffast-math
FC = gfortran $(FCFLAGS)

LINFLAGS = -lm -lgfortran -lblas -lf2c -l:libcsnopt.a -lsnopt_c -lsnprint_c
INCFLAGS = -I ../csrc -L ../csrc -I/home/neptune/include -L/home/neptune/lib
CCFLAGS = $(DEBUG) -fPIC -O3 -ftree-vectorize -msse3 -ftree-vectorizer-verbose=0 -ffast-math
CPPFLAGS = $(DEBUG) -fPIC -O3 -ftree-vectorize -msse3 -ftree-vectorizer-verbose=0 -ffast-math

CC = gcc $(CCFLAGS) $(INCFLAGS)
CPP = g++ $(CPPFLAGS) $(INCFLAGS)

OBJECTS = toy_problem.o
PROG = toy_problem

.f.o:
	$(FC) -c $< -o $@

.c.o:
	$(CC) -c $< -o $@

.cpp.o:
	$(CPP) -c $< -o $@

main: $(OBJECTS)
	$(CC) $(OBJECTS) -o $(PROG) $(LINFLAGS)

clean:
	rm -f *.o *~
