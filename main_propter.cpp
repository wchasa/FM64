#include<stdlib.h>
#include<string.h>
#include"FM.h"
#include<ctime>
#include<fstream>
#include<iostream>
#include<iomanip>
//#include "statics.h"

using namespace std;
void usage();
void helpbuild();
void helpload();
void helpsave();
void helpcount();
void helplocate();
void splitcommand(string command,string result[]);
void showpos( int * pos,int num);
int32_t floor_log2(uint64_t i){
		return floor(log(i) / log(2));}
int main(int argc, char* argv[])
{
	i64 totalsize = 0;
	i64 sumRun = 0,bitLen =0;
	if(argc < 3){
		fprintf(stderr, "Usage: ./my_fm <file> <speedlevel>");
		exit(EXIT_FAILURE);
	}
	fprintf(stderr, "read File %s\n", argv[1]);
	//map<>
	int speedlevel = atoi(argv[2]);
	FM* csa = new FM(argv[1], speedlevel);
	//	for(map<i64,i64>::iterator it = csa->wt.fm->BWTruns.begin();it!= csa->wt.fm->BWTruns.end();it++)
  	//  		cout<<it->first<<","<<it->second<<endl;
	  map<i64,i64> runblockmap = csa->wt.fm->getBitMapRuns();//averunblock
	  map<i64,i64> runmap = csa->wt.fm->getBitnodeRuns();//averun
	  map<i64,i64> BWTruns = csa->wt.fm->BWTruns;
		for(map<i64,i64>::iterator it = runmap.begin();it!= runmap.end();it++)
		{
			bitLen += it->first * it->second;
			sumRun += it->second;
		}
		cout << "AveRun = " <<  (double)bitLen / sumRun << endl;
		bitLen = 0, sumRun = 0;
		for(map<i64,i64>::iterator it = runblockmap.begin();it!= runblockmap.end();it++)
		{
			bitLen += it->first * it->second;
			//cout<<it->first<<","<<it->second<<endl;
			sumRun += it->second;
		}
		cout << "AveRunBlock = " <<  (double)bitLen / sumRun << endl;
		bitLen = 0, sumRun = 0;
		for(map<i64,i64>::iterator it = BWTruns.begin();it!= BWTruns.end();it++)
		{
			bitLen += it->first * it->second;
			sumRun += it->second;
		}
		cout << "AveBWTRun = " <<  (double)bitLen / sumRun << endl;
		
		i64 sumGamma = 0;
		i64 sumDelta = 0;
		i64 sumGolomb = 0;
		i64 logx = 0;
		i64 runTmp;
		sumRun = 0;
		for(map<i64,i64>::iterator it = runmap.begin();it!= runmap.end();it++)
		{
//			bitLen += it->first * it->second;
//			sumRun += it->second;

			sumRun += it->second;
			logx = floor_log2(it->first);
//			Statics::floor_log2(i);
			sumGamma += (2*logx + 1)*it->second;
			sumDelta += (logx + 2*floor_log2(logx + 1) + 1)*it->second;
			sumGolomb+=	(floor_log2(it->first>>10)*2+1+10)*it->second;

		}
		cout << "aveDelta = " << (double)sumDelta / sumRun << endl;
		cout << "aveGamma = " << (double)sumGamma / sumRun << endl;
		cout << "aveGolomb = " << (double)sumGolomb / sumRun << endl;
		sumGamma=0;
		sumDelta = 0;
		sumGolomb = 0;
		logx = 0;
		sumRun = 0;
		for(map<i64,i64>::iterator it = runblockmap.begin();it!= runblockmap.end();it++)
		{
//			bitLen += it->first * it->second;
//			sumRun += it->second;

			sumRun += it->second;
			logx = floor_log2(it->first);
//			Statics::floor_log2(i);
			sumGamma += (2*logx + 1)*it->second;
			sumDelta += (logx + 2*floor_log2(logx + 1) + 1)*it->second;
			sumGolomb+=	(floor_log2(it->first>>10)*2+1+10)*it->second;
		}
//	Statics::aveGamma = (double)sumGamma / sumRun;
//	Statics::aveDelta = (double)sumDelta / sumRun;
	cout<< "aveDeltaOfBlock = " << (double)sumDelta / sumRun << endl;
	cout<< "aveGammaOfBlock = " <<(double)sumGamma / sumRun << endl;
	cout << "aveGolombOfBlock = " << (double)sumGolomb / sumRun << endl;
	//average Delta an Gamma in blocks
	int Plaincount, AL0, AL1, RL0, RL1, Fixcount;
	csa->Codedistribution(Plaincount, AL0, AL1, RL0, RL1, Fixcount);
	totalsize =Plaincount+AL0+AL1+RL0+RL1+Fixcount;
	cout << " Plain编码个数,占比:    :" <<setw(10)<<double(Plaincount)/totalsize<<endl;
	cout << " RunLength编码个数,占比::" <<setw(10)<<double(RL0+RL1)/totalsize<<endl;
	cout << " ALL0/1编码个数,占比:   :" <<setw(10)<<double(AL0+AL1)/totalsize<<endl;
	cout << " Fixcode编码个数,占比:  :" <<setw(10)<<double(Fixcount)/totalsize<<endl;
	totalsize =  csa->wt.fm->SizeInBytePart_count("style")+csa->wt.fm->SizeInBytePart_count("SB")+csa->wt.fm->SizeInBytePart_count("B")+csa->wt.fm->SizeInBytePart_count("code");
	cout<< " 数据分的块大小:"<<BitMap::Block_size<<",数据分的超块大小:"<< BitMap::superblock_size << endl;
	cout<< " 压缩后Header数据的大小：" << csa->wt.fm->SizeInBytePart_count("style") <<",占比:"<<setw(10)<<(double)( csa->wt.fm->SizeInBytePart_count("style"))/totalsize <<endl;
	cout<< " 压缩后超级块（包含rank和偏移量两种信息）的总大小：" << csa->wt.fm->SizeInBytePart_count("SB") <<",占比:"<<setw(10)<<(double)( csa->wt.fm->SizeInBytePart_count("SB"))/totalsize<< endl;
	cout<< " 压缩后块（包含块的rank和偏移量两种信息）的总大小： "<< csa->wt.fm->SizeInBytePart_count("B") <<",占比:"<<setw(10)<<(double)( csa->wt.fm->SizeInBytePart_count("B"))/totalsize<< endl;
	cout<< " 压缩后数据（不包含附加信息）的总大小: " <<csa->wt.fm->SizeInBytePart_count("code")<<",占比:"<<setw(10)<<(double)(csa->wt.fm->SizeInBytePart_count("code"))/totalsize<<endl;
	totalsize = BitMap::plainSize+BitMap::RLSize+BitMap::FixSize;
	cout<< " 压缩后数据Plain编码占有的总大小: "<<BitMap::plainSize/8 <<",占比:"<<setw(10)<<(double)(BitMap::plainSize)/totalsize<<endl;
	cout<< " 压缩后数据run_length_gama编码占有的总大小： " <<BitMap::RLSize/8<<",占比:"<<setw(10)<<(double)(BitMap::RLSize)/totalsize<<endl;
	cout<< " 压缩后数据fix编码占有的总大小： " <<BitMap::FixSize/8<<",占比:"<<setw(10)<<(double)(BitMap::FixSize)/totalsize<<endl;
	cout<< " SA采样后大小为："<<csa->wt.fm->sizeOfSAL()<<endl;
	cout<< " Rank采样后大小为："<<csa->wt.fm->sizeOfSAL()<<endl;
	cout<<"--------------------------------------------------------------------------------------"<<endl;
	return 0;
}


int chenLongGangMain(){

	usage();
	string command;
	string  result[2];
	char filename[100]={'\0'};
	char indexname[100]={'\0'};
	FM *csa=NULL;
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
			csa=new FM(result[1].data());
		}
		else if(result[0]=="count")
		{
			int  num=0;
			if(csa!=NULL)
			{
				//csa->counting(result[1].data(),num);
				cout<<"occs: "<<num<<endl;
			}
			else
			{
				cout<<"build a FM first"<<endl;
			}
		}
		else if(result[0]=="locate")
		{
			int * pos;
			int num=0;
			if(csa!=NULL)
			{
			//	pos=csa->Locating(result[1].data(),num);
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
			csa=new FM();
			csa->load(result[1].data());
		}
		else if(result[0]=="save")
		{
			csa->save(result[1].data());
		}
		else if(result[0]=="size")
		{
			if(csa!=NULL)
				cout<<csa->compressRatioForCount()<<endl;
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

void showpos(int * pos,int num)
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
