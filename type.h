#pragma once

#define DEBUG_  1

//空会议在空会议列表里的超时时间
#if DEBUG_
const int TimeoutEmptyConf = 20;//暂定为20分钟
const int TimeInterval = 5;//暂定任务的时间间隔为5分钟
#else
const int TimeoutEmptyConf = 20;//暂定为20分钟
const int TimeInterval = 5;//暂定任务的时间间隔为5分钟
#endif

const long CidStartValue = 100000001;
const long CidEndValue   = 999999999;

enum class UserStatus
{
	OFFLINE,  //离线、断开连接
	ONLINE_NO_CONF, //在线，但是不在会议中
	IN_CONF,  // 在会议中
	UNKNOW    //未知状态
};

//定义此类型主要防止类型出错，确保cmd正确
enum class CmdType
{
	JOIN_MEET,
	LEAVE_MEET //暂时只定义两个
};

//cmd的种类
const std::string JOIN_MEET_STRING  = "JoinMeet";
const std::string GET_MSG_STRING    = "GetMsg";
const std::string SEND_MSG_STRING   = "SendMsg";
const std::string LEAVE_MEET_STRING = "LeaveMeet";
const std::string LOGON_STRING      = "Logon";

//json数据中的key字符
const std::string CMD       = "cmd";

//连接登录

//创建会议的请求
const std::string MEET_ID   = "meetId";
const std::string PASSWD    = "pwd";
const std::string USER_ID   = "userId";
const std::string USER_NAME = "userName";

//创建会议的返回
const std::string RES          = "res";
const std::string SUCCESS      = "success";
const std::string PSWD_MISS    = "Password Wrong!";
const std::string USERID_MISS  = "Userid Wrong!";
const std::string REPEATE_JOIN = "Repeat to join!";
const std::string NAME_MISS    = "User name Wrong!";
const std::string OTHER_ERR    = "System error!";

const std::string USER_LIST    = "userList";

//发送数据的请求
const std::string IS_SAVE      = "isSave";
const std::string MESSAGE      = "msg";

//发送数据的返回
const std::string MSG_ID       = "msgId"; 
const std::string IS_RENDER    = "isRender";

//登录失败
const std::string USERID_WRONG = "UserId error!";

#if 0
struct MessageData
{
	//MessageData();
	//virtual ~MessageData();
};
#endif

//客户端发送的创建会议的结构体（除去cmd)
struct JoinMeetJsonData
{
	//std::string cmd;

	//data部分
	std::string meetId{};
	std::string pwd{};
	unsigned long long userId{0};
	std::string userName{};
};

////会议管理类中管理一个存放空会议的队列，此结构体为改队列元素的类型
//struct EmptyConfInfo
//{
//	time_t     mins;//分钟时间戳
//	std::string cid;//会议ID
//};
