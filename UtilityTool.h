#pragma once

#include <iostream>

constexpr int CID_LEN   = 9;
//MIN_LEN为每天的分钟数最大值为1440，故其长度为4位
constexpr int MIN_LEN = 4;
constexpr int MIN_PER_HOUR = 60;

class UtilityTool
{
public:
	//创建会议ID
	static std::string createCid(const int cid_lenth = 9);

	//获取指定长度的随机整数
	static int getRandomNum(const int lenth);

	//获取当前系统时间的时间戳（分钟）
	static time_t getTimeStamp();

	//获取当前系统时间的时间戳毫秒，其实也不是，获取毫秒数之后，取余100000
	static std::time_t getMillSeconds();
};

