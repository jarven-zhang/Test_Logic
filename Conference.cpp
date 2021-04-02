#include "Conference.h"
#include "DataHandle.h"
#include "UtilityTool.h"

//const int CREATOR_USERID = 1;

//构造会议，即创建会议
Conference::Conference(PUser user, const std::string& cid, const std::string& passwd) noexcept
	: m_seqNum{0}     //会议开始时数据存放数量为0
	, m_userSerial{0}
	, m_cid{cid}
	, m_passwd{passwd}
{
	user->setCid(cid);
	user->setStatus(UserStatus::IN_CONF);

	//const uint64 t_userId = createUserId(cid);
	//user->setUserId(t_userId);
	m_userMap.insert(std::make_pair(user->getUserId(), user));

#if DEBUG_
	std::cout << "[Create conference]   creator info:" << std::endl;
	user->printInfo();
#endif
}

/*
*	加入会议，即将用户添加进会议
*   参数：
*		1. user  : 用户的对象，提供用户所有信息
*		2. passwd: 会议密码
*   返回值：错误信息
* -----------------------------------------------------
* 以下为废弃掉的注释：
* StringString
*		第一部分：useId
*			其值为空，说明参数错误，可能是密码错误或是用户状态、用户名称不匹配
*			非空说明成功, 其值为userId,即用户在会议中的ID
*		第二部分：错误原因
*			成功时为空，否则返回错误原因
* -----------------------------------------------------
*/
const std::string Conference::addUser(PUser user, const std::string& passwd)
{
	std::string error_reason{};

	if (nullptr == user)
	{
		error_reason = OTHER_ERR;
		return error_reason;
	}

	//当会议的密码本身为空时，不需要判断加入者的密码
	if (!(getPasswd().empty()) and getPasswd() != passwd)//密码不匹配的情况
	{
		std::cerr << "The passwd is wrong! User name:" << user->getUserName() << std::endl;
		error_reason = PSWD_MISS;
		return  error_reason;
	}

	//密码验证成功,可以加入会议
	uint64 user_id = user->getUserId();
	auto pUser = m_userMap.find(user_id);
	if (pUser != m_userMap.end())
	{
		//当用户重连时，依然可以通过user_id来获取User对象
		// 但其重新加入会议成功，必须避免下面情形：
		// 1.找到User对象，但是其状态是在会议中
		// 2.名字不匹配(会议中不允许改名，I don't know why)
		if (UserStatus::IN_CONF == pUser->second->getStatus())
		{
			error_reason = REPEATE_JOIN;
			return error_reason;
		}
		
		if (user->getUserName() != pUser->second->getUserName())
		{
			std::cerr << "The userName is wrong!" << std::endl;
			error_reason = NAME_MISS;
			return error_reason;
		}
		//用户为重连用户
		pUser->second->setStatus(UserStatus::IN_CONF);
		//重置Map中用户对象的连接Fd
		pUser->second->setConnectionFd(user->getConnectionFd());
	}
	else 
	{	//至此为新用户
		//将user存入会议的用户列表中
		m_userMap.insert(std::make_pair(user_id, user));
	}

	//设置User的状态为已经进入会议
	user->setStatus(UserStatus::IN_CONF);

#if DEBUG_ 
	std::cout << "[Join conference]   creator info:" << std::endl;
	user->printInfo();
#endif

	return error_reason;
}

/* 退出会议，不要将用户删除出会议，只改变状态: 在线但不在会议中
*  原名称为deleteUser， 现改为setUserOut
* 
* 参数   user：用户对象指针
* 返回值 list<PUser>：会议中在线用户列表
*/
std::list<PUser> Conference::setUserOut(const PUser user)
{
	//判断用户的合法性，包括User是否为空的情况
	const uint64 user_id = userInConference(user);
	if (0 == user_id)
	{  //用户非法，返回空用户列表
		return std::list<PUser>{};
	}

	std::lock_guard<std::recursive_mutex> lk(m_userMutex);
	//m_userMap[user_id]->cleanConferenceInfo();//清除此会议的信息
	//退出会议，只设置其状态为离开会议状态
	m_userMap[user_id]->setStatus(UserStatus::ONLINE_NO_CONF);

	return getUserConnectIdList();
}

