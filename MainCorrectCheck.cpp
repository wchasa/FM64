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
#define MAX 1000
void usage();
void helpbuild();
void helpload();
void helpsave();
void helpcount();
void helplocate();
void splitcommand(string command, string result[]);
void quick_sort(i64 *s, i64 l, i64 r);
void compare(vector<i64> ivector, i64 *pos, i64 num);
void showpos(vector<i64> ivector);
void showpos(i64 *pos, i64 num);
int* generateRandom(int count);
int stupidRank(unsigned char* c,int length,int& ch,int pos);
//this main  compare result of locate
int main(int argc, char *argv[])
{
	double stime,etime,stime1,etime1,tcost,tcost2;
    FM *csa = NULL;
    string strpathFM,strpathfile,str;
	strpathFM ="/home/lab/testfile/english.fm";
	strpathfile ="/home/lab/testfile/english";
	//strpathfile ="/home/wch/testfile/einstein.en.txt";
    cout<<"input file path:";
    //getline(cin,strpath);/home/wch/codebase
	//strpath = "/home/wch/codebase/sources";
    FILE * fp = fopen(strpathfile.c_str(),"r+");
	FILE * fpw = fopen("result.txt","w+");
	//FILE * fp = fopen("./bible","r+");
    if(fp==NULL)
	{
		
		cout<<"Be sure the file is available"<<endl;
		exit(0);
	}
    fseek(fp,0,SEEK_END);
	i64 n = ftell(fp)+1;
	unsigned char *T = new unsigned char[n];
	fseeko(fp,0,SEEK_SET);
	i64 e   = 0;
	i64 num = 0,num2 = 0;
	num = fread(T,sizeof(unsigned char),n,fp);
	T[n-1]=0;
    string strtxt((char*)T);
	stime = clock();
	if (csa != NULL)
	    delete csa;

	csa = new FM();
	etime = clock();
	tcost = etime-stime;
	cout<<"build takes:"<<setw(10)<<tcost/CLOCKS_PER_SEC<<"sec"<<endl;
	cout << "build complete;" << endl;
	FILE *fh = fopen(strpathFM.c_str(), "r");
	if (fh == NULL)
	{
	    stime = clock();
	    csa = new FM(strpathfile.c_str());
	    etime = clock();
	    csa->save(strpathFM.data());
	}
	else
	{
	    csa = new FM();
	    csa->load(strpathFM.data());
	}
	//cout<<"Plain:"<<setw(10)<<Plaincount<<"";
	tcost = 0;
	int* randarray =  generateRandom(MAX);
	stime1 = clock();
/*	for(int i1 = 0;i1<MAX;i1++)
	{
		const  char* array =(const char*) csa->extracting_parrel(randarray[i1]%10000,50007);
		string str = strtxt.substr(randarray[i1]%10000,50007);
		if(0 !=strcmp(str.c_str(),array))
		{
			cout<<"extract error"<<endl;
		}
	}
	*/
	for(int i2 =0;i2<MAX;i2++)
    {
		//cout<<"**********************************"<<endl;
		//str = "military age (19)";
	   	str = strtxt.substr(rand()%100000,20);
		//str = strtxt.substr(i2,15 );
		//fseeko(fp,0,SEEK_SET);
		//num = fread(T,sizeof(unsigned char),20,fp);
		//T[20-1]=0;
		//string strtxt((char*)T);
		cout<<"Patten:"<<str<<endl;
		i64 *pos = csa->locating(str.c_str(), num);
		
		//i64 *pos2 = csa->Locating(str.data(), num);
		//cout << setw(20) << "locating" << num << endl;
		//showpos(pos,num);
		int i = strtxt.find(str);
		int p = 0;
		if(num<=0)
		{
			cout<<"locate havnt find"<<endl;
			//fprintf(fpw,"%s\r\n",str.c_str());
			continue;
		}
		quick_sort(pos,0,num-1);
		int stringFind=0;
        while(i>= 0)
        {	
		    stringFind++;
            if(pos[p++]!=i)
            {
				cout<<"-----------------------------------"<<endl;
				cout<<"i"<<setw(10)<<p-1<<",i2="<<i2<<endl;
				cout<<setw(10)<<"strFind"<<setw(10)<<i;
				cout<<setw(10)<<"locate"<<setw(10)<<pos[p-1]<<endl;
				cout<<"Patten:"<<setw(10)<<str<<"count:"<<num<<endl;
				//fpw.write((str+"\r\n").c_str);
				
				//fprintf(fpw,"%s\r\n",str.c_str());
				break;
			}

            i = strtxt.find(str,i+1);

        }
		//cout<<setw(20)<<"stringFind="<<stringFind<<endl;
		//cout<<"---------------------------"<<endl;
		//;
		cout<<"right"<<endl;
	//	delete [] pos2;
	//	pos2 =NULL;
		delete [] pos;
		pos =NULL;
    }
	//csa->DestroyWaveletTree();
	fclose(fp);
	fclose(fpw);
}

