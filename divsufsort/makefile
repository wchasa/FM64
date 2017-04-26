CC=g++ -pg -std=gnu++11
CFLAGS=-O3  -DHAVE_CONFIG_H -DBUILD_DIVSUFSORT64
libdivsufsort.a:divsufsort.o sssort.o trsort.o utils.o
	ar rc libdivsufsort.a divsufsort.o sssort.o trsort.o utils.o

%.o:%.cpp *.h
	$(CC) -c $(CFLAGS) $< -o $@
clean:
	rm *.a *.o
