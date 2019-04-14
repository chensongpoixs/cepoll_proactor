/***********************************************************************************************
	created: 		2019-03-06
	
	author:			chensong
					
	purpose:		net_mgr
************************************************************************************************/
#include "cnet_mgr.h"
#include "clog.h"

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
			m_reactor 	= new creactor;
			if (!m_reactor)
			{
				ERROR_EX_LOG("create alloc fail \n");
				destroy();
				return false;
			}
			if (!m_reactor->init(2000))
			{
				destroy();
				return false;
			}
			// thread
			m_threads.emplace_back(&cnet_mgr::_work_thread, this);
			
			return true;
		}
		bool 		cnet_mgr::send_msg(uint32 sessionId, const void* msg_ptr, uint32 msg_size)
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
			//	m_reactor->shutdown();
				delete m_reactor;
			}
			
			for (std::thread &thread : m_threads)
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
					cmsg_connect* p = (cmsg_connect*)msg_ptr->get_buf();
					m_connect_callback(msg_ptr->get_session_id(), p->para, p->buf);
				}
				else if (msg_ptr->m_msg_id == EMIR_Disconnect)
				{
					m_disconnect_callback(msg_ptr->m_client);
				}
				else
				{
					m_msg_callback(msg_ptr->m_client, msg_ptr->m_msg_id
						, msg_ptr->m_ptr, msg_ptr->m_size);
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
			while (!m_shuting)
			{
				// write read queue
				int64 num_event = m_reactor->select(200);
				for (int64 i = 0; i < num_event; ++i)
				{
					cnet_session * session = static_cast<cnet_session*>(m_reactor->get_para(i));
					
					if (!session)
					{
						if (m_reactor->is_exception(i))
						{
							ERROR_EX_LOG("listen socket has exception\n");
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
						//if (!session->ProcessInputs())
						{
							_clearup_session(session);
							continue;
						}
					}

					if (m_reactor->is_exception(i))
					{
						ERROR_EX_LOG("pSession %u has exception, disconnected\n", session->get_sockfd());
						_clearup_session(session);
						continue;
					}

					if (m_reactor->is_write(i))
					{
						//if (!session->ProcessOutputs())
						{
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
			uint32 session = psession->get_sockfd();
			psession->clearup();
			// disconnect
			//m_msgs.push();
		}
		void cnet_mgr::_new_connect()
		{
			sockaddr_in sock_addr;
			int err_code;
			while (true)
			{
				socket_type	tmpsocket;

				memset(&sock_addr, 0, sizeof(sock_addr));
				sock_addr.sin_family = AF_INET;
				//-- accept socket
				if (!m_acceptor_ptr->accept(tmpsocket, sock_addr, err_code))
				{
					if (err_code == WSAEWOULDBLOCK)
					{
						break;
					}
					else
					{
						continue;
					}
				}

				if (!csocket_ops::set_nonblocking(tmpsocket))
				{
					continue;
				}
				cnet_session *psession = new cnet_session;
				if (!psession)
				{
					return;
				}
				m_reactor->register_readwrite_descriptor(tmpsocket, psession);

				uint32 port = ntohs(sock_addr.sin_port);
				uint32 ip = (uint32)(sock_addr.sin_addr.s_addr);
				// connect
				//m_msgs.push();
			}
		}
}// namespce chen


