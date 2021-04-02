#include "BaseCommand.h"
#include "DataHandle.h"

///*定义了一个user管理队列(Map), 用来存放所有user对象
/// key: user id， value: User对象指针*/
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
        delay_timer.timer_id_ = 1001;  //这个ID必须是全局唯一的

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
    //测试服务器间链接，链接本地10003端口
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
    //测试服务器间链接，链接本地10003端口
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

        //测试关闭链接
        //session_service_->close_io_session(source.connect_id_);
    }
    else if (source.type_ == EM_CONNECT_IO_TYPE::CONNECT_IO_SERVER_UDP)
    {
        PSS_LOGGER_DEBUG("[logic_connect]connand={}, CONNECT_IO_SERVER_UDP", source.connect_id_);
        PSS_LOGGER_DEBUG("[logic_connect]connand={}, server_id={}", source.connect_id_, source.connect_mark_id_);

        /*测试发送UDP消息*/
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
    //处理发送数据(同步)
    send_packet->buffer_.append(recv_packet->buffer_.c_str(), recv_packet->buffer_.size());
}

void CBaseCommand::logic_test_asyn(const CMessage_Source& source, std::shared_ptr<CMessage_Packet> recv_packet, std::shared_ptr<CMessage_Packet> send_packet)
{
    //处理发送数据(异步)
    auto send_asyn_packet = std::make_shared<CMessage_Packet>();
    send_asyn_packet->buffer_.append(recv_packet->buffer_.c_str(), recv_packet->buffer_.size());

    session_service_->send_io_message(source.connect_id_, send_asyn_packet);
}

