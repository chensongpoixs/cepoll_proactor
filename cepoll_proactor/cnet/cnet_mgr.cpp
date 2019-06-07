/***********************************************************************************************
	created: 		2019-03-06
	
	author:			chensong
					
	purpose:		net_mgr
************************************************************************************************/
#include "cnet_mgr.h"
#include "clog.h"
#include "cnet_msg_reserve.h"
#include "cnet_msg_header.h"
namespace chen {
	cnet_mgr::cnet_mgr()
		:m_connect_callback(NULL)
		, m_disconnect_callback(NULL)
		, m_msg_callback(NULL)
		, m_shuting(false)
		,m_msg_id_key(0)
		, m_msg_size_key(0)
		, m_max_received_msg_size(0)
		, m_wan(false)
		, m_reconnect_second(0)
		, m_active_num(0)
	{
#if defined(_MSC_VER)
		//begin 初始化网络环境
		WSADATA wsaData;
		if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		{            //加载winsock2库
			printf("加载失败./n");
			return;
		}

#endif // !#if defined(_MSC_VER)
	}
	cnet_mgr::~cnet_mgr()
	{
#if defined(_MSC_VER)
		::WSACleanup();
#endif // !#if defined(_MSC_VER)

	}
	cnet_mgr * cnet_mgr::construct()
	{
		return new cnet_mgr();
	}
	void cnet_mgr::destroy(cnet_mgr * p)
	{
		//ERROR_EX_LOG("destroy net");
		delete p;
	}
	bool 		cnet_mgr::init(const std::string& name, uint32 client_session, uint32 max_session
		, uint32 send_buff_size, uint32 recv_buff_size)
		{
			m_name = name;

			/*if (client_session > max_session)
			{
				ERROR_EX_LOG(" client_session= %lu > max_session = %lu", client_session, max_session);
				return false;
			}*/

			// client init
			if (client_session > max_session)
			{
				m_sessions.reserve(client_session);
				for (uint32 i = 0; i < client_session; ++i)
				{
					m_sessions.push_back(new cnet_session(*this, i));
				}
				for (cnet_session * session : m_sessions)
				{
					if (!session->init(send_buff_size, recv_buff_size))
					{
						return false;
					}

				//	m_available_sessions.push_back(session);  // client m_session_ptrs
					//std::move(m_available_sessions, m_sessions);
				}
				m_active_num = client_session;
			}
			// server init
			if (client_session < max_session)
			{
				m_sessions.reserve(max_session);
				for (uint32 i = 0; i < max_session; ++i)
				{
					m_sessions.push_back(new cnet_session(*this, i));
				}
				if (m_sessions.size() != max_session)
				{
					ERROR_EX_LOG("max_session new session fail  max_session = %lu", max_session);
					return false;
				}
				for (cnet_session * session : m_sessions)
				{
					if (!session->init(send_buff_size, recv_buff_size))
					{
						return false;
					}

					m_available_sessions.push_back(session);
					//std::move(m_available_sessions, m_sessions);
				}
				m_active_num = max_session;
			}
			
			return true;
		}
		void 		cnet_mgr::destroy()
		{
			m_shuting = true;
			SYSTEM_LOG(" net destroy");
			if (m_reactor)
			{
				m_reactor->destroy();
				delete m_reactor;
			}
			if (m_acceptor_ptr)
			{
				//m_acceptor_ptr->shutdown();
				m_acceptor_ptr->destroy();
				cacceptor::destroy(m_acceptor_ptr);
			}
			
			for (cnet_session *client : m_sessions)
			{
				client->destroy();
			}
			
			for (std::thread &thread : m_threads)
			{
				if (thread.joinable())
				{
					thread.join();
				}
			}
			m_threads.clear();
		}
	
