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
	//构造会议，即创建会议
	Conference(PUser user, const std::string& cid, const std::string& passwd) noexcept;

	//加入会议，即将用户添加进会议, 返回cid
	const std::string addUser(PUser user, const std::string& passwd);

	//退出会议，重置用户的状态不删除
	std::list<PUser> setUserOut(const PUser user);

	//存储数据，判断是否需要存储数据。然后将数据发送给其他用户。
	MsgId_UserList addData(const PUser user, const std::string& data, const bool isSave);

	//查询数据获取数据
	const std::list<std::string> getData(const PUser user, const int num);

	//获取会议ID
	std::string getCid() const;

	//ConferenceManage对象需要查询某个会议中所有在线的参与者ID
	std::list<PUser> getUserConnectIdList();

private:
	//查询会议密码
	std::string getPasswd() const;

	//std::string createUserId(const std::string& cid);

	//inline void storeData(const std::string& data)
	//{
	//	std::lock_guard<std::mutex> lk(m_dataMutex);
	//	m_data.emplace_back(data);
	//	m_seqNum++;
	//}

	////向某一个用户发送某个数据
	//inline void sendData(const PUser user, const std::string& dataString) const
	//{
	//	//user->getConnectionFd()->send_ws_string(dataString);
	//	std::cout << " send to" << user->getUserName() << " ,data:" << dataString << std::endl;
	//}

	//判断参数用户是否在本会议中，即使其已经退出，返回其user id
	uint64 userInConference(const PUser user);

	std::string   m_cid;   //会议ID 
	std::string m_passwd;  //会议密码

	//所有与会者名称 key: 用户ID， value: 用户对象
	std::map<uint64, PUser> m_userMap;
	std::recursive_mutex  m_userMutex; // for用户列表
	//std::mutex  m_userMutex; // for用户列表
	
	///* 说明：为了让ConferenceManage更快的获取会议当前在线用户的数量而创建此变量
	//* 
	//* 因为m_userVector中不仅存放所有与会者对象，还存放离开会议的用户对象
	//* 故特别设置一个参数用来存储真正的与会者人数，避免遍历
	//*/
	//std::atomic<int> m_userNumber;    //与会者的个数

	//创建用户的会议ID，需要cid和此变量
	//此变量表示最近的用户进会议时的序号
	std::atomic<uint32> m_userSerial;   //判断此为大于1小于99的数
	std::mutex           m_SeqMutex;    // for序列号

	std::list<std::string> m_dataList;  //所有历史数据
	std::mutex            m_dataMutex;  // for历史数据
	std::atomic<int> m_seqNum;          //本会议的当前同步到的数据的序列号
};
