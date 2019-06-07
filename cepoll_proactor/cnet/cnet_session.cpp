/***********************************************************************************************
	created: 		2019-03-01
	
	author:			chensong
					
	purpose:		net_session
************************************************************************************************/
#include "cnet_session.h"
#include "clog.h"
#include "cscoped_ptr.h"
#include "csocket_ops.h"
#include "cnet_msg_reserve.h"
#include "cnet_msg_header.h"
#include "cstr2digit.h"
namespace chen  {
	
	
	
	cnet_session::cnet_session(cnet_mgr & _ptr, uint32 session_id)
		: m_net_ptr(_ptr) 
		, m_client_session(session_id)
		, m_status(ESS_NONE)
		, m_sockfd(0)
		, m_event(0)
		, m_recv_size(0)
		, m_recv_buf_ptr(NULL)
		, m_data_buf_size(0)
		, m_data_buf_ptr(NULL)
		, m_send_size(0)
		, m_sending_buf_ptr(NULL)
		
	{
	}

	cnet_session::~cnet_session()
	{
	}

	bool 	cnet_session::init(uint32 send_buff_size, uint32 recv_buff_size)
	{
		m_recv_buf_ptr = new char [recv_buff_size];
		if (!m_recv_buf_ptr)
		{
			ERROR_EX_LOG ("alloc m_recv_buf_ptr memory error!");
			return false;
		}

		m_recv_buf_data_ptr = new char[recv_buff_size];
		if (!m_recv_buf_data_ptr)
		{
			ERROR_EX_LOG("alloc m_recv_buf_data_ptr memory error!");
			return false;
		}

		m_data_buf_ptr = new char[send_buff_size];
		if (!m_data_buf_ptr)
		{
			ERROR_EX_LOG("alloc send_buff_size memory error!");
			return false;
		}

		m_sending_buf_ptr = new char[send_buff_size];
		if (!m_sending_buf_ptr)
		{
			ERROR_EX_LOG("alloc sending_buff_size memory error!");
			return false;
		}

		m_status = ESS_INIT;
		m_send_size = send_buff_size;
		m_recv_size = recv_buff_size;
		m_send_available_size = send_buff_size;
		m_recv_available_size = recv_buff_size;
	
		/*if (sockfd <= 0)
		{
			WARNING_EX_LOG(" sockfd <= 0 ");
			return false;
		}*/
		
		/*if (m_sockfd == 0 )
		{
			std::cout << "[" << FUNCTION << "][" << __LINE__ << "][Warn]" <<  " sockfd =  " << m_sockfd << std::endl;
		}*/
		
		//{
		//	cscoped_ptr<std::mutex> lock;
		//	m_sockfd = sockfd;
		//	//m_event = event;
		//}
		
		return true;
	}
	bool cnet_session::processinputs()
	{
		//int64 temp_recv_size = ::recv(m_sockfd, m_recv_buf_ptr + (m_recv_size - m_recv_available_size), m_recv_available_size, 0);
		int64 temp_recv_size = csocket_ops::async_recv(m_sockfd, m_recv_buf_ptr + (m_recv_size - m_recv_available_size), m_recv_available_size, 0);
#if defined(_MSC_VER)
		//对于一个异步的SOCKET 常见的10035错误 发生在 1 发送端要发送的数据超过了 设置的缓冲区大小，也就是发送缓冲区已经满了 这时候send会返回-1 GetLastError为10035
		//ERROR_EX_LOG("temp_recv_size =%d WSAGetLastError = %ld", temp_recv_size, WSAGetLastError());
		if ((temp_recv_size == 0) || (temp_recv_size == SOCKET_ERROR /*&& WSAGetLastError() == 10054*/ /*WSAECONNRESET*/))//nRet == SOCKET_ERROR && WSAGetLastError() == WSAECONNRESET
		{
			SYSTEM_LOG(" recv faild ");
			return false;
		}
		//SYSTEM_LOG(" recv ok ");
#elif defined(__GNUC__)
		if (temp_recv_size <= 0)
		{
			return false;
		}
#else
#pragma error "unknow platform!!!"

#endif
		
		m_recv_available_size -= temp_recv_size;
		//m_recv_size += temp_recv_size;
		//memcpy(m_data_buf_ptr + m_data_buf_size, m_recv_buf_ptr, temp_recv_size);
		//m_data_buf_size += temp_recv_size;
		while (temp_recv_size > m_recv_size)
		{
			temp_recv_size = csocket_ops::async_recv(m_sockfd, m_recv_buf_ptr + (m_recv_size - m_recv_available_size), 1024/*m_recv_available_size*/, 0);
			if (temp_recv_size > 0)
			{
				m_recv_available_size -= temp_recv_size;
				//m_recv_size += temp_recv_size;
				/*memcpy(m_data_buf_ptr + m_data_buf_size, m_recv_buf_ptr, temp_recv_size);
				m_data_buf_size += temp_recv_size;*/
			}
		}
		/// 接收数据包的大小
		while (m_recv_size - m_recv_available_size >= MSG_HEADER_SIZE)
		{
			//ERROR_EX_LOG(" recv_msg_ptr recv_size = %lu", (m_recv_size - m_recv_available_size));
			//分包 沾包问题的处理
			cnet_msg * recv_msg_ptr = _next_new_net_msg(EMIR_Close);
			if (recv_msg_ptr)
			{
				m_net_ptr.msg_push(recv_msg_ptr);
			}
			else
			{
				//数据包不完整
				break;
			}
			//m_recv_size = 0;
			//m_recv_available_size = m_recv_size;
		}
		
		//m_data_buf_size = 0;
		return true;
	}
	bool cnet_session::processoutputs()
	{
		clock_guard			guard(m_send_mutex);
	//	std::lock_guard<std::mutex> guard(m_send_mutex);
		if (m_data_buf_size <= 0)
		{
			return true;
		}
		if (m_data_buf_ptr && m_data_buf_size > 0)
		{
			memcpy(m_sending_buf_ptr , m_data_buf_ptr, m_data_buf_size);
			
			m_send_available_size -= m_data_buf_size;
			m_data_buf_size = 0;
		}
		//ERROR_EX_LOG("send = %lu", m_send_size - m_send_available_size);
		if ((m_send_size - m_send_available_size) > 0)
		{
			//ERROR_EX_LOG("send gm -> %d", m_send_size);
			int64 temp_send_size = csocket_ops::async_send(m_sockfd, m_sending_buf_ptr, (m_send_size - m_send_available_size), 0);
			//已经发送的数据块的大小
			int64 send_size = temp_send_size;
			while ((m_send_available_size + temp_send_size) < m_send_size)
			{
				m_send_available_size += temp_send_size;
				send_size += temp_send_size;
				temp_send_size = csocket_ops::async_send(m_sockfd, m_sending_buf_ptr + send_size, (m_send_size - m_send_available_size)/*m_send_size*/, 0);
			}
			m_send_available_size = m_send_size;
			//m_send_available_size = m_send_size;
			//m_send_size = 0;
		}
		return true;
	}
	cnet_msg * cnet_session::_next_new_net_msg(uint32 socket_status)
	{


		/*{
			std::string str_hex;
			byte2hex(str_hex, (const unsigned char *) m_recv_buf_ptr, ( m_recv_size - m_recv_available_size));
			ERROR_LOG(" recv = hex = %s, size = %lu , packet = %s", str_hex.c_str(), (m_recv_size - m_recv_available_size), m_recv_buf_ptr);
		}*/

		if ((m_recv_size - m_recv_available_size) < MSG_HEADER_SIZE)
		{
			return NULL;
		}
		cnet_msg * msg_ptr = NULL;
		if (m_recv_size > 0 && m_recv_buf_ptr)
		{
			//数据包头信息
			cnet_msg_header m_net_header;
			memcpy(&m_net_header, m_recv_buf_ptr, MSG_HEADER_SIZE);

			//有可能是非法客户端数据
			if (m_net_header.m_size > m_recv_size)
			{
				m_recv_available_size = m_recv_size;
				ERROR_EX_LOG("recv header size tail size %d, msg_id = %d", m_net_header.m_size, m_net_header.m_msg_id);
				return NULL;
			}
			//数据包接收的不完整
			if (m_net_header.m_size > (m_recv_size - m_recv_available_size))
			{
				//ERROR_EX_LOG("recv size %d, msg_id = %d", m_net_header.m_size, m_net_header.m_msg_id);
				return NULL;
			}
			msg_ptr = new cnet_msg();
			if (!msg_ptr)
			{
				return NULL;
			}
			//cmsg_header m_net_header;
			//memcpy(&m_net_header, m_recv_buf_ptr, MSG_HEADER_SIZE);
			if (m_net_header.m_size)
			{
				if (!msg_ptr->alloc_me(m_net_header.m_size/*m_recv_size - m_recv_available_size*/))
				{
					delete msg_ptr;
					return NULL;
				}
			}
			msg_ptr->set_session_id(m_client_session);
			msg_ptr->set_status(socket_status);
			msg_ptr->set_msg_id(m_net_header.m_msg_id);
			//data_ptr->m_client = m_client_session/*m_sockfd*/;
			//data_ptr->m_status = socket_status;
			//data_ptr->m_msg_id = 0;
			//data_ptr->m_ptr = new char[/*m_net_header.m_size*/(m_recv_size - m_recv_available_size)];
			/*if (!data_ptr->m_ptr)
			{
				delete data_ptr;
			}*/
			if (m_net_header.m_size > 0)
			{
				memcpy(msg_ptr->get_buf(), m_recv_buf_ptr + MSG_HEADER_SIZE, m_net_header.m_size/*(m_recv_size - m_recv_available_size)*/);
			}
			//一个包体的大小 
			uint32 temp_recv_available_size = m_recv_available_size + MSG_HEADER_SIZE + m_net_header.m_size;
			// 弹出第一包 
			memcpy(m_recv_buf_data_ptr, m_recv_buf_ptr + MSG_HEADER_SIZE + m_net_header.m_size, (m_recv_size - temp_recv_available_size));
			
			// 计算出余下的数据包大小
			//ERROR_EX_LOG("recv_header size = %d, m_recv_available_size=%lu ", m_net_header.m_size, m_recv_available_size);
			
			//printf("m_recv_available_size = %lu", m_recv_available_size);
			//ERROR_EX_LOG("recv_header size = %d, recv_buff_size=%lu ", m_net_header.m_size, m_recv_available_size);
			//当没有数据时不需要拷贝了
			if ((m_recv_size - temp_recv_available_size) > 0)
			{
				memcpy(m_recv_buf_ptr, m_recv_buf_data_ptr, m_recv_size - temp_recv_available_size);
				m_recv_available_size = temp_recv_available_size;
			}
			else
			{
				m_recv_available_size = m_recv_size;   //没有数据时 恢复接收缓存区的大小
			}
			
			//msg_ptr->set_msg_id() = (m_recv_size - m_recv_available_size);
			//m_recv_available_size = m_recv_size;
			//char buf[m_recv_size] = {0};
			//memcpy(m_recv_buf_data_ptr, m_recv_buf_ptr + MSG_HEADER_SIZE + m_net_header.m_size , ( m_recv_size - m_send_available_size - (MSG_HEADER_SIZE + m_net_header.m_size - 1) ));
			//memcpy(m_recv_buf_ptr, m_recv_buf_data_ptr, m_recv_size - m_send_available_size - (MSG_HEADER_SIZE + m_net_header.m_size ));
			//ERROR_EX_LOG("av recv =%lu, m size = %d", m_recv_available_size, m_net_header.m_size);
			//m_recv_available_size += MSG_HEADER_SIZE + m_net_header.m_size ;
		}
		else
		{
			return NULL;
		}
		return msg_ptr;
	}
	
