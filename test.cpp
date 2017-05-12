#include <iostream>
#include <vector>
#include <string>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <time.h>
#include <sys/types.h>
#include "FM.h"

using namespace std;

const int TIME = 100;

int main(int argc,char *argv[])
{
	
	cout<<"============="<<argv[1]<<"============"<<endl;
	int fd = open(argv[1],'r',S_IRUSR | S_IWUSR | S_IXUSR);
	if(fd == -1)
	{
		cout<<"open error"<<endl;
		exit(-1);
	}

	int size = 0;
	if((size = lseek(fd,0,SEEK_END)) == -1)
	{
		cout<<"lseek error"<<endl;
		exit(-1);
	}
	string strp[TIME];
	char *p[TIME];
	int *pos = new int[TIME];
	memset(pos,0,sizeof(int)*TIME);
//	for(int i = 0; i < TIME; i++)
//		cin >> pos[i];
	
	for(int i = 0; i < TIME; i++)
	{
		p[i] = new char[21];
		memset(p[i],0,sizeof(char)*21);
		if(lseek(fd,pos[i],SEEK_SET) != -1)
		{
			if(read(fd,p[i],20) != 20)
			{
				cout<<"read error"<<endl;
				exit(-1);
			}
		}
		else
		{
			exit(-1);
		}
	}

	close(fd);
	FM *fm = NULL;
	char StrLineFM[1024]; 
	struct timespec stime, etime;
	clock_gettime(CLOCK_REALTIME, &stime);
	int speedlevel=atoi(argv[2]);
	strcpy(StrLineFM,argv[1]);
    fm = NULL;
    FILE *fh = fopen(strcat(StrLineFM, ".fm"), "r");
    if (fh == NULL)
    {
        //stime = clock();
        fm = new FM(argv[1],speedlevel);
        //etime = clock();
        fm->save(StrLineFM);
    }
    else
    {
        fm = new FM();
        fm->load(StrLineFM);
    }
	//FM fm(argv[1],speedlevel);
	
	if(strcmp(argv[3],"cx")==0)
	{
	clock_gettime(CLOCK_REALTIME, &etime);
	cout<<"Build time is "<<((double)((etime.tv_sec*1000000000.0 + etime.tv_nsec) - (stime.tv_sec*1000000000.0 + stime.tv_nsec)))/1000000000.0<<" s"<<endl;

	
	cout<<"rationForAll is "<<fm->compressRatio()<<endl;
	cout<<"rationForCount is "<<fm->compressRatioForCount()<<endl;
	cout<<"rationForLocate is "<<fm->compressRatioForLocate()<<endl;
	cout<<"rationForExtract is "<<fm->compressRatioForExtract()<<endl;

	
	for(int i = 0; i < TIME; i++)
		strp[i] = p[i];

	clock_gettime(CLOCK_REALTIME, &stime);
	for(int i = 0; i < TIME; i++)
	{
		i64 num = 0;
		fm->counting(strp[i].data(),num);
	}
	clock_gettime(CLOCK_REALTIME, &etime);
	cout<<"Count time is "<<((double)((etime.tv_sec * 1000000000.0 + etime.tv_nsec) - (stime.tv_sec * 1000000000.0 + stime.tv_nsec)))/1000000000.0/TIME<<" s"<<endl;
	clock_gettime(CLOCK_REALTIME, &stime);

	for(int i = 0; i < TIME; i++)
	{
		i64 num = 0;
		i64 *ptmp = fm->locating(strp[i].data(),num);
		delete [] ptmp;
	}
	clock_gettime(CLOCK_REALTIME, &etime);


	cout<<"Locate time is "<<((double)((etime.tv_sec * 1000000000.0 + etime.tv_nsec) - (stime.tv_sec*1000000000.0 + stime.tv_nsec)))/1000000000.0/TIME<<" s"<<endl;


	for(int i = 0; i < TIME; i++)
		delete [] p[i];

	clock_gettime(CLOCK_REALTIME, &stime);
	for(int i = 0; i < TIME; i++)
	{
		fm->extracting(pos[i],20);
	}
	clock_gettime(CLOCK_REALTIME, &etime);

	cout<<"Extract time is "<<((double)((etime.tv_sec*1000000000.0 + etime.tv_nsec) - (stime.tv_sec*1000000000.0 + stime.tv_nsec)))/1000000000.0/TIME<<" s"<<endl;
}
	else if(strcmp(argv[2],"bx")==0)
	{
		clock_gettime(CLOCK_REALTIME, &stime);
		for(int i = 0; i < TIME; i++)
		{
			i64 num = 0;
			i64 *ptmp = fm->locating_parrel(strp[i].data(),num);
			delete [] ptmp;
		}
		clock_gettime(CLOCK_REALTIME, &etime);
		cout<<"Locate time is "<<((double)((etime.tv_sec * 1000000000.0 + etime.tv_nsec) - (stime.tv_sec*1000000000.0 + stime.tv_nsec)))/1000000000.0/TIME<<" s"<<endl;
	}
	return 0;
}
