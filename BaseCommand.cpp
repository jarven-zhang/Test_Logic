#include "BaseCommand.h"
#include "DataHandle.h"

///*������һ��user�������(Map), �����������user����
/// key: user id�� value: User����ָ��*/
//thread_local std::unordered_map<uint64, PUser> userMap;

void CBaseCommand::Init(ISessionService* session_service)
{
    session_service_ = session_service;

    pConferenceManage = std::make_shared<ConferenceManage>();

    if (TEST_FRAME_WORK_FLAG == 1)
    {
        session_service_->create_frame_work_thread(plugin_test_logic_thread_id);

        auto send_message = std::make_shared<CMessage_Packet>();
        CFrame_Message_Delay delay_timer;

        delay_timer.delay_seconds_ = std::chrono::seconds(5);
        delay_timer.timer_id_ = 1001;  //���ID������ȫ��Ψһ��

        send_message->command_id_ = COMMAND_TEST_FRAME;
        send_message->buffer_ = "freeeyes";
        session_service_->send_frame_message(plugin_test_logic_thread_id, "time loop", send_message, delay_timer);

        session_service_->run_work_thread_logic(plugin_test_logic_thread_id, delay_timer, [this](){
            PSS_LOGGER_DEBUG("[run_work_thread_logic]arrived.");
            });
    }

    PSS_LOGGER_DEBUG("[load_module]({0})io thread count.", session_service_->get_io_work_thread_count());
}

void CBaseCommand::logic_connect_tcp()
{
    //���Է����������ӣ����ӱ���10003�˿�
    CConnect_IO_Info io_info;
    EM_CONNECT_IO_TYPE io_type = EM_CONNECT_IO_TYPE::CONNECT_IO_TCP;

    io_info.send_size = 1024;
    io_info.recv_size = 1024;
    io_info.server_ip = "127.0.0.1";
    io_info.server_port = 10005;
    io_info.server_id = 1001;
    io_info.packet_parse_id = 1;

    session_service_->connect_io_server(io_info, io_type);
}

void CBaseCommand::logic_connect_udp()
{
    //���Է����������ӣ����ӱ���10003�˿�
    CConnect_IO_Info io_info;
    EM_CONNECT_IO_TYPE io_type = EM_CONNECT_IO_TYPE::CONNECT_IO_UDP;

    io_info.send_size = 1024;
    io_info.recv_size = 1024;
    io_info.server_ip = "127.0.0.1";
    io_info.server_port = 10005;
    io_info.server_id = 1002;
    io_info.packet_parse_id = 1;

    session_service_->connect_io_server(io_info, io_type);
}

void CBaseCommand::logic_connect(const CMessage_Source& source, std::shared_ptr<CMessage_Packet> recv_packet, std::shared_ptr<CMessage_Packet> send_packet)
{
   /* PSS_LOGGER_DEBUG("[logic_connect]connand={}, connect", source.connect_id_);
    PSS_LOGGER_DEBUG("[logic_connect]connand={}, local ip={} local port={}", source.connect_id_, source.local_ip_.m_strClientIP, source.local_ip_.m_u2Port);
    PSS_LOGGER_DEBUG("[logic_connect]connand={}, remote ip={} remote port={}", source.connect_id_, source.remote_ip_.m_strClientIP, source.remote_ip_.m_u2Port);
    PSS_LOGGER_DEBUG("[logic_connect]connand={}, work thread id={}", source.connect_id_, source.work_thread_id_);*/

    if (source.type_ == EM_CONNECT_IO_TYPE::CONNECT_IO_TCP)
    {
        PSS_LOGGER_DEBUG("[logic_connect]connand={}, CONNECT_IO_TCP", source.connect_id_);
    }
    else if (source.type_ == EM_CONNECT_IO_TYPE::CONNECT_IO_UDP)
    {
        PSS_LOGGER_DEBUG("[logic_connect]connand={}, CONNECT_IO_UDP", source.connect_id_);
    }
    else if (source.type_ == EM_CONNECT_IO_TYPE::CONNECT_IO_TTY)
    {
        PSS_LOGGER_DEBUG("[logic_connect]connand={}, CONNECT_IO_TTY", source.connect_id_);
    }
    else if (source.type_ == EM_CONNECT_IO_TYPE::CONNECT_IO_SERVER_TCP)
    {
        PSS_LOGGER_DEBUG("[logic_connect]connand={}, CONNECT_IO_SERVER_TCP", source.connect_id_);
        PSS_LOGGER_DEBUG("[logic_connect]connand={}, server_id={}", source.connect_id_, source.connect_mark_id_);

        //���Թر�����
        //session_service_->close_io_session(source.connect_id_);
    }
    else if (source.type_ == EM_CONNECT_IO_TYPE::CONNECT_IO_SERVER_UDP)
    {
        PSS_LOGGER_DEBUG("[logic_connect]connand={}, CONNECT_IO_SERVER_UDP", source.connect_id_);
        PSS_LOGGER_DEBUG("[logic_connect]connand={}, server_id={}", source.connect_id_, source.connect_mark_id_);

        /*���Է���UDP��Ϣ*/
        if (TEST_FRAME_WORK_FLAG == 1)
        {
            auto send_asyn_packet = std::make_shared<CMessage_Packet>();
            send_asyn_packet->command_id_ = 0x1002;
            send_asyn_packet->buffer_ = "111111";

            session_service_->send_io_message(source.connect_id_, send_asyn_packet);
        }
    }
}