//    int *pos;
//    int num = 0;
//    //	usage();
//    string command;
//    string result[2];
//    string path,path2;
//    string patten = "ABBA";
//    char filename[100] = {'\0'};
//    char indexname[100] = {'\0'};
//    FM *csa = NULL;
//    result[0] = "";
//    path = "";
//    command = "";
//    path = "/home/wch/CMake Practice.pdf";
//    //path2 ="/home/wch/CMake Practice.pdf";
//    if (csa != NULL)
//	delete csa;
//    csa = NULL;
//    csa = new FM(path.data());
//    //csa->save(path2.data());
//    //csa=new FM();
//	//csa->load(path2.data());
//    int bit = 'M';
// //	int rankresult = csa->wt.fm->GetRoot()->Rank(100,bit);
// //   int strankresult = stupidRank(csa->wt.fm->bwt,1024*1024,bit,100);
// //  csa->wt.fm->bwt[2];
// // bwt
// // cout << "Rank(100,bit) =  " << rankresult << endl;
// // cout << "stupidRank(100,bit) =  " << strankresult << endl;
//    if (csa != NULL)
//    {
//		{
//
//			if (csa != NULL)
//			{
//            //  csa->counting(patten.data(),num);
//			 pos = csa->locating(patten.data(), num);
//             showpos(pos,num);
//			cout << "occs:" << num << endl;
//
//		//	showpos(pos, num);
//			delete[] pos;
//			}
//			else
//			cout << "build a FM first" << endl;
//		}
//    }
//    cout << "string search patten " << path << endl;
//    vector<int> ivector;
//    FILE *fp = fopen(path.data(), "r+");
//    if (fp == NULL)
//    {
//		cout << "Be sure the file is available" << endl;
//		exit(0);
//    }
//    fseek(fp, 0, SEEK_END);
//    long count = ftell(fp) + 1;
//    unsigned char *T = new unsigned char[count];
//    fseek(fp, 0, SEEK_SET);
//    
//    int e = 0;
//    int num2 = 0;
//    while ((e = fread(T + num2, sizeof(uchar), count - 1 - num2, fp)) != 0)
//	num2 = num2 + e;
//    if (num2 != count - 1)
//    {
//		cout << "Read source file failed" << endl;
//		exit(0);
//    }
//    T[count - 1] = 0;
//    string str((char *)T);
//    int ipos = str.find(patten);
//    //pos = csa->locating(patten.data(), num);
//    // for(int i =0;i<num;i++)
//    // {
//    //     string strtemp = str.substr(pos[i],4);
//    //     cout << strtemp<<endl;
//    // }
//   
//    while (ipos != -1)
//    {
//		ivector.push_back(ipos);
//		ipos = str.find(patten, ipos + 1);
//		
//    }
//   showpos(ivector);
//   compare(ivector, pos, num);
//    cout << ">";
//    char c;
//    cin >> c;
//    return 0;
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
