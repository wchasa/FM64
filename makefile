CC=g++ -pg -std=gnu++11
CFLAGS=  -O2 -g -Wall
my_fm_original:div wchMain.o fm.a
	g++ -std=gnu++11 -g -O2 wchMain.o fm.a   -o my_fm_Fixcode

fm.a:ABS_WT.o Balance_WT.o Huffman_WT.o Hutacker_WT.o FM.o BitMap.o UseCount.o WT_Handle.o InArray.o loadkit.o savekit.o divsufsort.o sssort.o trsort.o utils.o
	ar rc fm.a ABS_WT.o Balance_WT.o Huffman_WT.o Hutacker_WT.o FM.o BitMap.o UseCount.o WT_Handle.o  InArray.o loadkit.o savekit.o divsufsort.o sssort.o trsort.o utils.o

%.o:%.cpp *.h
	$(CC) -c  $(CFLAGS) $< -o $@

wchMain.o:wchMain.cpp  FM.h
	g++ -std=gnu++11 -g -O2 -c  wchMain.cpp

clean:
	rm *.a  *.o  my_fm ./divsufsort/*.a ./divsufsort/*.o

div:
	make -C ./divsufsort/; cp divsufsort/libdivsufsort.a .;ar x libdivsufsort.a;rm libdivsufsort.a 
