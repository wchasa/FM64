	/*============================================
# Filename: BitMap.cpp
# Ver 1.0 2014-06-08
# Copyright (C) 2014 ChenLonggang (chenlonggang.love@163.com)
#
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 or later of the License.
#
# Description: 
=============================================*/
#include"BitMap.h"
#include<math.h>
#include<iostream>
#include <iomanip> 
using namespace std;
#define lookuptable
const int FixLenBit = 3;
inline int popcnt(unsigned long long int x)
{
	x = x -((x & 0xAAAAAAAAAAAAAAAA)>>1);
	x = (x & 0x3333333333333333)+((x>>2) & 0x3333333333333333);
	x =((x+(x>>4)) & 0x0F0F0F0F0F0F0F0F);
	return (x*0x0101010101010101)>>56;
}
int blog64(u64 x)
{
	int ans = 0;
	while(x>0)
	{
		ans++;
		x=x>>1;
	}
	return ans;
}

int blog(int x)
{
	int ans = 0;
	while(x>0)
	{
		ans++;
		x=x>>1;
	}
	return ans;
}

BitMap::BitMap(unsigned long long int * bitbuff,i64 bit_len,int level,int block_size,unsigned char label,uchar ** tables)
{
	this->data = bitbuff;
	this->bitLen = bit_len;
	this->memorysize = 0;
	this->level = level;
	this->block_size = block_size;
	this->block_width = blog(block_size);
	this->super_block_size = 16*block_size;
	this->label = label;
	left=NULL;
	right=NULL;
	superblock =NULL;
	block=NULL;
	coding_style=NULL;
	R=NULL;
	Z=NULL;
	
	if(data!=NULL)
	{
		this->Z = tables[0];
		this->R = tables[1];
		Coding();
		
		buff =NULL;
	}

}

i64 BitMap::SizeInByte()
{

	int size = 0;
	if(data!=NULL)
	{
		//return bitLen/8;
		size+= superblock->GetMemorySize();
		size+= block->GetMemorySize();
		size+= coding_style->GetMemorySize();
		size+= memorysize;
	
	}
	return size;
}
i64 BitMap::SizeInSuperblock()
{
	int size = 0;
	if(data!=NULL)
		size+= superblock->GetMemorySize();
    return size;
}
i64 BitMap::SizeInblock()
{
	int size = 0;
	if(data!=NULL)
		size+= block->GetMemorySize();
    return size;
}
i64 BitMap::SizeIncodesytle()
{
	int size = 0;
	if(data!=NULL)
		size+= coding_style->GetMemorySize();
    return size;
}
i64 BitMap::SizeInMemory()
{
	int size = 0;
	if(data!=NULL)
		size+= memorysize;
    return size;
}

		//return bitLen/8;
// void BitMap::Coding()
// {
// 	u64 * temp = new u64[256];
// 	memset(temp,0,256*8);
// 	//u64 * temp = new u64[256];
// 	int index2 = 0;
// 	for(int i =0;i<64;i++)
// 	{
// 		//Append_f(temp,index2,runs_tmp[i],maxrl);
// 		Append_f(temp,index2,i,6);
// 	}

