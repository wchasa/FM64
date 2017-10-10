//wch
#include <stdlib.h>
#include<iomanip>
#include <string.h>
#include "FM.h"
#include <ctime>
#include <fstream>
#include <iostream>
#include <string.h>
#include <vector>
#include <stdio.h> 
#include <time.h>
#include<sys/time.h>
using namespace std;
#define MAX 300
#define PATTENLEN 20
#define PATTENLEN2 2
void usage();
void helpbuild();
void helpload();
void helpsave();
void helpcount();
void helplocate();
void splitcommand(string command, string result[]);
void quick_sort(int *s, int l, int r);
void compare(vector<int> ivector, int *pos, int num);
void showpos(vector<int> ivector);
void showpos(int *pos, int num);
int stupidRank(unsigned char* c,int length,int& ch,int pos);
int* generateRandom(int count,int seed);
int length = 100;
struct timer{
    public:
    struct timeval begin, end;
    timer(){ gettimeofday(&begin, NULL); }
    void start(){ gettimeofday(&begin, NULL); }
    void finish(){ gettimeofday(&end, NULL); }
    friend inline ostream & operator<<(ostream &os, timer &a){
        double use = 1000000 * (a.end.tv_sec - a.begin.tv_sec) + a.end.tv_usec - a.begin.tv_usec;
        use /= 1000000.0;
        os << use;
        return os;
    }
    double value(){
        double use = 1000000 * (end.tv_sec - begin.tv_sec) + end.tv_usec - begin.tv_usec;
        return use;
    }
};
//argv[1] = filepath argv[2] = cx bx px argv[3] = seed argv[4] = fragpart
int main(int argc, char *argv[])
{
    using FM_NAME = FM;
  //  i64 totalsize = 0;
	i64 sumRun = 0,bitLen =0;
	if(argc < 3){
		fprintf(stderr, "Usage: ./my_fm <file> <speedlevel>");
		exit(EXIT_FAILURE);
	}
    timer st1,st2;
    double stime,etime,tcost;
    int *pos;
    i64 num = 0;
    string command;
    string result[2];
    string path,path2;
    string patten = "ABBA";
    FILE *fp,*fp_result;
    FM_NAME *csa = NULL;
    char StrLineFM[1024]; 
    strcpy(StrLineFM,argv[1]);
    csa = NULL;
    FILE *fh = fopen(strcat(StrLineFM, ".fmfull"), "r");
    csa = new FM_NAME();
   /* if(argc == 5)
        if(csa->load(StrLineFM,atoi(argv[4]))==0){
            stime = clock();
            csa = new FM_NAME(argv[1],atoi(argv[4]));
            etime = clock();
            csa->save(StrLineFM);}  
            */  
    //if(argc == 4)
        if(csa->load(StrLineFM)==0){
            stime = clock();
            csa = new FM_NAME(argv[1]);
            etime = clock();
            csa->save(StrLineFM);}    
    tcost = (double)(etime - stime) / CLOCKS_PER_SEC;
    cout << argv[1] << endl;
    //cout <<"build time:"<< tcost << "sec" << endl;
    //cout << "File Size =" <<setw(10)<< csa->getN() << " Byte,TreeSize =" <<setw(10)<< csa->sizeInByteForCount() << " Byte,CompressRate = " <<setw(10)<< csa->compressRatioForCount() << endl;
    cout << "File Size :" << csa->getN() << ",TreeSize:" << csa->sizeInByteForCount() << ",CompressRate:" << csa->compressRatio()<<endl ;
    int Plaincount, AL0, AL1, RL0, RL1, Fixcount;
    int seed = atoi(argv[3]);
    csa->Codedistribution(Plaincount, AL0, AL1, RL0, RL1, Fixcount);
    int* randarray =  generateRandom(MAX,seed);
    FILE *fp2;
    if ((fp2 = fopen(argv[1], "r")) == NULL) //判断文件是否存在及可读
    {
        printf("Open Falied!");
        return -1;
    }
    fseek(fp2, 0, SEEK_END);
    int n = ftell(fp2) + 1;
    unsigned char * searchT = new unsigned char[1024];
    fseeko(fp2, 0, SEEK_SET);
    int e=0;
    if(strcmp(argv[2],"bx")==0){
        st1.start();
        //cout<<length<<endl;
        for (int i2 = 0; i2 < MAX; i2++)
        {
            fseek(fp2, randarray[i2] % (n), SEEK_SET);
            fread(searchT, sizeof(unsigned char), PATTENLEN, fp2);
            // i64 num;
            i64 i ;
            num = 0;
            csa->counting_parrel((const char *)searchT,num);

         //i64 *pos = csa->counting((const char *)searchT,&num);
          //cout<<"Patten:"<<setw(30)<<searchT<<",num:"<<setw(10)<<num<<endl;
            }
        st1.finish();
        cout << "count_parrel:" << st1.value() / MAX / 1000 << "ms" << endl;
        st1.start();
        //#pragma omp parallel for num_threads(2) shared(randarray,fp2,csa)
        for (int i2 = 0; i2 < MAX; i2++)
        {
       //     cout<<omp_get_thread_num()<<endl;
            fseek(fp2, randarray[i2] % (n), SEEK_SET);
            fread(searchT, sizeof(unsigned char), PATTENLEN, fp2);
            int tnum = 4;
            if(argc == 6){
                num = atoi(argv[4]);
                tnum = atoi(argv[5]);
               } 
            else 
                num = 100;
            i64 *pos = csa->locating_parrel((const char *)searchT, num,tnum);
//            #pragma omp parallel for
//           for(int j = 0 ;j<csa->part;j++)
//                {
//                    i64 *pos = csa->fm[j].locating((const char *)searchT, num);
//                    //delete [] pos;
//                    //pos = NULL;
//                }
           // cout<<"Patten:"<<setw(30)<<searchT<<",num:"<<setw(10)<<num<<endl;
        }
        st1.finish();
        cout << "locating_parrel:" << st1.value() / MAX / 1000 << "ms" << endl;
        st1.start();
        for (int i2 = 0; i2 < MAX; i2++)
        {
            fseek(fp2, randarray[i2] % (n), SEEK_SET);
                fread(searchT, sizeof(unsigned char), PATTENLEN, fp2);
         //   unsigned char *p = csa->extracting_parrel(randarray[i2] % (n-PATTENLEN2), PATTENLEN2);
           //   cout<<"Patten:"<<setw(30)<<searchT<<",num:"<<setw(10)<<num<<endl;
        }
        st1.finish();
        cout << "extracting_parrel:" << st1.value() / MAX / 1000 << "ms" << endl;
    }
     if(strcmp(argv[2],"px")==0){
        st1.start();
        //cout<<length<<endl;
        for (int i2 = 0; i2 < MAX; i2++)
        {
            fseek(fp2, randarray[i2] % (n), SEEK_SET);
            fread(searchT, sizeof(unsigned char), PATTENLEN, fp2);
            // i64 num;
            i64 i ;
            num = 0;
            csa->counting_pool((const char *)searchT,num);
            }
        st1.finish();
        cout << "count_pool:" << st1.value() / MAX / 1000 << "ms" << endl;
        st1.start();
        for (int i2 = 0; i2 < MAX; i2++)
        {
            fseek(fp2, randarray[i2] % (n), SEEK_SET);
                fread(searchT, sizeof(unsigned char), PATTENLEN, fp2);
            i64 *pos = csa->locating_pool((const char *)searchT, num);
        }
        st1.finish();
        cout << "locating_pool:" << st1.value() / MAX / 1000 << "ms" << endl;
        st1.start();
        for (int i2 = 0; i2 < MAX; i2++)
        {
            fseek(fp2, randarray[i2] % (n), SEEK_SET);
                fread(searchT, sizeof(unsigned char), PATTENLEN, fp2);
            unsigned char *p = csa->extracting_parrel(randarray[i2] % (n-PATTENLEN2), PATTENLEN2);
        }
        st1.finish();
        cout << "extracting_parrel:" << st1.value() / MAX / 1000 << "ms" << endl;
    }
    if(strcmp(argv[2],"cx")==0){
        st1.start();
        cout<<length<<endl;
        for (int i2 = 0; i2 < MAX; i2++)
        {
            fseek(fp2, randarray[i2] % (n), SEEK_SET);
            fread(searchT, sizeof(unsigned char), PATTENLEN, fp2);
            i64 i ;
            num = 0;
            csa->counting((const char *)searchT,num);
        }
        st1.finish();
        cout << "count:" << st1.value()/MAX/1000<<"ms"<<endl;
        st1.start();
        for (int i2 = 0; i2 < MAX; i2++)
        {
            fseek(fp2, randarray[i2] % (n), SEEK_SET);
            fread(searchT, sizeof(unsigned char), PATTENLEN, fp2);
            if(argc == 5)
                num = atoi(argv[4]);
            else 
                num = 100;
            i64 *pos = csa->locating((const char *)searchT, num);
            delete []pos;
            pos = NULL;
        }
             st1.finish();
             cout << "locating:" << st1.value()/MAX/1000<<"ms"<<endl;
             st1.start();
             for (int i2 = 0; i2 < MAX; i2++)
             {
             // fseek(fp2, randarray[i2] % (n), SEEK_SET);
              //  fread(searchT, sizeof(unsigned char), PATTENLEN, fp2);
               unsigned char *p = csa->extracting(randarray[i2] % ((length-PATTENLEN2)), PATTENLEN2);
               delete []p;
               p = NULL;
               //  cout<<"Patten:"<<setw(30)<<searchT<<",num:"<<setw(10)<<num<<endl;
             }
             st1.finish();
            // cout << "extracting:" << st1.value() / MAX / 1000 << "ms" << endl;
        }
   
      //delete csa;
      fclose(fp2);
     //  cout << "--------------------------------------------------------------------" << endl;
   
   // fclose(fp);
    return 0;
}
int* generateRandom(int count,int seed)
{
    int* result = new int[count];
    srand(unsigned(seed));

    for(int i = 0;i<count;i++)
    {
        result[i] = rand();
    }
    return result;
}
int stupidRank(unsigned char* c,int length,int& ch,int pos)
{
    int occTimes = 0;
    for (int i = 0; i < (pos < length ? pos : length); i++)	{
		if (c[i] == ch)
	    occTimes++;
	}
    return occTimes;
}
void showpos(int *pos, int num)
{
	quick_sort(pos,0,num-1);
    cout << "occs:" << num << endl;
    for (long int i = 0; i < num; i++)
    {
	cout << pos[i] << endl;
	if ((i + 1) % 20 == 0)
	{
	    char command;
	    cout << "-----------------more---------------------";
	    system("stty raw");
	    command = getchar();
	    cout << endl<< '\r';
	    system("stty cooked");
	    if (command == 27)
	    {
		cout << endl;
		return;
	    }
	}
    }
}
void compare(vector<int> ivector, int *pos, int num)
{
    quick_sort(pos, 0, num - 1);
    vector<int> mismatchIndex;
    if (ivector.size() == num)
    {
	int index = 0;
	while (index < num)
	{
	    if (ivector[index] != pos[index])
		mismatchIndex.push_back(ivector[index]);
	}
	if (mismatchIndex.size() > 0)
	{
	    cout << "the mismatch pos number is:" << ivector.size() << endl
		 << "the mismatch pos are as follows:" << endl;
	    for (int i = 0; i < mismatchIndex.size(); i++)
	    {
		cout << mismatchIndex[i] << ",";
		if ((i + 1) % 10 == 0)
		    cout << endl;
	    }
	}
    }
}
void showpos(vector<int> ivector)
{
    cout << "occs:" << ivector.size() << endl;
    for (long int i = 0; i < ivector.size(); i++)
    {
	cout << ivector[i] << endl;
	if ((i + 1) % 20 == 0)
	{
	    char command;
	    cout << "-----------------more---------------------";
	    system("stty raw");
	    command = getchar();
	    cout << endl
		 << '\r';
	    system("stty cooked");
	    if (command == 27)
	    {
		cout << endl;
		return;
	    }
	}
    }
}