void CBaseCommand::logic_test_frame(const CMessage_Source& source, std::shared_ptr<CMessage_Packet> recv_packet, std::shared_ptr<CMessage_Packet> send_packet)
{
    //处理插件处理任务
    PSS_LOGGER_DEBUG("[logic_test_frame] tag_name={0},data={1}.", source.remote_ip_.m_strClientIP, recv_packet->buffer_);

    if (TEST_FRAME_WORK_FLAG == 1)
    {
        auto send_message = std::make_shared<CMessage_Packet>();
        CFrame_Message_Delay delay_timer;

        delay_timer.delay_seconds_ = std::chrono::seconds(5);
        delay_timer.timer_id_ = 1001;  //这个ID必须是全局唯一的

        send_message->command_id_ = COMMAND_TEST_FRAME;
        send_message->buffer_ = "freeeyes";
        session_service_->send_frame_message(plugin_test_logic_thread_id, "time loop", send_message, delay_timer);

        //测试定时器(删除)
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
    //返回http消息
    send_packet->buffer_ = recv_packet->buffer_;
}

void CBaseCommand::logic_http_websocket_shark_hand(const CMessage_Source& source, std::shared_ptr<CMessage_Packet> recv_packet, std::shared_ptr<CMessage_Packet> send_packet)
{
    //PSS_LOGGER_DEBUG("[logic_http_websocket_shark_hand]server key={0}", recv_packet->buffer_);
    PSS_LOGGER_DEBUG("---------[connect]server connect_id_={0}", source.connect_id_);

    //返回http消息
    send_packet->buffer_ = recv_packet->buffer_;
}

void CBaseCommand::logic_http_websocket_data(const CMessage_Source& source, 
    std::shared_ptr<CMessage_Packet> recv_packet, std::shared_ptr<CMessage_Packet> send_packet)
{
    //PSS_LOGGER_DEBUG("[logic_http_websocket_data]text data={0}", recv_packet->buffer_);
    //返回websocket消息(同步测试)
    //send_packet.buffer_ = recv_packet.buffer_;

    handleMessage(source.connect_id_, recv_packet->buffer_, send_packet);

    ////返回websocket消息(异步测试)
    //auto send_asyn_packet = std::make_shared<CMessage_Packet>();
    //send_asyn_packet->buffer_.append(recv_packet->buffer_.c_str(), recv_packet->buffer_.size());
    //session_service_->send_io_message(source.connect_id_, send_asyn_packet);
}

/*
* 处理客户端发送过来的请求，对数据进行解析并处理
* 参数：
*   1. pUser：当前用户
*   2. data : 接收到的当前用户发送的数据
*   3. sendPtr: 发送数据的句柄
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


    //解析CMD命令
    const std::string cmd_type = ::DataHandle::getStringValue(data, CMD);
    if (LOGON_STRING == cmd_type)
    {   //登录时，可能没有Uer对象，只需要传connect fd即可
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
        //仅通过cmd就可以退出会议，无需参数解析
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
* 功能：处理登录的接口
* 说明：此接口为后加接口。为了唯一确定客户端，需要给客户唯一标识userId。
*   User对象与user id对应，而不再与连接(fd)对应。
*   当客户端发送Logon请求，可以传递user id。如果没有，服务器生成一个并返回给客户端
*   当客户端传递了user id，搜索此id, 如果有则返回成功，没有则生成新的并返回
*/
void CBaseCommand::handleLogon(uint32 fd, const std::string& data, std::shared_ptr<CMessage_Packet> sendPtr)
{
    shared_ptr<User> p_user = nullptr;

    //需要判断用户是否已经存在对应的User对象
    //从发送过来的参数中获取User id
    uint64 user_id = DataHandle::getUint64Value(data, USER_ID);
    //只有当user id不为0且能在全局用户Map中找到，说明其已经存在
    if ((0 != user_id) and nullptr != (p_user = getUserByUserId(user_id)))
    {
        return;
    }
    
    //否则就是新用户，需要重新创建UserId，创建User对象
    p_user = make_shared<User>(fd);

    //为其创建一个唯一标识user id
    user_id = (uint64)(&p_user);
    p_user->setUserId(user_id);
    std::cout << "New user id:" << user_id << std::endl;

    registUser(user_id, p_user);

    sendPtr->buffer_ = DataHandle::createLogonData(user_id);
}

//处理加入会议的接口
void CBaseCommand::handleJoinMeet(PUser pUser, const std::string& data, std::shared_ptr<CMessage_Packet> sendPtr)
{
    if (nullptr == pUser)
    {
        return;
    }

    //从发送的request消息中获取参数信息
    const JoinMeetJsonData params = ::DataHandle::getJoinMeetData(data);

    //如果用户已经在会议中，阻止其重复加入会议，不管是不是之前的会议
    if (pUser->getStatus() == UserStatus::IN_CONF)
    {
        sendPtr->buffer_ = DataHandle::createJoinMeetFalseReturnData(REPEATE_JOIN);
        return;
    }

    //当用户不在会议中，就可以初始化本次会议的信息，即使失败也可以再清空
    pUser->initConferenceInfo(params);

    std::string notifyData{};//通知其他人的数据
    std::string returnData{};//返回自己的数据，需要所有人的用户名和User id

    //获取用户对象
    auto pConf = pConferenceManage->getConference(params.meetId);
    if (nullptr == pConf)
    {
        //说明此次请求是创建会议
        pConf = pConferenceManage->createConference(pUser, params.pwd);

        //发送消息
        sendPtr->buffer_ = DataHandle::createJoinMeetReturnData(pUser->getCid(),
            pConf->getUserConnectIdList());
        return;
    }
    
    //至此，是加入会议的情况，调用加入会议接口
    auto err_string = pConf->addUser(pUser, params.pwd);
    //针对成功和失败，创建不同的数据
    if (!err_string.empty())
    {
        std::cerr << __func__ <<"::Join conference faile! "  << std::endl;
        pUser->setCid("");//失败则清除cid
        sendPtr->buffer_ = DataHandle::createJoinMeetFalseReturnData(err_string);
        return;
    }

    //加入会议成功，发送成功加入会议的数据
    auto t_userList = pConf->getUserConnectIdList();
    returnData = DataHandle::createJoinMeetReturnData(params.meetId, t_userList);
    notifyData = ::DataHandle::createDataToOthers(CmdType::JOIN_MEET, pUser->getUserId(), params.userName);
  
    //加入会议成功，需要判断此会议是否在pConferenceManage的空会议列表里面，如果在则删除
    pConferenceManage->delConfInEmptyConfList(params.meetId);

    sendMsgToUser(pUser, sendPtr, t_userList, returnData, notifyData);
}

//处理退出会议的接口
void CBaseCommand::handleExitMeet(PUser pUser, std::shared_ptr<CMessage_Packet> sendPtr)
{
    //获取用户所对应的会议
    const std::string t_cid = pUser->getCid();
    auto pConf = pConferenceManage->getConference(t_cid);
    if (nullptr != pConf)
    {
        //如果用户不在会议中，阻止其重复退出会议
        if (pUser->getStatus() != UserStatus::IN_CONF)
        {
            //sendPtr->buffer_ = DataHandle::createJoinMeetFalseReturnData(REPEATE_JOIN);
            std::cerr << __func__ << "::No conference to leave! " << std::endl;
            return;
        }
        //退出会议，有关会议的数据会被清空，这里需要临时保存以便推送消息。
        //const std::string user_id = pUser->getUserId();

        auto t_userList = pConf->setUserOut(pUser);
        sendMsgToUser(pUser, sendPtr, t_userList, DataHandle::createLeaveReturnData(),
            DataHandle::createDataToOthers(CmdType::LEAVE_MEET,
                pUser->getUserId(), pUser->getUserName()));

        //如果退出会议之后，在线用户人数为0则此会议为空会议
        if (0 == t_userList.size())
        {
            pConferenceManage->addEmptyConference(t_cid);
        }
    }
    //暂不考虑失败的情况
}

//处理发送数据的接口
void CBaseCommand::handleSendData(PUser pUser, const std::string& data, std::shared_ptr<CMessage_Packet> sendPtr)
{
    //获取请求的信息
    auto [is_save, msg] = ::DataHandle::getSendMsgData(data);
    auto pConf = pConferenceManage->getConference(pUser->getCid());
    if (nullptr != pConf)
    {
        //msgId为数据序列，不保存的数据此值为0， t_userList为会议中在线用户列表
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
    for (auto pData : dataList)//获取数据后，将数据全部粘在一起
    {
        //TODO 这里可能涉及粘包发送、流量控制的逻辑，暂略
        //sendPtr->buffer_.append(pData, pData.size());
        //sendPtr->buffer_ = pData;
        send_dat += pData;
    }
    sendPtr->buffer_ = send_dat;
}

/*
*  功能：发送数据的接口
*  以同步的方式返回数据给发送者，以异步的方式推送信息给同一会议中其他用户信息
*  参数：
*   1. pSender    : 当前发送消息的用户
*   2. sendPtr    ：发送数据句柄
*   3. userList   ：会议中所有在线用户的列表
*   4. returnData ：返回给发送者用户的数据
*   5. notifyData ：返回给其他用户的数据
*  返回值：无
*/
void CBaseCommand::sendMsgToUser(PUser pSender, std::shared_ptr<CMessage_Packet> sendPtr,
    list<PUser> userList, const string& returnData, const string& notifyData)
{
    std::cout << "sendto self:" << returnData << std::endl;
    //发送同步信息给自己
    sendPtr->buffer_ = returnData;

    //发送异步消息给其他用户数据
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

//通过user id来获取User对象，返回值为User对象
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

//通过connect fd来获取User对象，返回值为User对象
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