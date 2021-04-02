#include "Conference.h"
#include "DataHandle.h"
#include "UtilityTool.h"

//const int CREATOR_USERID = 1;

//������飬����������
Conference::Conference(PUser user, const std::string& cid, const std::string& passwd) noexcept
	: m_seqNum{0}     //���鿪ʼʱ���ݴ������Ϊ0
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
*	������飬�����û���ӽ�����
*   ������
*		1. user  : �û��Ķ����ṩ�û�������Ϣ
*		2. passwd: ��������
*   ����ֵ��������Ϣ
* -----------------------------------------------------
* ����Ϊ��������ע�ͣ�
* StringString
*		��һ���֣�useId
*			��ֵΪ�գ�˵���������󣬿����������������û�״̬���û����Ʋ�ƥ��
*			�ǿ�˵���ɹ�, ��ֵΪuserId,���û��ڻ����е�ID
*		�ڶ����֣�����ԭ��
*			�ɹ�ʱΪ�գ����򷵻ش���ԭ��
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

	//����������뱾��Ϊ��ʱ������Ҫ�жϼ����ߵ�����
	if (!(getPasswd().empty()) and getPasswd() != passwd)//���벻ƥ������
	{
		std::cerr << "The passwd is wrong! User name:" << user->getUserName() << std::endl;
		error_reason = PSWD_MISS;
		return  error_reason;
	}

	//������֤�ɹ�,���Լ������
	uint64 user_id = user->getUserId();
	auto pUser = m_userMap.find(user_id);
	if (pUser != m_userMap.end())
	{
		//���û�����ʱ����Ȼ����ͨ��user_id����ȡUser����
		// �������¼������ɹ�����������������Σ�
		// 1.�ҵ�User���󣬵�����״̬���ڻ�����
		// 2.���ֲ�ƥ��(�����в����������I don't know why)
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
		//�û�Ϊ�����û�
		pUser->second->setStatus(UserStatus::IN_CONF);
		//����Map���û����������Fd
		pUser->second->setConnectionFd(user->getConnectionFd());
	}
	else 
	{	//����Ϊ���û�
		//��user���������û��б���
		m_userMap.insert(std::make_pair(user_id, user));
	}

	//����User��״̬Ϊ�Ѿ��������
	user->setStatus(UserStatus::IN_CONF);

#if DEBUG_ 
	std::cout << "[Join conference]   creator info:" << std::endl;
	user->printInfo();
#endif

	return error_reason;
}

/* �˳����飬��Ҫ���û�ɾ�������飬ֻ�ı�״̬: ���ߵ����ڻ�����
*  ԭ����ΪdeleteUser�� �ָ�ΪsetUserOut
* 
* ����   user���û�����ָ��
* ����ֵ list<PUser>�������������û��б�
*/
std::list<PUser> Conference::setUserOut(const PUser user)
{
	//�ж��û��ĺϷ��ԣ�����User�Ƿ�Ϊ�յ����
	const uint64 user_id = userInConference(user);
	if (0 == user_id)
	{  //�û��Ƿ������ؿ��û��б�
		return std::list<PUser>{};
	}

	std::lock_guard<std::recursive_mutex> lk(m_userMutex);
	//m_userMap[user_id]->cleanConferenceInfo();//����˻������Ϣ
	//�˳����飬ֻ������״̬Ϊ�뿪����״̬
	m_userMap[user_id]->setStatus(UserStatus::ONLINE_NO_CONF);

	return getUserConnectIdList();
}

/*�洢���ݣ����ͻ��˷������ݣ��ж��Ƿ���Ҫ�洢���ݡ�
*	������1.user :�û����� 2. data:�������� 3.isSave:�Ƿ񱣴�
*	����ֵ��Ԫ��
*		��һ���֣����ݵ�MsgId
*		�ڶ����֣���ǰ��������������û��б�
*/
MsgId_UserList Conference::addData(const PUser user, const std::string& data, const bool isSave)
{
	//����Ҫ�洢�����ݣ���msgIdΪ0
	int msgId = 0;
	//�ж��û��ĺϷ��ԣ�����User�Ƿ�Ϊ�յ����
	if (userInConference(user) == 0)
	{  //�û��Ƿ�
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
*   ��ѯ���ݻ�ȡ����
*	������ 
		1. user �û����� 
*		2. num �û����µ����ݵ����
*	����ֵ����ʷ��������
		��Ҫ�жϷ���ֵ�Ƿ�Ϊ�գ����û�Ȩ�޳��������ʷ����Ϊ��ʱ������ֵΪ�ա�
*/
const std::list<std::string> Conference::getData(const PUser user, const int num)
{
	//�ж��û��ĺϷ��ԣ�����User�Ƿ�Ϊ�յ����
	if (userInConference(user) == 0)
	{   //�����Ƿ�
		return std::list<std::string>{};
	}

	std::lock_guard<std::mutex> lk_t(m_dataMutex);

	//ͨ�������ķ�ʽ�ҵ�Num��Ӧ�����ݵĵ�����
	auto pData = m_dataList.cbegin();
	for (int i = 1; pData != m_dataList.cend() and i <= num; ++pData, ++i);

	//��ʷ����Ϊ��ʱ�����ؿ�List, Ҳ���赥���ж�
	return std::list<std::string>(pData, m_dataList.cend());
}

//��ѯ��������
std::string Conference::getPasswd() const
{
	return m_passwd;
}

//��ȡ����ID
std::string Conference::getCid() const
{
	return m_cid;
}

/* �ж��û��ڴ˻�����
 * ������user �û�ָ��
 * ����ֵ��Ϊ����˵���Ҳ����û������򷵻�user id
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
	{//���û���user id���û�����ƥ��ʱ���϶���μӹ�����
		return user->getUserId();
	}

	return 0;
}

//���ػ����������ڻ������û��Ķ����б�
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
/* ͨ������ID�����������û�ID
*  ������
*	cid: ����ID
*  ����ֵ��
*	�û�ID
* 
*  ʾ����cid: 123456789
*        ������Ϊ7
*  ���ɵ�UID��1234567897
*/
std::string Conference::createUserId(const std::string& cid)
{
	std::string num{ cid };

	std::lock_guard<std::mutex> lk(m_SeqMutex);
	//������Ϊ�����е��û������ܳ���99��
	if (++m_userSerial < 10)
	{
		num += "0";
	}
	num += std::to_string(m_userSerial);

	return std::move(num);
}
#endif