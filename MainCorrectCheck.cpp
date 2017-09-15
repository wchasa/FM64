//wch
#include <stdlib.h>
#include "FM.h"
#include <ctime>
#include <fstream>
#include <iostream>
#include <vector>
#include<iomanip>
#include <string>
using namespace std;
#define MAX 200
#define PATTENLEN 20
void usage();
void helpbuild();
void helpload();
void helpsave();
void helpcount();
void helplocate();
void splitcommand(string command, string result[]);
extern void quick_sort(i64 *s, i64 l, i64 r);
void compare(vector<i64> ivector, i64 *pos, i64 num);
void showpos(vector<i64> ivector);
void showpos(i64 *pos, i64 num);
int* generateRandom(int count);
int stupidRank(unsigned char* c,int length,int& ch,int pos);
//this main  compare result of locate
int main(int argc, char *argv[])
{

	using FM_NAME = FM_M;
	//  i64 totalsize = 0;
	  i64 sumRun = 0,bitLen =0;
	  if(argc < 3){
		  fprintf(stderr, "Usage: ./my_fm <file> <speedlevel>");
		  exit(EXIT_FAILURE);
	  }
	  //timer st1,st2;
	  double stime,etime,tcost;
	  int *pos;
	  i64 num = 0;
	  string command;
	  string result[2];
	  string path,path2;
	  string patten = "ABBA";
	  FILE *fp,*fp_result,*fp2;
	  FM_NAME *csa = NULL;
	  char StrLineFM[1024]; 
	  strcpy(StrLineFM,argv[1]);
	  csa = NULL;
	  FILE *fh = fopen(strcat(StrLineFM, ".fmfull"), "r");
	  csa = new FM_NAME();
	  if(argc == 5)
	  if(csa->load(StrLineFM,atoi(argv[4]))==0){
		  stime = clock();
		  csa = new FM_NAME(argv[1],atoi(argv[4]));
		  etime = clock();
		  csa->save(StrLineFM);}    
  	  if(argc == 4)
	  if(csa->load(StrLineFM,3)==0){
		  stime = clock();
		  csa = new FM_NAME(argv[1]);
		  etime = clock();
		  csa->save(StrLineFM);}  
	  
	  fp = fopen(argv[1],"r+");
	  fseek(fp,0,SEEK_END);
	  i64 n = ftell(fp)+1;
	  unsigned char *T = new unsigned char[n];
	  fseeko(fp,0,SEEK_SET);
	  i64 e   = 0;
	  i64 num2 = 0;
	  num = fread(T,sizeof(unsigned char),n,fp);
	  T[n-1]=0;
		string strtxt((char*)T);
	//cout<<"Plain:"<<setw(10)<<Plaincount<<"";
	tcost = 0;
	int* randarray =  generateRandom(MAX);
	if ((fp2 = fopen(argv[1], "r")) == NULL) //判断文件是否存在及可读
	{
		printf("Open Falied!");
		return -1;
	}
	fseek(fp2, 0, SEEK_END);
	n = ftell(fp2) + 1;
	unsigned char * searchT = new unsigned char[1024];
	fseeko(fp2, 0, SEEK_SET);
	string str;
	for(int i2 =-100;i2<MAX;i2++)
    {
		//fseek(fp2, randarray[i2] % (n/3), SEEK_SET);
		fseek(fp2, n/3-i2, SEEK_SET);
		fread(searchT, sizeof(unsigned char), PATTENLEN, fp2);
		
		string str((char*)searchT);
//		str = "n unto the house of";
        str = "TGATGATGGATTGGATAACC";
		cout<<"Patten:"<<str<<endl;
		num = 0;
		i64 *pos = csa->locating_pool(str.c_str(), num);

		//i64 *pos = csa->locating(str.c_str(), num);
		int i = strtxt.find(str);
		int p = 0;
		if(num<=0)
		{
			cout<<"locate havnt find"<<endl;
			continue;
		}
		//quick_sort(pos,0,num-1);
		int stringFind=0;
        while(i>= 0)
        {	
		    stringFind++;
            if(pos[p++]!=i)
            {
				cout<<"------------------wrong-----------------"<<endl;
				cout<<"i"<<setw(10)<<p-1<<",i2="<<i2<<endl;
				cout<<setw(10)<<"strFind"<<setw(10)<<i<<endl;
				cout<<setw(10)<<"locate"<<setw(10)<<pos[p-1]<<endl;
				cout<<"Patten:"<<setw(10)<<str<<endl<<"count:"<<num<<endl;
				cout<<"------------------wrong-----------------"<<endl;
				//break;
			}

            i = strtxt.find(str,i+1);

        }
		//cout<<setw(20)<<"stringFind="<<stringFind<<endl;
		//cout<<"---------------------------"<<endl;
		//;
		cout<<"right,count:"<<stringFind<<endl;
	//	delete [] pos2;
	//	pos2 =NULL;
		delete [] pos;
		pos =NULL;
    }
	//csa->DestroyWaveletTree();
	fclose(fp);
	fclose(fp2);
	//fclose(fpw);
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
void showpos(i64 *pos, i64 num)
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
void compare(vector<i64> ivector, i64 *pos, i64 num)
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
/*
void quick_sort(i64 *s, i64 l, i64 r)
{
    if (l < r)
    {
	//Swap(s[l], s[(l + r) / 2]); //将中间的这个数和第一个数交换 参见注1
	i64 i = l, j = r, x = s[l];
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
*/