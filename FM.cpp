/*============================================
# Filename: FM.cpp
# Ver 1.0 2014-06-08
# Copyright (C) 2014 ChenLonggang (chenlonggang.love@163.com)
#
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 or later of the License.
#
# Description: 
=============================================*/
#include"FM.h"

//FM::FM(const char *filename,int speedlevel):wt(filename,256,32,1,speedlevel){}
FM::FM(const char *filename,int speedlevel,int part,int pos):wt(filename,part ,pos,256,32,1,speedlevel){}
FM::FM():wt(){}

i64 FM::getN(){
	return wt.GetN()-1;
}

int FM::getAlphabetSize(){
	return wt.GetAlphabetsize();
}

i64 FM::sizeInByte()
{
	return wt.SizeInByte();
}

void FM::Codedistribution(int &Plain,int &AL0,int &AL1,int &RL0,int &RL1,int &Fix)
{
	/*int Gamacount  = 0;
	int Fixcount   = 0;
	int Plaincount = 0;
	while (root !=NULL)
	{
		coding_style->GetValue()
	}
	*/
	Plain = BitMap::Plaincount;
	Fix   = BitMap::Fixcount;
	AL0 = BitMap::ALL0;
	AL1 = BitMap::ALL1;
	RL0 = BitMap::RL0;
	RL1 = BitMap::RL1;
}
i64 FM::sizeInByteForCount()
{
	return wt.SizeInByte_count();
}
i64 FM::sizeInByteForExtract()
{
	return wt.SizeInByte_extract();
}
i64 FM::sizeInByteForLocate()
{
	return wt.SizeInByte_locate();
}
double FM::compressRatioForLocate(){
	return sizeInByteForLocate()/(getN()*1.0);
}
double FM::compressRatioForExtract(){
	return sizeInByteForExtract()/(getN()*1.0);
}
double FM::compressRatioForCount(){
	return sizeInByteForCount()/(getN()*1.0);
}
double FM::compressRatio(){
	return sizeInByte()/(getN()*1.0);
}

int FM::save(const char * indexfile)
{
	savekit s(indexfile);
	s.writeu64(198809102510);
	wt.Save(s);
	s.close();
	return 0;
}


int FM::load(const char * indexfile)
{
	loadkit s(indexfile);
	unsigned long long int magicnum=0;
	s.loadu64(magicnum);
	if(magicnum!=198809102510)
	{
		cerr<<"Not a FM_Index file"<<endl;
		exit(0);
	}
	wt.Load(s);
	s.close();
	//cout <<indexfile<< " Load is ok" << endl;
	return 0;
}


void FM::counting(const char * pattern,i64 &num)
{
	wt.Counting(pattern,num);
}


i64 * FM::locating(const char * pattern,i64 & num)
{
	return wt.Locating(pattern,num);
}
i64 * FM::locating_parrel(const char * pattern,i64 & num)
{
	return wt.Locating_parrel(pattern,num);
}


unsigned char * FM::extracting(i64 pos,i64 len)
{
	return wt.Extracting(pos,len);
}


unsigned char * FM::extracting_parrel(i64 pos,i64 len)
{
	return wt.Extracting_parrel(pos,len);
}

FM_M::FM_M(const char * filename,int speedlevel)
{
	int part = 3;
	for(int i = 0 ;i<part;i++)
	fm.emplace_back(new FM(filename,speedlevel,part,i))
}

void FM_M::counting_parrel(const char *pattern,i64 &num)
{	
	int i1 = 0;
	i64 num0 = 0,num1=0,num2 = 0;
	std::thread t1([this,pattern,&num0]{this->fm[0]->counting(pattern,num0);});	
	std::thread t2([this,pattern,&num1]{this->fm[1]->counting(pattern,num1);});	
	std::thread t3([this,pattern,&num2]{this->fm[2]->counting(pattern,num2);});	
	t1.join();
	t2.join();
	t3.join();
	num = num0+num1 + num2;
	//std::cout<<"Main Thread"<<std::endl;
	return;
}

void counting(const char *pattern,i64 &num)
{	
	i64 i1;		
	//fm[0]->counting(pattern,i1);
	//fm[1]->counting(pattern,i2);
	//fm[2]->counting(pattern,i3);
	for(auto f : fm)
	{
		f.counting(pattern,i1);
		num += i1;
	}
	//for(auto && result: results)
	//	num += result.get();
}

void counting_pool(const char *pattern,i64 &num)
{	
	int i1 = 0;
	i64 num1[3]={0,0,0};
	std::vector< std::future<void> > results;
	for(int i = 0; i < 3; ++i) {
		results.emplace_back(
			pool.enqueue([&,i,pattern,&num1] {
				this->fm[i]->counting(pattern,num1[i]);
				return ;
			}      ) );
	}
	num = num1[0]+num1[1]+num1[2];
	//std::cout<<"Main Thread"<<std::endl;
	return;
}