#pragma once
#include <iostream>
#include <memory>
#include "define.h"
#include "type.h"

class User
{
public:
	User()  = default;
	virtual ~User() = default;

	User(const User &) = delete;
	User& operator=(const User& clientUser)=delete;

	//每个User对象对应一个全局唯一标识userID，其作为构造函数参数
	User(const uint32 fd);

	//在创建会议时初始化一些必要参数
	void initConferenceInfo(const JoinMeetJsonData& data);

	//在退出会议时，设置状态
	void cleanConferenceInfo();

	//获取用户名称
	const std::string getUserName() const;
	//设置用户名称
	void setUserName(const std::string& name);

	//获取、设置用户的会议号
	const std::string getCid() const;
	void setCid(const std::string& cid);

	//获取、设置用户的状态
	const UserStatus getStatus() const;
	void setStatus(const UserStatus& cid);

	//获取、设置用户在会议中的ID序号
	const uint64 getUserId() const;
	void setUserId(const uint64 userId);

	//获取、设置用户的连接对象
	uint32 getConnectionFd() const;
	void setConnectionFd(const uint32 new_connect);

	//打印信息，便于调试
	void printInfo() const;
	
private:
	std::string m_name;    // 用户名
	std::string m_cid;     // 所属会议的ID，无会议时此值为0
	uint64      m_userId;  // 用户序列号ID, 身份唯一标识
	UserStatus  m_status;  // 用户的状态
	uint32      m_fd;      // PSS中用户的连接Fd
};

using PUser = std::shared_ptr<User>;