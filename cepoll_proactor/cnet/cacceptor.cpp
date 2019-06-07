/***********************************************************************************************
	created: 		2019-03-06
	
	author:			chensong
					
	purpose:		acceptor
************************************************************************************************/
#include "cacceptor.h"
#include "clog.h"
#include "cnet_types.h"
#include <cerrno>
namespace chen {
	cacceptor * cacceptor::construct()
	{
		return new cacceptor();
	}
	void cacceptor::destroy(cacceptor * p)
	{
		delete p;
	}
	bool 	cacceptor::init(const char * ip, uint32 port)
	{
		if (!ip )
		{
			ERROR_EX_LOG("ip not");
			return false;
		}
		if (port <= 0)
		{
			ERROR_EX_LOG("port not ");
			return false;
		}
		m_ip = ip;
		m_port = port;
		return true;
	}
	bool	cacceptor::create()
	{   
		m_sockfd = csocket_ops::socket(AF_INET, SOCK_STREAM, 0);
		if (m_sockfd == INVALID_SOCKET)
		{
			ERROR_EX_LOG("create socket ");
			return false;
		}
		// nlonl
		if (!csocket_ops::set_nonblocking(m_sockfd))
		{
			ERROR_EX_LOG("set create socket nonblocking ");
			return false;
		}
		return true;
	}
	bool cacceptor::async_connect(/*socket_type& 	sockfd*/)
	{
		//sockfd = csocket_ops::socket(AF_INET, SOCK_STREAM, 0);
		//if (sockfd == INVALID_SOCKET)
		//{
		//	ERROR_EX_LOG("create socket ");
		//	return false;
		//}
		//// nlonl
		//if (!csocket_ops::set_nonblocking(sockfd))
		//{
		//	ERROR_EX_LOG("set create socket nonblocking ");
		//	return false;
		//}
		struct sockaddr_in addrSrv;
		addrSrv.sin_family = AF_INET;
		addrSrv.sin_port = htons(m_port);
		addrSrv.sin_addr.s_addr = ::inet_addr(m_ip.c_str());  //要连接的服务器的ip地址


		/*SOCKET listenfd, maxfd;
		listenfd = ::socket(AF_INET, SOCK_STREAM, 0);      */              //建立客户端流式套接口
		return csocket_ops::async_connect(m_sockfd, (struct sockaddr *)&addrSrv, sizeof(addrSrv));
		// return true;
		//return  csocket_ops::async_send(m_sockfd, "", 1, 0) > 0 ? true :false;
		//int32 ret = ::connect(m_sockfd, (struct sockaddr *)&addrSrv, sizeof(addrSrv));      //与服务器进行连接
		//if (ret == 0)
		//{
		//	return true;
		//} 
		//ERROR_EX_LOG("connect ret =%d, error = %d", ret, errno);
		//ERROR_EX_LOG("WSAGetLastError = %d", WSAGetLastError()); /// 10035  ip port --> 网络不通
		// 10036 已经连接的socket
		//return false;
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
		struct sockaddr_in sock_addr;
		memset( &sock_addr , 0 , sizeof(sock_addr) );

		sock_addr.sin_family = AF_INET;
		sock_addr.sin_addr.s_addr = inet_addr(m_ip.c_str()); //htonl(INADDR_ANY);	// inet_addr(addr);
		sock_addr.sin_port        = htons(m_port);
		/*if (inet_aton(m_ip, &sock_addr.sin_addr) == 0)
		{
			ERROR_EX_LOG("bind convert ip = %s failed", m_ip);
			return InValidIp;
		}*/
		//int64 ret = csocket_ops::bind(m_sockfd, (const struct sockaddr *)&sock_addr, sizeof(sock_addr));
		if (!csocket_ops::bind(m_sockfd, (const struct sockaddr *)&sock_addr, sizeof(sock_addr)) )
		{
			ERROR_EX_LOG("bind  ip %s, port = %u", m_ip.c_str(), m_port);
			return false;
		}
		return true;
	}
	
	bool 	cacceptor::listen()
	{
		if (csocket_ops::listen(m_sockfd) == SOCKET_ERROR)
		{
			ERROR_EX_LOG("listen %lu", m_sockfd);
			return false;
		}
		return	true;
	}
	bool cacceptor::set_recvbuf_sendbuf(int64 recv_buff_size, int64 send_buff_size)
	{
		csocket_ops::setsocketopt(m_sockfd, SOL_SOCKET, SO_SNDBUF, &recv_buff_size, sizeof(recv_buff_size));
		csocket_ops::setsocketopt(m_sockfd, SOL_SOCKET, SO_RCVBUF, &recv_buff_size, sizeof(recv_buff_size));
		//  setsockopt(sock, SOL_SOCKET, SO_SNDBUF, &sendbuf, sizeof(sendbuf));
		return true;
	}
	bool cacceptor::set_nonagle(bool on)
	{
		int32 opt = on ? 1 : 0;
		int32 ret = csocket_ops::setsocketopt(m_sockfd, IPPROTO_TCP, TCP_NODELAY, (char *)&opt, static_cast<socklen_t>(sizeof(opt)));
		if (ret == SOCKET_ERROR)
		{
			ERROR_EX_LOG(" No nagle failed !");
			return false;
		}
		return true;
	}
	socket_type& 	cacceptor::get_sockfd()
	{
		return m_sockfd;
	}
	
	bool 	cacceptor::accept(socket_type& socket, struct sockaddr_in& sock_addr, int32& err_code)
	{

		uint32 addrlen = sizeof(sock_addr);
		socket = csocket_ops::accept(m_sockfd, (struct sockaddr *)(&(sock_addr)),(socklen_t*) &addrlen, err_code);
		if (socket == SOCKET_ERROR)
		{
			return false;
		}
		return true;
	}
	
	void 	cacceptor::shutdown()
	{
		::shutdown(m_sockfd, 0);
		//csocket_ops::close_socket(m_sockfd);
	}
} // namespace chen