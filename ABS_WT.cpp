/*============================================
# Filename: ABS_WT.cpp
# Ver 1.0 2014-06-08
# Copyright (C) 2014 ChenLonggang (chenlonggang.love@163.com)
#
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 or later of the License.
#
# Description:
=============================================*/
#include <limits>
#include"ABS_WT.h"
//#include"ds_ssort.h"
#include<string.h>
#include<map>
#include <iomanip>
#include<vector>
#include <thread>
#include <sys/shm.h>
#include <error.h>
#include <string.h>
#include <sys/wait.h>
#include<math.h>
#include <thread>
//#define LOOP 35
#define SIZE 1024
#define READSIZE 1024*1024*200
u64 GetBits(u64 * buff,i64 &index,int bits)
{
	if((index & 0x3f) + bits < 65)
		return (buff[index>>6] >>( 64 -((index&0x3f) + bits))) & ((0x01ull<<bits)- 1);
	int first = 64 - (index &0x3f);
	int second = bits - first;
	u64 high = (buff[index>>6] & ((0x01ull<<first)-1)) << second;
	return high + (buff[(index>>6)+1]>>(64-second));
}

int Zeros(u16 x,ABS_FM *t)
{
	if(t->Z[x>>8]==8)
		return t->Z[x>>8]+t->Z[(uchar)x];
	else
		return t->Z[x>>8];
}

i64 GammaDecode(u64 * buff,i64 & index,ABS_FM * t)
{
	u32 x = GetBits(buff,index,32);
	int runs = Zeros(x>>16,t);
	int bits = (runs<<1)+1;
	index = index + bits;
	return x>>(32-bits);
}


ABS_FM::ABS_FM(const char * filename,int block_size,int D)
{
	this->block_size = block_size;
	this->D =D*2;
	//cout<<"samplerate:"<<this->D<<endl;
	this->T=NULL;
	T = Getfile(filename);
	Inittable();
	v_hittimes.insert(v_hittimes.begin(),n,0);
}

ABS_FM::~ABS_FM()
{
	DestroyWaveletTree();
	if(T)
		delete [] T;
	if(bwt)
		delete [] bwt;
	if(SAL)
		delete SAL;
	if(RankL)
		delete RankL;
	if(C)
		delete [] C;
	if(code)
		delete [] code;
	if(Z)
		delete [] Z;
	if(R)
		delete [] R;
}
i64 ABS_FM::SizeInByte_locate()
{
	return TreeSizeInByte(root) + SAL->GetMemorySize();
}
i64 ABS_FM::SizeInByte()
{
	return TreeSizeInByte(root) + SAL->GetMemorySize() + RankL->GetMemorySize();
}

i64 ABS_FM::SizeInByte_extract()
{
	return TreeSizeInByte(root) + RankL->GetMemorySize();
}

void ABS_FM::Codedistribution(int &Plain,int &AL0,int &AL1,int &RL0,int &RL1,int &Fix)
{
	Plain = BitMap::Plaincount;
	Fix   = BitMap::Fixcount;
	AL0 = BitMap::ALL0;
	AL1 = BitMap::ALL1;
	RL0 = BitMap::RL0;
	RL1 = BitMap::RL1;
}

BitMap* ABS_FM::GetRoot()
{
	return root;
}
i64 ABS_FM::SizeInByte_count()
{
	return TreeSizeInByte(root);
}
i64 ABS_FM::SizeInBytePart_count(string str)
{
	return SizeOfpart(root,str);
}

map<i64, i64> ABS_FM::getBitMapRuns()
{
	return BitMap::mapruns;
	//return MergeBitMapRuns(root);
}
map<i64, i64> ABS_FM::getBitnodeRuns()
{
	return BitMap::noteruns;
	//return MergeBitnodeRuns(root);
}
int ABS_FM::TreeNodeCount(BitMap * r)
{
	if(r==NULL)
		return 0;
	return TreeNodeCount(r->Left()) + TreeNodeCount(r->Right()) + 1;
}

int ABS_FM::TreeSizeInByte(BitMap * r)
{
	int size = 0;
	if(r->Left())
		size += TreeSizeInByte(r->Left());
	if(r->Right())
		size+=TreeSizeInByte(r->Right());
	size = size + r->SizeInByte();
	return size;
}
//test
i64 ABS_FM::SizeOfpart(BitMap * r,string str)
{
	int size = 0;
	if(r->Left())
		size += SizeOfpart(r->Left(),str);
	if(r->Right())
		size+=SizeOfpart(r->Right(),str);
	//size = size + r->getsize();
	if(str =="SB")
		size = size + r->SizeInSuperblock();
	else if(str =="B")
		size = size + r->SizeInblock();
	else if(str =="style")
		size = size + r->SizeIncodesytle();
	else if(str =="code")
		size = size + r->SizeInMemory();
}
void MapMerge(map<i64,i64> &map1,map<i64,i64> map2)
{
	for(map<i64,i64>::iterator it = map2.begin();it!=map2.end();it++)
	{
		if(map1.find(it->first)!=map1.end())
		{
			map1[it->first] += it->second;
		}
		else
		{
			map1.insert(pair<i64,i64>(it->first,it->second));
		}
	}
}
map<i64,i64> ABS_FM::MergeBitMapRuns(BitMap *r)
{
	map<i64,i64> mapresult;
	if(r->Left())
		MapMerge(mapresult,MergeBitMapRuns(r->Left()));
    if(r->Right())
		MapMerge(mapresult,MergeBitMapRuns(r->Right()));
    MapMerge(mapresult,r->mapruns);
	return mapresult;

}
map<i64,i64> ABS_FM::MergeBitnodeRuns(BitMap *r)
{
	map<i64,i64> mapresult;
	if(r->Left())
		MapMerge(mapresult,MergeBitMapRuns(r->Left()));
    if(r->Right())
		MapMerge(mapresult,MergeBitMapRuns(r->Right()));
    MapMerge(mapresult,r->noteruns);
	return mapresult;

}
//test
int flag3=0;
void ABS_FM::DrawBackSearch(const char * pattern,i64 & Left,i64 &Right)
{
	//if
	int len = 0;
	len = strlen(pattern);
	i64 occ_left=0;
	i64 occ_right=0;
	if(len <=0)
	{
		Left =1;
		Right = 0;
		return;
	}
	int i = len -1;
	unsigned char c = pattern[i];
	int coding = code[c];
	//uchar被定义为-1 结果是255 这里判断会错
	//if (coding==-1)
	//wei
	if(coding==-1)
	{
		Left = 1;
		Right = 0;
		return ;
	}
	//wei
	Left = C[coding];
	Right = C[coding+1]-1;
	//cout<<"Left="<<setw(10)<<Left<<","<<"Right="<<setw(10)<<Right<<","<<Right-Left<<endl;//flag
	i=i-1;
	while ((Left <= Right) and (i>=0))
	{
		c = pattern[i];
		coding = code[c];
		//uchar被定义为-1 结果是255 这里判断会错
		//if (coding==-1)
		if(coding == -1)
		{
			Left = 1;
			Right = 0;
			return;
		}
	/*
	    Left = C[coding]+Occ(c,Left-1);
		Right =C[coding]+Occ(c,Right)-1;
		i=i-1;
	*/
		if(Left == 2405788 && Right == 2406292)
			cout<<__LINE__<<endl;
		Occ(c,Left-1,Right,occ_left,occ_right);
		//cout<<flag3++<<".occ_left:"<<setw(10)<<occ_left<<",occ_right"<<setw(10)<<occ_right<<",value"<<occ_right-occ_left<<endl;
		Left = C[coding]+occ_left;
		Right = C[coding]+occ_right-1;
		i=i-1;
	}
	if(Right < Left)
	{
		Left = 1;
		Right = 0;
		return ;
	}
	return;
}