		bool 		cnet_mgr::startup(uint32 thread_num, const char *ip, uint16 port)
		{
			
			//m_acceptor_ptr = new cacceptor();
			m_acceptor_ptr = cacceptor::construct();
			if (!m_acceptor_ptr)
			{
				ERROR_EX_LOG(" acceptor alloc fail ");
				return false;
			}
			/*if (!m_acceptor_ptr->init(ip, port))
			{
				ERROR_EX_LOG(" acceptor init fail ");
				return false;
			}*/
			/*if (!m_acceptor_ptr->create())
			{
				
				ERROR_EX_LOG(" create socket faill ");
				return false;
			}*/

			// active
			m_reactor = new creactor();
			if (!m_reactor)
			{
				ERROR_EX_LOG("create alloc fail ");
				return false;
			}
			if (!m_reactor->init(m_acceptor_ptr->get_sockfd(), m_active_num /*反应堆的数量*/))
			{
				
				return false;
			}
			if (port)
			{
				_listen_start(ip, port);
			}
			
			for (uint32 i = 0; i < thread_num; ++i)
			{
				// thread
				m_threads.emplace_back(&cnet_mgr::_work_thread, this);
			}
			return true;
		}
		
		
		bool 		cnet_mgr::connect_to(uint32 index, const std::string& ip_address, uint16 port)
		{

			if (!_connect_start(index, ip_address.c_str(), port))
			{
				return false;
			}
		
			return true;
		}
		bool 		cnet_mgr::send_msg(uint32 sessionId, uint16 msg_id, const void* msg_ptr, uint32 msg_size)
		{
			// session --> status
			cnet_session * session = m_sessions[sessionId];
			if (!session)
			{
				WARNING_EX_LOG(" find failed session id = %d", sessionId);
				return false;
			}
			if (!session->is_open())
			{
				return false;
			}
			/*
			**信息包  size;
			**/
			cnet_msg_header msg_header;
			msg_header.m_msg_id = msg_id;
			msg_header.m_size = msg_size;
			if (!session->send_data(&msg_header, MSG_HEADER_SIZE))
			{
				return false;
			}
			/************************************************************************/
			/* 包                                                                     */
			/************************************************************************/
			if (!session->send_data(msg_ptr, msg_size))
			{
				return false;
			}
			return true;
		}

		bool cnet_mgr::send_msg(uint32 sessionId, uint16 msg_id, const void* msg_ptr, uint32 msg_size, int32 extra)
		{
			// session --> status
			cnet_session * session = m_sessions[sessionId];
			if (!session)
			{
				WARNING_EX_LOG(" find failed session id = %d", sessionId);
				return false;
			}
			if (!session->is_open())
			{
				return false;
			}
			/*
			**信息包  size;
			**/
			cnet_msg_header msg_header;
			msg_header.m_msg_id = msg_id;
			msg_header.m_size = msg_size;
			msg_header.m_extra = extra;
			if (!session->send_data(&msg_header, MSG_HEADER_SIZE))
			{
				return false;
			}
			/************************************************************************/
			/* 包                                                                     */
			/************************************************************************/
			if (!session->send_data(msg_ptr, msg_size))
			{
				return false;
			}
			return true;
		}

		void 		cnet_mgr::shutdown()
		{
			m_shuting.store(true);
			//destroy();
			// accept 
			//if (m_acceptor_ptr)
			//{
			//	delete m_acceptor_ptr;
			//}
			//if (m_reactor)
			//{
			////	m_reactor->shutdown();
			//	delete m_reactor;
			//}
			
			/*for (std::thread &thread : m_threads)
			{
				if (thread.joinable())
				{
					thread.join();
				}
			}
			m_threads.clear();*/
		}
		
		//void cnet_mgr::set_times_second(uint32 ms)
		//{
		//	//m_reconnection
		//}

		/*void cnet_mgr::set_reconnection(uint32 msleep)
		{
			m_reconnection = msleep;
		}*/

		void 		cnet_mgr::process_msg()
		{
		
			if (m_msgs.size() < 1)
			{
				return;
			}
			//SYSTEM_LOG("msgs size = %lu", m_msgs.size());
			cnet_msg* msg_ptr = m_msgs.front();
			while (msg_ptr)
			{
				m_msgs.pop();
				if (msg_ptr->get_status() == EMIR_Connect)
				{
					//cmsg_connect* p = (cmsg_connect*)msg_ptr->get_buf();
					m_connect_callback(msg_ptr->get_session_id(), msg_ptr->get_size(), (const char *)msg_ptr->get_buf());
				}
				else if (msg_ptr->get_status() == EMIR_Disconnect)
				{
					m_disconnect_callback(msg_ptr->get_session_id());
				}
				else
				{
					m_msg_callback(msg_ptr->get_session_id(), msg_ptr->get_msg_id()
						, msg_ptr->get_buf(), msg_ptr->get_size());
				}
				if (msg_ptr->get_buf())
				{
					msg_ptr->free_me();
					//delete[] msg_ptr->m_ptr;
				}
				delete msg_ptr;
				msg_ptr = NULL;
				if (m_msgs.size() > 0)
				{
					msg_ptr = m_msgs.front();
				}
				else
				{
					break;
				}
				
			}
		}
		void 		cnet_mgr::close(uint32 sessionId)
		{
			
			cnet_session * session_ptr = m_sessions[sessionId];
			if (!session_ptr)
			{
				WARNING_LOG(" close session session %lu", sessionId);
				return;
			}

			_clearup_session(session_ptr);
		}

