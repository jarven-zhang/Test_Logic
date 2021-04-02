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

	//ÿ��User�����Ӧһ��ȫ��Ψһ��ʶuserID������Ϊ���캯������
	User(const uint32 fd);

	//�ڴ�������ʱ��ʼ��һЩ��Ҫ����
	void initConferenceInfo(const JoinMeetJsonData& data);

	//���˳�����ʱ������״̬
	void cleanConferenceInfo();

	//��ȡ�û�����
	const std::string getUserName() const;
	//�����û�����
	void setUserName(const std::string& name);

	//��ȡ�������û��Ļ����
	const std::string getCid() const;
	void setCid(const std::string& cid);

	//��ȡ�������û���״̬
	const UserStatus getStatus() const;
	void setStatus(const UserStatus& cid);

	//��ȡ�������û��ڻ����е�ID���
	const uint64 getUserId() const;
	void setUserId(const uint64 userId);

	//��ȡ�������û������Ӷ���
	uint32 getConnectionFd() const;
	void setConnectionFd(const uint32 new_connect);

	//��ӡ��Ϣ�����ڵ���
	void printInfo() const;
	
private:
	std::string m_name;    // �û���
	std::string m_cid;     // ���������ID���޻���ʱ��ֵΪ0
	uint64      m_userId;  // �û����к�ID, ���Ψһ��ʶ
	UserStatus  m_status;  // �û���״̬
	uint32      m_fd;      // PSS���û�������Fd
};

using PUser = std::shared_ptr<User>;