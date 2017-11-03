/*============================================
# Filename: FM.h
# Ver 1.0 2014-06-08
# Copyright (C) 2014 ChenLonggang (chenlonggang.love@163.com)
#
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 or later of the License.
#
# Description:
  External class,you can build a hybrid and adaptive fm-index 
  for a document.and then you can know:
  
  How many times a pattern occs in the document using Counting,
  the reference parmater num will holds the times.
  
  All the positions where the pattern occs,and the times.
  the reference pointer pos will holds all the places,but
  REMEMBER that: prepare and clean the space of pos is your 
  duty.

  Extract a piece of the document using Extracting,the sequence
  will hold T[pos...pos+len-1].T denotes the original document.
  REMEMBER that: prepare and clean space for sequence is your
  duty too.

  Save and Load support serialize a FM object to a file or restore 
  a FM object from a file.

  GetN will tell you the size of the file in byte.
  SizeInByte will tell you the totle space needed for all operations in byte
  SizeInByte_count will tell you sapce only needed for Counting operation
=============================================*/
#ifndef FM_H
#define FM_H
#include"loadkit.h"
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/shm.h>
#include"savekit.h"
#include"ABS_WT.h"
#include <stdlib.h>
#include"Huffman_WT.h"
#include"Balance_WT.h"
#include"Hutacker_WT.h"
#include"WT_Handle.h"
#include "ThreadPool.h"   // std::threadmak
#include <thread>
#include <unistd.h>
#include <cilk/cilk.h>

class FM
{
	public:
		FM(const char * filename,int D = 32,int speedlevel=1);
		FM();
		~FM(){};
		FM(const FM & h):wt(h.wt){}
		FM& operator =(const FM&h){wt=h.wt;return *this;};
		void DrawBackSearch(const char * pattern,i64 & Left,i64 &Right){wt.DrawBackSearch(pattern,Left,Right);}
		i64 Lookup(i64 startpos){return wt.Lookup(startpos);};
		//void ABS_FM::Counting(const char * pattern,i64 & num){};
		void counting(const char *pattern,i64 &num);
		void counting_parrel(const char *pattern,i64 &num){};
		void counting_pool(const char *pattern,i64 &num){};
		i64 * locating(const char *pattern,i64 & num);
		i64 * locating_parrel(const char *pattern,i64 & num);
		i64 * locating_pool(const char *pattern,i64 & num){return NULL;};
		//void  GetMaps(Map<i64,i64> &bwtmap,Map<i64,i64> &runsmap);
		unsigned char * extracting(i64 pos,i64 len);
		unsigned char * extracting_parrel(i64 pos,i64 len);
		int load(const char * indexfile);
		int save(const char * indexfile);
		int load(const char * indexfile,int part){this->load(indexfile);};
		int save(const char * indexfile,int part){this->save(indexfile);};
		bool loadfileExist(const char * indexfile);
		i64 getN();
		void Codedistribution(int &Plain, int &AL0, int &AL1, int &RL0, int &RL1, int &Fix);
		int getAlphabetSize();
		i64 sizeInByte();
		i64 sizeInByteForCount();
		i64 sizeInByteForExtract();
		i64 sizeInByteForLocate();
		double compressRatio();
		double compressRatioForCount();
		double compressRatioForExtract();
		double compressRatioForLocate();
		vector<i64> GetHittimes(){return wt.GetHittimes();};
		void Lookupall();
		WT_Handle wt;
		int part;
	private:
		//WT_Handle wt;
};
/*
class FM_M
{
public:
    FM_M(){};
   // FM();
    FM_M(const char * filename,int frag = 4 ,int speedlevel=1);
	int part;
    vector<FM> fm;
	//ThreadPool pool;	
	//vector<i64> counting(const char *pattern,i64 &num);
	vector<tuple<i64,i64>> counting(const char *pattern,i64 &num);
	void counting_parrel(const char *pattern,i64 &num);
	void counting_pool(const char *pattern,i64 &num);//
	i64 * locating(const char *pattern,i64 & num);
	i64 * locating_parrel(const char *pattern,i64 & num);
	i64* locating_pool(const char * pattern,i64 & num);
	//void  GetMaps(Map<i64,i64> &bwtmap,Map<i64,i64> &runsmap);
	unsigned char * extracting(i64 pos,i64 len){return 0;};
	unsigned char * extracting_parrel(i64 pos,i64 len){return 0;};
	int load(const char * indexfile,int part=3);
	int save(const char * indexfile);
	i64 getN(){return 0;};
	void Codedistribution(int &Plain, int &AL0, int &AL1, int &RL0, int &RL1, int &Fix){};
	int getAlphabetSize(){return 0;};//
	i64 sizeInByte(){return 0;};
	i64 sizeInByteForCount(){return 0;};
	i64 sizeInByteForExtract(){return 0;};
	i64 sizeInByteForLocate(){return 0;};
	double compressRatio(){return 0;};
	double compressRatioForCount(){return 0;};
	double compressRatioForExtract(){return 0;};
	double compressRatioForLocate(){return 0;};
	i64 Lookup(i64 startpos,vector<tuple<i64,i64>> v_i64,i64& piece);//startpos is the kth postion
	vector<i64> GetHittimes(){vector<i64> v;return v;};
};

*/
#endif
