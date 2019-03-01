/***********************************************************************************************
	created: 		2019-03-01
	
	author:			chensong
					
	purpose:		csocket_ops
************************************************************************************************/

#include "csocket_ops.h"


namespace chen {
	namespace csocket_ops {
		
		int32 socket(int32 domain, int32 type, int32 protocol)
		{
			return ::socket(domain, type, protocol);
		}
		
		int32 bind(socket_type s, const struct sockaddr * addr, std::size_t addrlen)
		{
			return ::bind(s, addr, addrlen);
		}
		
		socket_type accept(socket_type sockfd, struct sockaddr *addr, socklen_t *addrlen)
		{
			return ::accept(sockfd, addr, addrlen);
		}
		
		int32 listen(socket_type sockfd, int32 backlog = 20000)
		{
			return ::listen(sockfd, backlog);
		}
		
		int64 async_send(socket_type sockfd, const void *buf, int64 len, int32 flags)
		{
			return ::send(sockfd, buf, len, flags);
		}
		 
		 // async recv
		 int64 async_recv(socket_type sockfd, void *buf, int64 len, int32 flags)
		 {
			 return ::recv(sockfd, buf, len, flags);
		 }
		 
		 
		 bool set_nonblocking(socket_type sockfd, bool on = true)
		 {
			int32 flags = ::fcntl( sockfd , F_GETFL , 0 );
			if(flags == -1)
			{
				return false;
			}

			if ( on )
			{   // //非阻塞事件
				flags |= O_NONBLOCK;
			}
			else
			{
				flags &= ~O_NONBLOCK;
			}
			if(-1 == ::fcntl( sockfd , F_SETFL , flags ))
			{
				return false;
			}
			return true;
		 }
		 
		 int32 shutdown(socket_type sockfd, int32 how)
		 {
			 return ::shutdown(sockfd, how);
		 }
	}
}