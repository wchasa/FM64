//wch
#include <stdlib.h>
#include "FM.h"
#include <ctime>
#include <fstream>
#include <iostream>
#include <vector>
#include<iomanip>
#include <string>
#include<sys/time.h>
using namespace std;
#define MAX 10000
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
int stupidRank(unsigned char* c,int length,int& ch,int pos);
//this main  compare result of locate
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
	double stime,etime,stime1,etime1,tcost,tcost2;
    FM *csa = NULL;
    string strpathFM,strpathfile,str;
	strpathFM ="/home/lab/testfile/kernel.fm";
	//strpathfile ="./bible";
	strpathfile ="/home/wch/testfile/kernel";
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
	int n = ftell(fp)+1;
	unsigned char *searchT = new unsigned char[1024];
	fseeko(fp,0,SEEK_SET);
	int e   = 0;
	i64 num = 0,num2 = 0;
	//num = fread(T,sizeof(unsigned char),n,fp);
	//T[n-1]=0;
    //string strtxt((char*)T);

	stime = clock();
	if (csa != NULL)
	    delete csa;
//	csa = new FM(strpathfile.c_str());

	csa = new FM();
	csa->load(strpathFM.data());
	etime = clock();
	tcost = etime-stime;
	cout<<"build takes:"<<setw(10)<<tcost/CLOCKS_PER_SEC<<"sec"<<endl;
	cout<<"build complete;"<<endl;
	//cout<<"Plain:"<<setw(10)<<Plaincount<<"";
	tcost = 0;
	tcost2 = 0;
	//stime1 = clock();
	//engthen his for""
	timer t1;
	t1.start();
	for(int i2 =0;i2<MAX;i2++)
	{
		int count = 500000;
//		fseek(fp,rand()%n-100,SEEK_SET);
//	    fread(searchT, sizeof(unsigned char), 20, fp);
		const  char* array =(const char*) csa->extracting_parrel(i2,count);
		//const  char* array =(const char*) csa->extracting(i2,count);
		delete []array;
		array = NULL;
//		stime = clock();
//		i64 *pos2 = csa->locating((const char*)searchT, num2);
//		etime = clock();
//		tcost2 += etime-stime;
	}
	t1.finish();
	 cout << "bx:" << t1.value()/MAX/1000<<"ms"<<endl;
	//etime = clock();
	//tcost = etime-stime;
   // cout<<"chuan:"<<setw(10)<<tcost2/CLOCKS_PER_SEC/MAX*1000<<"ms"<<endl;
   // cout<<" bing:"<<setw(10)<<tcost /CLOCKS_PER_SEC/MAX*1000<<"ms"<<endl;
    tcost = 0;
//	stime = clock();
//	for (int i2 = 0; i2 < MAX; i2++)
//	{
//	    fseek(fp, rand() % n-100, SEEK_SET);
//	    fread(searchT, sizeof(unsigned char), 40, fp);
//	    
//	    //cout<<setw(20)<<"Count:"<<setw(10)<<num<<endl;
//	}
//	etime = clock();
//	//tcost = etime-stime;
//   // cout<<"totalbing:"<<setw(10)<<(etime1-stime1)/CLOCKS_PER_SEC/MAX<<"sec"<<endl;
   // cout<<"bing :"<<setw(10)<<tcos<<"sec"<<endl;
	//cout<<"chuanread :"<<setw(10)<<tcost2/CLOCKS_PER_SEC/MAX<<"sec"<<endl;
	int Plaincount,Gamacount,Fixcount;
	//csa->Codedistribution(Plaincount,Gamacount,Fixcount);
	//cout<<"Plaincount="<<setw(10)<<Plaincount<<",Gamacount="<<setw(10)<<Gamacount<<",Fixcode="<<setw(10)<<Fixcount<<endl;
   // str ="fell on her face, a";
    //while(true)

			//cout<<setw(20)<<"stringFind="<<stringFind<<endl;
		//cout<<"---------------------------"<<endl;
		//;
    
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
