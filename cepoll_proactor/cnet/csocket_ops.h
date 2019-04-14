/***********************************************************************************************
	created: 		2019-03-01
	
	author:			chensong
					
	purpose:		csocket_ops
************************************************************************************************/
#ifndef _C_SOCKET_OPS_H_
#define _C_SOCKET_OPS_H_
#include "cnet_types.h"
namespace chen {
	
	namespace csocket_ops {
		
		 // create socket
		 int32 socket(int32 domain, int32 type, int32 protocol);
		 // bind socket port
		 int32 bind(socket_type s, const struct sockaddr * addr, std::size_t addrlen);
		 
		 // accpet 
		 socket_type accept(socket_type sockfd, struct sockaddr *addr, socklen_t *addrlen, int32 &error_code);
		 // listen queue 并发量
		 int32 listen(socket_type sockfd, int32 backlog = 1000);
		 // async send
		 int64 async_send(socket_type sockfd, const void *buf, int64 len, int32 flags);
		 
		 // async recv  
		 int64 async_recv(socket_type sockfd, void *buf, int64 len, int32 flags);
		 

		 // select 
		 uint32 select(uint32 maxfd, fd_set* read, fd_set *write, fd_set *exce, const struct timeval* time_out);
		 /***设套接字为非阻塞模式***/
		 bool set_nonblocking(socket_type sockfd, bool on = true);


		 bool close_socket(socket_type sockfd);
		 int32 getsocketopt(socket_type sockfd, int32 level, int32 optname, void* optval, int64 optlen);
		 //int result = ::getsockopt(s, level, optname, (char*)optval, &tmp_optlen);
		 int32 shutdown(socket_type sockfd, int32 how);
	}  // csocket_ops
}  // namespace chen 
#endif //#ifndef  _C_SOCKET_OPS_H_