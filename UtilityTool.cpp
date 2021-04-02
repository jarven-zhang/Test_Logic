
#include <thread>
#include <chrono>
#include <random>

#include "UtilityTool.h"

//���ź������ļ��Ľӿ���ʱ��û�б�ʹ��

/*
*  ���ܣ�����һ�������������
*  ������ 1. lenth : ��Ҫָ����������ĳ��ȣ�λ����
*  ����ֵ�����ɵ������
*/
int UtilityTool::getRandomNum(const int lenth)
{
	int start{ static_cast<int>(pow(10,lenth - 1))};
	int end  { static_cast<int>(pow(10,lenth))};

	std::default_random_engine rand(static_cast<unsigned int>(time(NULL)));
	//ȷ��ȡֵ��Χ����������[start, end]�ڵ���������
	std::uniform_int_distribution<int> rand1(start, end);

	return rand1(rand);
}

/******************************************************************************************
*  ˵����
*  ����ϣ������һ��Ψһ�Ļ���ID�ţ���ͬʱ�䳤�Ȳ���̫��������Ĭ��Ϊ9λ����
*  �˺���������ID���Ϊ����λ9λ��������������֣�5λ�����+4λ����ķ�������
*  ��������ID�ͼ�ӵ��ĳ��Ψһ������������ԡ�
*  ʾ�������統ǰ��ʱ��Ϊ 15��16��
*    �� 1. ��ͨ������������������Ϊ54603
*       2. ��ȡ��ǰ�ķ�����Ϊ 15*60+16=916������������4λ��0��
*   ��������cidΪ��546030916
* 
* 
*  ���ܣ����������ID��
*  ������
*	1. cid_lenth : ����ID�ĳ��ȣ�Ĭ��ֵΪ0
******************************************************************************************/
std::string UtilityTool::createCid(const int cid_lenth)
{
	//1. �õ����������Ĭ��5λ����cid����Ϊ9ʱ��������
	int rd_num = getRandomNum(cid_lenth - MIN_LEN);
	std::string rst = std::to_string(rd_num);

	//2. ��ȡ����ʱ�ڵ���ķ�����
	auto tt = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	struct tm* ptm = localtime(&tt);
	int min_num = (ptm->tm_hour * MIN_PER_HOUR) + ptm->tm_min;

	//3. ȷ����Ҫ��0�ĸ���
	size_t zero_len = MIN_LEN - std::to_string(min_num).size();
	for (size_t i = 0; i < zero_len; ++i)
	{
		rst += "0";
	}

	return (rst + std::to_string(min_num));
}

/******************************************************************************************
*  ˵����
*     ��ʱ����ջ��飬��Ҫ��ȡ��ǰϵͳ��ʱ���������Ҫ�ܸߵľ��ȣ����Ӽ��ɡ�
*     �˽ӿڵľ���Ϊ���ӣ�ʹ������ȷ�������Ƿ����������
*
*  ���ܣ���ȡ��ǰϵͳʱ���ʱ��������ӣ�
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
*  ˵����
*     ��ʱ����ջ��飬��Ҫ��ȡ��ǰϵͳ��ʱ���������Ҫ�ܸߵľ��ȣ����Ӽ��ɡ�
*     �˽ӿڵľ���Ϊ���ӣ�ʹ������ȷ�������Ƿ����������
*
*  ���ܣ���ȡ��������Ի�ȡ��ǰϵͳʱ���ʱ����ĺ������ķ�ʽ��ȡ��4λ����
*
******************************************************************************************/
std::time_t UtilityTool::getMillSeconds()
{
	auto tp = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());
	auto tmp = std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch());
	return tmp.count() % 100000;
}