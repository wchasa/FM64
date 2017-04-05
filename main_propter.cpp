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
int main(int argc, char* argv[])
{
	if(argc < 3){
		fprintf(stderr, "Usage: ./my_fm <file> <speedlevel>");
		exit(EXIT_FAILURE);
	}
	fprintf(stderr, "read File %s\n", argv[1]);
	//map<>
	int speedlevel = atoi(argv[2]);
	FM* csa = new FM(argv[1], speedlevel);
	if(csa!=NULL)
	{
	    cout << "File Size :" << csa->getN() << ",TreeSize:" << csa->sizeInByteForCount() << ",CompressRate:" << csa->compressRatioForCount() << endl;
	    int Plaincount, AL0, AL1, RL0, RL1, Fixcount;
	    csa->Codedistribution(Plaincount, AL0, AL1, RL0, RL1, Fixcount);
	    cout << "Plaincount=" << setw(10) << Plaincount << ",AAL0count=" << setw(10) << AL0 << ",AAL1count=" << setw(10) << AL1 << ",RL0count=" << setw(10) << RL0
		 << ",RL1count=" << setw(10) << RL1 << ",Fixcode=" << setw(10) << Fixcount << endl;
	}
	else
	    cout << "build a FM first" << endl;
		
	//	for(map<i64,i64>::iterator it = csa->wt.fm->BWTruns.begin();it!= csa->wt.fm->BWTruns.end();it++)
  	//  		cout<<it->first<<","<<it->second<<endl;
	  map<i64,i64> bitmapruns = csa->wt.fm->getBitMapRuns();
		for(map<i64,i64>::iterator it = bitmapruns.begin();it!= bitmapruns.end();it++)
  	  		cout<<it->first<<","<<it->second<<endl;
						/*
	for(int i = 0; i < 5; i ++){
		cout << Statics::coding_styles[i] << " ";
	}
	cout << endl;

	// for(int64_t i = 0; i < Statics::N; i++){
	// 	if(Statics::runs_num[i]){
	// 		cout << i << " : " << Statics::runs_num[i] << endl;
	// 	}
	// }

	//
	int64_t bitLen = 0;
	int64_t sumRun = 0;
	for(int32_t i = 0; i < Statics::N; i++){
		bitLen += i * Statics::runs_num[i];
		sumRun += Statics::runs_num[i];
	}
	Statics::aveRun = (double)bitLen / sumRun;
	cout << "AveRun = " << Statics::aveRun << endl;

	bitLen = 0, sumRun = 0;
	for(int32_t i = 0; i < Statics::M; i++){
		bitLen += i * Statics::runs_blocks[i];
		sumRun += Statics::runs_blocks[i];
	}
	Statics::aveRunBlock = (double)bitLen / sumRun;
	cout << "AveRunBlock = " << Statics::aveRunBlock << endl;


	//average Delta and Gamma of data
	int64_t sumGamma = 0;
	int64_t sumDelta = 0;
	int64_t logx = 0;
	int64_t runTmp;
	sumRun = 0;
	for(int32_t i = 0; i < Statics::N; i++){
		runTmp = Statics::runs_num[i];
		if(runTmp != 0){
			sumRun += runTmp;
			logx = Statics::floor_log2(i);
			sumGamma += (2*logx + 1)*runTmp;

			sumDelta += (logx + 2*Statics::floor_log2(logx + 1) + 1)*runTmp;
		}
	}
	Statics::aveGamma = (double)sumGamma / sumRun;
	Statics::aveDelta = (double)sumDelta / sumRun;
	cout << "aveDelta = " << Statics::aveDelta << endl;
	cout << "aveGamma = " << Statics::aveGamma << endl;
	//average Delta an Gamma in blocks
	sumGamma=0;
	sumDelta=0;
	logx=0;
	sumRun=0;
	for(int32_t i = 0; i < Statics::M; i++){
		runTmp = Statics::runs_blocks[i];
		if(runTmp != 0){
			sumRun += runTmp;
			logx = Statics::floor_log2(i);
			sumGamma += (2*logx + 1)*runTmp;

			sumDelta += (logx + 2*Statics::floor_log2(logx + 1) + 1)*runTmp;
		}
	}
	Statics::aveGammaOfBlock = (double)sumGamma / sumRun;
	Statics::aveDeltaOfBlock = (double)sumDelta / sumRun;
	cout << "aveDeltaOfBlock = " << Statics::aveDeltaOfBlock << endl;
	cout << "aveGammaOfBlock = " << Statics::aveGammaOfBlock << endl;
	cout<< " 数据分的块大小:"<<Statics::block_size<<"  数据分的超块大小:"<< Statics::superblock_size << endl;
	cout<< " 压缩后Header数据的大小：" << Statics::headerSize << endl;
	cout<< " 压缩后超级块（包含rank和偏移量两种信息）的总大小：" << Statics::SBSize << endl;
	cout<< " 压缩后块（包含块的rank和偏移量两种信息）的总大小： "<< Statics::BSize << endl;
	cout<< " 压缩后数据Plain编码占有的总大小: "<< Statics::plainSize <<endl;
	cout<< " 压缩后数据run_length_gama编码占有的总大小： " <<Statics::SSize - Statics::plainSize<<endl;
	cout<< " 压缩后数据（不包含附加信息）的总大小: " << Statics::SSize<<endl;


	//runs in block
	// for(int32_t i = 0; i < Statics::M; i++){
	// 	if(Statics::runs_blocks[i]){
	// 		cout << i << " :(blocks) " << Statics::runs_blocks[i] << endl;
	// 	}		
	// }


	//write into test/<file>.result
	char* resFile = new char[1024];
	strcpy(resFile, "./test/");
	// cout << Statics::getFile(argv[1]);
	strcat(resFile, Statics::getFile(argv[1]));
	strcat(resFile, ".result");
	FILE* out = fopen(resFile, "w");
	if(!out){
		fprintf(stderr, "Open file %s Error!", resFile);
		exit(EXIT_FAILURE);
	}

	fprintf(stderr, "Write File %s\n", resFile);
	fprintf(out, "BWT变换后，得到的L中字符的平均runs： = %f\n", Statics::aveRunsOfL);
	fprintf(out, "数据未分块前 ，得到的0/1的平均runs = %f\n", Statics::aveRun);	
	fprintf(out, "数据未分块前 ，Run-length-Gamma编码的平均码长: %f\n", Statics::aveGamma);
	fprintf(out, "数据未分块前 ，Run-length-Delta编码的平均码长：%f\n\n", Statics::aveDelta);
	fprintf(out, "数据分块后，得到（所有rlg0和rlg1块）的0/1的平均runs = %f\n", Statics::aveRunBlock);
	fprintf(out, "数据分块后 ，Run-length-Gamma编码的平均码长: %f\n", Statics::aveGammaOfBlock);
	fprintf(out, "数据分块后 ，Run-length-Delta编码的平均码长：%f\n\n", Statics::aveDeltaOfBlock);
	fprintf(out, "\n数据分的块大小:%d  数据分的超块大小%d \n",Statics::block_size,Statics::superblock_size);
	fprintf(out, "各个编码块所占比例：\n ");
	fprintf(out, "\t\tRun-length-0 : Run-length-1 : Plain : All-0 : All-1 = %d : %d : %d : %d : %d\n", Statics::coding_styles[0],
		Statics::coding_styles[1], Statics::coding_styles[2], Statics::coding_styles[3], Statics::coding_styles[4]);
	fprintf(out, "\n ---------------------------数据压缩后各部分的数据大小情况------------------------------\n" );
	fprintf(out, "压缩后Header数据的大小：%f MB\n",Statics::headerSize/1024);
	fprintf(out, "压缩后超级块（包含rank和偏移量两种信息）的总大小：%f MB\n",Statics::SBSize/1024 );
	fprintf(out, "压缩后块（包含块的rank和偏移量两种信息）的总大小：%f MB\n",Statics::BSize/1024);
	fprintf(out, "压缩后数据Plain编码占有的总大小：%f MB\n",Statics::plainSize/1024);
	fprintf(out, "压缩后数据run_length_gama编码占有的总大小：%f MB\n", (Statics::SSize-Statics::plainSize)/1024);
	fprintf(out, "压缩后数据（不包含附加信息）的总大小：%f MB\n",Statics::SSize/1024);
	fprintf(out, "附加的表大小为2的16方bit，大小是8MB\n\n");

	//fprintf(out, "BWT变换后，得到的L中字符的平均runs： = %f\n", Statics::aveRunsOfL);
	
	fprintf(out, "------------------分块数据的runs的分布计数(run-length-0 + run-length-1)---------------------\n");
	fprintf(out, "\truns的长度\t\truns的个数\n");
	for(int32_t i = 0; i < Statics::M; i++){
		if(Statics::runs_blocks[i]){
			fprintf(out, "\t%d\t\t\t\t%d\n",i,Statics::runs_blocks[i]);
		}
	}

	fprintf(out, "\n-----------------未分块数据的runs的分布计数(run-length-0 + run-length-1)-------------------\n");
	fprintf(out, "\truns的长度\t\truns的个数\n");
	for(int32_t i = 0; i < Statics::N; i++){
		if(Statics::runs_num[i]){
			fprintf(out, "\t%d\t\t\t\t%d\n", i, Statics::runs_num[i]);
		}
	}

	fclose(out);
	fprintf(stderr, "Write File %s end\n\n", resFile);

	delete fm;
	*/
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
