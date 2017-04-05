CC=g++ -pg -std=gnu++11
CFLAGS=-O0 -g -Wall
my_fm:div Main.o fm.a
	$(CC)  $(CFLAGS) Main.o fm.a   -o my_fm

mult:div MainMulti.o fm.a
	$(CC)  $(CFLAGS) MainMulti.o fm.a   -o my_fm

test:div main_propter.o fm.a
	$(CC)  $(CFLAGS) main_propter.o fm.a   -o my_fm

my_fm_1:div Main1.o fm.a
	$(CC)  $(CFLAGS) Main1.o fm.a   -o my_fm
correctcheck:div MainCorrectCheck.o fm.a
	$(CC)  $(CFLAGS) MainCorrectCheck.o fm.a   -o my_fm

speedtest:div MainSpeedtest.o fm.a
	$(CC)  $(CFLAGS) MainSpeedtest.o fm.a   -o my_fm

fm.a:ABS_WT.o Balance_WT.o Huffman_WT.o Hutacker_WT.o FM.o BitMap.o UseCount.o WT_Handle.o InArray.o loadkit.o savekit.o divsufsort.o sssort.o trsort.o utils.o
	ar rc fm.a ABS_WT.o Balance_WT.o Huffman_WT.o Hutacker_WT.o FM.o BitMap.o UseCount.o WT_Handle.o  InArray.o loadkit.o savekit.o divsufsort.o sssort.o trsort.o utils.o

%.o:%.cpp *.h
	$(CC) -c  $(CFLAGS) $< -o $@

Main.o:Main.cpp  FM.h
	$(CC)  $(CFLAGS) -c  Main.cpp

MainCorrectCheck.o:MainCorrectCheck.cpp  FM.h
	$(CC)  $(CFLAGS) -c  MainCorrectCheck.cpp
	
MainSpeedtest.o:MainSpeedtest.cpp  FM.h
	$(CC)  $(CFLAGS) -c  MainSpeedtest.cpp

MainMulti.o:MainMulti.cpp  FM.h
	$(CC)  $(CFLAGS) -c  MainMulti.cpp
main_propter.o:main_propter.cpp  FM.h
	$(CC)  $(CFLAGS) -c  main_propter.cpp

Main1.o:Main1.cpp  FM.h
	$(CC)  $(CFLAGS) -c  Main1.cpp
clean:
	rm *.a  *.o  my_fm ./divsufsort/*.a ./divsufsort/*.o

div:
	make -C ./divsufsort/; cp divsufsort/libdivsufsort.a .;ar x libdivsufsort.a;rm libdivsufsort.a 