void ABS_FM::Counting(const char * pattern,i64 & num)
{
	i64 Left=1;
	i64 Right =0;
	DrawBackSearch(pattern,Left,Right);//count
//	cout<<"Counting "<<Left<<" "<<Right<<endl;
	if(Right < Left)
		num = 0;
	else
		num = Right -Left +1;
}


i64 * ABS_FM::Locating(const char * pattern,i64 &num)
{
	i64 Left=1;
	i64 Right = 0;
	DrawBackSearch(pattern,Left,Right);
	//cout<<"Left:"<<Left<<endl<<"Right:"<<Right<<endl;
	if(Right < Left )
	{
		num=0;
		return NULL;
	}
	num = Right - Left + 1;
	i64 *pos =new i64[num];
	for(int i=0;i<num;i++)
		//pos[i]=Lookup(Left + i);
		pos[i]=Lookup(Left+ i);
	return pos;
}
//mutex g_pages_mutex;
/*
i64 * ABS_FM::Locating_parrel(const char * pattern,i64 &num)
{
	int modvalue = 0;
	int numberOfthread = 0;
	i64 Left=1;
	i64 Right = 0;
	DrawBackSearch(pattern,Left,Right);
	if(Right < Left )
	{
		num=0;
		return NULL;
	}
	num = Right - Left + 1;
	i64 *pos =new i64[num];
	i64 posindex = 0;
	//numberOfthread = 1;
	numberOfthread = (num>>7)+1>10?10:((num>>7))+1;
	numberOfthread = 2;
	if(numberOfthread==1)
	{
	    for (int i = 0; i < num; i++)
			pos[i] = Lookup(Left + i);
	}
	else
	{
		//future<vector<i64>> results;
		vector<thread> v_threads;
		//vector<vector<i64>> v_i64;
		vector<i64> v_i64_1,v_i64_2;
		numberOfthread =1000;
		modvalue = num % numberOfthread;
		for(int i = 0;i<numberOfthread;i++)
		v_threads.emplace_back([&]
		{
			LookupALL(num/numberOfthread);
		});
		for (auto& th : v_threads) th.join();
		/*for(int i = 0;i<numberOfthread;i++)
		{
			 v_threads.emplace_back([=,&v_i64,&posindex]
				{

				vector<i64> v_i64temp;
				//cout<<"start:"<<Left+num / numberOfthread * i + (modvalue > i ? i : modvalue)<<",end"<<Left+num / numberOfthread * i + (modvalue > i ? i : modvalue)+(num / numberOfthread) + (modvalue > i ? 1 : 0);
				Lookup(Left+num / numberOfthread * i + (modvalue > i ? i : modvalue),Left+num / numberOfthread * i + (modvalue > i ? i : modvalue)+(num / numberOfthread) + (modvalue > i ? 1 : 0),v_i64temp);
				unique_lock<mutex> guard(g_pages_mutex);
				v_i64.emplace_back(v_i64temp);
				//return v_i64temp;
				//std::lock_guard<std::mutex> guard(g_pages_mutex);
				//memcpy(pos+posindex,(i64*)&v_i64temp[0],v_i64temp.size()*sizeof(i64));
				//std::lock_guard<std::mutex> guard2(g_pages_mutex2);
				//posindex += v_i64temp.size();
				//}
			});
		}
		i64 pos123 =0 ;
		for(auto &&t : v_threads)
		{
			t.join();
		}
		int pospos = 0;
		for(int i = 0 ;i < v_i64.size();i++)
			for(int j = 0 ;j < v_i64[i].size();j++)
				pos[pospos++] = v_i64[i][j];
	}


	return pos;
}
}*/