/*存储数据，当客户端发送数据，判断是否需要存储数据。
*	参数：1.user :用户对象 2. data:具体数据 3.isSave:是否保存
*	返回值：元组
*		第一部分：数据的MsgId
*		第二部分：当前会议的所有在线用户列表
*/
MsgId_UserList Conference::addData(const PUser user, const std::string& data, const bool isSave)
{
	//不需要存储的数据，其msgId为0
	int msgId = 0;
	//判断用户的合法性，包括User是否为空的情况
	if (userInConference(user) == 0)
	{  //用户非法
		return std::make_tuple(-1, std::list<PUser>{});
	}

	if (isSave)
	{
		std::lock_guard<std::mutex> lk(m_dataMutex);
		m_dataList.emplace_back(data);
		msgId = ++m_seqNum;
	}

	return std::make_tuple(msgId, getUserConnectIdList());
}

/*
*   查询数据获取数据
*	参数： 
		1. user 用户对象 
*		2. num 用户最新的数据的序号
*	返回值：历史数据链表
		需要判断返回值是否为空，当用户权限出错或是历史数据为空时，返回值为空。
*/
const std::list<std::string> Conference::getData(const PUser user, const int num)
{
	//判断用户的合法性，包括User是否为空的情况
	if (userInConference(user) == 0)
	{   //参数非法
		return std::list<std::string>{};
	}

	std::lock_guard<std::mutex> lk_t(m_dataMutex);

	//通过遍历的方式找到Num对应的数据的迭代器
	auto pData = m_dataList.cbegin();
	for (int i = 1; pData != m_dataList.cend() and i <= num; ++pData, ++i);

	//历史数据为空时将返回空List, 也无需单独判断
	return std::list<std::string>(pData, m_dataList.cend());
}

//查询会议密码
std::string Conference::getPasswd() const
{
	return m_passwd;
}

//获取会议ID
std::string Conference::getCid() const
{
	return m_cid;
}

/* 判断用户在此会议中
 * 参数：user 用户指针
 * 返回值：为空则说明找不到用户，否则返回user id
 */
uint64 Conference::userInConference(const PUser user)
{
	if (nullptr == user)
	{
		return 0;
	}

	std::lock_guard<std::recursive_mutex> lk(m_userMutex);

	auto ptr = m_userMap.find(user->getUserId());
	if (ptr != m_userMap.end() and 
		ptr->second->getUserName() == user->getUserName())
	{//当用户的user id和用户名都匹配时，认定其参加过会议
		return user->getUserId();
	}

	return 0;
}

//返回会议中所有在会议中用户的对象列表
std::list<PUser> Conference::getUserConnectIdList()
{
	std::list<PUser> idList;

	std::lock_guard<std::recursive_mutex> lk(m_userMutex);
	for (auto ptr : m_userMap)
	{
		if (UserStatus::IN_CONF == ptr.second->getStatus())
		{
			idList.emplace_back(ptr.second);
		}
	}
	return idList;
}

#if 0
/* 通过会议ID和自增生成用户ID
*  参数：
*	cid: 会议ID
*  返回值：
*	用户ID
* 
*  示例：cid: 123456789
*        自增量为7
*  生成的UID：1234567897
*/
std::string Conference::createUserId(const std::string& cid)
{
	std::string num{ cid };

	std::lock_guard<std::mutex> lk(m_SeqMutex);
	//我们认为会议中的用户不可能超过99人
	if (++m_userSerial < 10)
	{
		num += "0";
	}
	num += std::to_string(m_userSerial);

	return std::move(num);
}
#endif