#pragma once
#include <iostream>
#include <thread>
#include <vector>
#include <list>

#include "define.h"
#include "User.h"
#include "ConferenceManage.h"
#include "IFrameObject.h"

//0�ǲ����Կ�������ӿڹ��ܣ�1�ǲ��ԡ�
const uint8 TEST_FRAME_WORK_FLAG = 0;

//���崦������command_id
const uint16 COMMAND_TEST_SYNC = 0x2101;
const uint16 COMMAND_TEST_ASYN = 0x2102;
const uint16 COMMAND_TEST_FRAME = 0x1100;
const uint16 COMMAND_TEST_HTTP_POST = 0x1001;
const uint16 COMMAND_WEBSOCKET_SHARK_HAND = 0x1002;
const uint16 COMMAND_WEBSOCKET_DATA = 0x1003;

const uint32 plugin_test_logic_thread_id = 1001;

class CBaseCommand
{
public:
	void Init(ISessionService* session_service);

	void logic_connect_tcp();
	void logic_connect_udp();

	void logic_connect(const CMessage_Source& source, std::shared_ptr<CMessage_Packet> recv_packet, std::shared_ptr<CMessage_Packet> send_packet);
	void logic_disconnect(const CMessage_Source& source, std::shared_ptr<CMessage_Packet> recv_packet, std::shared_ptr<CMessage_Packet> send_packet);
	void logic_test_sync(const CMessage_Source& source, std::shared_ptr<CMessage_Packet> recv_packet, std::shared_ptr<CMessage_Packet> send_packet);
	void logic_test_asyn(const CMessage_Source& source, std::shared_ptr<CMessage_Packet> recv_packet, std::shared_ptr<CMessage_Packet> send_packet);
	void logic_test_frame(const CMessage_Source& source, std::shared_ptr<CMessage_Packet> recv_packet, std::shared_ptr<CMessage_Packet> send_packet);
	void logic_test_connect_error(const CMessage_Source& source, std::shared_ptr<CMessage_Packet> recv_packet, std::shared_ptr<CMessage_Packet> send_packet);
	void logic_test_listen_error(const CMessage_Source& source, std::shared_ptr<CMessage_Packet> recv_packet, std::shared_ptr<CMessage_Packet> send_packet);
	void logic_http_post(const CMessage_Source& source, std::shared_ptr<CMessage_Packet> recv_packet, std::shared_ptr<CMessage_Packet> send_packet);
	void logic_http_websocket_shark_hand(const CMessage_Source& source, std::shared_ptr<CMessage_Packet> recv_packet, std::shared_ptr<CMessage_Packet> send_packet);
	void logic_http_websocket_data(const CMessage_Source& source, std::shared_ptr<CMessage_Packet> recv_packet, std::shared_ptr<CMessage_Packet> send_packet);

	ISessionService* session_service_ = nullptr;

private:
	//�������ݵ���ں���
	int handleMessage(const uint32 fd, const std::string& data, std::shared_ptr<CMessage_Packet> sendPtr);

	//�����û���¼�ĺ���
	void handleLogon(uint32 pUser, const std::string& data, std::shared_ptr<CMessage_Packet> sendPtr);

	//�������������Ľӿں���
	void handleJoinMeet(PUser pUser, const std::string& data, std::shared_ptr<CMessage_Packet> sendPtr);

	//�����˳�����Ľӿں���
	void handleExitMeet(PUser pUser, std::shared_ptr<CMessage_Packet> sendPtr);

	//���������ݵĽӿں���
	void handleSendData(PUser pUser, const std::string& data, std::shared_ptr<CMessage_Packet> sendPtr);

	//�����ѯ��ȡ���ݵĽӿ�
	void handleGetData(PUser pUser, const std::string& data, std::shared_ptr<CMessage_Packet> sendPtr);

	//ͨ��user id��ȡȫ�ֵ�User����
	PUser getUserByUserId(const uint64);
	//ͨ������fd��ȡȫ�ֵ�User����
	PUser getUserByFd(const uint32 fd);

	//���û��б���ע�����û�
	void registUser(const uint64 userId, PUser pUser);

	//�������ݸ������û��ĺ���
	void sendMsgToUser(PUser pUser, std::shared_ptr<CMessage_Packet> sendPtr,
		list<PUser> userList, const string& returnData, const string& notifyData);

	std::shared_ptr<ConferenceManage> pConferenceManage;
	//std::mutex  m_manageMutex;

	std::unordered_map<uint64, PUser> m_userMap;
	std::mutex  m_userMutex;
};