// }
i64	BitMap::Fixcount   = 0;
i64 BitMap::ALL0 	   = 0;
i64 BitMap::ALL1	   = 0;
i64 BitMap::RL0 	   = 0;
i64 BitMap::RL1 	   = 0;
i64 BitMap::Plaincount = 0;
i64 BitMap::plainSize = 0;
i64 BitMap::FixSize = 0;
i64 BitMap::RLSize = 0;
i32 BitMap::Block_size = 0;
i32 BitMap::superblock_size = 0;
map<i64,i64> BitMap::mapruns;
map<i64,i64> BitMap::noteruns;
vector<int> BitMap::ConvertGapTODiv(vector<int> gaps,i64& firstbit)
{
    vector<int> divs;
	if(gaps.size()<=0)
		return gaps;
    divs.push_back(gaps[0]);
    for(int i = 0 ;i<gaps.size()-1;i++){
        divs.push_back(gaps[i+1] - gaps[i]);
    }
    return divs;
}
vector<int> BitMap::ConvertRLtoDiv(int * run,int len,i64& firstbit)
{
	return ConvertGapTODiv(ConvertRLtoGap(run,len,firstbit),firstbit);
}
vector<int> BitMap::ConvertRLtoGap(int * run,int len,i64& firstbit)
{
    vector<int> gaps1,gaps2;
    int curpos = 0;
    int curgap = 0;
    // for(auto i : values){   
    for(int i = 0;i<len;i++){   
        for(int j = 0 ;j< run[i];j++){
            if(curgap%2 ==0)
                gaps1.push_back(curpos++);
            else
                gaps2.push_back(curpos++);
            
        }
        curgap++;
    }
	// if(gaps1.size() == 0|| gaps2.size()==0){
	// 	cout<<__LINE__<<":123"<<endl;
	// }
    if(gaps1.size()>gaps2.size()){
		
        return gaps2;
	}
    else{
		firstbit = 1-firstbit;
        return gaps1;
	}

}
void BitMap::Coding()
{
	int u64Len =0;
	if(bitLen%64 == 0)
		u64Len = bitLen/64;
	else
		u64Len = bitLen/64+1;
	u64 * temp = new u64[u64Len];
	memset(temp,0,u64Len*8);
	
	i64 index = 0;
	i64 step1 = block_size*16;//supersize
	i64 step2 = block_size;//bsize
	//int block_width = blog(block_size);
	superblock = new InArray(2*(bitLen/step1)+2,log2(bitLen)+1);
//
	block      = new InArray(2*(bitLen/step2)+2,blog(step1-step2));
	coding_style      = new InArray(bitLen/step2+1,3);
//wch add
    nbit 	   = new InArray(2*(bitLen/step1)+2,blog(step1-step2));
	Flag	   = new InArray(2*(bitLen/step1)+2,1);
	int maxrl=0;
	int divrl = 0;
	int maxrlgap=0;
	int maxtotal =0;
	int rl_gap = 0;
	int maxgaptotal =0;
//wch add

	i64 rank=0;
	i64 space=0;
	i64 bits =0;
	i64 firstbit;
	i64 gapfirstbit;
	int rl_g=0;
	int runs = 0;
	int bit=0;
	int * runs_tmp = new int[block_size];
	i64 k=0;
	i64 index2=0;
	int cs = 0;
	i64 pre_rank=0;
	i64 pre_space =0 ;
	superblock->SetValue(0,0);
	superblock->SetValue(1,0);
	block->SetValue(0,0);
	block->SetValue(1,0);
	//todo
	while(index < bitLen)
	{
		if(index == bitLen)
			break;
		rl_g = 0;
		rl_gap=0;
		bits = 0;
		firstbit = 0;
		runs = 0;
		firstbit = GetBit(data,index);
		gapfirstbit = firstbit;
		memset(runs_tmp,0,block_size*4);
		maxrl=0;
		maxtotal =0;
		maxrlgap = 0;
		maxgaptotal = 0;
		k=0;
 		runs=0;
		 //
		while(bits < block_size && index < bitLen)
		{

			runs = GetRuns(data,index,bit);
			bits = bits +runs;
			if(bit ==1)
				rank=rank+runs;
			runs_tmp[k] = runs;
			k++;

		}
		
		if(bits > block_size)
		{
			int step =0;
			index = index -(bits - block_size);
			step = block_size+runs-bits;
			if(bit ==1)
				rank = rank -runs+step;
			runs_tmp[k-1] = step;
		}

		//wch 
		auto gaps = ConvertRLtoDiv(runs_tmp,k,gapfirstbit);
		// cout<<__LINE__<<"Div:size:"<<gaps.size()<<":";index>datanum-1
		for(auto i : gaps){
			// cout<<i<<";";
			maxrlgap = max(maxrlgap,i);
			rl_gap = rl_gap + 2*blog(i)-1;
		}
		// cout<<endl;
		// cout<<__LINE__<<"runs:size:"<<k<<":";
		for(int i=0;i<k;i++)
			{
				//  cout<<runs_tmp[i]<<";";
				rl_g = rl_g + 2*blog(runs_tmp[i])-1;
				maxrl = max(maxrl,runs_tmp[i]);
			}
			//  cout<<endl;
			//maxtotal =blog(maxrl)*k;
			maxrl = blog(maxrl);//maxrl bit length
			// maxtotal =FixLenBit+maxrl*k;
			maxrlgap = blog(maxrlgap);
			if(maxrlgap ==0){
				maxrlgap=1;
			}
			maxgaptotal =FixLenBit+maxrlgap*gaps.size();
		//wch
		int thred=20;
		//todo judge
		int len = min(rl_g,block_size-thred);
		// len = min(len,rl_gap);
		// len = min(len,maxtotal);
		len = min(len,maxgaptotal);
		// cout<<"rl_g:"<<rl_g<<";maxtotal:"<<maxtotal<<";maxgtotalgap:"<<maxgaptotal<<endl;
		// cout<<endl;
		// if (maxtotal<len&&k!=1)
		// {
		// 	maxtotal =2*blog(maxrl)-1+blog(maxrl)*k;
		// 	cout<<"11";
		// }
		if(k==1)
		{
			// cout<<__LINE__<<"ALL"<<endl;
			if(firstbit==0)
				{	
					ALL0++;
					coding_style->SetValue((index-1)/block_size,3);//ALL0
					cs = 3;
				}
			else
				{
					ALL1++;
					coding_style->SetValue((index-1)/block_size,4);//ALL1
					cs =4;
				}
			space = space +0;
		}

		else if(len == (block_size-thred) || index == bitLen)//plain
		{
			// cout<<__LINE__<<"Plain"<<endl;
			Plaincount++;
			coding_style->SetValue((index-1)/block_size,2);
			cs = 2;
			int j=0;
			int num=0;
			if(index == bitLen)
			{
			    plainSize += bits;
			    space = space + bits;
			    j = (index - bits) / 64;
			    num = bits % 64 ? bits / 64 + 1 : bits / 64;
			}
			else
			{
			    plainSize += block_size;
			    space = space + block_size;
			    j = (index - block_size) / 64;
			    num = block_size / 64;
			}
			for(int kk=0;kk<num;kk++,j++)
				BitCopy(temp,index2,data[j]);
		}
		//p (int[256])*runs_tmp
		// else if(maxtotal>=len && maxgaptotal >=len)//rl_gamma
		else if(len==rl_g)//rl-gama
		{
			// cout<<__LINE__<<"RL"<<endl;
			RLSize+=rl_g;
			RL0++;
			//Gamacount++;
			if(firstbit == 0)
				{
					coding_style->SetValue((index-1)/block_size,0);//RLG0
					cs = 0;
				}
			else
				{
					coding_style->SetValue((index-1)/block_size,1);//RLG1
					cs =1;
				}
			space =space + rl_g;
			if(rl_g!=len)
			{
				cout<<"rl_gama rl_g!=len!";
			}
			for(int i=0;i<k;i++)
			{
				//cout<<runs_tmp[i]<<endl;
				Append_g(temp,index2,runs_tmp[i]);
			}
		}
		// else if(len == rl_gap){
		// 	RLSize+=rl_g;
		// 	//Gamacount++;
		// 	if(firstbit == 0)
		// 		{
		// 			RL0++;
		// 			coding_style->SetValue((index-1)/block_size,0);//RLG0
		// 			cs = 0;
		// 		}
		// 	else
		// 		{
		// 			coding_style->SetValue((index-1)/block_size,1);//RLG1
		// 			cs =1;
		// 			RL1++;
		// 		}
		// 	space =space + rl_g;
		// 	if(rl_gap!=len)
		// 	{
		// 		cout<<"rl_gama rl_g!=len!";
		// 	}
		// 	for(int i=0;i<gaps.size();i++)
		// 	{
		// 		//cout<<runs_tmp[i]<<endl;
		// 		Append_g(temp,index2,gaps[i]);
		// 	}
		// }
		// else if(maxtotal == len)//fixcoding
		// {	
		// 	// cout<<"maxtotal:"<<maxtotal<<";";
		// 	// cout<<"maxtotalgap:"<<maxgaptotal<<";";
		// 	// cout<<"rllen:"<<rl_g<<";"<<endl;
		// 	// if(maxrl == 8){
		// 	// 	cout<<"HEADLEN:"<<maxrl<<endl;
		// 	// }
		// 	FixSize += maxtotal;
		// 	Fixcount++;
		// 	if(firstbit == 0)
		// 	{
		// 			coding_style->SetValue((index-1)/block_size,5);//Fix0
		// 			cs =5;
		// 	}
		// 	else
		// 		{
		// 			coding_style->SetValue((index-1)/block_size,6);//Fix1
		// 			cs =6;
		// 		}
		// 	space =space + maxtotal;
		// 	// Append_g(temp,index2,maxrl);
		// 	// if(maxrl>4)
		// 	Append_f(temp,index2,maxrl,FixLenBit);
		// 	// else
		// 	// 	cout<<__LINE__<<"WRONG";
		// 	// cout<<__LINE__<<":fix"<<endl;
		// 	// cout<<"runs:"<<endl;
		// 	for(int i=0;i<k;i++)
		// 	{
		// 		// cout<<runs_tmp[i]<<";";
		// 		Append_f(temp,index2,runs_tmp[i],maxrl);
		// 	}
		// 	// cout<<endl;
		// 	// cout<<"line"<<"divs"<<endl;
		// 	// for(auto v : gaps)
		// 	// 	cout<<v<<";";
		// 	// cout<<endl;
		// 	// cout<<endl;

		// }
		else{	
			// cout<<"maxtotal:"<<maxtotal<<";";
			// cout<<"maxtotalgap:"<<maxgaptotal<<";";
			// cout<<"rllen:"<<rl_g<<";"<<endl;
			// cout<<__LINE__<<"fix"<<endl;
			// if(maxrlgap == 8)
			// 	cout<<"HEADLEN:"<<maxrlgap<<endl;
			// FixSize += maxtotal;
			RL1++;
            auto indextemp = index2;
			if(gapfirstbit == 0)
			{
					coding_style->SetValue((index-1)/block_size,5);//记录０的位置
					cs =5;
			}
			else
				{
					coding_style->SetValue((index-1)/block_size,6);//记录１的位置
					cs =6;
				}
			space =space + maxgaptotal;
			// Append_g(temp,index2,maxrlgap);
			Append_f(temp,index2,maxrlgap-1,FixLenBit);
			// if(maxrl>4)
			// 	Append_f(temp,index2,maxrl-5,2);
			// else
			// 	cout<<__LINE__<<"WRONG";
			// cout<<__LINE__<<":fix"<<endl;
			// cout<<"runs:"<<endl;
			u64* temp3 = new u64[10];
			memset(temp3,0,sizeof(u64)*10);
			i64 index3g = 0;
			Append_f(temp3,index3g,maxrlgap-1,FixLenBit);
			for(int i=0;i<gaps.size();i++)
			{
				// cout<<gaps[i]<<";";
				Append_f(temp,index2,gaps[i],maxrlgap);
			}

		}
		//打表顺序，superblock在前,block在后.
		if(index % step1 == 0)
		{
			pre_rank = rank;
			superblock ->SetValue(2*(index/step1),pre_rank);

			pre_space = space;
			superblock->SetValue(2*(index/step1)+1,pre_space);
		}
		if(index % step2 ==0)
		{	
			// assert(space-pre_space<256);
			block->SetValue(2*(index/step2),rank - pre_rank);
			block->SetValue(2*(index/step2)+1,space - pre_space);
		}
		//test
		/*if(cs == 1||cs ==0)
		{
			for(int i2=0;i2<k;i2++)
			{
				map<i64, i64>::iterator it = mapruns.find(runs_tmp[i2]);
				if (it == mapruns.end())
				{
					mapruns.insert(pair<i64, i64>(runs_tmp[i2], 1));
				}
				else
				{
					mapruns[runs_tmp[i2]]++;
				}
			}
		}*/
		
	//test
	}
	//test
	Block_size = this->block_size;
	superblock_size=this->super_block_size;
	int bitVal = 0;
	int bit_one = 0, bit_zero = 0;
	for (i64 i = 0, j = 0; i < bitLen; i++)
	{
	    bitVal = GetBit(data, i);
	    if (bitVal == 1)
	    {
		bit_one++;
		if (bit_zero > 0)
		{
		    noteruns[bit_zero]++;
		    bit_zero = 0;
		}
	    }
	    else
	    {
		bit_zero++;
		if (bit_one > 0)
		{
		    noteruns[bit_one]++;
		    //if(bit_one>10000)
		    //	cout<<bit_one<<","<<noteruns[bit_one]<<endl;
		    bit_one = 0;
		}
	    }
	}
	if(bit_one > 0){
		noteruns[bit_one]++;
		bit_one = 0;
	}
	if(bit_zero > 0){
		noteruns[bit_zero]++;
		bit_zero = 0;
	}
	//maprun
	int32_t coding_len = coding_style->GetNum();
	for(int32_t i = 0, j; i < coding_len; i++){
		j = coding_style->GetValue(i);
		if(j == 0 || j == 1||j==5||j==6)
		{ //run length 0/1
			//Static::numOfRlg0_1++;
			int32_t k = i * block_size;
			int32_t cnt = 0;
			bit_zero = 0, bit_one = 0;
			for(; cnt < block_size && cnt + k < bitLen; cnt++){
				bitVal = GetBit(data, cnt + k);

				if(bitVal == 1){
					bit_one++;
					if(bit_zero > 0){
						mapruns[bit_zero]++;
						bit_zero = 0;
					}
				} else {
					bit_zero++;
					if(bit_one > 0){
						mapruns[bit_one]++;
						bit_one = 0;
					}
				}
			}//end-of-for
			if(bit_one > 0){
				mapruns[bit_one]++;
				bit_one = 0;
			}
			if(bit_zero > 0){
				mapruns[bit_zero]++;
				bit_zero = 0;
			}

		}//end-of-if
	}//end-of-for
	//test
	//释放runs_tmp
	delete [] runs_tmp;
	int u64_len_real = 0;
	if(space % 64==0)
		u64_len_real = space /64+1;
	else
		u64_len_real = space /64 +1+1;
	
	this->memorysize = u64_len_real*8;
	delete [] data;
	data = new u64[u64_len_real];

	memset(data,0,u64_len_real*8);
	memcpy(data,temp,(u64_len_real-1)*8);
	delete [] temp;
	//cout<<"Plain:"<<setw(10)<<Plaincount<<",RLcount"<<setw(10)<<Gamacount<<",FixCount="<<setw(10)<<Fixcount<<endl;
}