void splitcommand(string command, string result[])
{

    int i = 0;
    int start = 0;
    int len = command.length();
    result[0] = command;
    for (i = 0; i < len; i++)
    {
	if (command.at(i) != ' ')
	    continue;
	result[0] = command.substr(0, i);
	start = i + 1;
	break;
    }
    result[1] = command.substr(start, len);
}

void usage()
{
    cout << "--------------------------------------------------------------------------------------" << endl;
    cout << "The flowing commands are supported " << endl;
    cout << "	help XX: show the details for the command XX" << endl;
    cout << "	build XX:build the index of file XX" << endl;
    cout << "	load  XX: load the index file XX" << endl;
    cout << "	save XX: write the csa to a index file XX" << endl;
    cout << "	count XX: count the pattern XX's occs" << endl;
    cout << "	locate YY: enum eyery  position of the pattern" << endl;
    cout << "	size: the size of the csa" << endl;
    cout << "	quit: say goodbye" << endl;
}
void helpbuild()
{
    cout << "build XX" << endl;
    cout << "	XX:the source file,it's you responsibility to provide a correct path" << endl;
}
void helpcount()
{
    cout << "count XX" << endl;
    cout << "	XX:the pattern.you have the responsibility to ensure the index csa is nearby,otherwise,nothing you will get" << endl;
}
void helplocate()
{
    cout << "locate XX" << endl;
    cout << "	XX: the pattern.it's like count, and you can scan the positions of all the occs in a manner like the system command -more-" << endl;
}
void helpload()
{
    cout << "load XX" << endl;
    cout << "	XX: the FM-index file, the command will read the index file and build a csa secretly" << endl;
}
void helpsave()
{
    cout << "save XX" << endl;
    cout << "	XX: the FM-index file, the command will save the csa in file XX" << endl;
}
//快速排序
void quick_sort(int *s, int l, int r)
{
    if (l < r)
    {
	//Swap(s[l], s[(l + r) / 2]); //将中间的这个数和第一个数交换 参见注1
	int i = l, j = r, x = s[l];
	while (i < j)
	{
	    while (i < j && s[j] >= x) // 从右向左找第一个小于x的数
		j--;
	    if (i < j)
		s[i++] = s[j];

	    while (i < j && s[i] < x) // 从左向右找第一个大于等于x的数
		i++;
	    if (i < j)
		s[j--] = s[i];
	}
	s[i] = x;
	quick_sort(s, l, i - 1); // 递归调用
	quick_sort(s, i + 1, r);
    }
}
