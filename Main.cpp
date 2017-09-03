#include <stdlib.h>
#include <vector>

#include <algorithm>
//#include "Transform.h"
#include <iostream>
#include <tuple>
#include <stdio.h>
#include <cstring>
#include <time.h>
#include<stdlib.h>
#include<string.h>
#include"FM.h"
#include<ctime>
#include<fstream>
#include<iostream>
#include<iomanip>
//#define READSIZE 1024*1024*5
using namespace std;
void usage();
void helpbuild();
void helpload();
void helpsave();
void helpcount();
void helplocate();
void splitcommand(string command,string result[]);
void showpos(i64 * pos,i64 num);
int main(int argc, char* argv[])
{
	using FM_A = FM_M;
	usage();
	string command;
	string  result[2];
	char filename[100]={'\0'};
	char indexname[100]={'\0'};
	FM_A *csa=NULL;
	while(1)
	{
		result[0]="";
		result[1]="";
		command="";
		cout<<">";
		getline(cin,command);
		splitcommand(command,result);
		if(result[0]=="quit")
			break;
		else if(result[0]=="build")
		{
			if(csa!=NULL)
				delete csa;
			csa=NULL;
			csa=new FM_A(result[1].data());
		}
		else if(result[0]=="count")
		{
			i64  num=0;
			if(csa!=NULL)
			{
				csa->counting(result[1].data(),num);
				cout<<"occs: "<<num<<endl;
			}
			else
			{
				cout<<"build a FM first"<<endl;
			}
		}
		else if(result[0]=="locate")
		{
			i64 * pos;
			i64 num=0;
			if(csa!=NULL)
			{
				pos=csa->locating(result[1].data(),num);
				showpos(pos,num);
				delete [] pos;
			}
			else
				cout<<"build a FM first"<<endl;
		}
		else if(result[0]=="load")
		{
			if(csa!=NULL)
				delete csa;
			csa=new FM_A();
			csa->load(result[1].data());
		}
		else if(result[0]=="save")
		{
			csa->save(result[1].data());
		}
		else if(result[0]=="size")
		{
			if(csa!=NULL)
			{
				cout<<"File Size :"<<csa->getN()<<",TreeSize:"<<csa->sizeInByteForCount()<<",CompressRate:"<<csa->compressRatioForCount()<<endl;
				int Plaincount,AL0,AL1,RL0,RL1 , Fixcount;
				csa->Codedistribution(Plaincount, AL0,AL1,RL0,RL1, Fixcount);
				cout << "Plaincount=" << setw(10) << Plaincount << ",AAL0count=" << setw(10) <<AL0<<",AAL1count="<<setw(10)<<AL1<<",RL0count="<<setw(10)<<RL0
				<<",RL1count="<<setw(10)<<RL1  << ",Fixcode=" << setw(10) << Fixcount << endl;
				
			}
			else
				cout<<"build a FM first"<<endl;
		}
		else if(result[0]=="help")
		{
			if(result[1]=="build")
				helpbuild();
			if(result[1]=="count")
				helpcount();
			if(result[1]=="locate")
				helplocate();
			if(result[1]=="load")
				helpload();
			if(result[1]=="save")
				helpsave();
		}
		else
			usage();
	}
	return 0;
}
// int main(int argc, char* argv[])
// {
// 	int times =1000;
// 	int ii=0;
// 	FILE* fp;
// 	time_t end, start;
// 	double duration;
// 	double duration2;
// 	vector<string> pathList;
// 	//pathList.push_back("/home/wch/testdata/dna");
// 	//pathList.push_back("E:\\��������\\��������\\highly-repetive\\einstein.en.txt");
// 	pathList.push_back("/home/wch/testdata/FIle/dna");
// 	pathList.push_back("/home/wch/testdata/FIle/proteins");
// 	pathList.push_back("/home/wch/testdata/FIle/para");
// 	pathList.push_back("/home/wch/testdata/FIle/english");
// 	for (int i = 0; i < pathList.size(); i++)
// 	{
// 		cout<<"---------------------------------------------"<<endl;
// 		fp = fopen( pathList[i].c_str(), "r");
// 		if (fp==NULL)
// 		{
// 			printf("The file %s is not exist.", pathList[i].c_str());
// 			continue;
// 		}
// 		fseek(fp, 0L, SEEK_END);
// 		long size = ftell(fp);
// 		fseek(fp, 0, SEEK_SET);
// 		size = size < READSIZE ? size : READSIZE;
// 		unsigned char* list = new unsigned char[size+1];
// 		long count = fread(list, sizeof(char), size, fp);
// 		fclose(fp);
// 		printf("File Name: %s\nbefore compress size :%5fKB\n", pathList[i].c_str(), static_cast<double>(count / 1024));
// 		list[count] = '\0';
// 		FM *csa=NULL;
// 		start = clock();
// 		csa=new FM(pathList[i].c_str());
// 		end = clock();
// 		duration =(end-start)/CLOCKS_PER_SEC;
		