BitMap::~BitMap()
{
	if(left)
		delete left;
	if(right)
		delete right;
	delete [] data;
	delete superblock;
	delete block;
	delete coding_style;
}

//todo
i64 BitMap::Rank(i64 pos,int & bit)
{
	assert(0);
	if(pos < 0 || pos > bitLen)
	{
		cerr<<"BitMap::Rank(i64, int&) error parmater"<<endl;
		cout<<"Pos = "<<pos<<",BitLen = "<<bitLen<<endl;
		exit(0);
	}

	if((pos+1)%block_size!=0)//
	{
		i64 block_anchor      = (pos+1)/block_size;
		i64 superblock_anchor = (pos+1)/super_block_size;
		i64 type              = coding_style->GetValue(pos/block_size);
		i64 rank1             = superblock->GetValue(superblock_anchor*2);
		i64 offset1           = superblock->GetValue(superblock_anchor*2+1);
		i64 rank_base         = rank1+block->GetValue(block_anchor*2);
		i64 offset            = offset1 + block->GetValue(block_anchor*2+1);
		buff                  = data +(offset>>6);
		i64 overloop          = (pos+1)%block_size ;
		i64 index             = (offset &0x3f);
		i64 rank              = 0;
		i64 curblocksize 	  = GetCurrentBlocksize(block_anchor,superblock_anchor,offset1,offset);
		switch(type)
		{
			case 0:rank = RL_Rank(buff,index,overloop,0,bit);break;
			case 1:rank = RL_Rank(buff,index,overloop,1,bit);break;
			case 2:rank = Plain_Rank(buff,index,overloop,bit);break;
			case 3:rank = 0;bit=0;break;
			case 4:rank = overloop;bit = 1;break;
			case 5:rank = FRL_Rank_gap(buff,index,overloop,0,bit,curblocksize);break;
			case 6:rank = FRL_Rank_gap(buff,index,overloop,1,bit,curblocksize);break;
		}
		//cout<<"Rank="<<rank_base+rank<<endl;
		return rank_base + rank;
	}
	else//
	{
		i64 rank1   = superblock->GetValue(((pos+1)/super_block_size)*2);
		i64 offset1 = superblock->GetValue((pos/super_block_size)*2+1);
		i64 rank = rank1 + block->GetValue(((pos+1)/block_size)*2);
		i64 offset = offset1 + block->GetValue((pos/block_size)*2+1);

		i64 index = (offset&0x3f);
		buff = data+(offset>>6);
		i64 type = coding_style->GetValue(pos/block_size);
		i64 overloop = block_size;
		switch(type)
		{
			case 0:bit=RL0_Bit(buff,index,overloop);break;
			case 1:bit=RL1_Bit(buff,index,overloop);break;
			case 2:bit=Plain_Bit(buff,index,overloop);break;
			case 3:bit=0;break;
			case 4:bit=1;break;
			case 5:bit=FRL0_Bit(buff,index,overloop);break;
			case 6:bit=FRL1_Bit(buff,index,overloop);break;
		}
		//cout<<"Rank="<<rank<<endl;
		return rank;

	}
}