void CBaseCommand::logic_disconnect(const CMessage_Source& source, std::shared_ptr<CMessage_Packet> recv_packet, std::shared_ptr<CMessage_Packet> send_packet)
{
    PSS_LOGGER_DEBUG("[do_module_message]connand={}, disconnect", source.connect_id_);
    auto p_user = getUserByFd(source.connect_id_);
    if (nullptr == p_user)
    {
        PSS_LOGGER_ERROR("Can't find the connection in user map!\n");
        return;
    }
    handleExitMeet(p_user, send_packet);
}

void CBaseCommand::logic_test_sync(const CMessage_Source& source, std::shared_ptr<CMessage_Packet> recv_packet, std::shared_ptr<CMessage_Packet> send_packet)
{
    //����������(ͬ��)
    send_packet->buffer_.append(recv_packet->buffer_.c_str(), recv_packet->buffer_.size());
}

void CBaseCommand::logic_test_asyn(const CMessage_Source& source, std::shared_ptr<CMessage_Packet> recv_packet, std::shared_ptr<CMessage_Packet> send_packet)
{
    //����������(�첽)
    auto send_asyn_packet = std::make_shared<CMessage_Packet>();
    send_asyn_packet->buffer_.append(recv_packet->buffer_.c_str(), recv_packet->buffer_.size());

    session_service_->send_io_message(source.connect_id_, send_asyn_packet);
}

void CBaseCommand::logic_test_frame(const CMessage_Source& source, std::shared_ptr<CMessage_Packet> recv_packet, std::shared_ptr<CMessage_Packet> send_packet)
{
    //��������������
    PSS_LOGGER_DEBUG("[logic_test_frame] tag_name={0},data={1}.", source.remote_ip_.m_strClientIP, recv_packet->buffer_);

    if (TEST_FRAME_WORK_FLAG == 1)
    {
        auto send_message = std::make_shared<CMessage_Packet>();
        CFrame_Message_Delay delay_timer;

        delay_timer.delay_seconds_ = std::chrono::seconds(5);
        delay_timer.timer_id_ = 1001;  //���ID������ȫ��Ψһ��

        send_message->command_id_ = COMMAND_TEST_FRAME;
        send_message->buffer_ = "freeeyes";
        session_service_->send_frame_message(plugin_test_logic_thread_id, "time loop", send_message, delay_timer);

        //���Զ�ʱ��(ɾ��)
        if (TEST_FRAME_WORK_FLAG == 1)
        {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            session_service_->delete_frame_message_timer(1001);
        }
    }
}

void CBaseCommand::logic_test_connect_error(const CMessage_Source& source, std::shared_ptr<CMessage_Packet> recv_packet, std::shared_ptr<CMessage_Packet> send_packet)
{
    PSS_LOGGER_DEBUG("[CBaseCommand::logic_test_connect_error]{0}:{1}", 
        source.remote_ip_.m_strClientIP,
        source.remote_ip_.m_u2Port);
}

void CBaseCommand::logic_test_listen_error(const CMessage_Source& source, std::shared_ptr<CMessage_Packet> recv_packet, std::shared_ptr<CMessage_Packet> send_packet)
{
    PSS_LOGGER_DEBUG("[CBaseCommand::logic_test_listen_error]{0}:{1}",
        source.local_ip_.m_strClientIP,
        source.local_ip_.m_u2Port);
}

