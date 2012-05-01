F2C = f2c
CC = gcc $(CCFLAGS) $(INCFLAGS)

LINFLAGS = -lm -lf2c -lblas -latlas -lgfortran -lcsnopt -lsnopt_c -lsnprint_c
INCFLAGS = -I/home/neptune/include -L/home/neptune/lib -I. -L.

CCFLAGS = $(DEBUG) -fPIC -O3 -ftree-vectorize -msse2 -ftree-vectorizer-verbose=0 -ffast-math

F2COBJ = fortran-io-f77.c
OBJS = c-snopt.o c-snopt-sugar.o fortran-io-f77.o
LIBNAME = csnopt

.f.c:
	$(F2C) $<

.c.o:
	$(CC) -c $< -o $@

libs: $(F2COBJ)
	make static
	make dyn

static: $(OBJS)
	ar -cvq lib$(LIBNAME).a $(OBJS)

dyn: $(OBJS)
	gcc -shared $(OBJS) -o lib$(LIBNAME).so

clean:
	rm -f lib$(LIBNAME).a 
	rm -f lib$(LIBNAME).so
	rm -f $(F2COBJ)
	rm -f *.o *~