void BitMap::Rank(i64 pos_left,i64 pos_right,i64 &rank_left,i64 &rank_right)
{
/*	
	rank_left= Rank(pos_left);
	rank_right=Rank(pos_right);
*/
	if(pos_left<0 || pos_right <0 || pos_left > bitLen || pos_right > bitLen)
	{
		cerr<<"BitMap::Rank(int,int,int&,int&) error parmater"<<endl;
		exit(0);
	}

	int block_anchor = (pos_left+1)/block_size;
	if(block_anchor==(pos_right+1)/block_size)//同一个超快内
	{
		int superblock_anchor=(pos_left+1)/super_block_size;
		int rank_base = superblock->GetValue(superblock_anchor<<1)+block->GetValue(block_anchor<<1);
		int offset1 = superblock->GetValue((superblock_anchor<<1)+1);
		int offset = offset1+block->GetValue((block_anchor<<1)+1);
		int type = coding_style->GetValue(block_anchor);
		int overloop_left = (pos_left+1)%block_size;
		int overloop_right= (pos_right+1)%block_size;
		i64 curblocksize = GetCurrentBlocksize(block_anchor,superblock_anchor,offset1,offset);
		buff = data+ (offset>>6);
		i64 index = (offset&0x3f);
		rank_left = rank_right =rank_base;
		if(overloop_left!=0)
		{
			switch(type)
			{
				case 0:RL_Rank(buff,index,overloop_left,overloop_right,rank_left,rank_right,0);break;
				case 1:RL_Rank(buff,index,overloop_left,overloop_right,rank_left,rank_right,1);break;
				case 2:Plain_Rank(buff,index,overloop_left,overloop_right,rank_left,rank_right);break;
				case 3:break;
				case 4:rank_left +=overloop_left;rank_right += overloop_right;break;
				case 5:FRL_Rank_gap(buff,index,overloop_left,overloop_right,rank_left,rank_right,0,curblocksize);break;
				case 6:FRL_Rank_gap(buff,index,overloop_left,overloop_right,rank_left,rank_right,1,curblocksize);break;
			}
			return ;
		}
		switch(type)
		{   
			case 0:rank_right += RL_Rank(buff,index,overloop_right,0);break;
			case 1:rank_right += RL_Rank(buff,index,overloop_right,1);break;
			case 2:rank_right += Plain_Rank(buff,index,overloop_right);break;
			case 3:break;
			case 4:rank_right += overloop_right;break;
			case 5:FRL_Rank(buff,index,overloop_left,overloop_right,rank_left,rank_right,0);break;
			case 6:FRL_Rank(buff,index,overloop_left,overloop_right,rank_left,rank_right,1);break;
		}
	}
	else
	{
		rank_left= Rank(pos_left);
		rank_right=Rank(pos_right);
	}

}

i64 BitMap::GetCurrentBlocksize(i64 block_anchor,i64 superblock_anchor,i64 offset1,i64 preoffset)
{
	i64 offset_1 = 0;
	if (((block_anchor+1)<<1) >block->Getdatanum()-1)
		cout<<endl;
	else if((block_anchor+1) % 64  != 0)
		offset_1 = offset1 + block->GetValue(((block_anchor+1)<<1)+1);
	else
		offset_1 = superblock->GetValue(((superblock_anchor+1)<<1)+1);
	
	return offset_1 -preoffset;
}
i64 BitMap::Rank(i64 pos)
{//bug:type value is different;
	if (pos<0 || pos > bitLen)
	{
		cerr<<"BitMap::Rank  error paramater"<<endl;
		cerr<<pos<<" "<<bitLen<<endl;
		exit(0);
	}

	i64 block_anchor = (pos+1)/block_size;
	i64 superblock_anchor  = ((pos+1)/super_block_size);
	int type = coding_style->GetValue(block_anchor);
	
	i64 rank1 = superblock->GetValue(superblock_anchor<<1);
	i64 offset1 = superblock->GetValue((superblock_anchor<<1)+1);
	i64 rank_base = rank1 + block->GetValue(block_anchor<<1);
	i64 offset    = offset1 + block->GetValue((block_anchor<<1)+1);
	
	// i64 offset_1    = 0;
	i64 curblocksize = GetCurrentBlocksize(block_anchor,superblock_anchor,offset1,offset);
	// cout<<offset_1-offset<<endl;
	// if(offset>offset_1)
	// {
	// 	cout<<__LINE__<<"wrong"<<endl;
	// }
	
	buff = data + (offset>>6);
	int overloop = (pos+1)%block_size ;
	i64 index = (offset & 0x3f);
	if(overloop > 0)
	{
		switch(type)//todo
		{
			case 0:rank_base += RL_Rank(buff,index,overloop,0);break;//rg0
			case 1:rank_base += RL_Rank(buff,index,overloop,1);break;//rg1
			case 2:rank_base += Plain_Rank(buff,index,overloop);break;//plain
			case 3:break;//ALL0
			case 4:rank_base += overloop;break;//ALL1
			case 5:rank_base += FRL_Rank_gap(buff,index,overloop,0,curblocksize);break;//fix0
			case 6:rank_base += FRL_Rank_gap(buff,index,overloop,1,curblocksize);break;//fix1
		}
	}
	return rank_base;

}

