//wch
#include <stdlib.h>
#include <string.h>
#include "FM.h"
#include <ctime>
#include <fstream>
#include <iostream>
#include <vector>
using namespace std;

void usage();
void helpbuild();
void helpload();
void helpsave();
void helpcount();
void helplocate();
void splitcommand(string command, string result[]);
void quick_sort(i64 *s, i64 l, i64 r);
void compare(vector<int> ivector, int *pos, int num);
void showpos(vector<int> ivector);
void showpos(i64 *pos, i64 num);
int stupidRank(unsigned char* c,int length,int& ch,int pos);
int main(int argc, char *argv[])
{
    i64 *pos;
    i64 num = 0;
    //	usage();
    string command;
    string result[2];
    string path,path2;
    string patten = "aid to Paul thr";
    cout<<patten<<endl;
    char filename[100] = {'\0'};
    char indexname[100] = {'\0'};
    FM *csa = NULL;
    result[0] = "";
    path = "";
    command = "";
    path = "bible";
    //path2 ="/home/wch/CMake Practice.pdf";
    if (csa != NULL)
	delete csa;
    csa = NULL;
    csa = new FM(path.data());

    //csa->save(path2.data());
    //csa=new FM();
	//csa->load(path2.data());
    int bit = 'M';
 //	int rankresult = csa->wt.fm->GetRoot()->Rank(100,bit);
 //   int strankresult = stupidRank(csa->wt.fm->bwt,1024*1024,bit,100);
 //  csa->wt.fm->bwt[2];
 // bwt
 // cout << "Rank(100,bit) =  " << rankresult << endl;
 // cout << "stupidRank(100,bit) =  " << strankresult << endl;
    if (csa != NULL)
    {
		{

			if (csa != NULL)
			{
                //csa->counting(patten.data(),num);
                //pos = csa->locating(patten.data(), num);
                //cout<<"chuanxing:";
                //showpos(pos, num);
                pos = csa->Locating_parrel(patten.data(), num);
                cout<<"bingxing:";
                showpos(pos, num);
                //cout << "occs:" << num << endl;
                // shmctl(pos, IPC_RMID, NULL) ;
                //	showpos(pos, num);
                delete[] pos;
			}
			else
			cout << "build a FM first" << endl;
		}
    }
    vector<int> ivector;
    cout << "string search patten " << path << endl;
    FILE *fp = fopen(path.data(), "r+");
    if (fp == NULL)
    {
		cout << "Be sure the file is available" << endl;
		exit(0);
    }
    fseek(fp, 0, SEEK_END);
    long count = ftell(fp) + 1;
    unsigned char *T = new unsigned char[count];
    fseek(fp, 0, SEEK_SET);
    
    int e = 0;
    int num2 = 0;
    while ((e = fread(T + num2, sizeof(uchar), count - 1 - num2, fp)) != 0)
	num2 = num2 + e;
    if (num2 != count - 1)
    {
		cout << "Read source file failed" << endl;
		exit(0);
    }
    T[count - 1] = 0;
    string str((char *)T);
    int ipos = str.find(patten);
    //pos = csa->locating(patten.data(), num);
    // for(int i =0;i<num;i++)
    // {
    
    //     string strtemp = str.substr(pos[i],4);
    //     cout << strtemp<<endl;
    // }
   
    while (ipos != -1)
    {
		ivector.push_back(ipos);
		ipos = str.find(patten, ipos + 1);
		
    }
   showpos(ivector);
   //compare(ivector, pos, num);
   cout << ">";
   char c;
   cin >> c;
   return 0;
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
///void compare(vector<int> ivector, i64 *pos, i64 num)
///{
///    quick_sort(pos, 0, num - 1);
///    vector<int> mismatchIndex;
///    if (ivector.size() == num)
///    {
///	int index = 0;
///	while (index < num)
///	{
///	    if (ivector[index] != pos[index])
///		mismatchIndex.push_back(ivector[index]);
///	}
///	if (mismatchIndex.size() > 0)
///	{
///	    cout << "the mismatch pos number is:" << ivector.size() << endl
///		 << "the mismatch pos are as follows:" << endl;
///	    for (int i = 0; i < mismatchIndex.size(); i++)
///	    {
///		cout << mismatchIndex[i] << ",";
///		if ((i + 1) % 10 == 0)
///		    cout << endl;
///	    }
///	}
///    }
///}
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
