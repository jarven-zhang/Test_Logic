#include <memory>
#include "DataHandle.h"
#include "json.h"
#include "type.h"

/*
* 从客户端发送的请求中解析出value为string的值
*/

std::string DataHandle::getStringValue(const std::string& jsonData, const std::string& key)
{
	pJson jRoot;
	jRoot.ParseJson(jsonData);
	cJSON* m_data = jRoot.get_json_root();

	//用来存放cmd
	std::string cmd_type{};
	jRoot.get_value(m_data, key, cmd_type);

	return cmd_type;
}

/*
* 从客户端发送的请求中解析出value为uint64的值
*/
uint64 DataHandle::getUint64Value(const std::string& jsonData, const std::string& key)
{
	pJson jRoot;
	jRoot.ParseJson(jsonData);
	cJSON* m_data = jRoot.get_json_root();

	//用来存放cmd
	uint64 id{ 0 };
	jRoot.get_value(m_data, key, id);

	return id;
}

/*
* 从客户端发送的请求“创建或加入会议”中解析出相应的参数值
*/
JoinMeetJsonData DataHandle::getJoinMeetData(const std::string& jsonData)
{
	pJson jRoot;
	jRoot.ParseJson(jsonData);
	cJSON* m_data = jRoot.get_json_root();

	JoinMeetJsonData t_data;
	jRoot.get_value(m_data, MEET_ID,   t_data.meetId);
	jRoot.get_value(m_data, PASSWD,    t_data.pwd);
	jRoot.get_value(m_data, USER_ID,   t_data.userId);
	jRoot.get_value(m_data, USER_NAME, t_data.userName);

	return t_data;
}

/*
* 从客户端发送的请求“发送数据”中解析出相应的参数值
*/
std::tuple<int, std::string> DataHandle::getSendMsgData(const std::string& jsonData)
{
	pJson jRoot;
	jRoot.ParseJson(jsonData);
	cJSON* m_data = jRoot.get_json_root();

	int tmp{};
	std::string src{};
	jRoot.get_value(m_data, IS_SAVE, tmp);
	jRoot.get_value(m_data, MESSAGE, src);

	return std::make_tuple(tmp, src);
}

/*
*  客户端发送创建、加入会议的请求成功后，创建如下例所示的Json数据，返回给客户端
*  参数：
* 	1. cid     : 会议ID
*	2. userList: 在线用户列表
*  返回值：返回给会议创建者的数据
* 
*   {
    "cmd": "JoinMeet",
    "res": "success"
    "userList":[{
        "userId":1,
        "userName":"jack"
         }]
    }
*/
const std::string DataHandle::createJoinMeetReturnData(const string &cid, std::list<PUser> userList)
{
	pJson jRoot;

	cJSON* arrayRoot = jRoot.create_object();
	cJSON* pArry = jRoot.create_array(arrayRoot, USER_LIST);

	cJSON* param = jRoot.create_object();
	jRoot.set_value(arrayRoot, CMD, JOIN_MEET_STRING);
	jRoot.set_value(arrayRoot, RES, SUCCESS);
	jRoot.set_value(arrayRoot, MEET_ID, cid);

	for (auto ptr: userList)
	{
		cJSON* pUser = jRoot.create_object();

		jRoot.set_value(pUser, USER_ID, ptr->getUserId());
		jRoot.set_value(pUser, USER_NAME, ptr->getUserName());
		jRoot.set_array(pArry, pUser);
	}

	return cJSON_Print(arrayRoot);
}

/*
*  客户端加入会议失败后，创建如下例所示的Json数据，发送给客户端
*   {
	"cmd": "JoinMeet",
    "res": "Failed to join meeting!"
     }
*/
const std::string DataHandle::createJoinMeetFalseReturnData(const string& err_string)
{
	pJson jRoot;
	cJSON* cJSONRoot = jRoot.create_object();

	jRoot.set_value(cJSONRoot, CMD, JOIN_MEET_STRING);
	jRoot.set_value(cJSONRoot, RES, err_string);

	return cJSON_Print(cJSONRoot);
}

