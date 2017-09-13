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
#include <stdio.h>
#include <stdlib.h>
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

bool FM::loadfileExist(const char * indexfile)
{
	FILE* r=fopen(indexfile,"rb");
	if(r==NULL)
	{
		cout<<"Fopen error"<<endl;
		return false;
	}
	return true;
}
int FM::load(const char * indexfile)
{
	loadkit s(indexfile);
	unsigned long long int magicnum=0;
	s.loadu64(magicnum);
	if(magicnum!=198809102510)
	{
		cerr<<"Not a FM_Index file"<<endl;
		return 0;
		//exit(0);
	}
	wt.Load(s);
	s.close();
	//cout <<indexfile<< " Load is ok" << endl;
	return 1;
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
	part = 3;
	for(int i = 0 ;i<part;i++)
	fm.emplace_back(filename,speedlevel,part,i);
//	ThreadPool pool;
}

void FM_M::counting_parrel(const char *pattern,i64 &num)
{	
	vector<std::thread> v_Thread;
	i64 i22 = 0;
	vector<int> v_num[fm.size()];
	for(int i = 0 ;i<fm.size();i++)
	{
		v_Thread.emplace_back([this,pattern,&i22,&num,i]{this->fm[i].counting(pattern,i22);num +=i22;});
	}
	for(int i = 0 ;i<fm.size();i++)
		v_Thread[i].join();
/*	int i1 = 0;
	i64 num0 = 0,num1=0,num2 = 0;
	std::thread t1([this,pattern,&num0]{this->fm[0].counting(pattern,num0);});	
	std::thread t2([this,pattern,&num1]{this->fm[1].counting(pattern,num1);});	
	std::thread t3([this,pattern,&num2]{this->fm[2].counting(pattern,num2);});	
	t1.join();
	t2.join();
	t3.join();
	num = num0+num1 + num2;
	//std::cout<<"Main Thread"<<std::endl;
	return;*/
}

void FM_M::counting(const char *pattern,i64 &num)
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

void FM_M::counting_pool(const char *pattern,i64 &num)
{	
	int i1 = 0;
	// vector<i64> v_i64;
	std::vector< std::future<i64> > results;
	for(int i = 0; i < 3; ++i) {
		results.emplace_back(
			pool.enqueue([&,i,pattern] {
				i64 temp = 0;
				this->fm[i].counting(pattern,temp);
				//std::cout<<temp<<std::endl;
				//v_i64.emplace_back(temp);
				return temp;
			}));
	}
	//pool.Pooljoin();
	for(auto && result:results)
		num += result.get();
	//std::cout<<"Main Thread"<<std::endl;
	return;
}
int FM_M::load(const char * indexfile,int part)
{
	string str = indexfile;	
	this->part = part;
		for(int i = 0;i<part;i++)
		{
			FM* fmtemp = new FM();
			fm.emplace_back(*fmtemp);

		}
		for(int i = 0;i<part;i++)
		{
			char buffer[5];
			string strtemp = str;
			sprintf(buffer,"_%d_%d",part,i);
			strtemp+=buffer;
			if(fm[i].loadfileExist(strtemp.c_str()))
				fm[i].load(strtemp.c_str());
			else 
				return false;
		}
	return 1;
};
int FM_M::save(const char * indexfile)
{
	string str = indexfile;	
		
	for(int i = 0;i<part;i++)
	{
		char buffer[5];
		string strtemp = str;
		sprintf(buffer,"_%d_%d",part,i);
		strtemp+=buffer;
		fm[i].save(strtemp.c_str());
	}
	return 0;
};

extern void quick_sort(i64 *s, i64 l, i64 r);
i64 * FM_M::locating(const char * pattern,i64 & num)
{
	vector<i64> v_num(3,0);
	//i64 totalnum;
	//i64 ** posarray = new (i64 *)[part];
	vector<i64 *> v_pos; 
	i64 offset = 0;
	int i = 0;
	for(i = 0;i<part;i++)
	{
		i64* temppos;
		//i64 tempnum  = 0;
		temppos = fm[i].locating(pattern,v_num[i]);
		
		v_pos.emplace_back(temppos);
		quick_sort(temppos,0,v_num[i]-1);
		num +=v_num[i];
	}
	i64 * pos = new i64[num];
	memcpy(pos,(i64*)v_pos[0],v_num[0]*sizeof(i64));
	i = v_num[0];
	for(int j = 1; j<part;j++)
	{
		//if(j>0)
		offset += fm[j-1].wt.GetN()-2*PATTENLEN;//previous piece of fm read 2 patternlen more txt,so next piece need to minus it
		int startpos = i-1;
		for(int k=0;k<v_num[j];k++)
		{
			//i++;

			pos[i++] = v_pos[j][k]+offset;
			if(i>0&&pos[startpos]==pos[i-1])
			{
				i--;
				num--;
			}
		}
	}
	return pos;
}
i64* FM_M::locating_parrel(const char * pattern,i64 & num)
{
	int i1 = 0;
	i64 i = 0;
	vector<i64> v_num;
	i64 offset = 0;
	i64 tempnum =0;
	std::vector< std::future<tuple<i64,i64* >> > results;
	for(int i = 0; i < part; i++) {
		results.emplace_back(
			pool.enqueue([&,i,pattern,&tempnum] {
				i64* temp = 0;
				temp = this->fm[i].locating(pattern,tempnum);
				
				quick_sort(temp,0,tempnum-1);
				//std::cout<<tempnum<<std::endl;
				//v_i64.emplace_back(temp);
				return make_tuple(tempnum,temp);
			}));
	}
	vector<tuple<i64,i64*>> v_tuple;
	for(auto  && result : results)
	{
		auto t = result.get();
		v_tuple.emplace_back(t) ;
		num += get<0>(t);
	}
	i64 * pos = new i64[num];
	//auto tup0 = results[0].get();
	//cout<<num<<std::endl;
	memcpy(pos,get<1>(v_tuple[0]),get<0>(v_tuple[0])*sizeof(i64));
	i = get<0>(v_tuple[0]);
	for(int j = 1; j<part;j++)
	{
		//if(j>0)
		offset += fm[j-1].wt.GetN()-2*PATTENLEN;//previous piece of fm read 2 patternlen more txt,so next piece need to minus it
		int startpos = i-1;
		for(int k=0;k<get<0>(v_tuple[j]);k++)
		{
			//i++;

			pos[i++] = get<1>(v_tuple[j])[k]+offset;
			if(i>0&&pos[startpos]==pos[i-1])
			{
				i--;
				num--;
			}
		}
	}
	return pos;
}

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