/*
i64 * ABS_FM::Locating_parrel(const char * pattern,i64 &num)
{
	int modvalue = 0;
	int numberOfthread = 0;
	i64 Left=1;
	i64 Right = 0;
	DrawBackSearch(pattern,Left,Right);
	if(Right < Left )
	{
		num=0;
		return NULL;
	}
	num = Right - Left + 1;
	i64 *pos =new i64[num];
	i64 posindex = 0;
	//numberOfthread = 1;
	numberOfthread = (num>>7)+1>10?10:((num>>7))+1;
	numberOfthread = num/10>50?50:num/10;
	numberOfthread = 2;
	if(numberOfthread==1)
	{
	    for (int i = 0; i < num; i++)
			pos[i] = Lookup(Left + i);
	}
	else
	{
		//vector< future<vector<i64>> > results;
		//future<vector<i64>> results;
		vector<thread> v_threads;
		vector<i64> v_i64;
		modvalue = num % numberOfthread;
        ThreadPool pool(4);
		cout<<"---------------------";
		std::vector< std::future<i64> > results;
		for(int i = 0 ;i<num;i++)
		{
			//auto ans = result.add()
			results.emplace_back(pool.enqueue([=]{
				cout<<"start cal:"<<i+Left<<endl;
				return Lookup(i+Left);
			}));
		}
		i64 pos123 = 0 ;
		for(auto && r : results)
		{
			pos[pos123++] = r.get();
		}
		/*for(int i = 0;i<numberOfthread;i++)
		{
			results.emplace_back(std::async([=,&v_i64,&posindex]{
				vector<i64> v_i64temp;
				//cout<<"start:"<<Left+num / numberOfthread * i + (modvalue > i ? i : modvalue)<<",end"<<Left+num / numberOfthread * i + (modvalue > i ? i : modvalue)+(num / numberOfthread) + (modvalue > i ? 1 : 0);
				Lookup(Left+num / numberOfthread * i + (modvalue > i ? i : modvalue),Left+num / numberOfthread * i + (modvalue > i ? i : modvalue)+(num / numberOfthread) + (modvalue > i ? 1 : 0),v_i64temp);
				return v_i64temp;
				//std::lock_guard<std::mutex> guard(g_pages_mutex);
				//memcpy(pos+posindex,(i64*)&v_i64temp[0],v_i64temp.size()*sizeof(i64));
				//std::lock_guard<std::mutex> guard2(g_pages_mutex2);
				//posindex += v_i64temp.size();
			}));
		}
		i64 pos123 =0 ;

		cout<<"---------------------";
		for(auto &t : results)
		{
			//pos[pos123] = t.get().size();
			auto v_i64 = t.get();
			for(int i = 0 ; i< v_i64.size();i++)
			{
				pos[pos123++] = v_i64[i];
			}
		}

	}

	return pos;
}*/
i64 ABS_FM::LookupALL(i64 startpos)
{
	vector<i64> v_i64;
	v_i64.reserve( startpos+1);
	for(i64 i = 0;i<startpos;i++){
		v_i64.emplace_back(Lookup(i));
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		cout<<i<<"++"<<Lookup(i)<<endl;
		}
		return 0;
}
void ABS_FM::Lookup(i64 startpos,i64 endpos,vector<i64>& v_i64)
{
	v_i64.reserve(endpos - startpos+1);
	for(i64 i = startpos;i<endpos;i++){
		v_i64.emplace_back(Lookup(i));
		cout<<i<<"++"<<Lookup(i)<<endl;
		}
}
/*
i64 * ABS_FM::Locating_parrel(const char * pattern,i64 &num)
{
	int modvalue = 0;
	int numberOfthread = 0;
	i64 Left=1;
	i64 Right = 0;
	DrawBackSearch(pattern,Left,Right);
	if(Right < Left )
	{
		num=0;
		return NULL;
	}
	num = Right - Left + 1;
	i64 *pos = new i64[num];
	//cout<<num<<endl;
	//numberOfthread = num/1024;
	numberOfthread = 4;
//	int i = 0;
	#pragma omp parallel for schedule (dynamic)
	for (int i = 0; i < num; i++)
		pos[i] = Lookup(Left + i);
	return pos ;
}*/
/*创建了新的进程，开销较大*/
i64 * ABS_FM::Locating_parrel(const char * pattern,i64 &num)
{
	int modvalue = 0;
	int numberOfthread = 0;
	i64 Left=1;
	i64 Right = 0;
	DrawBackSearch(pattern,Left,Right);
	if(Right < Left )
	{
		num=0;
		return NULL;
	}
	num = Right - Left + 1;

	i64 *pos =new i64[num];
	//numberOfthread = 1;
	numberOfthread = (num>>7)+1>10?10:((num>>7))+1;
	//numberOfthread = threadnum;
	if(numberOfthread==1)
	{
	    for (int i = 0; i < num; i++)
			pos[i] = Lookup(Left + i);
	}
	else
	{
	    i64 *shmaddr;
	    pid_t *fpid = new pid_t[numberOfthread];
	    pid_t mainpid = getpid();
	    modvalue = num % numberOfthread;
	    //add parrel
	    int shmid;
	    shmid = shmget(IPC_PRIVATE, num * sizeof(i64), IPC_CREAT | 0600);
	    if (shmid < 0)
	    {
		//perror("get shm  ipc_id error") ;
		perror("Error:");
		return NULL ;
	    }
	    //cout << "num:" << num << endl;
	    for (int i = 0; i < numberOfthread; i++)
	    {
		if(getpid()==mainpid)
				fpid[i] = fork();
		if (fpid[i] < 0)
		    printf("error in fork!");
		else if (fpid[i] == 0)
		{
		    //cout<<"child1 loop from 0 to "<<num/2<<endl;
		    shmaddr = (i64 *)shmat(shmid, NULL, 0);
		    //int looptime = ;
		    // modvalue--;
		    //cout << "looptime:" << (num / numberOfthread) + (modvalue > i ? 1 : 0) << endl;
		    for (int k = 0; k < (num / numberOfthread) + (modvalue > i ? 1 : 0); k++) //
		    {
			//	cout<<Left + k + num / numberOfthread * i + (modvalue > i ? i : modvalue)<<endl;
			//cout << "start:" << k + num / numberOfthread * i + (modvalue > i ? i : modvalue) << endl;
				shmaddr[k + num / numberOfthread * i + (modvalue > i ? i : modvalue)] = Lookup(Left + k + num / numberOfthread * i + (modvalue > i ? i : modvalue));
			//cout << "k=" << k << ",Left+i" << Left + i << setw(4) << ".child" << i << ":LookUp=" << setw(10) << shmaddr[k + num / numberOfthread * i] << ",pid" << getpid() << endl;
		    }
		    //memcpy(shmaddr,pos,sizeof(i64)*num/2);
		    exit(0);
		}
		}
		int st1, st2;
		for (int i = 0; i < numberOfthread; i++)
		{
		    waitpid(fpid[i], &st1, 0);
		}
		shmaddr = (i64 *)shmat(shmid, NULL, 0);
		memcpy(pos, shmaddr, num * sizeof(i64));
		shmctl(shmid, IPC_RMID, NULL);
		delete[] fpid;
	}
	return pos;
}
unsigned char* ABS_FM::Extracting_parrel(i64 pos,i64 len)
{
	if(pos + len > n-1 || pos <0)
	{
		cout<<"pos:"<<pos<<"  len:"<<len<<endl;
		cout<<"pos+len ="<<pos+len<<" n-1:"<<n-1<<" "<<pos<<endl;
		cout<<"ABS_FM::Extracting  error parmaters"<<endl;
		return NULL;
	}
	int modvalue = 0;
	int numberOfthread = 0;
	numberOfthread = (len>>11)+1>10?10:((len>>11))+1;
//	cout<<"numberOfthread:"<<numberOfthread<<endl;
	//numberOfthread = 7;
	if(numberOfthread==1)
	{
		return	Extracting(pos,len);
	 //   for (int i = 0; i < num; i++)
		//	pos[i] = Lookup(Left + i);
	}
	else
	{
	    unsigned char *sequence = new unsigned char[len + 1];
	    unsigned char *shmaddr;
	    pid_t *fpid = new pid_t[10];
	    pid_t mainpid = getpid();
	    modvalue = len % numberOfthread;
	    int shmid;
	    shmid = shmget(IPC_PRIVATE, len * sizeof(unsigned char), IPC_CREAT | 0600);
	    if (shmid < 0)
	    {
			perror("Error:");
			return NULL;
		}
		for (int i = 0; i < numberOfthread; i++)
	    {
			if (getpid() == mainpid)
			    fpid[i] = fork();
			if (fpid[i] < 0)
			    printf("error in fork!");
			else if(fpid[i]==0)
			{
				shmaddr = (unsigned char *)shmat(shmid, NULL, 0);
				i64 start = pos+len/numberOfthread*i+(modvalue > i ? i : modvalue);
				i64 leng  = (len / numberOfthread) + (modvalue > i ? 1 : 0);
				//cout<<"start:"<<start<<".end"<<leng<<endl;
				unsigned char *temp = Extracting(pos+len/numberOfthread*i+(modvalue > i ? i : modvalue),(len / numberOfthread) + (modvalue > i ? 1 : 0));
				memcpy(shmaddr+start-pos,temp,leng*sizeof(unsigned char));
				delete []temp;
				temp = NULL;
				exit(0);
			}

		}
		int st1;
		for (int i = 0; i < numberOfthread; i++)
		{
		    waitpid(fpid[i], &st1, 0);
		}
		shmaddr = (unsigned char *)shmat(shmid, NULL, 0);
		memcpy(sequence, shmaddr, len * sizeof(unsigned char));
		shmctl(shmid, IPC_RMID, NULL);
		delete[] fpid;
		return sequence;
	}
}



