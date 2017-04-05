/*============================================
# Filename: BitMap.h
# Ver 1.0 2014-06-08
# Copyright (C) 2014 ChenLonggang (chenlonggang.love@163.com)
#
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 or later of the License.
#
# Description: Hybrid-bitmap,support rank(int pos) and rank(int pos,int &bit)
=============================================*/
#ifndef WT_NODE_H
#define WT_NODE_H
#include<string.h>
#include"loadkit.h"
#include"savekit.h"
#include"InArray.h"
#include"BaseClass.h"
#include<math.h>
#include<iostream>
#include <map>
using namespace std;

class BitMap
{
	public:
		//static i64 Gamacount ;
		static i64 ALL0;
		static i64 ALL1;
		static i64 RL0 ;
		static i64 RL1 ;
		static i64 Fixcount  ;
		static i64 Plaincount;
		BitMap(unsigned long long int * bitbuff,i64 bit_len,int level,int block_size=1024,unsigned char label='\0',uchar ** tables=NULL);
		//bit_len:0,1串的实际长度，单位bit
		//level:层数
		//block_size:块大小
		//label:当前节点代表的字符.只有叶节点的label域又意义.
		
		BitMap(){};
		BitMap(uchar ** tables):Z(tables[0]),R(tables[1]){}
		~BitMap();

		i64 Rank(i64 pos);
		i64 Rank(i64 pos,int &bit);
		void Rank(i64 pos_left,i64 pos_right,i64 &rank_left,i64 &rank_right);


		void Left(BitMap * left);
		//设置左孩子
		
		BitMap * Left(){return left;};
		//返回左孩子

		void Right(BitMap * right);
		//...

		BitMap * Right(){return right;};
		//...
		//test
		 map<i64,i64> mapruns;
		//test
		unsigned char Label();
		int Load(loadkit & s);
		int Save(savekit & S);
		i64 SizeInByte();
		i64 SizeInSuperblock();
		i64 SizeIncodesytle();
		i64 SizeInblock();
		i64 SizeInMemory();
	      private:
		uchar* Z;
		//uchar* R1;
		//uchar* R2;
		//uchar* R3;
		//uchar* R4;
		uchar *R;
		BitMap(const BitMap &);
		BitMap & operator =(const BitMap& right);
		void Coding();

		//得到存储在data中的0,1串中的第index位
		int GetBit(u64 * data,i64 index);
		//从buff保存的0.1串中，由index位置开始，返回后续bits位表示的
		//数值.

		u16 Zeros(u16 x){return (Z[x>>8]==8)?Z[x>>8]+Z[(uchar)x]:Z[x>>8];}
		u64 GetBits(u64 * buff,i64 &index,int bits);
	
		//得到0的runs.
		i64 GetZerosRuns(u64 * buff,i64 &index);
		//gamma解
		

		i64 FixedDecode(u64 * buff,i64 &index,i64 Len);
		i64 GammaDecode(u64 * buff,i64 &index);
 		
		//得到0,1串中的runs长度，bit标示该runs是针对谁的
		i64 GetRuns(u64 * data,i64 &index,int &bit);
		//index64
		void Append_g(u64 * temp,i64 &index,u32 value);
		void Append_f(u64 * temp,i64 &index,u32 value,int maxrl);
		//把u64类型的value拷贝到data串的index处.
		void BitCopy(u64 * temp,i64 &index,u64 value);

		//返回rl0编码的串中，由index位置开始，长度位bits
		//内的1的个数.
		void RL_Rank(u64 * buff,i64 & index,int bits_left,int bits_right,i64 &rank_left,i64 &rank_right,int rl_type);
		i64  RL_Rank(u64 * buff,i64 & index,int bits_num,int rl_type);
		i64  RL_Rank(u64 * buff,i64 & index,int bits_num,int rl_type,int &bit);
		i64 FRL_Rank(u64 * buff,i64 & index,int bits_num,int rl_type,int &bit);
		i64 FRL_Rank(u64 * buff,i64 & index,int bits_num,int rl_type);
        void FRL_Rank(u64 *buff,i64 & index,int bits_left,int bits_right,i64 &rank_left,i64 &rank_right,int rl_type);
		//返回容量编码的串中，由index位置开始，bits位内的0的个数.
		i64 RL0_Rank(u64 * buff,i64 & index,i64 bits_num);
		i64 RL0_Rank(u64 * buff,i64 & index,i64 bits_num,int &bit);
		//返回容量编码的串中，由index位置开始，bits位内的1的个数.
		i64 RL1_Rank(u64 * buff,i64 & index,i64 bits);
		i64 RL1_Rank(u64 * buff,i64 & index,i64 bits,int &bit);
		//返回容量编码的串中，由index位置开始，bits位内的1的个数.
		i64 RL1_Bit(u64 * buff,i64 & index,i64 bits_num);
		i64 RL0_Bit(u64 * buff,i64 & index,i64 bits_num);
		i64 FRL1_Bit(u64 * buff,i64 & index,i64 bits_num);
		i64 FRL0_Bit(u64 * buff,i64 & index,i64 bits_num);
		//int RL1_Bit(u64 * buff,int & index,int bits);
		//int RL1_Rank(u64 * buff,int &index,int bits,int &bit);
		//buff从index位置开始是直接存储的，从index位置开始，bits
		//位内有几个1.
		void Plain_Rank(u64 *buff,i64 &index,i64 bits_left,i64 bits_right,i64 &rank_left,i64&rank_right);
		i64 Plain_Rank(u64 * buff,i64 &index,i64 bits);
		i64 Plain_Bit(u64 * buff,i64 &index,i64 bits);
		i64 Plain_Rank(u64 * buff,i64 &index,i64 bits,int & bit);

		
		int level;//该串的层数.
		
		unsigned char label;
		//只有叶节点又意义，表示该节点代表的字符

		unsigned long long int * data;
		//0,1串的压缩存储体.

		int bitLen;
		//0,1串的长度，单位bit。

		int memorysize;
		int block_size;
		int block_width;
		int super_block_size;

		BitMap * left;
		BitMap * right;

		InArray *superblock;//超快偏移量
		InArray *block;//块的偏移量
//		InArray *superblock_rank;//超快的偏移量
//		InArray *block_rank;//块的偏移量
		InArray *coding_style;//每个块的编码方案.0:plain, 1:RLG0, 2:RLG1;
		//wch add for fixed FixedDecode
		InArray *nbit;
		InArray *Flag;
		//wch add
		//这是个工作变量.
		unsigned long long int * buff;
};

#endif