void CBaseCommand::logic_http_post(const CMessage_Source& source, std::shared_ptr<CMessage_Packet> recv_packet, std::shared_ptr<CMessage_Packet> send_packet)
{
    PSS_LOGGER_DEBUG("[logic_http_post]post data={0}", recv_packet->buffer_);
    //����http��Ϣ
    send_packet->buffer_ = recv_packet->buffer_;
}

void CBaseCommand::logic_http_websocket_shark_hand(const CMessage_Source& source, std::shared_ptr<CMessage_Packet> recv_packet, std::shared_ptr<CMessage_Packet> send_packet)
{
    //PSS_LOGGER_DEBUG("[logic_http_websocket_shark_hand]server key={0}", recv_packet->buffer_);
    PSS_LOGGER_DEBUG("---------[connect]server connect_id_={0}", source.connect_id_);

    //����http��Ϣ
    send_packet->buffer_ = recv_packet->buffer_;
}

void CBaseCommand::logic_http_websocket_data(const CMessage_Source& source, 
    std::shared_ptr<CMessage_Packet> recv_packet, std::shared_ptr<CMessage_Packet> send_packet)
{
    //PSS_LOGGER_DEBUG("[logic_http_websocket_data]text data={0}", recv_packet->buffer_);
    //����websocket��Ϣ(ͬ������)
    //send_packet.buffer_ = recv_packet.buffer_;

    handleMessage(source.connect_id_, recv_packet->buffer_, send_packet);

    ////����websocket��Ϣ(�첽����)
    //auto send_asyn_packet = std::make_shared<CMessage_Packet>();
    //send_asyn_packet->buffer_.append(recv_packet->buffer_.c_str(), recv_packet->buffer_.size());
    //session_service_->send_io_message(source.connect_id_, send_asyn_packet);
}

/*
* ����ͻ��˷��͹��������󣬶����ݽ��н���������
* ������
*   1. pUser����ǰ�û�
*   2. data : ���յ��ĵ�ǰ�û����͵�����
*   3. sendPtr: �������ݵľ��
*/
int CBaseCommand::handleMessage(const uint32 fd, const std::string& data, std::shared_ptr<CMessage_Packet> sendPtr)
{
    if (data.empty())
    {
        //std::cout << "The message is empty!" << std::endl;
        return 0;
    }
    std::cout << __func__ << ", recv data   :" << data << std::endl;
    std::cout << "-----------------------------------------" << std::endl;


    //����CMD����
    const std::string cmd_type = ::DataHandle::getStringValue(data, CMD);
    if (LOGON_STRING == cmd_type)
    {   //��¼ʱ������û��Uer����ֻ��Ҫ��connect fd����
        handleLogon(fd, data, sendPtr);
        return 0;
    }

    auto pUser = getUserByFd(fd);
    if (nullptr == pUser)
    {
        return -1;
    }

    if (JOIN_MEET_STRING == cmd_type)
    {
        handleJoinMeet(pUser, data, sendPtr);
    }
    else if (GET_MSG_STRING == cmd_type)
    {
        handleGetData(pUser, data, sendPtr);
    }
    else if (SEND_MSG_STRING == cmd_type)
    {
        handleSendData(pUser, data, sendPtr);
    }
    else if (LEAVE_MEET_STRING == cmd_type)
    {
        //��ͨ��cmd�Ϳ����˳����飬�����������
        handleExitMeet(pUser, sendPtr);
    }
    else
    {
        std::cout << "The cmd type is error! : " << cmd_type << std::endl;
        return -1;
    }

    return 0;
}

/*
* ���ܣ������¼�Ľӿ�
* ˵�����˽ӿ�Ϊ��ӽӿڡ�Ϊ��Ψһȷ���ͻ��ˣ���Ҫ���ͻ�Ψһ��ʶuserId��
*   User������user id��Ӧ��������������(fd)��Ӧ��
*   ���ͻ��˷���Logon���󣬿��Դ���user id�����û�У�����������һ�������ظ��ͻ���
*   ���ͻ��˴�����user id��������id, ������򷵻سɹ���û���������µĲ�����
*/
void CBaseCommand::handleLogon(uint32 fd, const std::string& data, std::shared_ptr<CMessage_Packet> sendPtr)
{
    shared_ptr<User> p_user = nullptr;

    //��Ҫ�ж��û��Ƿ��Ѿ����ڶ�Ӧ��User����
    //�ӷ��͹����Ĳ����л�ȡUser id
    uint64 user_id = DataHandle::getUint64Value(data, USER_ID);
    //ֻ�е�user id��Ϊ0������ȫ���û�Map���ҵ���˵�����Ѿ�����
    if ((0 != user_id) and nullptr != (p_user = getUserByUserId(user_id)))
    {
        return;
    }
    
    //����������û�����Ҫ���´���UserId������User����
    p_user = make_shared<User>(fd);

    //Ϊ�䴴��һ��Ψһ��ʶuser id
    user_id = (uint64)(&p_user);
    p_user->setUserId(user_id);
    std::cout << "New user id:" << user_id << std::endl;

    registUser(user_id, p_user);

    sendPtr->buffer_ = DataHandle::createLogonData(user_id);
}