unsigned char* ABS_FM::Extracting(i64 pos,i64 len)
{
	if(pos + len > n-1 || pos <0)
	{
		cout<<"pos:"<<pos<<"  len:"<<len<<endl;
		cout<<"pos+len ="<<pos+len<<" n-1:"<<n-1<<" "<<pos<<endl;
		cout<<"ABS_FM::Extracting  error parmaters"<<endl;
		return NULL;
	}

	unsigned char * sequence=new unsigned char[len+1];
	sequence[len]='\0';
	i64 end = pos + len -1;
	i64 anchor = 0;
	i64 overloop = 0;
	int step = this->D*16;
	overloop = (n-2-end)%step;
	anchor = (n-2-end)/step;

	i64 i= RankL->GetValue(anchor);
	for(i64 j=0;j<overloop;j++)
		i = LF(i);

	for(int j=0;j<len;j++)
	{
		sequence[len-1-j]=L(i);
		i = LF(i);
	}
	return sequence;
}

int flag=0;

i64 ABS_FM::Lookup(i64 i)
{
	//cout<<i<<"++"<<endl;
	//if(v_hittimes.size()==0){
	//	v_hittimes.insert(v_hittimes.begin(),n,0);
//	}
	int ori_i = i;
	//cout<<"i:"<<i;
	int step = 0;
	int D = this->D;
	while(i%D!=0)
	{
	//	v_hittimes[i]++;
		i=LF(i);
		step =step +1;
	//	cout<<",i:"<<i<<endl;
	}
	//cout<<",step:"<<step<<endl;
	i=i/D;
	//std::this_thread::sleep_for(std::chrono::milliseconds(10));
	//if(i*D==418880)
	//cout<<ori_i<<",step:"<<step<<",SAL Pos："<<i*D<<","<<(SAL->GetValue(i)+step)%n<<endl;
	//cout<<(SAL->GetValue(i)+step)%n<<endl;
	return (SAL->GetValue(i)+step)%n;
}
/*
i64 ABS_FM::Lookup(i64 i)
{
	int step = 0;
	//int D = this->D*16;
	int flag = 0;
	int rank = posroot->Rank(i,flag);
	while(flag==0)
	{
		//v_hittimes[i]++;
		i=LF(i);
		step =step +1;
		rank = posroot->Rank(i,flag);
	//	cout<<",i:"<<i<<endl;
	}
	//cout<<",step:"<<step<<endl;
	//i=i/D;
	//std::this_thread::sleep_for(std::chrono::milliseconds(10));
	//if(i*D==418880)
	//cout<<ori_i<<",step:"<<step<<",SAL Pos："<<i*D<<","<<(SAL->GetValue(i)+step)%n<<endl;
	//cout<<(SAL->GetValue(i)+step)%n<<endl;
	return (SAL->GetValue(rank-1)*(this->D)+step)%n;
}*/
int flag2 =0;
//返回L串中c字符在位置pos_left 和pos_right之前出现的次数，结果由rank_left 和rank_right带回.
void ABS_FM::Occ(unsigned char c,i64 pos_left,i64 pos_right,i64 &rank_left,i64 &rank_right)
{
	BitMap *r = root;
	int level=0;
	char code = '0';
	while(r->Left())
	{
		code = codeTable[c][level];
 		if(code == '1')//编码是1,走右分支
		{
			//wch
			if(pos_left>-1 && pos_right >-1&&(pos_left<pos_right)) //left right 都有待查找
			//if(pos_left>-1 && pos_right >-1) //left right 都有待查找
			//wch
			{
				r->Rank(pos_left,pos_right,rank_left,rank_right);
				//cout<<flag2++<<".char="<<setw(10)<<c<<";rank_right="<<setw(10)<<rank_right<<";rank_left="<<setw(10)<<rank_left<<endl;
				pos_left = rank_left -1;
				pos_right = rank_right -1;

			/*	pos_left = r->Rank(pos_left)-1;
				pos_right=r->Rank(pos_right)-1;
			*/
			}
			else if(pos_right > -1)//只查右分支
			{
				pos_right=r->Rank(pos_right)-1;
				//cout<<flag2++<<"pos_right="<<setw(10)<<pos_right<<endl;
			}
			else//该跳出循环了,此时pos_left 和pos_right都是-1.
			{
				break;
			}
			r= r->Right();
		}
		else //编码是0,走左分支.
		{
			if(pos_left>-1 && pos_right >-1&&(pos_left<pos_right))
			//if(pos_left>-1 && pos_right >-1)
			{
				r->Rank(pos_left,pos_right,rank_left,rank_right);
			//	cout<<flag2++<<".char="<<setw(10)<<c<<";rank_right="<<setw(10)<<rank_right<<";rank_left="<<setw(10)<<rank_left<<endl;
				pos_left = (pos_left+1) - rank_left-1;
				pos_right= (pos_right+1)- rank_right-1;

			/*
				pos_left = (pos_left+1)-r->Rank(pos_left)-1;
				pos_right= (pos_right+1)-r->Rank(pos_right)-1;
			*/
			}
			else if(pos_right > -1)
			{
				pos_right = (pos_right+1)-r->Rank(pos_right)-1;
				//cout<<flag2++<<"pos_right="<<setw(10)<<pos_right<<endl;
			}
			else
			{
				break;
			}
			r=r->Left();
		}
		level++;
	}
	rank_left = pos_left+1;
	rank_right= pos_right+1;
	return ;
}
i64 ABS_FM::Occ(unsigned char c,i64 pos)
{
	BitMap * r = root;
	int level = 0;
	char code ='0';
	while(r->Left() && pos > -1)
	{
		code = codeTable[c][level];
		if(code == '1')
		{
			pos = r->Rank(pos) - 1;
			r = r->Right();
		}
		else
		{
			pos = (pos+1) - r->Rank(pos)-1;
			r = r->Left();
		}
		level = level +1;
	}
	return pos+1;
}
i64 ABS_FM::sizeOfSAL()
{
	return SAL->GetMemorySize();
}