// 		cout<<"Adaptive-FM durationOF constructwavelettre："<<duration<<"S"<<endl;
// 		cout<<"Adaptive-FM compress rate:"<<csa->compressRatioForCount()<<endl;
// 			for (ii = 0; ii < times; ii++)
// 			{

// 				srand(time(NULL));
// 				int r = rand() % READSIZE;
// 				//int r = 2;
// 				char ch[10];
// 				memcpy(ch, list+ r, 10);
// 				string strpatten(ch);
// 				strpatten = strpatten.substr(0, 10);
// 				//strpatten._BUF_SIZE = ch;
// 				start = clock();
// 				int num = 0;
// 				csa->counting(strpatten.data(),num);
// 				end = clock();
// 				duration += end - start;

// 				if( num > 0)
// 				{
// 					//printf("the patten exist %d times", get<1>(postuple)-get<0>(postuple)+1);
// 					//printf(", their position are as follows :");
// 					start = clock();
// 					int *pos=csa->locating(strpatten.data(),num);
// 					//vector<unsigned int> i = tree->locate(postuple);
// 					end = clock();
// 					duration2 += end - start ;
// 				//printf("\ndurationOF locate %fms\n", duration);
// 				}
// 			}	
// 			duration = static_cast<double>(duration) / times/1000;
// 			duration2 = static_cast<double>(duration2) / times/1000;
// 			printf("\nAdaptive-FM durationOF count %fms\n", duration);
// 			printf("\nAdaptive-FM durationOF locat %fms\n", duration2);
// 			cout << "\end!\n";
// 		}
// }
void showpos(i64 * pos,i64 num)
{
	cout<<"occs:"<<num<<endl;
	for(long int i=0;i<num;i++)
	{
		cout<<pos[i]<<endl;
		if((i+1)%20==0)
		{
			char command;
			cout<<"-----------------more---------------------";
			system("stty raw");
			command=getchar();
     		cout<<endl<<'\r';
			system("stty cooked");
			if(command==27)
			{
				cout<<endl;
				return ;
			}
		}
	}
}


void splitcommand(string command,string result[])
{

	int i=0;
	int start=0;
	int len=command.length();
	result[0]=command;
	for(i=0;i<len;i++)
	{
		if(command.at(i)!=' ')
			continue;
		result[0]=command.substr(0,i);
		start=i+1;
		break;
	}
	result[1]=command.substr(start,len);
}

void usage()
{
	cout<<"--------------------------------------------------------------------------------------"<<endl;
	cout<<"The flowing commands are supported "<<endl;
	cout<<"	help XX: show the details for the command XX"<<endl;
	cout<<"	build XX:build the index of file XX"<<endl;
	cout<<"	load  XX: load the index file XX"<<endl;
	cout<<"	save XX: write the csa to a index file XX"<<endl;
	cout<<"	count XX: count the pattern XX's occs" <<endl;
	cout<<"	locate YY: enum eyery  position of the pattern"<<endl;
	cout<<"	size: the size of the csa"<<endl;
	cout<<"	quit: say goodbye"<<endl;

}
void helpbuild()
{
	cout<<"build XX"<<endl;
	cout<<"	XX:the source file,it's you responsibility to provide a correct path"<<endl;
}
void helpcount()
{
	cout<<"count XX"<<endl;
	cout<<"	XX:the pattern.you have the responsibility to ensure the index csa is nearby,otherwise,nothing you will get"<<endl;
}
void helplocate()
{
	cout<<"locate XX"<<endl;
	cout<<"	XX: the pattern.it's like count, and you can scan the positions of all the occs in a manner like the system command -more-"<<endl;
}
void helpload()
{
	cout<<"load XX"<<endl;
	cout<<"	XX: the FM-index file, the command will read the index file and build a csa secretly"<<endl;
}
void helpsave()
{
	cout<<"save XX"<<endl;
	cout<<"	XX: the FM-index file, the command will save the csa in file XX"<<endl;
}
