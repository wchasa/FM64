/*============================================
# Filename: ABS_WT.h
# Ver 1.0 2014-06-08
# Copyright (C) 2014 ChenLonggang (chenlonggang.love@163.com)
#
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 or later of the License.
#
# Description: 
  Abstract class for FM-index.the datamember root denotes
  the root bitmap of the tree.
=============================================*/
// #ifndef _GNU_SOURCE
// #define _GNU_SOURCE /* for O_DIRECT */
// #endif
#define __USE_FILE_OFFSET64
#define __USE_LARGEFILE64
#define _LARGEFILE64_SOURCE
#ifndef ABS_FM_H
#define ABS_FM_H
#define CHAR_SET_SIZE 256
#define CODE_MAX_LEN 256
#define _LARGEFILE64_SOURCE     /* See feature_test_macros(7) */
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include<string.h>
#include"BitMap.h"	
#include"InArray.h"
#include"loadkit.h"
#include"savekit.h"
#include"divsufsort64.h"
#include <map>

//#include"divsufsort_private.h"
class ABS_FM
{
	public:
		//D:SA数组采样步长,Rank采样步长D*16
		ABS_FM(const char * filename,int block_size=256,int D=32);
		ABS_FM(){};
		virtual ~ABS_FM();
		void Counting(const char * partten,i64 &num);
		i64 * Locating(const char * pattern,i64 &num);
		i64 * Locating_parrel(const char * pattern,i64 &num);
		unsigned char* Extracting(i64 pos,i64 len);
		unsigned char *Extracting_parrel(i64 pos, i64 len);
		int Load(loadkit &s);
		int Save(savekit & s);
		int BuildTree(int speedlevel=1);

		int GetAlphabetsize(){return alphabetsize;}
		int GetN(){return n;}
		int SizeInByte();
		int SizeInByte_count();
		void Codedistribution(int &Plain, int &AL0, int &AL1, int &RL0, int &RL1, int &Fix);
		BitMap *GetRoot();
		unsigned char * bwt;
	//test
		 i64 SizeOfpart(BitMap * r,string str);
		 map<i64,i64> BWTruns;
		 map<i64, i64> MergeBitMapRuns(BitMap *r);
		 map<i64, i64> getBitMapRuns();
		 map<i64, i64> getBitnodeRuns();
		 void getbwtRuns(unsigned char *bwt, int len);
		 i64 SizeInBytePart_count(string str);
		 map<i64,i64> MergeBitnodeRuns(BitMap *r);
		 //map<i64, i64> MergeBitMapRuns(BitMap *r);
		 i64 sizeOfSAL();
		 i64 sizeOfRankL();
		 //void ABS_FM::MapMerge(map<i64, i64> &map1, map<i64, i64> map2);
		 //test
	protected:
		BitMap * root;
		uchar * Z;
	 	uchar * R;
		 void Inittable();
		 //int BuildTree();

		 unsigned char *T;

		 unsigned char *filename;
		 i64 n;		 //file size in byte
		 int block_size; //the block_size of wavelettree's node.default 1024
		 int D;		 //step of SAL
		 InArray *SAL;   //the sampling of SA array
		 InArray *RankL; // The sampling of the Rank Array

		 bool charMap[256];
		 // if charMap[i] is true,char[i] is a member of alphabet

		 i64 *C; // Cumulative Frequency sum
		 //wch
		 //uchar *code;
		 i16 *code;
		 //wch
		 i64 charFreq[CHAR_SET_SIZE];
		 i32 alphabetsize;
		 char codeTable[CHAR_SET_SIZE][CODE_MAX_LEN];

		 i64 Occ(i64 &rank, unsigned char &label, i64 pos);
		 i64 Occ(unsigned char c, i64 pos);
		 void Occ(unsigned char c, i64 pos_left, i64 pos_right, i64 &rank_left, i64 &rank_right);
		 i64 LF(i64 i);
		 unsigned char L(int i);
		 void DrawBackSearch(const char *pattern, i64 &Left, i64 &Right);
		 i64 Lookup(i64 i);

		 virtual int TreeCode() { return -1; };

		 int BWT(unsigned char *T, int *SA, unsigned char *bwt, int n);
		 int BWT64(unsigned char *T, saidx64_t *SA, unsigned char *bwt, saidx64_t len);
		 BitMap *CreateWaveletTree(unsigned char *bwt, int n);
		 BitMap *FullFillWTNode(unsigned char *bwt, int len, int level);
		 int DestroyWaveletTree();
		 int blog(int);
		 unsigned char *Getfile(const char *filename);

		 int SaveNodePosition(BitMap *, u32, savekit &);
		 int SaveNodeData(BitMap *, savekit &s);
		 int SaveWTTree(savekit &s);
		 int LoadWTTree(loadkit &s, uchar **tables = NULL);

		 int TreeNodeCount(BitMap *root);

		 int TreeSizeInByte(BitMap *r);
		 //	int TreeSizeInByte();
		 friend int GammaDecode(u64 *buff, int &index, ABS_FM *t);
		 friend int Zeros(u16 x, ABS_FM *t);
		 void Test_L();
		 void Test_Occ();
		 void Test_Shape(BitMap *);


};
#endif