i64 ABS_FM::sizeOfRankL()
{
	return RankL->GetMemorySize();
}
i64 ABS_FM::LF(i64 i)
{
/*
	unsigned char c = L(i);
//	cout<<"L(i)  "<<c<<endl;zerostable = zerostable;
	int coding = code[c];
	return C[coding]+Occ(c,i)-1;
*/

	i64 occ =0;
	unsigned char label =0;
	Occ(occ,label,i);
	int coding = code[label];
	return occ + C[coding] -1;

}


unsigned char ABS_FM::L(i64 i)
{
	BitMap * r = root;
	int bit =0;
	i64 rank = 0;

	while(r->Left())
	{
	//	bit = r->Rank(i) - r->Rank(i-1);
		rank = r->Rank(i,bit);
		if(bit ==1)
		{
			//i = r->Rank(i)-1;
			i = rank -1;
			r=r->Right();
		}
		else
		{
			//i = (i+1) - r->Rank(i)-1;
			i = (i+1) - rank -1;
			r=r->Left();
		}
	}
	return r->Label();

}
int flag4 = 0;
i64 ABS_FM::Occ(i64 & occ , unsigned char & label,i64 pos)
{
	BitMap * r = root;
	//cout<<"occ:root"<<endl;
	int bit =0;
	i64 rank =0;
	while(r->Left())
	{
		flag4++;
		rank = r->Rank(pos,bit);
	//	cout<<setw(20)<<flag4++<<"  pos="<<setw(10)<<pos<<",Bit = "<<bit<<",rank="<<setw(10)<<rank<<endl;
		if(bit==1)
		{
			//cout<<"r"<<endl;
			pos = rank -1;
			r =r ->Right();
		}
		else
		{
			//cout<<"l"<<endl;
			pos = (pos+1) - rank -1;
			r = r->Left();
		}
	}
	occ = pos +1;
	label = r->Label();
	return 0;
}
extern int length;
//read filedate into T and cal Cumulative Frequency sum(C)
unsigned char * ABS_FM::Getfile(const char *filename)
{
	FILE * fp = fopen(filename,"r+");
	//int fd = open(filename, O_LARGEFILE | O_RDWR | O_CREAT);
	if(fp==NULL)
	{
		cout<<"Be sure the file is available"<<endl;
		exit(0);
	}
	fseeko(fp,0,SEEK_END);
	n = ftello(fp)+1;
	//n = n>READSIZE?READSIZE:n;
	//n=n/2;
//	length = n;
	unsigned char * T = new unsigned char[n];
	fseeko(fp,0,SEEK_SET);
	int e=0;
	int num=0;
	num = fread(T,sizeof(uchar),n,fp);
	T[n-1]=0;
	fclose(fp);
	memset(charFreq,0,256*sizeof(i64));
	memset(charMap,0,256*sizeof(bool));
	for(saidx64_t i=0;i<n;i++)
		charFreq[T[i]]++;
	this->alphabetsize = 0;
	for(i32 i=0;i<256;i++)
		if(charFreq[i])
		{
			this->alphabetsize++;
			this->charMap[i]=true;
		}
	this->code = new short[256];
	this->C = new i64[alphabetsize+1];
	memset(C,0,(alphabetsize+1)*4);
	this->C[alphabetsize] = n;
	this->C[0] = 0;
	int k=1;
	i64 pre =0;
	for(int i=0;i<256;i++)
	{
		if(charFreq[i]!=0)
		{
			code[i]=k-1;
			C[k]=pre + charFreq[i];
			pre = C[k];
			k++;
		}
		else
			code[i]=-1;
	}
	return T;
}
/*unsigned char * ABS_FM::Getfile(const char *filename,int part,int pos)
{
	FILE * fp = fopen(filename,"r+");
	if(fp==NULL)
	{
		cout<<"Be sure the file is available"<<endl;
		exit(0);
	}
	fseeko(fp,0,SEEK_END);
	n = ftello(fp)+1;
	//n = n>READSIZE?READSIZE:n;
	//n=n/2;
	//length = n;
	fseeko(fp,n/part*pos,SEEK_SET);
	int e=0;
	int num=0;
	if(pos+1!=part)
	{
		n = n/part+2*PATTENLEN;
		//n = n/part;
	}
	else
	{
		n = n-pos*n/part;
	}
	unsigned char * T = new unsigned char[n];
	num = fread(T,sizeof(uchar),n,fp);
	T[n-1]=0;
	fclose(fp);
	memset(charFreq,0,256*sizeof(i64));
	memset(charMap,0,256*sizeof(bool));
	for(saidx64_t i=0;i<n;i++)
		charFreq[T[i]]++;
	this->alphabetsize = 0;
	for(i32 i=0;i<256;i++)
		if(charFreq[i])
		{
			this->alphabetsize++;
			this->charMap[i]=true;
		}
	this->code = new short[256];
	this->C = new i64[alphabetsize+1];
	memset(C,0,(alphabetsize+1)*4);
	this->C[alphabetsize] = n;
	this->C[0] = 0;
	int k=1;
	i64 pre =0;
	for(int i=0;i<256;i++)
	{
		if(charFreq[i]!=0)
		{
			code[i]=k-1;
			C[k]=pre + charFreq[i];
			pre = C[k];
			k++;
		}
		else
			code[i]=-1;
	}

	return T;
}*/