//get designated position(index) bit value
int BitMap::GetBit(u64 * data,i64 index)
{
	int anchor = index/64;
	int pos = 63-index%64;
	return ((data[anchor] &(0x01ull<<pos))>>pos);
}


//2014.5.8:16:53:这三段程序的性能相当.
//
i64 BitMap::GetRuns(u64 * data,i64 &index,int &bit)
{
	bit = GetBit(data,index);
	index = index +1;
	int totle_runs = 1;
	int runs=0;
	
	while(totle_runs < block_size)
	{
		u16 x= GetBits(data,index,16);//index不联动
		if(bit==1)
			x=(~x);
		runs = Zeros(x);
		totle_runs +=runs;
		index+=runs;
		if(runs < 16)
			break;
	}
	return totle_runs;
}


//gamma编码,index联动
void BitMap::Append_g(u64 *temp,i64 &index,u32 value)
{
	u64 y=value;
	int zerosnum = blog(value)-1;
	index+=zerosnum;
	int onesnum = zerosnum+1;
	if(index%64 + onesnum < 65)//in the same longlong
	{
		temp[index/64] = (temp[index/64] | (y<<(64-(index%64 + onesnum))));
	}
	else
	{
		int first = 64 - index%64;
		int second = onesnum - first;
		temp[index/64] = (temp[index/64] | (y>>second));
		temp [index/64 +1] = (temp[index/64+1] | (y<<(64-second)));
	}
	index = index + onesnum;
}
//fix code and add to temp
void BitMap::Append_f(u64 *temp,i64 &index,u32 value,int maxrl)
{
	u64 y=value;
	int valuenum = blog(value);
	int zerosnum = maxrl-valuenum;
	index+=zerosnum;
	int onesnum = valuenum;
	if(index%64 + onesnum < 65)//in the same longlong
	{
		temp[index/64] = (temp[index/64] | (y<<(64-(index%64 + onesnum))));
	}
	else
	{
		int first = 64 - index%64;
		int second = onesnum - first;
		temp[index/64] = (temp[index/64] | (y>>second));
		temp [index/64 +1] = (temp[index/64+1] | (y<<(64-second)));
	}
	index = index + onesnum;
}


void BitMap::BitCopy(u64 * temp,i64 & index,u64 value)
{
	if(index%64!=0)
	{
		int first = 64 - index % 64;
		int second = 64 - first;
		temp[index/64] = (temp[index/64] | (value>>second));
		temp[index/64 + 1] = (temp[index/64+1] | (value<<first));
	}
	else
		temp[index/64]  = value;
	index = index +64;
}

i64 BitMap::RL0_Rank(u64 * buff,i64 &index,i64 bits_num)
{
	int bit=0;
	return RL0_Rank(buff,index,bits_num,bit);
}

i64 BitMap::RL0_Rank(u64 * buff,i64 &index,i64 bits_num,int &bit)
{
	i64 rank = 0;
	i64 bit_count = 0;
	i64 bits = 0;
	while(true)
	{
		 bits=GammaDecode(buff,index);
		 bit_count = bit_count + bits;
		 if(bit_count >= bits_num)
		 {
			 bit = 0;
			 return rank;
		 }
		 bits=GammaDecode(buff,index);
		 bit_count = bit_count + bits;
		 rank = rank + bits;
		 if(bit_count >= bits_num)
		 {
			 bit = 1;
			 return rank - (bit_count-bits_num);
		 }
	}
}

i64 BitMap::RL0_Bit(u64 * buff,i64 &index,i64 bits_num)
{
	//int rank = 0;
	int bit_count =0;
	int bits = 0;
	while(true)
	{
		bits=GammaDecode(buff,index);
		bit_count = bit_count + bits;
		if(bit_count >= bits_num)
			return  0;
		bits=GammaDecode(buff,index);
		bit_count = bit_count + bits;
		if(bit_count >= bits_num)
			return 1;
	}
}
// i64 BitMap::FRL0_Bit_gap(u64 * buff,i64 &index,i64 bits_num,int len)
// {
// 	//int rank = 0;
// 	int bit_count =0;
// 	//int bits = 0;
// 	//int  bits = 0;
// 	//get block len
// 	int len = 0; 
// 	int value = 0;	
// 	u32 x = GetBits(buff,index,32);
// 	int runs = Zeros(x>>16);
// 	int bits = (runs<<1)+1;
// 	index = index + bits;
// 	value = x>>(32-bits);
// 	while(true)
// 	{
// 		bits=FixedDecode(buff,index,value);
// 		bit_count = bit_count + bits;
// 		if(bit_count >= bits_num)
// 			return  0;
// 		bits=FixedDecode(buff,index,value);
// 		bit_count = bit_count + bits;
// 		if(bit_count >= bits_num)
// 			return 1;
// 	}
// }
i64 BitMap::FRL0_Bit(u64 * buff,i64 &index,i64 bits_num)
{
	//int rank = 0;
	int bit_count =0;
	//int bits = 0;
	//int  bits = 0;
	//get block len
	int len = 0; 
	int value = 0;	
	u32 x = GetBits(buff,index,32);
	int runs = Zeros(x>>16);
	int bits = (runs<<1)+1;
	index = index + bits;
	value = x>>(32-bits);
	while(true)
	{
		bits=FixedDecode(buff,index,value);
		bit_count = bit_count + bits;
		if(bit_count >= bits_num)
			return  0;
		bits=FixedDecode(buff,index,value);
		bit_count = bit_count + bits;
		if(bit_count >= bits_num)
			return 1;
	}
}
i64 BitMap::RL1_Rank(u64 * buff,i64 &index,i64 bits_num)
{
	int bit =0;
	return RL1_Rank(buff,index,bits_num,bit);
}

