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
#include"savekit.h"
#include"ABS_WT.h"
#include"Huffman_WT.h"
#include"Balance_WT.h"
#include"Hutacker_WT.h"
#include"WT_Handle.h"
#include "ThreadPool.h"   // std::thread
class FM
{
	public:
	//	FM(const char * filename,int speedlevel=1);
		FM(const char * filename,int speedlevel=1,int part=1,int pos=0);
		FM();
		~FM(){};
		FM(const FM & h):wt(h.wt){}
		FM& operator =(const FM&h){wt=h.wt;return *this;};
		
		void counting(const char *pattern,i64 &num);
		i64 * locating(const char *pattern,i64 & num);
		i64 * locating_parrel(const char *pattern,i64 & num);
		//void  GetMaps(Map<i64,i64> &bwtmap,Map<i64,i64> &runsmap);
		unsigned char * extracting(i64 pos,i64 len);
		unsigned char * extracting_parrel(i64 pos,i64 len);
		int load(const char * indexfile);
		int save(const char * indexfile);
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
		WT_Handle wt;
	private:
		//WT_Handle wt;
};
class FM_M
{
public:
    FM_M(){};
   // FM();
    FM_M(const char * filename,int speedlevel=1);
    FM* fm[3] ;
//    FM* fm1;
//	FM* fm2;
	ThreadPool* pool;
	//
	void counting(const char *pattern,i64 &num)
	{	
		i64 i1;
		std::vector< std::future<i64>> results;
		for(int i = 0; i < 3; ++i) {
			results.emplace_back(
				pool->enqueue([&,i1,i] {
					//std::cout << "hello " << i << std::endl;
					//std::this_thread::sleep_for(std::chrono::seconds(1));
					//std::cout << "world " << i << std::endl;
					fm[i]->counting(pattern,i1)
					return i1;
				})
			);
		}
	
	/*	fm[0]->counting(pattern,i1);
		fm[1]->counting(pattern,i2);
		fm[2]->counting(pattern,i3);*/
		//num = i1[0]+i1[1]+i1[2];
		for(auto && result: results)
			num += result.get();
	};
	i64 * locating(const char *pattern,i64 & num){return 0;};
	i64 * locating_parrel(const char *pattern,i64 & num){return 0;};
	//void  GetMaps(Map<i64,i64> &bwtmap,Map<i64,i64> &runsmap);
	unsigned char * extracting(i64 pos,i64 len){return 0;};
	unsigned char * extracting_parrel(i64 pos,i64 len){return 0;};
	int load(const char * indexfile){return 0;};
	int save(const char * indexfile){return 0;};
	i64 getN(){return 0;};
	void Codedistribution(int &Plain, int &AL0, int &AL1, int &RL0, int &RL1, int &Fix){};
	int getAlphabetSize(){return 0;};
	i64 sizeInByte(){return 0;};
	i64 sizeInByteForCount(){return 0;};
	i64 sizeInByteForExtract(){return 0;};
	i64 sizeInByteForLocate(){return 0;};
	double compressRatio(){return 0;};
	double compressRatioForCount(){return 0;};
	double compressRatioForExtract(){return 0;};
	double compressRatioForLocate(){return 0;};
};

#endif