int ABS_FM::BWT(unsigned char *T,int * SA,unsigned char * bwt,int len)
{
	int i=0;
	int index=0;
	for(i=0;i<len;i++)
	{
		index = (SA[i]-1+len)%len;
		bwt[i]=T[index];
	}
	return 0;
}
//WCH BWT64
int ABS_FM::BWT64(unsigned char *T,saidx64_t * SA,unsigned char * bwt,saidx64_t len)
{
	saidx64_t i=0;
	saidx64_t index=0;
	for(i=0;i<len;i++)
	{
		index = (SA[i]-1+len)%len;
		bwt[i]=T[index];
	}
	return 0;
}
// void ABS_FM::SASample(saidx64_t* SA)
// {
// 	int step1 =this->D;
// 	//int step2 =this->D*16;
// 	int datewidth = log2(n/step1)+1;
// 	SAL=new InArray(n/step1+1,datewidth);//SA sample
// 	SALPos = new InArray(n/step1+1,1);
// 	unsigned char *pos = new unsigned char[n];
// 	//for(i=0,j=0;i<n;i=i+step1,j++)
// 	//	SAL->SetValue(j,SA[i]);
// 	int j = 0 ;
// 	//cout<<"SASample start"<<endl;
// 	for(int i = 0 ;i<n;i++)
// 	{
// 		if(SA[i]%step1 == 0){
// 			SAL->SetValue(j++,SA[i]/step1);
// 			pos[i] = '1';
// 			//cout<<i<<endl;
// 		}
// 		else
// 			pos[i] = '0';
// 	}
// 	//cout<<"SASample end"<<endl;
// 	char ct[CHAR_SET_SIZE][CHAR_SET_SIZE];
// 	memset(ct,0,sizeof(ct));
// 	ct['1'][0] = '1';
// 	ct['0'][0] = '0';
// 	//ct['0'][0] = '1';
// 	//ct['0'][1] = '0';
// 	//ct['1'][1] = '1';
// 	posroot = CreateWaveletTree(pos,n,ct);
// 	int tpos ;
// 	//cout<<"SA[1520]:"<<SA[1520]<<endl;
// 	delete [] pos;
// 	/*cout<<"posroot->Rank(58,tpos),"<<posroot->Rank(58,tpos)<<","<<endl<<tpos<<endl;
// 	cout<<"posroot->Rank(60,tpos),"<<posroot->Rank(60,tpos)<<","<<tpos<<endl;
// 	cout<<"posroot->Rank(93,tpos),"<<posroot->Rank(93,tpos)<<","<<tpos<<endl;
// 	cout<<"posroot->Rank(70,tpos),"<<posroot->Rank(70,tpos)<<","<<tpos<<endl;
// 	cout<<"posroot->Rank(80,tpos),"<<posroot->Rank(80,tpos)<<","<<tpos<<endl;
// 	cout<<"posroot->Rank(90,tpos),"<<posroot->Rank(90,tpos)<<","<<tpos<<endl;
// 	cout<<"posroot->Rank(100,tpos),"<<posroot->Rank(100,tpos)<<","<<tpos<<endl;
// */
// }
int ABS_FM::BuildTree(int speedlevel)
{
	saidx64_t* SA = new saidx64_t[n];
	//int *SA = new int[n];
	//for(i64 i = 0;i<n;i++)
	//	SA[i] = n-i;
	divsufsort64(T,SA,n);
	//SA和Rank数组的采样
	int step1 =this->D;
	int step2 =this->D*16;
	int datewidth = log2(n)+1;

	RankL=new InArray(n/step2+1,datewidth);//rank sample
	//SASample(SA);
	i64 i=0;
	i64 j=0;
	SAL=new InArray(n/step1+1,datewidth);//SA sample
	for(i=0,j=0;i<n;i=i+step1,j++)
		{
			SAL->SetValue(j,SA[i]);
		}


	for(i=0;i<n;i++)
	{
		if(SA[i]==0)
			continue;
		if((n-2-(SA[i]-1))%step2 == 0)
		{
			RankL->SetValue((n-2-(SA[i]-1))/step2,i);
		}
	}

	bwt = new unsigned char[n];
	BWT64(T,SA,bwt,n);
	//BWT(T,SA,bwt,n);
	getbwtRuns(bwt, n);
	//	divbwt64(T,bwt,SA,n);
	double runs=0.0;
	for(i64 i=0;i<n-1;i++)
		if(bwt[i]!=bwt[i+1])
			runs++;
	runs=n/runs;
//	cout<<runs<<endl;
	int a=0;
	int b=0;
	if(speedlevel<0 || speedlevel >2)
	{
		cerr<<"speedlevel error"<<endl;
		exit(0);
	}
	switch(speedlevel)
	{
		case 0:a=2;b=10;break;
		case 1:a=4;b=20;break;
		case 2:a=10;b=50;break;
		default:a=4;b=20;break;
	}

	if(runs<a)
		block_size=block_size*1;
	else if(runs<b)
		block_size=block_size*2;
	else
		block_size=block_size*4;

//	cout<<"block_size: "<<block_size<<endl;
	TreeCode();
	root=CreateWaveletTree(bwt,n,codeTable);
//	cout<<"CreatWaveletTree"<<endl;

	//Test_L();
	//Test_Occ();
	//Test_Shape(root);

//	delete [] T;
//	T=NULL;
//	delete [] SA;
//	SA=NULL;
//	delete[] bwt;
//	bwt=NULL;

	return 0;
}
//test
void ABS_FM::getbwtRuns(unsigned char* bwt,int len)
{
	unsigned char ch =bwt[0];
	i64 runs = 1;
	for(int i =1;i<len;i++)
	{
		if(bwt[i]==ch)
		    runs++;
		else
		{
		    map<i64, i64>::iterator it = BWTruns.find(runs);
		    if (it == BWTruns.end())
		    {
				BWTruns.insert(pair<i64, i64>(runs, 1));
			}
			else
			{
			    BWTruns[runs]++;
			}
			ch   = bwt[i];
			runs = 1;
		}
	}
}
void ABS_FM::Test_Shape(BitMap * r)
{
	if(r->Left() && r->Right())
	{
		Test_Shape(r->Left());
		Test_Shape(r->Right());
	}
	else if(r->Left() || r->Right())
	{
		cout<<"one child"<<endl;
	}
}

