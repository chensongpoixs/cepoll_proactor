/***********************************************************************************************
	created: 		2019-03-06
	
	author:			chensong
					
	purpose:		acceptor
************************************************************************************************/
#include "cacceptor.h"


namespace chen {
	bool 	cacceptor::init(const char * ip, uint32& port)
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
		m_ip = ip;
		m_port = port;
		return true;
	}
	bool	cacceptor::create()
	{   
		m_fd = csocket_ops::socket(AF_INET, SOCK_STREAM, 0);
		if (m_fd == -1)
		{
			ERROR_EX_LOG("create socket \n");
			return false;
		}
		// nlonl
		if (!csocket_ops::set_nonblocking(m_fd))
		{
			ERROR_EX_LOG("set create socket nonblocking \n");
			return false;
		}
		return true;
	}
	void 	cacceptor::destroy()
	{
		if (m_fd)
		{
			shutdown();
		}
	}
	
	/*bool 	cacceptor::open()
	{
		
			
		return true;
	}*/
	bool 	cacceptor::bind()
	{
		sockaddr_in sock_addr;
		memset( &sock_addr , 0 , sizeof(sock_addr) );

		sock_addr.sin_family = AF_INET;
		sock_addr.sin_addr.s_addr = inet_addr(m_ip); //htonl(INADDR_ANY);	// inet_addr(addr);
		sock_addr.sin_port        = htons(m_port);
		if (csocket_ops::bind(m_fd, (const struct sockaddr *)&sock_addr , sizeof(sock_addr)) == -1)
		{
			ERROR_EX_LOG("bind ip %s, port = %u\n", m_ip, m_port);
			return false;
		}
		return true;
	}
	bool 	cacceptor::listen()
	{
		if (csocket_ops::listen(m_fd) == -1)
		{
			ERROR_EX_LOG("listen \n");
			return false;
		}
		return	true;
	}
	socket_type 	get_sockfd()
	{
		return m_fd;
	}
	bool 	cacceptor::accpet()
	{
		return true;
	}
	
	void 	cacceptor::shutdown()
	{
		shutdown(m_fd, 0);
	}
} // chen