/***********************************************************************************************
	created: 		2019-03-06
	
	author:			chensong
					
	purpose:		net_mgr
************************************************************************************************/
#include "cnet_mgr.h"


namespace chen {
	
		bool 		cnet_mgr::init()
		{
			return true;
		}
		void 		cnet_mgr::destroy()
		{
			
		}
	
		bool 		cnet_mgr::startup(const char *ip, uint16 port)
		{
			if (!ip )
			{
				ERROR_EX_LOG("ip not \n");
				return false;
			}
			if (port <= 0)
			{
				ERROR_EX_LOG("port not \n");
				return false;
			}
			
			m_acceptor_ptr = new cacceptor;
			if (!m_acceptor_ptr)
			{
				ERROR_EX_LOG(" acceptor alloc fail \n");
				return false;
			}
			if (!m_acceptor_ptr->init(ip, port))
			{
				ERROR_EX_LOG(" acceptor init fail \n");
				return false;
			}
			if (!m_acceptor_ptr->create())
			{
				destroy();
				ERROR_EX_LOG(" create socket faill \n");
				return false;
			}
			
			// bind
			if (!m_acceptor_ptr->bind())
			{
				destroy();
				return false;
			}
			
			// listen
			if (!m_acceptor_ptr->listen())
			{
				destroy();
				return false;
			}
			
			
			// active
			m_reactor 	= new cepoll_reactor;
			if (!m_reactor)
			{
				ERROR_EX_LOG("create alloc fail \n");
				destroy();
				return false;
			}
			if (!m_reactor->init(m_acceptor_ptr->get_sockfd()))
			{
				destroy();
				return false;
			}
			// thread
			m_threads.emplace_back(&cnet_mgr::_work_thread, this);
			
			return true;
		}
		bool 		cnet_mgr::send_msg(uint32 sessionId, uint16 msg_id, const void* msg_ptr, uint32 msg_size)
		{
				return false;
		}
		void 		cnet_mgr::shutdown()
		{
			m_shuting.store(true);
			// accept 
			if (m_acceptor_ptr)
			{
				delete m_acceptor_ptr;
			}
			if (m_reactor)
			{
				m_reactor->shutdown();
				delete m_reactor;
			}
			
			for (std::thread &thread : m_pthread)
			{
				if (thread.joinable())
				{
					thread.join();
				}
			}
			
			
		}
		
		void 		cnet_mgr::process_msg()
		{
			cmsg_data* msg_ptr = m_msgs.front();
			while (msg_ptr)
			{
				m_msgs.pop();
				if (msg_ptr->m_msg_id == EMIR_Connect)
				{
					wmsg_connect* p = (wmsg_connect*)msg_ptr->get_buf();
					m_connect_callback(msg_ptr->get_session_id(), p->para, p->buf);
				}
				else if (msg_ptr->m_msg_id == EMIR_Disconnect)
				{
					m_disconnect_callback(msg_ptr->m_client);
				}
				else
				{
					m_msg_callback(msg_ptr->m_client, msg_ptr->m_msg_id
						, msg_ptr->ptr, msg_ptr->m_size);
				}
				
				delete msg_ptr;
				msg_ptr = m_msgs.front();
				
			}
		}
		void 		cnet_mgr::close(uint32 sessionId)
		{
			
		}
		void 		cnet_mgr::_work_thread()
		{
			while (!m_stoped)
			{
				// write read queue
				int64 num_event = m_reactor->select(m_acceptor_ptr->get_sockfd());
				for (int64 i = 0; i < num_event; ++i)
				{
					cmsg_data * msg = new cmsg_data;
					if (!msg)
					{
						ERROR_EX_LOG("alloc msg_data fail\n");
					}
					if (m_acceptor_ptr->get_sockfd() == get_event_descriptor(num_event)) // new client accept
					{
						csocket_type fd = m_acceptor_ptr->accept();
						cnet_session * session 	= new cnet_session;
						if (!session)
						{
							ERROR_EX_LOG("alloc accept new session \n");
							continue;
						}
						// nonblock
						if (!csocket_ops::set_nonblocking(fd))
						{
							ERROR_EX_LOG(" accept new session set nonblocking\n");
							delete session;
							continue;
						}
						session->set_sockfd(fd);
						if (!m_reactor->register_descriptor(fd, session))
						{
							ERROR_EX_LOG(" add descriptor fail \n");
							delete session;
							continue;
						}
						msg_ptr->m_client = session->sockfd();
						msg_ptr->m_ptr = buf;
						msg_ptr->msg_id = EMIR_Connect;
						m_msgs<cmsg_data>.push(msg);
						
					}
					else 
					{
						cnet_session *session = static_cast<cnet_session*>m_reactor->get_ptr(num_event);
						if (!session)
						{
							WARNING_EX_LOG("session == null\b");
							continue;
						}
						//判断是否有client发送信息
						if (m_reactor->descriptor_read_state(num_event))
						{
							char * buf = new char[2048];
							int64 ret = csocket_ops::sync_read(session->get_sockfd(), buf, 2048);
							if (ret < 0) 
							{
								perror("Read error ");
								m_reactor->deregister_descriptor(session->get_sockfd());
								close(session->get_sockfd());
								
								delete[] buf;
								msg_ptr->m_msg_id = EMIR_Disconnect;
							}
							else if (ret == 0) 
							{
								ERROR_EX_LOG("client 退出\n");
								m_reactor->deregister_descriptor(session->get_sockfd());
								close(session->get_sockfd());
								
								msg_ptr->m_msg_id = EMIR_Disconnect
							}
							else 
							{
								
								msg_ptr->m_client = session->sockfd();
								msg_ptr->m_ptr = buf;
								msg_ptr->m_msg_id = 3;
								msg_ptr->m_size = ret;
								m_msgs<cmsg_data>.push(msg);
							}
							msg_ptr->m_client = session->sockfd();
							m_msgs<cmsg_data>.push(msg);
						}
					}
				}
			}
		}
}// chen