void ABS_FM::Test_Occ()
{
	int count[256]={0};
	unsigned long long int mis=0;
	for(int i=0;i<n;i++)
	{
		count[bwt[i]]++;
		if(Occ(bwt[i],i) != count[bwt[i]])
		{
			cout<<count[bwt[i]]<<" "<<Occ(bwt[i],i)<<" "<<(int)bwt[i]<<" "<<(char)bwt[i]<<" "<<i<<endl;
			cout<<codeTable[bwt[i]]<<endl;
			mis++;
		}
		if(i%100000==0)
			cout<<(i*1.0)/n<<endl;
	}
	cout<<"missing :"<<mis<<endl;

}

void ABS_FM::Test_L()
{
	int mis=0;
	for(int i=0;i<n;i++)
	{
		if(bwt[i]!=L(i))
		{
			cout<<bwt[i]<<" "<<L(i)<<" "<<i<<endl;
			mis++;;
		}
	}
	cout<<"mis: "<<mis<<endl;
}


BitMap * ABS_FM::CreateWaveletTree(unsigned char * bwt,i64 n,char ctable[CHAR_SET_SIZE][CHAR_SET_SIZE])
{
	BitMap * root = NULL;

	root = FullFillWTNode(bwt,n,0,ctable);
	if(!root)
	{
		cout<<"FullfillWTNode failed"<<endl;
		exit(0);
	}
	return root;
}


BitMap * ABS_FM::FullFillWTNode(unsigned char * buff,i64 len,int level,char ctable[CHAR_SET_SIZE][CHAR_SET_SIZE])
{
//	cout<<level<<endl;
	int CurrentLevel = level;
	i64 CurrentBitLen = len;
	unsigned char CurrentLabel = '\0';
	unsigned long long int *CurrentBitBuff = NULL;
	if ((int)strlen((const char*)ctable[buff[0]])==level)
	{
		CurrentLabel = buff[0];
		CurrentBitBuff = NULL;
		//uchar * tables[5]={this->zerostable,this->R1,this->R2,this->R3,this->R4};
		uchar * tables[2] ={this->Z,this->R};
		BitMap * node = new BitMap(CurrentBitBuff,CurrentBitLen,CurrentLevel,block_size,CurrentLabel,tables);
		node->Left(NULL);
		node->Right(NULL);
		return node;
	}

	i64 u64Len=0;
	if(len%64==0)
		u64Len = len/64+1;
	else
		u64Len = len/64+2;
	CurrentBitBuff = new unsigned long long int[u64Len];
	memset(CurrentBitBuff,0,u64Len*8);
	unsigned char * lptr=NULL;
	unsigned char * rptr=NULL;
	i64 leftLen=0;
	i64 rightLen=0;

	lptr = new unsigned char[len];
	rptr = new unsigned char[len];
	memset(lptr,0,len);
	memset(rptr,0,len);

	//computer bitvect;

	i64 i=0;
	int bytePos=0;
	int bitOffset=0;
	u64 last = 0;
	for(i=0;i<len;i++)
	{
		if(ctable[buff[i]][level]=='1')//judge go left or right
		{
			//CurrentBitBuff[bytePos] |= (0x01<<(7-bitOffset));
			CurrentBitBuff[bytePos] |= (0x01ull<<(63-bitOffset));
			//construct right data buff
			rptr[rightLen++]=buff[i];
			last = 0;
		}
		else
		{
			lptr[leftLen++]=buff[i];
			last = 1;
		}
		bitOffset++;
		//if(bitOffset == 8)
		if(bitOffset == 64)
		{
			bytePos++;
			bitOffset = 0;
		}
	}
	CurrentBitBuff[bytePos] |= (last<<(63-bitOffset));

	//uchar * tables[5] = {this->zerostable,this->R1,this->R2,this->R3,this->R4};
	uchar * tables[2] = {this->Z,this->R};
	BitMap * node = new BitMap(CurrentBitBuff,CurrentBitLen,CurrentLevel,block_size,CurrentLabel,tables);
	
	if(leftLen !=0)
	{
		BitMap * left =FullFillWTNode(lptr,leftLen,level+1,ctable);
		node->Left(left);
		delete [] lptr;
		lptr=NULL;
	}
	if(rightLen!=0)
	{
		BitMap * right = FullFillWTNode(rptr,rightLen,level+1,ctable);
		node->Right(right);
		delete [] rptr;
		rptr=NULL;
	}
	return node;
}


int ABS_FM::DestroyWaveletTree()
{
	delete root ;
	root=NULL;
	return 0;
}


int ABS_FM::blog(int x)
{
	int ans=0;
	while(x>0)
	{
		ans++;
		x=(x>>1);
	}
	return ans;
}

//init tables
void ABS_FM::Inittable()
{
	this -> Z = new uchar[1<<8];
	int tablewidth = 8;
	for(int i=0;i<tablewidth;i++)
		for(int j=(1<<i);j<(2<<i);j++)
			Z[j] = tablewidth-1-i;
	Z[0] = tablewidth;

	u64 tablesize = (1<<16);
	R  = new uchar[tablesize<<2];

	//查找表的初始化：在16bits的0,1串上模拟gamma解码的过程，得到
	//这些表
	u64 B[2]={0xffffffffffffffffull,0xffffffffffffffffull};
	int sum =0;//gamma编码的和,含义为原串被编码的bits数目。
	i64 step=0;//16bits可以完整解码的bits数,该值不会大于16.
	int rank = 0;//16bits表示的几个完整的runs,假设第一个runs是1-runs,这几个runs的rank值。
	int runs = 0 ;//runs 个数.

	int x = 0;//工作变量，保存本次的gamma解码值.
	int prestep = 0;//前一次正确解码的bits数(累加),<=16.
	for(u64 i=0;i<tablesize;i++)
	{
		B[0] = (i<<48);
		sum  =0 ;
		step = 0;
		prestep=0;
		rank = 0;
		runs = 0;
		while(1)
		{
			x = GammaDecode(B,step,this);//step会联动.
			if(step > 16)
				break;
			sum = sum + x;
			prestep = step;
			runs ++;
			if(runs%2==1)
				rank = rank + x;
		}
		R[i<<2] = runs;//r4
		R[(i<<2)+1] = prestep;//r2
		R[(i<<2)+2] = sum; //r1;
		R[(i<<2)+3] = rank;//r3

	}
}