	bool cnet_session::send_data(const void * p, uint32 size)
	{
		clock_guard			guard(m_send_mutex);
		//lock
		if (!memcpy(m_data_buf_ptr + m_data_buf_size, p, size))
		{
			WARNING_LOG("send data memcpy fail");
			return false;
		}
		m_data_buf_size += size;
		return true;
	}
	void cnet_session::clearup()
	{
		m_status = ESS_INIT;
		csocket_ops::close_socket(m_sockfd);
		/*if ((m_sockfd & 0XFFFF0000) == 0XFFFF0000)
		{
			m_sockfd += 0X00020000;
		}
		else
		{
			m_sockfd += 0X00010000;
		}
		*/
	}
	void cnet_session::set_connect(socket_type sockfd)
	{
		m_status = ESS_OPEN;
		m_sockfd = sockfd;
	}
	
	void 	cnet_session::destroy()
	{
		// send -> data
		SYSTEM_LOG(" session destroy  status = %d, sockfd =%d", m_status, m_sockfd);
		if (is_open())
		{
			if (m_send_size - m_send_available_size > 0)
			{
				// send -> 
				processoutputs();
			}
			SYSTEM_LOG(" close client socket fd");
			csocket_ops::close_socket(m_sockfd);
		}
		if (m_recv_buf_data_ptr)
		{
			delete m_recv_buf_data_ptr;
		}
		if (m_sending_buf_ptr)
		{
			delete[] m_sending_buf_ptr;
		}
		if (m_data_buf_ptr)
		{
			delete[] m_data_buf_ptr;
		}
		if (m_recv_buf_ptr)
		{
			delete[] m_recv_buf_ptr;
		}
	}
	
	
}  // chen
