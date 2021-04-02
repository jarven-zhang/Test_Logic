#pragma once

#define DEBUG_  1

//�ջ����ڿջ����б���ĳ�ʱʱ��
#if DEBUG_
const int TimeoutEmptyConf = 20;//�ݶ�Ϊ20����
const int TimeInterval = 5;//�ݶ������ʱ����Ϊ5����
#else
const int TimeoutEmptyConf = 20;//�ݶ�Ϊ20����
const int TimeInterval = 5;//�ݶ������ʱ����Ϊ5����
#endif

const long CidStartValue = 100000001;
const long CidEndValue   = 999999999;

enum class UserStatus
{
	OFFLINE,  //���ߡ��Ͽ�����
	ONLINE_NO_CONF, //���ߣ����ǲ��ڻ�����
	IN_CONF,  // �ڻ�����
	UNKNOW    //δ֪״̬
};

//�����������Ҫ��ֹ���ͳ���ȷ��cmd��ȷ
enum class CmdType
{
	JOIN_MEET,
	LEAVE_MEET //��ʱֻ��������
};

//cmd������
const std::string JOIN_MEET_STRING  = "JoinMeet";
const std::string GET_MSG_STRING    = "GetMsg";
const std::string SEND_MSG_STRING   = "SendMsg";
const std::string LEAVE_MEET_STRING = "LeaveMeet";
const std::string LOGON_STRING      = "Logon";

//json�����е�key�ַ�
const std::string CMD       = "cmd";

//���ӵ�¼

//�������������
const std::string MEET_ID   = "meetId";
const std::string PASSWD    = "pwd";
const std::string USER_ID   = "userId";
const std::string USER_NAME = "userName";

//��������ķ���
const std::string RES          = "res";
const std::string SUCCESS      = "success";
const std::string PSWD_MISS    = "Password Wrong!";
const std::string USERID_MISS  = "Userid Wrong!";
const std::string REPEATE_JOIN = "Repeat to join!";
const std::string NAME_MISS    = "User name Wrong!";
const std::string OTHER_ERR    = "System error!";

const std::string USER_LIST    = "userList";

//�������ݵ�����
const std::string IS_SAVE      = "isSave";
const std::string MESSAGE      = "msg";

//�������ݵķ���
const std::string MSG_ID       = "msgId"; 
const std::string IS_RENDER    = "isRender";

//��¼ʧ��
const std::string USERID_WRONG = "UserId error!";

#if 0
struct MessageData
{
	//MessageData();
	//virtual ~MessageData();
};
#endif

//�ͻ��˷��͵Ĵ�������Ľṹ�壨��ȥcmd)
struct JoinMeetJsonData
{
	//std::string cmd;

	//data����
	std::string meetId{};
	std::string pwd{};
	unsigned long long userId{0};
	std::string userName{};
};

////����������й���һ����ſջ���Ķ��У��˽ṹ��Ϊ�Ķ���Ԫ�ص�����
//struct EmptyConfInfo
//{
//	time_t     mins;//����ʱ���
//	std::string cid;//����ID
//};