//递归保存节点的编号信息
int ABS_FM::SaveNodePosition(BitMap * r,u32 position,savekit &s)
{
	if(!r)
		return 1;
	s.writei32(position);
	SaveNodePosition(r->Left(), 2 * position,s);
	SaveNodePosition(r->Right(),2 * position +1,s);
	return 0;
}

//递归保存节点的数据信息
int ABS_FM::SaveNodeData(BitMap *r,savekit &s)
{
	if(!r)
		return 1 ;
	r->Save(s);
	SaveNodeData(r->Left(),s);
	SaveNodeData(r->Right(),s);
	return 0;
}

int ABS_FM::SaveWTTree(savekit &s,BitMap *r)
{
	//保存编号信息
	//int nodecount = 2*alphabetsize -1;
	//s.writei32(nodecount);
	SaveNodePosition(r,1,s);
	//保存节点数据信息
	SaveNodeData(r,s);
	return 0;
}

BitMap* ABS_FM::LoadWTTree(loadkit &s,int alphabetsize,uchar **tables)
{
	//读取数据，map的int域对应该节点的位置
	int nodecount = 2*alphabetsize -1;

//	cout<<alphabetsize<<endl;
//	s.loadi32(nodecount);
	int * p = new int[nodecount];
	s.loadi32array(p,nodecount);
	map<int,BitMap * > pmap;
	BitMap * r=NULL;
	for(int i=0;i<nodecount;i++)
	{
		if(tables)
			r = new BitMap(tables);
		else
			r = new BitMap();
		r->Load(s);
		pmap[p[i]] = r;
	}
	//挂链
	map<int ,BitMap *>::iterator iter;
	map<int ,BitMap *>::iterator f_iter;
	for(iter = pmap.begin();iter!=pmap.end();iter++)
	{
		f_iter = pmap.find(2*iter->first);
		if(f_iter != pmap.end())
			iter->second->Left(f_iter->second);
		else
			iter->second->Left(NULL);

		f_iter = pmap.find(2*iter->first +1);
		if(f_iter!=pmap.end())
			iter->second->Right(f_iter->second);
		else
			iter->second->Right(NULL);
	}
//	cout<<"767"<<endl;
	f_iter = pmap.find(1);
	if(f_iter !=pmap.end())
		root = f_iter->second;
	else
	{
		cerr<<"Load WTTree error"<<endl;
		root = NULL;
		exit(0);
	}
	delete [] p;
	p = NULL;
//	cout<<"778"<<endl;
	return root;
}

int ABS_FM::Load(loadkit &s)
{
	//s.loadi32(this->n);

	s.loadi64(this->n);
	s.loadi32(this->alphabetsize);
	s.loadi32(this->D);

	//for C
	this->C = new i64[alphabetsize+1];
	s.loadi64array(this->C,alphabetsize+1);
	//for code
	//wch
	//this->code = new uchar[256];
	//s.loadu8array(this->code,256);
	this->code = new i16[256];
	s.loadi16array(this->code,256);
	//wch
	//for codeTable;
	memset(codeTable,0,sizeof(codeTable));
	for(int i=0;i<256;i++)
	{
		uchar len=0;
		s.loadu8(len);
		if(len!=0)
		{
			int bytes = len%8?len/8+1:len/8;
			uchar * bits = new uchar[bytes];
			s.loadu8array(bits,bytes);
			int in_index =0;
			int off_index =0;
			for(int j=0;j<len;j++)
			{
				if(bits[off_index] & (0x01<<(7-in_index)))
					codeTable[i][j] = '1';
				else
					codeTable[i][j] = '0';
				in_index++;
				if(in_index==8)
				{
					in_index =0;
					off_index ++;
				}
			}
			delete []bits;
			bits = NULL;
		}
	}

	//for SAL
	this->SAL = new InArray();
	//cout<<"load sal:";
	this->SAL->load(s);
	//for Rankl
	//cout<<"load rankl:";
	this->RankL = new InArray();
	this->RankL->load(s);

	Inittable();
	uchar * par[2]={Z,R};
	//cout<<"cs"<<endl;
//	this->posroot=LoadWTTree(s,2,par);
	this->root = LoadWTTree(s,alphabetsize,par);
//	cout<<"835"<<endl;
	T=NULL;
	bwt=NULL;
	return 0;
}
int ABS_FM::Save(savekit &s)
{
	s.writei64(n);
	//cout<<"save n:"<<n;
	s.writei32(alphabetsize);
	s.writei32(D);//SA的采样率

	//C表
	//s.writei32(alphabetsize+1);
	s.writei64array(C,alphabetsize+1);
	//code表
	//s.writei32(256);
	//wch
	//s.writeu8array(code,256);
	s.writei16array(code,256);
	//wch
	//codeTable
	for(int i=0;i<256;i++)
	{
		uchar len = strlen(codeTable[i]);
		s.writeu8(len);
		if(0!=len)
		{
			int bytes = len%8?len/8+1:len/8;
			uchar *bits = new uchar[bytes];
			memset(bits,0,bytes);
			int off_index=0;
			int in_index =0;
			for(int j=0;j<len;j++)
			{
				if(codeTable[i][j]=='1')
					bits[off_index] = bits[off_index]|(0x01<<(7-in_index));
				in_index++;
				if(8==in_index)
				{
					in_index=0;
					off_index++;
				}
			}
			s.writeu8array(bits,bytes);
		}
	}

	//for SAL
	SAL->write(s);
	//for RankL
	RankL->write(s);
	//for WT tree
//	cout<<"SaveWTTree"<<endl;
//	SaveWTTree(s,posroot);
	SaveWTTree(s,this->root);
	return 0;
}
