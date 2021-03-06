/*============================================
# Filename: InArray.h
# Ver 1.0 2014-06-08
# Copyright (C) 2014 ChenLonggang (chenlonggang.love@163.com)
#
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 or later of the License.
#
# Description:A simple int-vector,data_width bits each cell
=============================================*/
/*#define _Inarray
#define i64 long long int
#include<math.h>
#define i32 int
#define u32 unsigned int
#define u64 unsigned long long int
using namespace std;*/
#ifndef _Inarray
#define _Inarray
#include"BaseClass.h"
#include"savekit.h"
#include"loadkit.h"

class InArray
{
public:
	InArray();
	InArray(u64 data_num,u32 data_width);
	~InArray(void);
	u64 GetValue(u64 index);
	void SetValue(u64 index,u64 value);
	u64 GetNum();
	u32 GetDataWidth();
	u64 GetMemorySize();
	u64 GetValue2(u64 index);
	i64 write(savekit & s);
	i64 load(loadkit & s);
private:
	u32 * data;
    u64 datanum;
	u32 datawidth;
	u64 mask;
};
#endif