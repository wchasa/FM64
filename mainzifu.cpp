#include <stdlib.h>
#include<iomanip>
#include <string.h>
#include<iomanip>
#include <ctime>
#include <fstream>
#include <iostream>
#include <vector>
#include <stdio.h> 
#include <time.h>
#include <sys/time.h>
using namespace std;
int main(int argc, char *argv[])
{
    //timer st1,st2;
    double stime,etime,tcost;
    //cout<<"This is add Fixcode FM"<<endl;
    //cout<<"Patten Len:"<<PATTENLEN<<endl;
    int *pos;
    long long int num = 0;
    string command;
    string result[2];
    string path,path2;
    char filename2[] = "/home/wch/path";
 //   char filename3[] = "/home/wch/result";
    FILE *fp;
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
        FILE* fp1;
        if ((fp1 = fopen(StrLine, "r")) == NULL) //判断文件是否存在及可读
        {
            //cout<<filename2<<"Open Failed"<<endl;
            return -1;
        }
        fseeko(fp1, 0, SEEK_END);
        long long int n = ftello(fp1)+1;
        fseeko(fp1,0,SEEK_SET);
        unsigned char *array = new unsigned char[1024];
        int remain = n;
        long long int *codearray = new long long int[256];
        memset(codearray,256,0); 
        int count =0;
        int total =0;
        while(remain>0)
        {
            int read = remain>1024?1024:remain;
            fread(array, sizeof(unsigned char), 1024, fp1);
            for(int i =0;i<(1024<remain?1024:remain);i++)
            {
                codearray[array[i]]++;
            }
            remain-=1024;
        }
        for(int i=0;i<256;i++)
        {
            total += codearray[i];
            if (codearray[i] > 0)
                count++;            
        }
        cout<<"FileName:"<<setw(30)<<StrLine<<setw(10)<<count<<",filesize:"<<setw(20)<<n<<",total:"<<setw(20)<<total<<endl;
    }
}