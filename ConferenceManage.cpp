#include "ConferenceManage.h"
#include "UtilityTool.h"

std::atomic<long> ConferenceManage::g_currentCid{ 100000001 };//cid��ʼ����

ConferenceManage::ConferenceManage()
{	//������ʱ����ȥ�����ʱ�Ŀջ���
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
*	��������
*   ������ 
*		1. user  : �û��Ķ����ṩ�û�������Ϣ
*		2. passwd: ��������
*   ����ֵ��
*		PtrConf �������ָ��
*/
PtrConf ConferenceManage::createConference(PUser user, const std::string& passwd)
{
	std::string cid = user->getCid();
	//cidΪ����Ϊ����
	if (cid.empty())
	{
		cid = createCid();
	}

	PtrConf p_conf = std::make_shared<Conference>(user, cid, passwd);
	addConference(cid, p_conf);
	return p_conf;

}

//ͨ��cid��ȡ�������, �ҵ��򷵻ػ������ָ�룬���򷵻ؿ�ָ��
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
* ���û�����������Ҫ�жϴ˻����Ƿ��ڿջ����б���
* ����cid�����Ҵ˻����Ƿ��ڿջ����б���,����ֱ��ɾ��  
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

//��ʱ�����жϿջ���Map�����Ƿ��г�ʱ������Ҫ���
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
		//��ʱ��ɾ��
		if (cur_time - ptr->second > TimeoutEmptyConf)
		{
			cleanEmptyConference(ptr->first);
			m_emptyConferenceMap.erase(ptr++);
			
			continue;
		}
		ptr++;
	}
}

//ȷ�ϸÿջ��鳬ʱ����������б��еĿջ���
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