		void cnet_mgr::show_info()
		{
			clock_guard guard(m_avail_session_mutex);
			SYSTEM_LOG("%s info m_listening_num = %lu, available_sessions %lu", m_name.c_str(), m_sessions.size(), m_available_sessions.size());
		}

		void cnet_mgr::msg_push(cnet_msg * msg_ptr)
		{
			m_msgs.push(msg_ptr);
		}
		void 		cnet_mgr::_work_thread()
		{
			while (!m_shuting)
			{
				// write read queue
				int32 num_event = m_reactor->select(200);
				//SYSTEM_LOG("num_event = %ld", num_event);
				for (int32 i = 0; i < num_event; ++i)
				{
					cnet_session * session = static_cast<cnet_session*>(m_reactor->get_para(i));
					
					if (!session)
					{
						if (m_reactor->is_exception(i))
						{
							ERROR_EX_LOG("listen socket has exception");
							continue;
						}
						if (m_reactor->is_read(i))
						{
							// new client
							_new_connect();
						}
						continue;
					}

					if (!session->is_open())
					{
						continue;
					}

					if (m_reactor->is_read(i))
					{
						if (!session->processinputs())
						{
						//	SYSTEM_LOG("read clearup");
							_clearup_session(session);
							continue;
						}
					}

					if (m_reactor->is_exception(i))
					{
						//SYSTEM_LOG("is_exception clearup");
						ERROR_EX_LOG("pSession %u has exception, disconnected", session->get_sockfd());
						_clearup_session(session);
						continue;
					}

					if (m_reactor->is_write(i))
					{
						
						if (!session->processoutputs())
						{
						//	SYSTEM_LOG("is_write clearup");
							_clearup_session(session);
						}
						continue;
					}
				}
			}
		}
		void cnet_mgr::_clearup_session(cnet_session * psession)
		{
			m_reactor->deregister_descriptor(psession->get_sockfd());
			uint32 session = psession->get_session();
			psession->clearup();
			_return_session(psession);
			cnet_msg * msg_ptr = new cnet_msg();
			if (!msg_ptr)
			{
				ERROR_EX_LOG("new faild msg_ptr = NULL");
				return;
			}
			msg_ptr->set_session_id(session);
			msg_ptr->set_status(EMIR_Disconnect);
			msg_ptr->set_msg_id(0);
			//msg_ptr->m_client = session;
			//msg_ptr->m_status = EMIR_Disconnect;
			//msg_ptr->m_ptr = NULL;
			//msg_ptr->m_size = 0;
			m_msgs.push(msg_ptr);
			// disconnect
			//m_msgs.push();
		}
		void cnet_mgr::_new_connect()
		{
			sockaddr_in sock_addr;
			int err_code;
			//while (true)
			{
				socket_type	tmpsocket;

				memset(&sock_addr, 0, sizeof(sock_addr));
				sock_addr.sin_family = AF_INET;
				//-- accept socket
				if (!m_acceptor_ptr->accept(tmpsocket, sock_addr, err_code))
				{
					if (err_code == -1 /*WSAEWOULDBLOCK*/)
					{
						return;
					}
					else
					{
						return;
					}
				}

				if (!csocket_ops::set_nonblocking(tmpsocket))
				{
					return;
				}
				cnet_session *session_ptr = _get_available_session();
				if (!session_ptr)
				{
					ERROR_EX_LOG("session ptr == NULL");
					return;
				}
				session_ptr->set_connect(tmpsocket);
				m_reactor->register_readwrite_descriptor(tmpsocket, session_ptr);

			//	uint32 port = ntohs(sock_addr.sin_port);
				//uint32 ip = (uint32)(sock_addr.sin_addr.s_addr);
				// connect
				cnet_msg * msg_ptr = new cnet_msg;
				if (!msg_ptr)
				{
					ERROR_EX_LOG("new faild msg_ptr = NULL");
					return;
				}
				msg_ptr->set_session_id(session_ptr->get_session());
				msg_ptr->set_status(EMIR_Connect);
				//msg_ptr->m_client = session_ptr->get_session();/*tmpsocket*/;
				//msg_ptr->m_status = EMIR_Connect;
				//msg_ptr->m_ptr = NULL;
				//msg_ptr->m_size = 0;
				m_msgs.push(msg_ptr);
			}
		}
		void cnet_mgr::_listen_start(const char * ip, uint16 port)
		{
			//m_acceptor_ptr = new cacceptor();
			//if (!m_acceptor_ptr)
			//{
			//	ERROR_EX_LOG(" acceptor alloc fail ");
			//	return ;
			//}
			////m_acceptor_ptr->init(ip, port);
			if (!m_acceptor_ptr->init(ip, port))
			{
				ERROR_EX_LOG("server acceptor init fail ip =%s, port =%lu ", ip, port);
				return ;
			}
			if (!m_acceptor_ptr->create())
			{
			//	destroy();
				ERROR_EX_LOG(" create socket faill ");
				return;
			}

			// bind
			if (!m_acceptor_ptr->bind())
			{
				//destroy();
				return ;
			}

			// listen
			if (!m_acceptor_ptr->listen())
			{
				//destroy();
				return;
			}
			if (!m_reactor)
			{
				// active
				m_reactor = new creactor();
				if (!m_reactor)
				{
					ERROR_EX_LOG("create alloc fail ");
					//destroy();
					return;
				}
			}
			//if (!m_reactor->init(m_acceptor_ptr->get_sockfd(), 2000 /*反应堆的数量*/))
			//{
			//	destroy();
			//	return ;
			//}
			m_reactor->register_read_descriptor(m_acceptor_ptr->get_sockfd(), NULL);
		}
		bool cnet_mgr::_connect_start(uint32 index, const char * ip, uint16 port)
		{
			if (!m_acceptor_ptr->init(ip, port))
			{
				ERROR_EX_LOG("clinet acceptor init failed ip =%s, port =%d ", ip, port);
				return false;
			}
			/// create socket 
			if (!m_acceptor_ptr->create())
			{
				//	destroy();
				ERROR_EX_LOG(" create socket faill ");
				return false;
			}
			// open nagle 算法
			if (!m_acceptor_ptr->set_nonagle(false))
			{
				ERROR_EX_LOG("open tcp_nodelay nagle failed");
				return false;
			}
			// async_connect
			/*socket_type	sockfd ;*/
			if (!m_acceptor_ptr->async_connect(/*sockfd*/))
			{
				ERROR_EX_LOG(" async_connect failed ");
				return false;   //TODO@chensong 暂时还没有找到win上连接connect 判断是否连接上错误判断 
			}
			//SYSTEM_LOG("async_connect ok");
			// active
			//if (!m_reactor)
			//{
			//	// active
			//	m_reactor = new creactor();
			//	if (!m_reactor)
			//	{
			//		ERROR_EX_LOG("create alloc failed ");
			//		destroy();
			//		return;
			//	}
			//}
			//if (!m_reactor->init(m_acceptor_ptr->get_sockfd(), 2 /*反应堆的数量*/))
			//{
			//	destroy();
			//	return ;
			//}
			// csession ptr ->
			//cnet_session *session_ptr = _get_available_session();
			cnet_session *session_ptr = m_sessions[index];
			if (!session_ptr)
			{
				ERROR_EX_LOG("session ptr == NULL");
				return false;
			}
			session_ptr->set_connect(m_acceptor_ptr->get_sockfd());
			m_reactor->register_readwrite_descriptor(m_acceptor_ptr->get_sockfd(), session_ptr);

		//	cmsg_data * msg_ptr = new cmsg_data;
			cnet_msg *msg_ptr = new cnet_msg();
			if (!msg_ptr)
			{
				ERROR_EX_LOG("new faild msg_ptr = NULL");
				return false;
			}
			msg_ptr->set_session_id(session_ptr->get_session());
			msg_ptr->set_status(EMIR_Connect);
			//msg_ptr->m_client = session_ptr->get_session();/*tmpsocket*/;
			//msg_ptr->m_status = EMIR_Connect;
			//msg_ptr->m_ptr = NULL;
			//msg_ptr->m_size = 0;
			m_msgs.push(msg_ptr);

			return true;
		}
		cnet_session * cnet_mgr::_get_available_session()
		{
			cnet_session* temp_ptr = NULL;
			{
				clock_guard guard(m_avail_session_mutex);
				if (m_available_sessions.empty())
				{
					return NULL;
				}
				temp_ptr = m_available_sessions.back();
				m_available_sessions.pop_back();
				//m_available_sessions.push_back(temp_ptr);
			}
			return temp_ptr;
		}
		void cnet_mgr::_return_session(cnet_session * session_ptr)
		{
			clock_guard guard(m_avail_session_mutex);
			//m_available_sessions.remove(m_avail_session_mutex);
			m_available_sessions.push_back(session_ptr);
		}
}// namespce chen


