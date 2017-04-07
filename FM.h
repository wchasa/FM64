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
class FM
{
	public:
		FM(const char * filename,int speedlevel=1);
		FM();
		~FM(){};
		FM(const FM & h):wt(h.wt){}
		FM& operator =(const FM&h){wt=h.wt;return *this;};
		
		void counting(const char *pattern,i64 &num);
		i64 * locating(const char *pattern,i64 & num);
		i64 * locating_parrel(const char *pattern,i64 & num);
		//void  GetMaps(Map<i64,i64> &bwtmap,Map<i64,i64> &runsmap);
		unsigned char * extracting(int pos,int len);
		unsigned char * extracting_parrel(int pos,int len);
		int load(const char * indexfile);
		int save(const char * indexfile);
		i64 getN();
		void Codedistribution(int &Plain, int &AL0, int &AL1, int &RL0, int &RL1, int &Fix);
		int getAlphabetSize();
		i64 sizeInByte();
		i64 sizeInByteForCount();
		double compressRatio();
		double compressRatioForCount();
		WT_Handle wt;
	private:
		//WT_Handle wt;
};
#endif

