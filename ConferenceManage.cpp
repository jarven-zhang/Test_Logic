#include "ConferenceManage.h"
#include "UtilityTool.h"

std::atomic<long> ConferenceManage::g_currentCid{ 100000001 };//cid起始基数

ConferenceManage::ConferenceManage()
{	//创建定时任务去清除超时的空会议
	std::thread([this]() {
		while (1)
		{
			TimingTask();
			std::this_thread::sleep_for(std::chrono::minutes(TimeInterval));
			//std::this_thread::sleep_for(std::chrono::seconds(10));
		}
		}).detach();
}

/*
*	创建会议
*   参数： 
*		1. user  : 用户的对象，提供用户所有信息
*		2. passwd: 会议密码
*   返回值：
*		PtrConf 会议对象指针
*/
PtrConf ConferenceManage::createConference(PUser user, const std::string& passwd)
{
	std::string cid = user->getCid();
	//cid为空则为生成
	if (cid.empty())
	{
		cid = createCid();
	}

	PtrConf p_conf = std::make_shared<Conference>(user, cid, passwd);
	addConference(cid, p_conf);
	return p_conf;

}

//通过cid获取会议对象, 找到则返回会议对象指针，否则返回空指针
PtrConf ConferenceManage::getConference(const std::string& cid)
{
	if (cid.empty())
	{
		return nullptr;
	}

	std::lock_guard<std::mutex> lk(m_confMap_mutex);
	auto ptr = m_conferenceMap.find(cid);
	if (ptr != m_conferenceMap.end())
	{
		return ptr->second;
	}
	return nullptr;
}

/* 
* 当用户加入会议后，需要判断此会议是否在空会议列表中
* 根据cid来查找此会议是否在空会议列表中,有则直接删除  
*/
void ConferenceManage::delConfInEmptyConfList(const std::string& cid)
{
	std::lock_guard<std::mutex> lk(m_emptyConf_mutex);

	auto ptr = m_emptyConferenceMap.find(cid);
	if (m_emptyConferenceMap.end() != ptr)
	{
		m_emptyConferenceMap.erase(ptr);
	}
}

//定时任务，判断空会议Map里面是否有超时会议需要清除
void ConferenceManage::TimingTask()
{
	const time_t cur_time = UtilityTool::getTimeStamp();

	std::lock_guard<std::mutex> lk(m_emptyConf_mutex);

	auto ptr = m_emptyConferenceMap.begin();
	for ( ; ptr != m_emptyConferenceMap.end();)
	{
		/*std::cout << " =====Timing task, cid:" << ptr->first 
			<< ", time:" <<ptr->second
			<< std::endl;*/
		//超时后删除
		if (cur_time - ptr->second > TimeoutEmptyConf)
		{
			cleanEmptyConference(ptr->first);
			m_emptyConferenceMap.erase(ptr++);
			
			continue;
		}
		ptr++;
	}
}

//确认该空会议超时，清除会议列表中的空会议
void ConferenceManage::cleanEmptyConference(const std::string& cid)
{
	std::lock_guard<std::mutex> lk(m_confMap_mutex);

	auto ptr = m_conferenceMap.find(cid);
	if (m_conferenceMap.end() != ptr)
	{
		std::cout << "Now clean the empty conference:" << cid << std::endl;
		m_conferenceMap.erase(ptr);
	}
}

const string ConferenceManage::createCid()
{
	g_currentCid += static_cast<long>(UtilityTool::getMillSeconds());
	if (g_currentCid >= CidEndValue)
	{
		g_currentCid = CidStartValue;
	}
	return to_string(g_currentCid);
}