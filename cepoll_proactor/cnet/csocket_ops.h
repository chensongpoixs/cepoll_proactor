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
		socket_type socket(int32 domain, int32 type, int32 protocol);
		 // bind socket port
		bool bind(socket_type descriptor, const struct sockaddr * addr, uint32 addrlen);
		 
		bool async_connect(socket_type descriptor, const struct sockaddr * addr, uint32 addrlen);
		 // accpet 
		 socket_type accept(socket_type descriptor, struct sockaddr *addr, socklen_t *addrlen, int32 &error_code);
		 // listen queue 并发量
		 int32 listen(socket_type descriptor, int32 backlog = 1000);
		 // async send
		 int64 async_send(socket_type descriptor, const void *buf, int32 len, int32 flags);
		 
		 // async recv  
		 int64 async_recv(socket_type descriptor, void *buf, int32 len, int32 flags);
		 

		 // select 
		 int32 select(int32 maxfd, fd_set* read, fd_set *write, fd_set *exce, const struct timeval* time_out);
		 /***设套接字为非阻塞模式***/
		 bool set_nonblocking(socket_type descriptor, bool on = true);


		 bool close_socket(socket_type descriptor);
		 int32 getsocketopt(socket_type descriptor, int32 level, int32 optname, void* optval, int32 optlen);
		 //int result = ::getsockopt(s, level, optname, (char*)optval, &tmp_optlen);
		 int32 setsocketopt(socket_type descriptor, int32 level, int32 optname, void* optval, int32 optlen);
		 int32 shutdown(socket_type descriptor, int32 how);
	}  // csocket_ops
}  // namespace chen 
#endif //#ifndef  _C_SOCKET_OPS_H_