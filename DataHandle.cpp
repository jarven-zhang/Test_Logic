#include <memory>
#include "DataHandle.h"
#include "json.h"
#include "type.h"

/*
* �ӿͻ��˷��͵������н�����valueΪstring��ֵ
*/

std::string DataHandle::getStringValue(const std::string& jsonData, const std::string& key)
{
	pJson jRoot;
	jRoot.ParseJson(jsonData);
	cJSON* m_data = jRoot.get_json_root();

	//�������cmd
	std::string cmd_type{};
	jRoot.get_value(m_data, key, cmd_type);

	return cmd_type;
}

/*
* �ӿͻ��˷��͵������н�����valueΪuint64��ֵ
*/
uint64 DataHandle::getUint64Value(const std::string& jsonData, const std::string& key)
{
	pJson jRoot;
	jRoot.ParseJson(jsonData);
	cJSON* m_data = jRoot.get_json_root();

	//�������cmd
	uint64 id{ 0 };
	jRoot.get_value(m_data, key, id);

	return id;
}

/*
* �ӿͻ��˷��͵����󡰴����������顱�н�������Ӧ�Ĳ���ֵ
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
* �ӿͻ��˷��͵����󡰷������ݡ��н�������Ӧ�Ĳ���ֵ
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
*  �ͻ��˷��ʹ�����������������ɹ��󣬴�����������ʾ��Json���ݣ����ظ��ͻ���
*  ������
* 	1. cid     : ����ID
*	2. userList: �����û��б�
*  ����ֵ�����ظ����鴴���ߵ�����
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
*  �ͻ��˼������ʧ�ܺ󣬴�����������ʾ��Json���ݣ����͸��ͻ���
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
*  ���ܣ�����Json������֪ͨ������
*  ������1. cmdType : �¼����������ͣ�����Ϊ������顢�˳�����
		 2. userId  : �û��Ļ���ID
*        3. userName: �û���
*  ����ֵ������
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
		break;//����Ӧ�ñ���
	}
	jRoot.set_value(cJSONRoot, CMD, cmd_type);
	jRoot.set_value(cJSONRoot, USER_ID,   userId);
	jRoot.set_value(cJSONRoot, USER_NAME, userName);

	return cJSON_Print(cJSONRoot);
}

/*
*  ���ܣ�����Json������֪ͨ�����������ݹ���
*  ������1. msgId  : ��Ϣ�ڴ˻����е�ȫ��ID
*        2. isRender: �Ƿ���Ҫ���ƣ�0Ϊ�񣨷��ͷ�Ϊ�񣩣�1Ϊ�ǣ������ͷ�Ϊ1��
*        3. data   : ��ʵ����
*  ����ֵ������
*   ���ʾ����
*   {
    "cmd": "sendMsg",
    "msgId":2,//��Ϣ��ȫ��ID
    "isRender":0,//���ظ�������0��
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
*  �ͻ����뿪����󣬴�����������ʾ��Json���ݣ����͸��ͻ���
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
* �ӿͻ��˷��͵����󡰲�ѯ��ȡ���ݡ��н�������Ӧ�Ĳ���ֵ
*  ������ jsonDataΪ�ϴ�������
* ����ֵ�� Ϊ��ȡ�����ݵ����
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
    "userId":"4758489644",//��һ������ʱ��ֵΪ0
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