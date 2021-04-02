#pragma once

#include <iostream>
#include <memory>
#include <list>
#include "type.h"
#include "User.h"

/*��������json����*/
class DataHandle
{
public:
	//��ȡcmd����, ����ǻ�ȡstring��������
	static std::string getStringValue(const std::string& jsonData, const std::string& key);

	//��ȡcmd����
	static uint64 getUint64Value(const std::string& jsonData, const std::string& key);

	//֪ͨ�����˵Ľӿڣ��統���������ɹ���ʱ�����˳����顱ʱ���ӿ�ͳһ��
	static const std::string createDataToOthers(const CmdType cmdType,
		const uint64 userId, const std::string& userName);

	//1. �������
	//��ȡ��������顱�����еĵ���������
	static JoinMeetJsonData getJoinMeetData(const std::string& jsonData);
	//���ɡ��������ɹ���ʱ����ʱ��Json����
	static const std::string createJoinMeetReturnData(const std::string& cid, std::list<PUser> userList);
	//���ɡ��������ʧ�ܡ�ʱ����ʱ��Json����
	static const std::string createJoinMeetFalseReturnData(const string& err_string);
	
	//2. ��������
	//��ȡ���������ݡ�����Ĳ���������ֵ
	static std::tuple<int, std::string> getSendMsgData(const std::string& jsonData);
	//���ɡ��������ݡ�ʱ���ظ��û���Json����
	static const std::string createSendData(const int msgId, const int isRender, const std::string& data);

	//3. �˳�����
	//���ɡ��˳����顱ʱ���ص�Json����
	static const std::string createLeaveReturnData();
	
	//4. ��ѯ����, ����Ҫ��ѯ�����ݵ����
	static int getQueryData(const std::string& jsonData);
	
	//5. �û���¼
	static const std::string createLogonData(const uint64 userId);
};