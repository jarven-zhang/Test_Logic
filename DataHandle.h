#pragma once

#include <iostream>
#include <memory>
#include <list>
#include "type.h"
#include "User.h"

/*解析处理json数据*/
class DataHandle
{
public:
	//获取cmd类型, 这个是获取string类型数据
	static std::string getStringValue(const std::string& jsonData, const std::string& key);

	//获取cmd类型
	static uint64 getUint64Value(const std::string& jsonData, const std::string& key);

	//通知其他人的接口，如当“加入会议成功”时、“退出会议”时，接口统一了
	static const std::string createDataToOthers(const CmdType cmdType,
		const uint64 userId, const std::string& userName);

	//1. 加入会议
	//获取“加入会议”请求中的的所有数据
	static JoinMeetJsonData getJoinMeetData(const std::string& jsonData);
	//生成“加入会议成功”时返回时的Json数据
	static const std::string createJoinMeetReturnData(const std::string& cid, std::list<PUser> userList);
	//生成“加入会议失败”时返回时的Json数据
	static const std::string createJoinMeetFalseReturnData(const string& err_string);
	
	//2. 发送数据
	//获取“发送数据”请求的参数变量的值
	static std::tuple<int, std::string> getSendMsgData(const std::string& jsonData);
	//生成“发送数据”时返回给用户的Json数据
	static const std::string createSendData(const int msgId, const int isRender, const std::string& data);

	//3. 退出会议
	//生成“退出会议”时返回的Json数据
	static const std::string createLeaveReturnData();
	
	//4. 查询数据, 返回要查询的数据的序号
	static int getQueryData(const std::string& jsonData);
	
	//5. 用户登录
	static const std::string createLogonData(const uint64 userId);
};