i64 BitMap::RL1_Rank(u64 * buff,i64 &index,i64 bits_num,int &bit)
{
	int rank = 0;
	int bit_count = 0 ;
	int  bits = 0;
	while(true)
	{
		 bits = GammaDecode(buff,index);
		 bit_count = bit_count + bits;
		 rank = rank + bits;
		 if(bit_count >= bits_num)
		 {
			 bit = 1;
			 return rank - (bit_count-bits_num);
		 }
		 bits = GammaDecode(buff,index);
		 bit_count = bit_count + bits;
		 if(bit_count >= bits_num)
		 {
			 bit = 0;
			 return rank;
		 }
	}
}

i64 BitMap::RL1_Bit(u64 * buff,i64 &index,i64 bits_num)
{
	int bit_count = 0 ;
	int  bits = 0;
	while(true)
	{
		 bits = GammaDecode(buff,index);
		 bit_count = bit_count + bits;
		 if(bit_count >= bits_num)
			 return 1;
		 bits = GammaDecode(buff,index);
		 bit_count = bit_count + bits;
		 if(bit_count >= bits_num)
			 return 0;
	}
}

i64 BitMap::FRL1_Bit(u64 * buff,i64 &index,i64 bits_num)
{
	int bit_count = 0;
	//int  bits = 0;
	//get block len
	int len = 0; 
	int value = 0;	
	u32 x = GetBits(buff,index,32);
	int runs = Zeros(x>>16);
	int bits = (runs<<1)+1;
	index = index + bits;
	value = x>>(32-bits);
	while(true)
	{
		bits=FixedDecode(buff,index,value);
		bit_count = bit_count + bits;
		if(bit_count >= bits_num)
			return  1;
		bits=FixedDecode(buff,index,value);
		bit_count = bit_count + bits;
		if(bit_count >= bits_num)
			return 0;
	}
}
//palin类型的比例较低，所以两种方式的区别不大.
void BitMap::Plain_Rank(u64 *buff,i64 &index,i64 bits_left,i64 bits_right,i64 &rank_left,i64&rank_right)
{
	
	int rank = Plain_Rank(buff,index,bits_left);
	rank_left += rank;
	index++;
	buff = buff +(index>>6);
	index = (index &0x3f);
	rank_right+= (Plain_Rank(buff,index,bits_right-bits_left)+rank);

/*	
   rank_left += Plain_Rank(buff,index,bits_left);
   index = index +1 - bits_left;
   rank_right+= Plain_Rank(buff,index,bits_right);
  */

}

i64 BitMap::Plain_Rank(u64 * buff,i64 &index,i64 bits_num,int &bit)
{
	if((index &0x3f) + bits_num < 65)
	{
		u64 temp = (buff[index>>6]<<(index&0x3f))>>(64-bits_num);
		index = index + bits_num -1;
		bit=(buff[index>>6]>>(63-(index&0x3f)))&0x01;
		return popcnt(temp);
	}
	int rank = 0;
	int head = 64 - (index&0x3f);
	u64 temp = (buff[index>>6]<<(index&0x3f));
	rank = rank + popcnt(temp);
	bits_num = bits_num - head;
	int times = bits_num>>6;
	int i=0;
	for(i=0;i<times;i++)
	rank = rank + popcnt(buff[i+(index>>6)+1]);
	if((bits_num&0x3f)!=0)
		rank = rank + popcnt((buff[i+(index>>6)+1] >> (64-(bits_num&0x3f))));
	index = index + head + bits_num - 1;
	bit=(buff[index>>6]>>(63-(index&0x3f)))&0x01;
	return rank;
}

i64 BitMap::Plain_Bit(u64 * buff,i64 &index,i64 bits_num)
{
	index = index + bits_num - 1;
	return (buff[index>>6]>>(63-(index&0x3f)))&0x01;
}



i64 BitMap::Plain_Rank(u64 * buff,i64 &index,i64 bits_num)
{
	int bit=0;
	return Plain_Rank(buff,index,bits_num,bit);
}

i64 BitMap::GammaDecode(u64 * buff,i64 & index)
{
	u32 x = GetBits(buff,index,32);
	int runs = Zeros(x>>16);
	int bits = (runs<<1)+1;
	index = index + bits;
	return x>>(32-bits);
}
i64 BitMap::FixedDecode(u64 * buff,i64 &index,i64 Len)
{
	u32 x = GetBits(buff,index,32);
	//int runs = Len;
	int bits = Len;
	index = index + bits;
	return x>>(32-bits);
}

//从buff的index位置开始,读取bits位数据,返回.
u64 BitMap::GetBits(u64 * buff,i64 &index,int bits)
{

	if((index & 0x3f) + bits < 65)
		return (buff[index>>6]<<(index &0x3f))>>(64-bits);

	int first = 64 - (index &0x3f);
	int second = bits - first;
	u64 high = (buff[index>>6] & ((0x01ull<<first)-1)) << second;
	return high + (buff[(index>>6)+1]>>(64-second));


}

i64 BitMap::GetZerosRuns(u64 * buff,i64 &index)
{
        u32 x = GetBits(buff,index,16);
        int runs = Zeros(x);
        index = index + runs;
        return runs;
}


void BitMap::Left(BitMap * left)
{
     this->left = left;
}


void BitMap::Right(BitMap * right)
{
     this->right = right;
}
 
unsigned char BitMap::Label()
{
    return label;
}


int BitMap::Load(loadkit & s)
{
	s.loadi32(level);
	s.loadu8(label);
	s.loadi64(bitLen);
	s.loadi32(block_size);
	block_width = blog(block_size);
	s.loadi32(super_block_size);
	s.loadi32(memorysize);
	this->data=NULL;
	this->superblock=NULL;
	this->block=NULL;
	this->coding_style=NULL;
	if(memorysize!=0)
	{
		this->data = new u64[memorysize/8];
		s.loadu64array(data,memorysize/8);
	
		superblock = new InArray();
		//cout<<"load sb:";
		superblock->load(s);
		
		block = new InArray();
		//cout<<"load b:";
		block->load(s);
		
		coding_style = new InArray();
		//cout<<"load cs:";
		coding_style->load(s);
	}
    return 0;
}


int BitMap::Save(savekit & s)
{
	s.writei32(level);
	s.writeu8(label);
	s.writei64(bitLen);
	s.writei32(block_size);
	s.writei32(super_block_size);
	s.writei32(memorysize);
	if(memorysize!=0)
	{
		s.writeu64array(data,memorysize/8);
	
		superblock->write(s);
		block->write(s);
		coding_style->write(s);
	}
	return 0;
}

