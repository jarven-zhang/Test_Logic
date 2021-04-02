
#include <thread>
#include <chrono>
#include <random>

#include "UtilityTool.h"

//很遗憾，此文件的接口暂时均没有被使用

/*
*  功能：生成一个正整形随机数
*  参数： 1. lenth : 需要指定的随机数的长度（位数）
*  返回值：生成的随机数
*/
int UtilityTool::getRandomNum(const int lenth)
{
	int start{ static_cast<int>(pow(10,lenth - 1))};
	int end  { static_cast<int>(pow(10,lenth))};

	std::default_random_engine rand(static_cast<unsigned int>(time(NULL)));
	//确定取值范围，生成区间[start, end]内的整型数字
	std::uniform_int_distribution<int> rand1(start, end);

	return rand1(rand);
}

/******************************************************************************************
*  说明：
*  我们希望创建一个唯一的会议ID号，但同时其长度不能太长（现在默认为9位）。
*  此函数将会议ID设计为长度位9位，其包括两个部分：5位随机数+4位当天的分钟数。
*  这样，此ID就既拥有某种唯一性又有其随机性。
*  示例：比如当前的时间为 15点16分
*    则 1. 先通过随机函数生成随机数为54603
*       2. 获取当前的分钟数为 15*60+16=916（分钟数不足4位则补0）
*   所以最后的cid为：546030916
* 
* 
*  功能：创建会议的ID号
*  参数：
*	1. cid_lenth : 会议ID的长度，默认值为0
******************************************************************************************/
std::string UtilityTool::createCid(const int cid_lenth)
{
	//1. 得到随机整数（默认5位，当cid长度为9时）的整形
	int rd_num = getRandomNum(cid_lenth - MIN_LEN);
	std::string rst = std::to_string(rd_num);

	//2. 获取到此时在当天的分钟数
	auto tt = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	struct tm* ptm = localtime(&tt);
	int min_num = (ptm->tm_hour * MIN_PER_HOUR) + ptm->tm_min;

	//3. 确定需要补0的个数
	size_t zero_len = MIN_LEN - std::to_string(min_num).size();
	for (size_t i = 0; i < zero_len; ++i)
	{
		rst += "0";
	}

	return (rst + std::to_string(min_num));
}

/******************************************************************************************
*  说明：
*     定时清理空会议，需要获取当前系统的时间戳，不需要很高的精度，分钟即可。
*     此接口的精度为分钟，使用者请确定精度是否满足情况。
*
*  功能：获取当前系统时间的时间戳（分钟）
*  
******************************************************************************************/
std::time_t UtilityTool::getTimeStamp()
{
	auto tp = std::chrono::time_point_cast<std::chrono::minutes>(std::chrono::system_clock::now());
	auto tmp = std::chrono::duration_cast<std::chrono::minutes>(tp.time_since_epoch());

	return tmp.count();
}

//
/******************************************************************************************
*  说明：
*     定时清理空会议，需要获取当前系统的时间戳，不需要很高的精度，分钟即可。
*     此接口的精度为分钟，使用者请确定精度是否满足情况。
*
*  功能：获取随机数，以获取当前系统时间的时间戳的毫秒数的方式，取后4位数字
*
******************************************************************************************/
std::time_t UtilityTool::getMillSeconds()
{
	auto tp = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());
	auto tmp = std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch());
	return tmp.count() % 100000;
}