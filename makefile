CC=g++ -pg -std=gnu++11
CFLAGS=-O0 -g -Wall -fcilkplus -fopenmp
#CFLAGS=-O3 -Wall -fcilkplus -fopenmp
my_fm:div Main.o fm.a
	$(CC)  $(CFLAGS) Main.o fm.a   -o my_fm -lpthread

mult:div MainMulti.o fm.a
	$(CC)  $(CFLAGS) MainMulti.o fm.a   -o my_fm -lpthread 

adaptive: div fm.a test.o
	g++ test.o fm.a $(CFLAGS) -o my_fm -lrt

count:div MainMulti_count.o fm.a
	$(CC)  $(CFLAGS) MainMulti_count.o fm.a   -o my_fm

extract:div MainMulti_extract.o fm.a
	$(CC)  $(CFLAGS) MainMulti_extract.o fm.a   -o my_fm

test:div main_propter.o fm.a 
	$(CC)  $(CFLAGS) main_propter.o fm.a   -o my_fm_test

my_fm_1:div Main1.o fm.a
	$(CC)  $(CFLAGS) Main1.o fm.a   -o my_fm
correctcheck:div MainCorrectCheck.o fm.a
	$(CC)  $(CFLAGS) MainCorrectCheck.o fm.a   -o my_fm -lpthread

speedtest:div MainSpeedtest.o fm.a
	$(CC)  $(CFLAGS) MainSpeedtest.o fm.a   -o my_fm

speed:div MainSpeed.o fm.a
	$(CC)  $(CFLAGS) MainSpeed.o fm.a   -o my_fm

fm.a:ABS_WT.o Balance_WT.o Huffman_WT.o Hutacker_WT.o FM.o BitMap.o UseCount.o WT_Handle.o InArray.o loadkit.o savekit.o divsufsort.o sssort.o trsort.o utils.o ThreadPool.o
	ar rc fm.a ABS_WT.o Balance_WT.o Huffman_WT.o Hutacker_WT.o FM.o BitMap.o UseCount.o WT_Handle.o  InArray.o loadkit.o savekit.o divsufsort.o sssort.o trsort.o utils.o ThreadPool.o

ThreadPool.o :ThreadPool.h
	$(CC) -c  $(CFLAGS) ThreadPool.h -o ThreadPool.o
%.o:%.cpp *.h
	$(CC) -c  $(CFLAGS) $< -o $@

Main.o:Main.cpp  FM.h
	$(CC)  $(CFLAGS) -c  Main.cpp

MainCorrectCheck.o:MainCorrectCheck.cpp  FM.h
	$(CC)  $(CFLAGS) -c  MainCorrectCheck.cpp

MainMulti_extract.o:MainMulti_extract.cpp  FM.h
	$(CC)  $(CFLAGS) -c  MainMulti_extract.cpp

MainMulti_count.o:MainMulti_count.cpp  FM.h
	$(CC)  $(CFLAGS) -c  MainMulti_count.cpp

MainSpeedtest.o:MainSpeedtest.cpp  FM.h
	$(CC)  $(CFLAGS) -c  MainSpeedtest.cpp

 MainSpeed.o:MainSpeed.cpp  FM.h
	$(CC)  $(CFLAGS) -c  MainSpeed.cpp

MainMulti.o:MainMulti.cpp  FM.h
	$(CC)  $(CFLAGS) -c  MainMulti.cpp
main_propter.o:main_propter.cpp  FM.h
	$(CC)  $(CFLAGS) -c  main_propter.cpp

test.o: test.cpp FM.h
	g++ -c $(CFLAGS) test.cpp -lrt

Main1.o:Main1.cpp  FM.h
	$(CC)  $(CFLAGS) -c  Main1.cpp
clean:
	rm *.a  *.o  my_fm ./divsufsort/*.a ./divsufsort/*.o

div:
	make -C ./divsufsort/; cp divsufsort/libdivsufsort.a .;ar x libdivsufsort.a;rm libdivsufsort.a 
