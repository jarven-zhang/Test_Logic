#pragma once
#include <iostream>
#include <list>
#include <map>
#include <vector>
#include <list>
#include <mutex>
#include <tuple>
#include "User.h"

using MsgId_UserList = std::tuple<int, std::list<PUser>>;
//using StringString = std::tuple<uint64, std::string>;

class Conference
{
public:
	//������飬����������
	Conference(PUser user, const std::string& cid, const std::string& passwd) noexcept;

	//������飬�����û���ӽ�����, ����cid
	const std::string addUser(PUser user, const std::string& passwd);

	//�˳����飬�����û���״̬��ɾ��
	std::list<PUser> setUserOut(const PUser user);

	//�洢���ݣ��ж��Ƿ���Ҫ�洢���ݡ�Ȼ�����ݷ��͸������û���
	MsgId_UserList addData(const PUser user, const std::string& data, const bool isSave);

	//��ѯ���ݻ�ȡ����
	const std::list<std::string> getData(const PUser user, const int num);

	//��ȡ����ID
	std::string getCid() const;

	//ConferenceManage������Ҫ��ѯĳ���������������ߵĲ�����ID
	std::list<PUser> getUserConnectIdList();

private:
	//��ѯ��������
	std::string getPasswd() const;

	//std::string createUserId(const std::string& cid);

	//inline void storeData(const std::string& data)
	//{
	//	std::lock_guard<std::mutex> lk(m_dataMutex);
	//	m_data.emplace_back(data);
	//	m_seqNum++;
	//}

	////��ĳһ���û�����ĳ������
	//inline void sendData(const PUser user, const std::string& dataString) const
	//{
	//	//user->getConnectionFd()->send_ws_string(dataString);
	//	std::cout << " send to" << user->getUserName() << " ,data:" << dataString << std::endl;
	//}

	//�жϲ����û��Ƿ��ڱ������У���ʹ���Ѿ��˳���������user id
	uint64 userInConference(const PUser user);

	std::string   m_cid;   //����ID 
	std::string m_passwd;  //��������

	//������������� key: �û�ID�� value: �û�����
	std::map<uint64, PUser> m_userMap;
	std::recursive_mutex  m_userMutex; // for�û��б�
	//std::mutex  m_userMutex; // for�û��б�
	
	///* ˵����Ϊ����ConferenceManage����Ļ�ȡ���鵱ǰ�����û��������������˱���
	//* 
	//* ��Ϊm_userVector�в��������������߶��󣬻�����뿪������û�����
	//* ���ر�����һ�����������洢������������������������
	//*/
	//std::atomic<int> m_userNumber;    //����ߵĸ���

	//�����û��Ļ���ID����Ҫcid�ʹ˱���
	//�˱�����ʾ������û�������ʱ�����
	std::atomic<uint32> m_userSerial;   //�жϴ�Ϊ����1С��99����
	std::mutex           m_SeqMutex;    // for���к�

	std::list<std::string> m_dataList;  //������ʷ����
	std::mutex            m_dataMutex;  // for��ʷ����
	std::atomic<int> m_seqNum;          //������ĵ�ǰͬ���������ݵ����к�
};
