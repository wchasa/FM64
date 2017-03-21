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

FM::FM(const char *filename,int speedlevel):wt(filename,256,32,1,speedlevel){}

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

void FM::Codedistribution(int &Plain,int &Gama,int &Fix)
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
	Gama  = BitMap::Gamacount;
	Fix   = BitMap::Fixcount;
}
i64 FM::sizeInByteForCount()
{
	return wt.SizeInByte_count();
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
	cout<<"Load is ok"<<endl;
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
i64 * FM::Locating_parrel(const char * pattern,i64 & num)
{
	return wt.Locating_parrel(pattern,num);
}

unsigned char * FM::extracting(int pos,int len)
{
	return wt.Extracting(pos,len);
}