i64 BitMap::RL_Rank(u64 * buff,i64 & index,int bits_num,int rl_type)
{
	int bit=0;
	return RL_Rank(buff,index,bits_num,rl_type,bit);
}

i64 BitMap::FRL_Rank(u64 * buff,i64 & index,int bits_num,int rl_type)
{
	int bit=0;
	return FRL_Rank(buff,index,bits_num,rl_type,bit);
}
i64 BitMap::FRL_Rank_gap(u64 * buff,i64 & index,int bits_num,int rl_type,int Blocklen)
{
	int bit=0;
	return FRL_Rank_gap(buff,index,bits_num,rl_type,bit,Blocklen);
}

i64 BitMap::FRL_Rank_gap(u64 * buff,i64 & index,int bits_num,int rl_type,int &bit,int Blocklen)
{
	i64 rrank =0,lrank = 0;
	FRL_Rank_gap(buff,index,0,bits_num,lrank,rrank,rl_type,Blocklen);
    return rrank-lrank;
}
i64 BitMap::FRL_Rank(u64 * buff,i64 & index,int bits_num,int rl_type,int &bit)
{
	//cout<<"FRL_Rank"<<endl;
	int offset = 0;
	int rank1  = 0;
	int value  = 0;
	int len    = 0; 
	int Fixlen = 0;	
	u64 x      = GetBits(buff,index,64);
	// int runs   = 64-blog64(x);
	int bits   = 3;
	offset 	  +=  bits;
	index     +=  bits;
	Fixlen     = x>>(64-bits);

	while(true)
	{
		 if(offset+Fixlen>64)
		{
		  x =  GetBits(buff,index,64);
		  offset = 0;
		}
		index  += Fixlen;
		offset += Fixlen;
		value   =  (x>>(64-offset))&((1<<(Fixlen))-1);
		if(bits_num-value<=0)
		{
			if(rl_type == 1)
				rank1 += bits_num;
			break;
		}
		else
		{
			bits_num-=value;
		}
		if(rl_type == 1)
			rank1 += value;
	//	cout<<"rl_type="<<setw(10)<<rl_type<<";value="<<setw(10)<<value<<endl;
		rl_type = 1-rl_type;
		
	}
	bit = rl_type;
	return rank1;
}
i64 BitMap::RL_Rank(u64 * buff,i64 & index,int bits_num,int rl_type,int &bit)
{
	cout<<__LINE__<<"RL_Rank"<<endl;
	i64 rank=0;
	i64 r=0;
	i64 already = 0;
	u64 x = GetBits(buff,index,64);
	i64 bits = 0;//通过查找表可以解码的被编码的0,1串的长度
	i64 step = 0;//
	i64 runs =0 ;//本次解码的runs数目
	i64 runs_num = 0;//累计的runs数目
	u32 anchor=0;
	rl_type=1-rl_type;
	if(bits_num > 32)
	{
		while(true)
		{
			anchor = (x>>48)<<2;
		    runs =R[anchor];
			if(runs >0)
			{
				step =R[1+anchor];//index need to move step bits 
				already = already + step;
				if(already > 64)
				{
					index = index + (already -step);
					x = GetBits(buff,index,64);
					already = 0;
					continue;
				}
				bits = R[2+anchor];//this R has bits origin code
				r=R[3+anchor];//this R has number r 1;
				bits=(bits==0)?256:bits;
				if((runs_num & 0x01) ==rl_type)
					rank = rank + r;
				else
					rank = rank + ( bits-r);
				bits_num = bits_num - bits;
				runs_num = runs_num + runs;
				if(bits_num <=0)
					break;
				x = (x<<step);
			}
			else
			{
				step = 1 + (Zeros(x>>48)<<1);
				already = already + step;
				if(already > 64)
				{
					index = index + (already - step);
					x = GetBits(buff,index,64);
					step = 1 + (Zeros(x>>48)<<1);
					already = step;
				}
				bits_num = bits_num - (x>>( 64 - step));
				if((runs_num &0x01) ==rl_type)
					rank = rank + (x>>( 64 - step));
				if(bits_num <=0)
				{
					if((runs_num &0x01)==rl_type)
					{
						bit=1;
						cout<<rank+bits_num<<endl;
						return rank + bits_num;
					}
					else
					{
						bit=0;
						cout<<rank<<endl;
						return rank;
					}
				}
				runs_num++;
				x = (x<<step);
			}
		}
	}
	index = index + (already - step);
	bits_num = bits_num + bits;
	runs_num = runs_num - runs;
	if((runs_num &0x01) ==rl_type)
		rank = rank - r;
	else
		rank = rank - (bits -r );
	already = 0;
	x = GetBits(buff,index,64);
	while(true)
	{
		step = 1+ (Zeros(x>>48)<<1);
		already = already + step;
		if(already > 64)
		{
			index = index + (already - step);
			x =GetBits(buff,index,64);
			step = 1+ (Zeros(x>>48)<<1);
			already = step;
		}
		bits_num = bits_num - (x>>( 64 - step));
		if((runs_num &0x01) ==rl_type)
			rank = rank + (x>>( 64 - step));
		if(bits_num <= 0)
		{
			if((runs_num&0x01)==rl_type)
			{
				bit =1;
				cout<<rank+bits_num<<endl;
				return rank + bits_num;
			}
			else
			{
				cout<<rank<<endl;
				bit=0;
				return rank;
			}
		}
		runs_num++;
		x=(x<<step);
	}
}
//todo
void BitMap::FRL_Rank(u64 *buff,i64 &index,int bits_left,int bits_right,i64 &rank_left,i64 &rank_right,int rl_type)
{
/*	
	int old_index = index;
	rank_left+=RL_Rank(buff,old_index,bits_left,rl_type);
	//index = old_index;
	rank_right+=RL_Rank(buff,index,bits_right,rl_type);
*/
	bool left  = true;
	i64 offset = 0;
	i64 rank1  = 0;
	i64 value  = 0;
	i64 len    = 0; 
	i64 Fixlen = 0;	
	u64 x      = GetBits(buff,index,64);
	// int runs   = 64-blog64(x);
	int bits   = 3;
	offset 	  +=  bits;
	index     +=  bits;
	Fixlen     = x>>(64-bits);

	while(true)
	{
		 if(offset+Fixlen>64)
		{
		  x =  GetBits(buff,index,64);
		  offset = 0;
		}
		index  += Fixlen;
		offset += Fixlen;
		value   =  (x>>(64-offset))&((1<<(Fixlen))-1);
		bits_left = bits_left - value;//对于左，右，bits数都要减少
		bits_right = bits_right- value;
		if(rl_type == 1)
			rank1 += value;
		if(left && bits_left<=0)
		{
			if(rl_type == 1)
				{
					//rank1 += bits_left;
					rank_left += (rank1 + bits_left);
				//	rank_right += (rank1 + bits_right);
				}
				else
				{
					rank_left += rank1;
				//	rank_right += rank1;
				}
				left=false;
			
		}
		if(bits_right<=0)
		{
			if(rl_type == 1)
				rank_right += (rank1 + bits_right);
			else
				rank_right += rank1;
			return ;
		}
		rl_type = 1-rl_type;
		
	}
}