//����������Ľӿ�
void CBaseCommand::handleJoinMeet(PUser pUser, const std::string& data, std::shared_ptr<CMessage_Packet> sendPtr)
{
    if (nullptr == pUser)
    {
        return;
    }

    //�ӷ��͵�request��Ϣ�л�ȡ������Ϣ
    const JoinMeetJsonData params = ::DataHandle::getJoinMeetData(data);

    //����û��Ѿ��ڻ����У���ֹ���ظ�������飬�����ǲ���֮ǰ�Ļ���
    if (pUser->getStatus() == UserStatus::IN_CONF)
    {
        sendPtr->buffer_ = DataHandle::createJoinMeetFalseReturnData(REPEATE_JOIN);
        return;
    }

    //���û����ڻ����У��Ϳ��Գ�ʼ�����λ������Ϣ����ʹʧ��Ҳ���������
    pUser->initConferenceInfo(params);

    std::string notifyData{};//֪ͨ�����˵�����
    std::string returnData{};//�����Լ������ݣ���Ҫ�����˵��û�����User id

    //��ȡ�û�����
    auto pConf = pConferenceManage->getConference(params.meetId);
    if (nullptr == pConf)
    {
        //˵���˴������Ǵ�������
        pConf = pConferenceManage->createConference(pUser, params.pwd);

        //������Ϣ
        sendPtr->buffer_ = DataHandle::createJoinMeetReturnData(pUser->getCid(),
            pConf->getUserConnectIdList());
        return;
    }
    
    //���ˣ��Ǽ���������������ü������ӿ�
    auto err_string = pConf->addUser(pUser, params.pwd);
    //��Գɹ���ʧ�ܣ�������ͬ������
    if (!err_string.empty())
    {
        std::cerr << __func__ <<"::Join conference faile! "  << std::endl;
        pUser->setCid("");//ʧ�������cid
        sendPtr->buffer_ = DataHandle::createJoinMeetFalseReturnData(err_string);
        return;
    }

    //�������ɹ������ͳɹ�������������
    auto t_userList = pConf->getUserConnectIdList();
    returnData = DataHandle::createJoinMeetReturnData(params.meetId, t_userList);
    notifyData = ::DataHandle::createDataToOthers(CmdType::JOIN_MEET, pUser->getUserId(), params.userName);
  
    //�������ɹ�����Ҫ�жϴ˻����Ƿ���pConferenceManage�Ŀջ����б����棬�������ɾ��
    pConferenceManage->delConfInEmptyConfList(params.meetId);

    sendMsgToUser(pUser, sendPtr, t_userList, returnData, notifyData);
}

//�����˳�����Ľӿ�
void CBaseCommand::handleExitMeet(PUser pUser, std::shared_ptr<CMessage_Packet> sendPtr)
{
    //��ȡ�û�����Ӧ�Ļ���
    const std::string t_cid = pUser->getCid();
    auto pConf = pConferenceManage->getConference(t_cid);
    if (nullptr != pConf)
    {
        //����û����ڻ����У���ֹ���ظ��˳�����
        if (pUser->getStatus() != UserStatus::IN_CONF)
        {
            //sendPtr->buffer_ = DataHandle::createJoinMeetFalseReturnData(REPEATE_JOIN);
            std::cerr << __func__ << "::No conference to leave! " << std::endl;
            return;
        }
        //�˳����飬�йػ�������ݻᱻ��գ�������Ҫ��ʱ�����Ա�������Ϣ��
        //const std::string user_id = pUser->getUserId();

        auto t_userList = pConf->setUserOut(pUser);
        sendMsgToUser(pUser, sendPtr, t_userList, DataHandle::createLeaveReturnData(),
            DataHandle::createDataToOthers(CmdType::LEAVE_MEET,
                pUser->getUserId(), pUser->getUserName()));

        //����˳�����֮�������û�����Ϊ0��˻���Ϊ�ջ���
        if (0 == t_userList.size())
        {
            pConferenceManage->addEmptyConference(t_cid);
        }
    }
    //�ݲ�����ʧ�ܵ����
}

