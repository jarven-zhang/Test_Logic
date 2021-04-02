#pragma once
#include <iostream>
#include <unordered_map>
#include <queue>

#include "Conference.h"
#include "UtilityTool.h"
#include "User.h"
#include "type.h"

using PtrConf = std::shared_ptr<Conference>;

//std::string: cid,  std::list<uint32>: �����������û���connect id
using CidUserList = std::tuple<std::string, const std::list<PUser>>;

//Ԫ��ĵ�һ��Ԫ�ر�ʾ�ɹ���ʧ�ܣ��ڶ�����ʾ�û��б�
using RstUserList = std::tuple<int, const std::list<PUser>>;

class ConferenceManage
{
public:
	ConferenceManage();

	//ͨ��cid��ȡconference����
	PtrConf getConference(const std::string& cid);

	//��������
	PtrConf createConference(PUser user, const std::string& passwd);

	//����cid�����Ҵ˻����Ƿ��ڿջ���map��, ����ֱ��ɾ��
	void delConfInEmptyConfList(const std::string& cid);

	//����Ϊ�գ���ӽ��ջ���Map
	inline void addEmptyConference(const std::string& cid)
	{
		std::lock_guard<std::mutex> lk(m_emptyConf_mutex);
		m_emptyConferenceMap.insert({ cid, UtilityTool::getTimeStamp() });
	}

private:
	//����һ���µ�cid
	const string createCid();

	//��ʱ�����жϿջ���Map�����Ƿ��г�ʱ������Ҫ���
	void TimingTask();

	//�ջ��鳬ʱ������û����������Ϣ
	void cleanEmptyConference(const std::string& cid);

	//��������󣬽�������ӽ�����Map
	inline void addConference(const std::string& cid, const PtrConf conf)
	{
		std::lock_guard<std::mutex> lk(m_confMap_mutex);
		m_conferenceMap.insert(std::make_pair(cid, conf));
	}

	static std::atomic<long> g_currentCid;

	std::mutex m_confMap_mutex;  //��������б�Ļ�����
	std::mutex m_emptyConf_mutex;//�ջ����б�Ļ�����

	//key: cid, value: Conference����ָ��
	std::unordered_map<std::string, PtrConf> m_conferenceMap; 

	//�ջ����Map��key:cid, value:����Ϊ��ʱ��ʱ��
	std::unordered_map<std::string, time_t> m_emptyConferenceMap;
};