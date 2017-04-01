//wch
#include <stdlib.h>
#include<iomanip>
#include <string.h>
#include "FM.h"
#include <ctime>
#include <fstream>
#include <iostream>
#include<string.h>
#include <vector>
#include <stdio.h> 
#include <time.h>
#include<sys/time.h>
using namespace std;
#define MAX 10000
#define PATTENLEN 20
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
int* generateRandom(int count);

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

int main(int argc, char *argv[])
{
    timer st1,st2;
    double stime,etime,tcost;
    //cout<<"This is add Fixcode FM"<<endl;
    //cout<<"Patten Len:"<<PATTENLEN<<endl;
    int *pos;
    i64 num = 0;
    string command;
    string result[2];
    string path,path2;
    string patten = "ABBA";
    char filename2[] = "/home/wch/path";
    char filename3[] = "/home/wch/result";
    FILE *fp,*fp_result;
    FM *csa = NULL;
    char StrLine[1024],StrLineFM[1024]; 
    if((fp = fopen(filename2,"r")) == NULL)      //判断文件是否存在及可读  
    {   
        //cout<<filename2<<"Open Failed"<<endl;
        return -1;   
    }   
    while (!feof(fp))                                   //循环读取每一行，直到文件尾
    {
        fgets(StrLine,1024,fp);
        if(StrLine[0]=='#')
            continue;
        StrLine[strlen(StrLine)-1]='\0';
        //cout<<"FileName:"<<StrLine<<"      ";
        strcpy(StrLineFM,StrLine);
        csa = NULL;
        FILE *fh = fopen(strcat(StrLineFM, ".fm"), "r");
        if (fh == NULL)
        {
            stime = clock();
            csa = new FM(StrLine);
            etime = clock();
            csa->save(StrLineFM);
        }
        else
        {
            csa = new FM();
            csa->load(StrLineFM);
        }
        tcost = (double)(etime - stime) / CLOCKS_PER_SEC;
        //cout << StrLine << endl;
        //cout <<"build time:"<< tcost << "sec" << endl;
        //cout << "File Size =" <<setw(10)<< csa->getN() << " Byte,TreeSize =" <<setw(10)<< csa->sizeInByteForCount() << " Byte,CompressRate = " <<setw(10)<< csa->compressRatioForCount() << endl;
        int Plaincount, Gamacount, Fixcount;
        csa->Codedistribution(Plaincount, Gamacount, Fixcount);
        //cout << "Plaincount=" << setw(10) << Plaincount << ",Gamacount=" << setw(10) << Gamacount << ",Fixcode=" << setw(10) << Fixcount << endl;
        FILE *fp2;
        if ((fp2 = fopen(StrLine, "r")) == NULL) //判断文件是否存在及可读
        {
            printf("Open Falied!");
            return -1;
        }
        fseek(fp2, 0, SEEK_END);
        int n = ftell(fp2) + 1;
        unsigned char * searchT = new unsigned char[1024];
        fseeko(fp2, 0, SEEK_SET);
        int e=0;
        i64 num = 0;
 /*       for (int i2 = 0; i2 < MAX; i2++)
        {

            fseek(fp2, rand() % (1024*1024*100-100), SEEK_SET);
            fread(searchT, sizeof(unsigned char), PATTENLEN, fp2);
            csa->counting((const char *)searchT, num);
        }*/
        int* randarray = generateRandom(MAX);
        //cout<<"start locate"<<endl;
        st1.start();
        for (int i2 = 0; i2 < MAX; i2++)
        {
            fseek(fp2, randarray[i2] % (1024 * 1024 * 100), SEEK_SET);
            fread(searchT, sizeof(unsigned char), PATTENLEN, fp2);
            i64 *pos = csa->Locating((const char *)searchT, num);
            //   cout<<"Patten:"<<setw(30)<<searchT<<",num:"<<setw(10)<<num<<endl;
       }
       st1.finish();
       cout << "cx:" << st1.value()/MAX/1000<<"ms"<<endl;
    //   st1.start();
    //   for (int i2 = 0; i2 < MAX; i2++)
    //   {
    //       fseek(fp2, randarray[i2]% (1024*1024*100), SEEK_SET);
    //       fread(searchT, sizeof(unsigned char), PATTENLEN, fp2);
    //       i64 *pos = csa->Locating_parrel((const char *)searchT, num);
    //     //  cout<<"Patten:"<<setw(30)<<searchT<<",num:"<<setw(10)<<num<<endl;
    //   }
    //  st1.finish();
    //  cout<<"bingxing:"<<st1.value()/MAX/1000<<"ms"<<endl;
      //delete csa;
      fclose(fp2);
     //  cout << "--------------------------------------------------------------------" << endl;
    }   
    fclose(fp);
    return 0;
}
int* generateRandom(int count)
{
    int* result = new int[count];
    srand(unsigned(time(NULL)));

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
