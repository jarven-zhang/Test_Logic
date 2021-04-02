#include <iostream>
#include "User.h"

//�ͻ�����������ʱ��ֻ�С����ӡ������ô˹��캯��
User::User(const uint32 fd):
	m_fd{ fd }
	, m_name{}
	, m_cid{}
	, m_userId{ }
	, m_status{ UserStatus::ONLINE_NO_CONF }
{
}

//User::User(const User& clientUser):
//	m_name{ clientUser.getUserName()}
//	, m_cid{clientUser.getCid()}
//	, m_userId{clientUser.getUserId()}
//	, m_status{clientUser.getStatus()}
//	, m_fd{clientUser.getConnectionFd()}
//{
//}

//User& User::operator=(const User& clientUser)
//{
//	this->m_name       = clientUser.getUserName();
//	this->m_cid        = clientUser.getCid();
//	this->m_userId     = clientUser.getUserId();
//	this->m_status     = clientUser.getStatus();
//	this->m_fd = clientUser.getConnectionFd();
//
//	return *this;
//}

//�ڴ�������ʱ�������ϴ������ݳ�ʼ��user����
void User::initConferenceInfo(const JoinMeetJsonData& data)
{
	this->m_name   = data.userName;
	this->m_cid    = data.meetId;
	//this->m_userId = data.userId;
	this->m_status = UserStatus::ONLINE_NO_CONF;
}

void User::cleanConferenceInfo()
{
	this->m_cid    = {};
	this->m_status = UserStatus::ONLINE_NO_CONF;
}

//��ȡ�û�����
const std::string User::getUserName() const
{
	return this->m_name;
}

void User::setUserName(const std::string& name)
{
	this->m_name = name;
}

const uint64 User::getUserId() const
{
	return this->m_userId;
}

void User::setUserId(const uint64 userId)
{
	this->m_userId = userId;
}

const std::string User::getCid() const
{
	return this->m_cid;
}

void User::setCid(const std::string& cid)
{
	this->m_cid = cid;
}

const UserStatus User::getStatus() const
{
	return this->m_status;
}

void User::setStatus(const UserStatus& status)
{
	this->m_status = status;
}

uint32 User::getConnectionFd() const
{
	return this->m_fd;
}

void User::setConnectionFd(const uint32 new_connect)
{
	this->m_fd = new_connect;
}

void User::printInfo() const
{
	std::cout << "----------- User info -------------\n" << std::endl;
	std::cout << "   name:" << this->getUserName() << std::endl;
	std::cout << "   cid :" << this->getCid() << std::endl;
	std::cout << "userid :" << this->getUserId() << std::endl;
	std::cout << "-----------------------------------\n" << std::endl;
}