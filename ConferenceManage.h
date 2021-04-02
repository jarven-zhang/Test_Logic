#pragma once
#include <iostream>
#include <unordered_map>
#include <queue>

#include "Conference.h"
#include "UtilityTool.h"
#include "User.h"
#include "type.h"

using PtrConf = std::shared_ptr<Conference>;

//std::string: cid,  std::list<uint32>: 会议中所有用户的connect id
using CidUserList = std::tuple<std::string, const std::list<PUser>>;

//元组的第一个元素表示成功或失败，第二个表示用户列表
using RstUserList = std::tuple<int, const std::list<PUser>>;

class ConferenceManage
{
public:
	ConferenceManage();

	//通过cid获取conference对象
	PtrConf getConference(const std::string& cid);

	//创建会议
	PtrConf createConference(PUser user, const std::string& passwd);

	//根据cid来查找此会议是否在空会议map中, 有则直接删除
	void delConfInEmptyConfList(const std::string& cid);

	//会议为空，添加进空会议Map
	inline void addEmptyConference(const std::string& cid)
	{
		std::lock_guard<std::mutex> lk(m_emptyConf_mutex);
		m_emptyConferenceMap.insert({ cid, UtilityTool::getTimeStamp() });
	}

private:
	//生成一个新的cid
	const string createCid();

	//定时任务，判断空会议Map里面是否有超时会议需要清除
	void TimingTask();

	//空会议超时后，清除该会议的所有信息
	void cleanEmptyConference(const std::string& cid);

	//新增会议后，将会议添加进会议Map
	inline void addConference(const std::string& cid, const PtrConf conf)
	{
		std::lock_guard<std::mutex> lk(m_confMap_mutex);
		m_conferenceMap.insert(std::make_pair(cid, conf));
	}

	static std::atomic<long> g_currentCid;

	std::mutex m_confMap_mutex;  //管理会议列表的互斥量
	std::mutex m_emptyConf_mutex;//空会议列表的互斥量

	//key: cid, value: Conference对象指针
	std::unordered_map<std::string, PtrConf> m_conferenceMap; 

	//空会议的Map，key:cid, value:会议为空时的时间
	std::unordered_map<std::string, time_t> m_emptyConferenceMap;
};