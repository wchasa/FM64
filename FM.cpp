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
FM::FM(const char *filename,int speedlevel):wt(filename,256,32,1,speedlevel){}
//FM::FM(const char *filename,int speedlevel,int pos):wt(filename,256,16,1,speedlevel){}
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
void FM::Lookupall()
{
	for(int i =0;i<getN();i++)
	{
		auto itemp = Lookup(i);
	}
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
	if(!this->loadfileExist(indexfile))
		return 0;
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
/*
FM_M::FM_M(const char * filename,int frag,int speedlevel)
{
	part = frag;
	for(int i = 0 ;i<part;i++)
	fm.emplace_back(filename,speedlevel,part,i);
//	ThreadPool pool;
}

void FM_M::counting_parrel(const char *pattern,i64 &num)
{	
	//i64* a_v64 = new i64[part];
	#pragma omp parallel for
	for(int i = 0 ; i < part ; i ++){
		i64 temp = 0; 
		fm[i].counting(pattern,temp);
		num += temp ; 
	}
	
	vector<std::thread> v_Thread;
	i64 i22 = 0;
	vector<int> v_num[fm.size()];
	for(int i = 0 ;i<fm.size();i++)
	{
		v_Thread.emplace_back([this,pattern,&i22,&num,i]{this->fm[i].counting(pattern,i22);num +=i22;});
	}
	for(int i = 0 ;i<fm.size();i++)
		v_Thread[i].join();


}

vector<tuple<i64,i64>> FM_M::counting(const char *pattern,i64 &num)
{	
	i64 i1;
	vector<tuple<i64,i64>> v_i64;
	i64 Left = 0,Right = 0;

	//fm[0]->counting(pattern,i1);
	//fm[1]->counting(pattern,i2);
	//fm[2]->counting(pattern,i3);
	for(i64 i =0 ; i<part;i++)
	{
		fm[i].DrawBackSearch(pattern,Left,Right);
		int numtemp = 0;
		if(Right < Left)
			numtemp = 0;
		else
			numtemp = Right -Left +1;
		v_i64.emplace_back(make_tuple(num,Left));
		num += numtemp;
	//	cout<<"num:"<<num<<"Left:"<<Left<<"Right:"<<Right<<endl;
	}
	//for(auto && result: results)
	//	num += result.get();
	return v_i64;
}

void FM_M::counting_pool(const char *pattern,i64 &num)
{	
	ThreadPool pool(4);
	int i1 = 0;
	// vector<i64> v_i64;
	std::vector< std::future<i64> > results;
	for(int i = 0; i < part; ++i) {
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

//void  * FM_M::locating(const char * pattern,i64 & num,i64* pos)
//{
//	 pos = this->locating(pattern,num);
//}
i64 * FM_M::locating(const char * pattern,i64 & num)
{
	vector<i64> v_num(part,0);
	vector<i64 *> v_pos; 
	i64 offset = 0;
	int i = 0;
	for(i = 0;i<part;i++)
	{
		i64* temppos;
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
i64* FM_M::locating_pool(const char * pattern,i64 & num)
{
	ThreadPool pool(4);
	int i1 = 0;
	i64 i = 0;
	i64 offset = 0;
	//i64 tempnum =0;
	std::vector< std::future<tuple<i64,i64* >> > results;
	for(int i = 0; i < part; i++) {
		results.emplace_back(
			pool.enqueue([&,i,pattern] {
				i64* temp = 0;
				i64 tempnum = 0;
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
	memcpy(pos,get<1>(v_tuple[0]),get<0>(v_tuple[0])*sizeof(i64));
	i = get<0>(v_tuple[0]);
	for(int j = 1; j<part;j++)
	{
		offset += fm[j-1].wt.GetN()-2*PATTENLEN;//previous piece of fm read 2 patternlen more txt,so next piece need to minus it
		int startpos = i-1;
		for(int k=0;k<get<0>(v_tuple[j]);k++)
		{
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

i64* FM_M::locating_parrel(const char *pattern,i64 &num)
{
	//vector<i64> v_i64;
	int modvalue = 0;
	//int numberOfthread = 0;
	i64 *shmaddr;
	i64 piece = 0;
	//i64 shmnum;
	pid_t *fpid = new pid_t[10];
	pid_t mainpid = getpid();
	auto v_i64 = counting(pattern,num);
	i64 offset = 0;
	vector<i64> v_offset;
	for(int j = 1;j<=part;j++)
	{
		v_offset.emplace_back(offset);
		offset +=  fm[j-1].wt.GetN()-2*PATTENLEN;
	}
	i64 *pos =new i64[num];
	int shmid;
	shmid = shmget(IPC_PRIVATE, num * sizeof(i64), IPC_CREAT | 0600);
	if (shmid < 0)
	{
	perror("Error:");
	return NULL ;
	}
	//cout<<"Part:"<<part<<endl;
	//i64 piece = 0,offset = 0;;
	//i64 dividemount = num/part ;
	modvalue = num % part;
	//modvalue = num % numberOfthread;
	for(int i=0;i<num;i++)
	{

		pos[i] = Lookup(i,v_i64,piece);
		pos[i] +=v_offset[piece];
	}
	for (int i = 0; i < part; i++)
	{
		if(getpid()==mainpid)
		{
				fpid[i] = fork();
		}
		if (fpid[i] < 0)
			printf("error in fork!");
		else if (fpid[i] == 0)
		{
			shmaddr = (i64 *)shmat(shmid, NULL, 0);
			for (int k = 0; k < (num / part) + (modvalue > i ? 1 : 0); k++)
			{
				i64  p = 0;
				auto val=  Lookup(k + num / part * i + (modvalue > i ? i : modvalue),v_i64,p);
				shmaddr[k + num / part * i + (modvalue > i ? i : modvalue)] = val+ v_offset[p];
			}
			exit(0);
		}
	}
	int* st1 = new int[part];
	for (int i = 0; i < part; i++)
	{
		//cout<<i<<endl;
		waitpid(fpid[i], &st1[i], 0);
	}
	shmaddr = (i64 *)shmat(shmid, NULL, 0);
	memcpy(pos, shmaddr, num * sizeof(i64));
	shmctl(shmid, IPC_RMID, NULL);
	delete[] fpid;
	return pos;
}
i64 FM_M::Lookup(i64 startpos,vector<tuple<i64,i64>> v_i64,i64& piece)
{
//	int piece =0;
	int offset = 0 ;
	int lo = 0;
	int hi = v_i64.size()-1;
	int mid = (lo+hi)/2;
	while(lo <= hi){
		if(startpos > get<0>(v_i64[mid])){
			lo = mid+1;
		}
		else if (startpos < get<0>(v_i64[mid])){
			hi = mid-1;
		}
		else{
			piece = mid;
			break;
		}
		mid = (lo+hi)/2;
	}
	if(get<0>(v_i64[mid+1])==get<0>(v_i64[mid]))
	mid ++;
	if(mid == v_i64.size())
		mid--;
	int Left = get<1>(v_i64[mid]);
	//cout<<"LOOKUP:"<<startpos<<";"<<mid<<endl;
	//cout<<"Lookup("<<Left+startpos- get<0>(v_i64[mid])<<")"<<endl;
	piece = mid;
	return fm[mid].Lookup(Left+startpos- get<0>(v_i64[mid]));

}*/