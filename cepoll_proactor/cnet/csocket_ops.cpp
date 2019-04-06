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
			return ::send(sockfd,(const char *) buf, len, flags);
		}
		 
		 // async recv
		 int64 async_recv(socket_type sockfd, void *buf, int64 len, int32 flags)
		 {
			 return ::recv(sockfd,(char *) buf, len, flags);
		 }
		 
		 
		 bool set_nonblocking(socket_type sockfd, bool on = true)
		 {

			 /***设套接字为非阻塞模式***/ 
#if defined(_MSC_VER)
			 unsigned long ul = 1;
			 int	flags = ::ioctlsocket(sockfd, FIONBIO, &ul); //设置套接字非阻塞模式
			 if (flags == -1)
			 {
				 return false;
			 }

#elif defined(__GNUC__)

			 int32 flags = ::fcntl(sockfd, F_GETFL, 0);
			 if (flags == -1)
			 {
				 return false;
			 }

			 if (on)
			 {   // //非阻塞事件
				 flags |= O_NONBLOCK;
			 }
			 else
			 {
				 flags &= ~O_NONBLOCK;
			 }
			 if (-1 == ::fcntl(sockfd, F_SETFL, flags))
			 {
				 return false;
			 }
			
#else
#pragma error "unknow platform!!!"

#endif
			
			 return true;
		 }

		 int32 getsocketopt(socket_type sockfd, int32 level, int32 optname, void * optval, int64 optlen)
		 {
			 
			 int result = ::getsockopt(sockfd, level, optname, (char*)optval, (int *)&optlen);
			 return result;
		 }
		 
		 int32 shutdown(socket_type sockfd, int32 how)
		 {
			 return ::shutdown(sockfd, how);
		 }
	}
}