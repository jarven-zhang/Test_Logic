#pragma once

#include <iostream>

constexpr int CID_LEN   = 9;
//MIN_LENΪÿ��ķ��������ֵΪ1440�����䳤��Ϊ4λ
constexpr int MIN_LEN = 4;
constexpr int MIN_PER_HOUR = 60;

class UtilityTool
{
public:
	//��������ID
	static std::string createCid(const int cid_lenth = 9);

	//��ȡָ�����ȵ��������
	static int getRandomNum(const int lenth);

	//��ȡ��ǰϵͳʱ���ʱ��������ӣ�
	static time_t getTimeStamp();

	//��ȡ��ǰϵͳʱ���ʱ������룬��ʵҲ���ǣ���ȡ������֮��ȡ��100000
	static std::time_t getMillSeconds();
};

