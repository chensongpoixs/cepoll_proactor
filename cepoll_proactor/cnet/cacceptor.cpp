/***********************************************************************************************
	created: 		2019-03-06
	
	author:			chensong
					
	purpose:		acceptor
************************************************************************************************/
#include "cacceptor.h"
#include "clog.h"

namespace chen {
	bool 	cacceptor::init(const char * ip, uint32 port)
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
		m_sockfd = csocket_ops::socket(AF_INET, SOCK_STREAM, 0);
		if (m_sockfd == -1)
		{
			ERROR_EX_LOG("create socket \n");
			return false;
		}
		// nlonl
		if (!csocket_ops::set_nonblocking(m_sockfd))
		{
			ERROR_EX_LOG("set create socket nonblocking \n");
			return false;
		}
		return true;
	}
	bool cacceptor::async_connect()
	{
		fd_set write_fds;
		FD_ZERO(&write_fds);
		FD_SET(m_sockfd, &write_fds);
		fd_set except_fds;
		FD_ZERO(&except_fds);
		FD_SET(m_sockfd, &except_fds);
		timeval zero_timeout;
		zero_timeout.tv_sec = 0;
		zero_timeout.tv_usec = 0;
		int ready = ::select(m_sockfd + 1, 0, &write_fds, &except_fds, &zero_timeout);
		return false;
	}
	void 	cacceptor::destroy()
	{
		if (m_sockfd)
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
		sock_addr.sin_addr.s_addr = inet_addr(m_ip.c_str()); //htonl(INADDR_ANY);	// inet_addr(addr);
		sock_addr.sin_port        = htons(m_port);
		if (csocket_ops::bind(m_sockfd, (const struct sockaddr *)&sock_addr , sizeof(sock_addr)) == -1)
		{
			ERROR_EX_LOG("bind ip %s, port = %u\n", m_ip, m_port);
			return false;
		}
		return true;
	}
	
	bool 	cacceptor::listen()
	{
		if (csocket_ops::listen(m_sockfd) == -1)
		{
			ERROR_EX_LOG("listen \n");
			return false;
		}
		return	true;
	}
	socket_type& 	cacceptor::get_sockfd()
	{
		return m_sockfd;
	}
	
	bool 	cacceptor::accept(socket_type& socket, struct sockaddr_in& sock_addr, int32& err_code)
	{

		uint32 addrlen = sizeof(sock_addr);
		socket = csocket_ops::accept(m_sockfd, (struct sockaddr *)(&(sock_addr)),(socklen_t*) &addrlen, err_code);
		if (socket == INVALID_SOCKET)
			return false;
		return true;
	}
	
	void 	cacceptor::shutdown()
	{
		//shutdown(m_sockfd, 0);
	}
} // namespace chen