void BitMap::FRL_Rank_gap(u64 *buff,i64 &index,int bits_left,int bits_right,i64 &rank_left,i64 &rank_right,int rl_type,int Blocklen)
{
/*	
	int old_index = index;
	rank_left+=RL_Rank(buff,old_index,bits_left,rl_type);
	//index = old_index;
	rank_right+=RL_Rank(buff,index,bits_right,rl_type);
*/	
	bool left  = true;
	i64 offset = 0;//当前６４ｂｉｔ已经读取的位数
	i64 curpos = 0 ;//读取ｆｉｘ编码比特长度
	i64 rank1  = 0;
	i64 value  = 0;
	i64 len    = 0; 
	i64 Fixlen = 0;	
	u64 x      = GetBits(buff,index,64);//
	// int runs   = 64-blog64(x);
	int bits   = FixLenBit;
	offset 	  +=  bits;
	index     +=  bits;
	// rl_type -= 5;//
    // curpos    += bits;
	Fixlen     = (x>>(64-bits))+1;
    while(curpos< bits_left ){
        if(offset + Fixlen >64){
            x = GetBits(buff,index,64);
            offset = 0;
        }
		value   =  (x>>(64-offset))&((1<<(Fixlen))-1);
        rank1++;
        offset += Fixlen;
        curpos +=value;
        index  +=Fixlen;
        }

    if(rl_type == 1 )
        rank_left  =(rank1 );
    else
        rank_left = bits_left - rank1;

	while(true){
        if(offset + Fixlen >64){//当前的６４比特已经读完
            x = GetBits(buff,index,64);
            offset = 0;
        }
		offset += Fixlen;
        index  +=Fixlen;
		value   =  (x>>(64-offset))&((1<<(Fixlen))-1);
        curpos +=value;
		if(	Fixlen*(rank1) + FixLenBit >= Blocklen ||  curpos>= bits_right  ){
			// if(Fixlen*(rank1+1) + FixLenBit == Blocklen || curpos == bits_right)
			// if(curpos == bits_right)
				// rank1++;
			break;
        	}
		rank1++;
		}

    if(rl_type == 1 )
        rank_right=(rank1 );
    else
        rank_right= bits_right - rank1;
	cout<<__LINE__<<"FRLRANK:";
	cout<<rank_left<<";"<<rank_right<<endl;
}

void BitMap::RL_Rank(u64 * buff,i64 & index,int bits_left,int bits_right,i64 &rank_left,i64 &rank_right,int rl_type)
{
/*	
	int old_index = index;
	rank_left+=RL_Rank(buff,old_index,bits_left,rl_type);
	//index = old_index;
	rank_right+=RL_Rank(buff,index,bits_right,rl_type);
*/
	
	cout<<__LINE__<<"RLRANK"<<endl;
	i64 rank = 0;//记录到解码位置的rank值
	//int rank_diff = 0;//记录left-right之间的rank值
	i64 r = 0;
	i64 already = 0;
	u64 x=GetBits(buff,index,64);
	i64 bits = 0;//通过查找表可以解码的0.1的长度
	i64 step = 0;//16位查找表可以解码的bit数
	i64 runs = 0;
	i64 runs_num = 0;
	u32 anchor = 0;
	bool left=true;//左rank还有待查找
	rl_type = 1-rl_type;

	while(true)
	{
		anchor = (x>>48)<<2;
		runs = R[anchor];
		if(runs>0)
		{
			step = R[anchor+1];
			already = already + step;
			if(already > 64)
			{
				index = index + (already - step);
				x = GetBits(buff,index,64);
				already = 0;
				continue;
			}

			bits = R[anchor+2];
			r = R[anchor+3];
			bits =(bits==0)?256:bits;
			if((runs_num & 0x01)==rl_type)
				rank  = rank + r;
			else
				rank = rank + (bits-r);

			bits_left = bits_left - bits;//对于左，右，bits数都要减少
			bits_right = bits_right- bits;
			runs_num = runs_num + runs;
			
			if(bits_left <= 0)
				break;
			x = (x<<step);
		}
		else
		{
			step = 1 + (Zeros(x>>48)<<1);
			already = already + step;
			if(already > 64)
			{
				index = index + (already - step);
				x = GetBits(buff,index,64);
				step = 1 + (Zeros(x>>48)<<1);
				already  = step;
			}
			bits = (x>>(64-step));
			bits_left = bits_left - bits;
			bits_right = bits_right - bits;
			r = bits;
			if((runs_num & 0x01)==rl_type)
				rank = rank + bits;
			runs = 1;
			runs_num += runs;
			if(bits_left <=0)
				break;
			x = (x<<step);
		}
	}
	index = index + (already  - step);
	bits_left = bits_left + bits;
	bits_right= bits_right+ bits;
	runs_num = runs_num - runs;
	if((runs_num & 0x01)==rl_type)
		rank = rank - r;
	else
		rank = rank - (bits -r );
	already = 0;
	x = GetBits(buff,index,64);

	while(true)
	{
		step = 1 + (Zeros(x>>48)<<1);
		already  = already + step;
		if(already > 64)
		{
			index = index + (already - step);
			x = GetBits(buff,index,64);
			step = 1 + (Zeros(x>>48)<<1);
			already = step;
		}
		bits  = (x>>(64-step));
		bits_left = bits_left - bits;
		bits_right= bits_right- bits;
		if((runs_num & 0x01)==rl_type)
			rank = rank + bits;
		if(left && bits_left <=0)
		{
			if((runs_num & 0x01)==rl_type)
				rank_left += (rank + bits_left);
			else
				rank_left += rank;
			left = false;//左rank以完成
		}

		if(bits_right <=0)
		{
			if((runs_num & 0x01)==rl_type)
				rank_right += (rank + bits_right);
			else
				rank_right += rank;
			return ;
		}
		runs_num++;
		x=(x<<step);
	}
}
