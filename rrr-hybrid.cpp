#include <sdsl/suffix_arrays.hpp>
#include <string>
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <time.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#define i64 long long
using namespace std;
using namespace sdsl;

const int TIME = 100;//随机位置的个数。比如文件长度为n，随机从[0, n - 50]中取出了TIME个位置。
const int PTN_LEN = 20;//模式的长度。
//得到模式串
void getPattern(string textname, char* pattern[], i64* pos);
i64* generateRandom(i64 count,i64 seed,i64 size);
template<class Index>
void test(string textname, string methodname, string pattern[], i64* pos)
{
	cout<<methodname.c_str()<<endl<<textname.c_str()<<endl;

    time_t nowtime;
	struct timespec stime, etime;
	clock_gettime(CLOCK_REALTIME, &stime);
	csa_wt< Index, 32, 32*16 > csa;
	construct(csa, textname, 1);
	clock_gettime(CLOCK_REALTIME, &etime);
	//cout.setw(10);
	cout<<"Build time is "<<((double)((etime.tv_sec*1000000000.0 + etime.tv_nsec) - (stime.tv_sec*1000000000.0 + stime.tv_nsec)))/1000000000.0<<" s"<<endl;


	clock_gettime(CLOCK_REALTIME, &stime);
    nowtime =time(NULL);
    //cout<<"startcount:"<<nowtime<<std::endl;
	for(i64 i = 0; i < TIME; i++)
	{
       //cout<< pattern[i]<<":"<<count(csa,pattern[i].begin(),pattern[i].end())<<std::endl;
        count(csa,pattern[i].begin(),pattern[i].end());
	}
    //nowtime =time(NULL);
    //cout<<"endcount:"<<nowtime<<std::endl;
	clock_gettime(CLOCK_REALTIME, &etime);
	cout<<"Count time is "<<((double)((etime.tv_sec*1000000000.0 + etime.tv_nsec) - (stime.tv_sec*1000000000.0 + stime.tv_nsec)))/1000/TIME<<" us"<<endl;

	clock_gettime(CLOCK_REALTIME, &stime);
    //nowtime =time(NULL);
    //cout<<"startlocat:"<<nowtime<<std::endl;
	for(i64 i = 0; i < TIME; i++)
	{
		auto z = locate(csa,pattern[i].begin(),pattern[i].begin()+PTN_LEN);

        //cout<<pattern[i]<<":locate count"<< z.size()<<endl;
        //for(auto i :z) {
            //cout<< i << endl;
        //}
	}
	clock_gettime(CLOCK_REALTIME, &etime);
    nowtime =time(NULL);
	cout<<"locate time is "<<((double)((etime.tv_sec*1000000000.0 + etime.tv_nsec) - (stime.tv_sec*1000000000.0 + stime.tv_nsec)))/1000/TIME<<" us"<<endl;


	clock_gettime(CLOCK_REALTIME, &stime);
	for(i64 i = 0; i < TIME; i++)
	{
		extract(csa,pos[i],pos[i]+PTN_LEN-1);
	}
	clock_gettime(CLOCK_REALTIME, &etime);
	cout<<"extract time is "<<((double)((etime.tv_sec*1000000000.0 + etime.tv_nsec) - (stime.tv_sec*1000000000.0 + stime.tv_nsec)))/1000/TIME<<" us"<<endl;


	int fd = open(textname.c_str(), O_RDONLY);
	i64 n = lseek(fd,0,SEEK_END);
	close(fd);
	cout<<"ration for all is "<<size_in_mega_bytes(csa)*1024*1024.0/n<<endl;

	//{
		//csa_wt< Index, (1ull<<30), 32*16 > csa_extrct;
		//construct(csa_extrct, textname, 1);
		//cout<<"ration for extract is "<<size_in_mega_bytes(csa_extrct)*1024*1024.0/n<<endl;
	//}
	//{
		//csa_wt< Index, 32, (1ull<<30) > csa_locate;
		//construct(csa_locate, textname, 1);
		//cout<<"ration for locate is "<<size_in_mega_bytes(csa_locate)*1024*1024.0/n<<endl;
	//}
	//{
		//csa_wt< Index, (1ull<<30), (1ull<<30) > csa_count;
		//construct(csa_count, textname, 1);
		//cout<<"ration for count is "<<size_in_mega_bytes(csa_count)*1024*1024.0/n<<endl;
	//}

}

int main(int argc, char* argv[])
{
    if(argc!=3)
        cout<<"fname , seed"<<endl;
	string textname = argv[1];
    int seed = argv[2]
	i64 size = 0;
	int fd = open(argv[1],'r',S_IRUSR | S_IWUSR | S_IXUSR);
	if(fd == -1)
	{
		cout<<"open error"<<endl;
		exit(-1);
	}
	if((size = lseek(fd,0,SEEK_END)) == -1)
	{
		cout<<"lseek error"<<endl;
		exit(-1);
	}
	close(fd);
	if(argc != 3)
		return -1;
	//int *pos = new int[TIME];
	//for(int i = 0; i < TIME; i++)
	//	cin>>pos[i];
	i64 *pos = generateRandom(TIME,atoi(argv[2]),size);
	char* pattern[TIME];
	for(i64 i = 0; i < TIME; i++)
	{
		pattern[i] = new char[PTN_LEN+1];
		memset(pattern[i], 0, sizeof(char)*(PTN_LEN+1));
	}

	getPattern(textname, pattern, pos);
	string patternstr[TIME];
	for(i64 i = 0; i < TIME; i++)
	{
		// for(int j = 0; j < PTN_LEN; j++)
		// {atoi(argv[3])
		// 	patternstr[i][j] = pattern[i][j];
		// }
		patternstr[i] = pattern[i];
	}
	for(i64 i = 0; i < TIME; i++)
	{
		if(patternstr[i].size() != 20)
		{
			cout<<"Wrong"<<endl;
			exit(-1);
		}
		//cout<<patternstr[i].c_str()<<endl;
	}

    test<wt_huff<rrr_vector<15>> >(textname, "rrr_vector<15>", patternstr, pos);
    test<wt_huff<rrr_vector<31>> >(textname, "rrr_vector<31>", patternstr, pos);
    test<wt_huff<rrr_vector<63>> >(textname, "rrr_vector<63>", patternstr, pos);
    test<wt_huff<rrr_vector<127>> >(textname, "rrr_vector<127>", patternstr, pos);

    test<wt_huff<hyb_vector<8>> > (textname, "hyb_vector<8>", patternstr, pos);
    test<wt_huff<hyb_vector<16>> > (textname, "hyb_vector<16>", patternstr, pos);
    test<wt_huff<hyb_vector<32>> > (textname, "hyb_vector<32>", patternstr, pos);
    test<wt_huff<hyb_vector<64>> > (textname, "hyb_vector<64>", patternstr, pos);

	return 0;
}


void getPattern(string textname, char* pattern[], i64* pos)
{
	int fd = open(textname.c_str(), O_RDONLY);
	if(fd == -1)
		exit(-1);

	for(int i = 0; i < TIME; i++)
	{
		if((lseek(fd, pos[i], SEEK_SET)) == -1)
			exit(-1);
		if((read(fd, pattern[i], PTN_LEN)) != PTN_LEN)
			exit(-1);
	}

	close(fd);
}

i64* generateRandom(i64 count,i64 seed,i64 size)
{
    i64* result = new i64[count];
    srand(unsigned(seed));

    for(int i = 0;i<count;i++)
    {
        result[i] = rand()%size;
    }
    return result;
}