//���������ݵĽӿ�
void CBaseCommand::handleSendData(PUser pUser, const std::string& data, std::shared_ptr<CMessage_Packet> sendPtr)
{
    //��ȡ�������Ϣ
    auto [is_save, msg] = ::DataHandle::getSendMsgData(data);
    auto pConf = pConferenceManage->getConference(pUser->getCid());
    if (nullptr != pConf)
    {
        //msgIdΪ�������У�����������ݴ�ֵΪ0�� t_userListΪ�����������û��б�
        auto [msgId, t_userList] = pConf->addData(pUser, msg, static_cast<bool>(is_save));

        if (msgId >= 0)
        {
            sendMsgToUser(pUser, sendPtr, t_userList, DataHandle::createSendData(msgId, 0, msg),
                DataHandle::createSendData(msgId, 1, msg));
        }
    }
}

void CBaseCommand::handleGetData(PUser pUser, const std::string& data, std::shared_ptr<CMessage_Packet> sendPtr)
{
    const int num = ::DataHandle::getQueryData(data);
    auto pConf = pConferenceManage->getConference(pUser->getCid());
    if (nullptr == pConf)
    {
        std::cerr << __func__ << "::Can't find the user in any conference! " << std::endl;
        return;
    }

    std::string send_dat{};

    auto dataList = pConf->getData(pUser, num);
    for (auto pData : dataList)//��ȡ���ݺ󣬽�����ȫ��ճ��һ��
    {
        //TODO ��������漰ճ�����͡��������Ƶ��߼�������
        //sendPtr->buffer_.append(pData, pData.size());
        //sendPtr->buffer_ = pData;
        send_dat += pData;
    }
    sendPtr->buffer_ = send_dat;
}

/*
*  ���ܣ��������ݵĽӿ�
*  ��ͬ���ķ�ʽ�������ݸ������ߣ����첽�ķ�ʽ������Ϣ��ͬһ�����������û���Ϣ
*  ������
*   1. pSender    : ��ǰ������Ϣ���û�
*   2. sendPtr    ���������ݾ��
*   3. userList   �����������������û����б�
*   4. returnData �����ظ��������û�������
*   5. notifyData �����ظ������û�������
*  ����ֵ����
*/
void CBaseCommand::sendMsgToUser(PUser pSender, std::shared_ptr<CMessage_Packet> sendPtr,
    list<PUser> userList, const string& returnData, const string& notifyData)
{
    std::cout << "sendto self:" << returnData << std::endl;
    //����ͬ����Ϣ���Լ�
    sendPtr->buffer_ = returnData;

    //�����첽��Ϣ�������û�����
    const uint32 fd = pSender->getConnectionFd();
    for (auto ptr : userList)
    {
        if (ptr->getConnectionFd() != fd)
        {
            auto send_asyn_packet = std::make_shared<CMessage_Packet>();
            send_asyn_packet->buffer_ = notifyData;

            std::cout << " Send -----> to fd:" << ptr->getConnectionFd()
                << ", data:" << send_asyn_packet->buffer_
                << std::endl;

            session_service_->send_io_message(ptr->getConnectionFd(), send_asyn_packet);
            //session_service_->send_io_message(8963, send_asyn_packet);
        }
    }
}

//ͨ��user id����ȡUser���󣬷���ֵΪUser����
PUser CBaseCommand::getUserByUserId(const uint64 userId)
{
    std::lock_guard<std::mutex> lk(m_userMutex);
    auto pUser = m_userMap.find(userId);
    if (pUser == m_userMap.end())
    {
        return nullptr;
    }
    return pUser->second;
}

//ͨ��connect fd����ȡUser���󣬷���ֵΪUser����
PUser CBaseCommand::getUserByFd(const uint32 fd)
{
    std::lock_guard<std::mutex> lk(m_userMutex);
    for (auto ptr : m_userMap)
    {
        if ( nullptr != ptr.second and
            ptr.second->getConnectionFd() == fd)
        {
            return ptr.second;
        }
    }
    return nullptr;
}

void CBaseCommand::registUser(const uint64 userId, PUser pUser)
{
    std::lock_guard<std::mutex> lk(m_userMutex);
    m_userMap.insert(std::make_pair(userId, pUser));
}