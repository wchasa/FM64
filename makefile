CC=g++ -pg -std=gnu++11
CFLAGS=  -O0 -g -Wall
my_fm:div Main.o fm.a
	g++ -std=gnu++11 -g -O0 Main.o fm.a   -o my_fm

Mult:div MainMulti.o fm.a
	g++ -std=gnu++11 -g -o0 MainMulti.o fm.a   -o Mult

Single:div MainSingle.o fm.a
	g++ -std=gnu++11 -g -o0 MainSingle.o fm.a   -o Single

fm.a:ABS_WT.o Balance_WT.o Huffman_WT.o Hutacker_WT.o FM.o BitMap.o UseCount.o WT_Handle.o InArray.o loadkit.o savekit.o divsufsort.o sssort.o trsort.o utils.o
	ar rc fm.a ABS_WT.o Balance_WT.o Huffman_WT.o Hutacker_WT.o FM.o BitMap.o UseCount.o WT_Handle.o  InArray.o loadkit.o savekit.o divsufsort.o sssort.o trsort.o utils.o

%.o:%.cpp *.h
	$(CC) -c  $(CFLAGS) $< -o $@

Main.o:Main.cpp  FM.h
	g++ -std=gnu++11 -g -O0 -c  Main.cpp

MainSingle.o:MainSingle.cpp  FM.h
	g++ -std=gnu++11 -g -O0 -c  MainSingle.cpp
	
MainMulti.o:MainMulti.cpp  FM.h
	g++ -std=gnu++11 -g -O0 -c  MainMulti.cpp
clean:
	rm *.a  *.o  my_fm ./divsufsort/*.a ./divsufsort/*.o

div:
	make -C ./divsufsort/; cp divsufsort/libdivsufsort.a .;ar x libdivsufsort.a;rm libdivsufsort.a 