/*
*  功能：生成Json数据来通知其他人
*  参数：1. cmdType : 事件或命令类型，可能为加入会议、退出会议
		 2. userId  : 用户的会议ID
*        3. userName: 用户名
*  返回值：数据
*   {
    "cmd": "JoinMeet",
    "userId": 1,
    "userName":"jack"
     }
*/
const std::string DataHandle::createDataToOthers(const CmdType cmdType,
	const uint64 userId, const std::string& userName)
{
	pJson jRoot;
	cJSON* cJSONRoot = jRoot.create_object();

	std::string cmd_type{};
	switch (cmdType)
	{
	case CmdType::JOIN_MEET:
		cmd_type = JOIN_MEET_STRING;
		break;
	case CmdType::LEAVE_MEET:
		cmd_type = LEAVE_MEET_STRING;
		break;
	default:
		break;//这里应该报错
	}
	jRoot.set_value(cJSONRoot, CMD, cmd_type);
	jRoot.set_value(cJSONRoot, USER_ID,   userId);
	jRoot.set_value(cJSONRoot, USER_NAME, userName);

	return cJSON_Print(cJSONRoot);
}

/*
*  功能：生成Json数据来通知其他人有数据过来
*  参数：1. msgId  : 消息在此会议中的全局ID
*        2. isRender: 是否需要绘制，0为否（发送方为否），1为是（被推送方为1）
*        3. data   : 真实数据
*  返回值：数据
*   结果示例：
*   {
    "cmd": "sendMsg",
    "msgId":2,//消息的全局ID
    "isRender":0,//返回给发送者0；
    "msg":""
    }
*/
const std::string DataHandle::createSendData(const int msgId, const int isRender, const std::string& data)
{
	pJson jRoot;
	cJSON* cJSONRoot = jRoot.create_object();

	jRoot.set_value(cJSONRoot, CMD, SEND_MSG_STRING);
	jRoot.set_value(cJSONRoot, MSG_ID, msgId);
	jRoot.set_value(cJSONRoot, IS_RENDER, isRender);
	jRoot.set_value(cJSONRoot, MESSAGE, data);

	return cJSON_Print(cJSONRoot);
}

/*
*  客户端离开会议后，创建如下例所示的Json数据，发送给客户端
*   
  {
	"cmd": "leaveMeet",
	"res": "success"
  }
*/
const std::string DataHandle::createLeaveReturnData()
{
	pJson jRoot;
	cJSON* cJSONRoot = jRoot.create_object();

	jRoot.set_value(cJSONRoot, CMD, LEAVE_MEET_STRING);
	jRoot.set_value(cJSONRoot, RES, SUCCESS);

	return cJSON_Print(cJSONRoot);
}

/*
* 从客户端发送的请求“查询获取数据”中解析出相应的参数值
*  参数： jsonData为上传的数据
* 返回值： 为获取的数据的序号
*/
int DataHandle::getQueryData(const std::string& jsonData)
{
	pJson jRoot;
	jRoot.ParseJson(jsonData);
	cJSON* m_data = jRoot.get_json_root();

	int number{};
	jRoot.get_value(m_data, MSG_ID, number);

	return number;
}

/*
* {
    "cmd": "Logon",
    "userId":"4758489644",//第一次连接时此值为0
  }
*/
const std::string DataHandle::createLogonData(const uint64 userId)
{
	pJson jRoot;
	cJSON* cJSONRoot = jRoot.create_object();

	jRoot.set_value(cJSONRoot, CMD, LOGON_STRING);
	jRoot.set_value(cJSONRoot, USER_ID, userId);
	//jRoot.set_value(cJSONRoot, RES, res);

	return cJSON_Print(cJSONRoot);
}