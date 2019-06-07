/***********************************************************************************************
	created: 		2019-03-01
	
	author:			chensong
					
	purpose:		csocket_ops
************************************************************************************************/

#include "csocket_ops.h"
#include "cnet_types.h"
#include "clog.h"

namespace chen {
	namespace csocket_ops {
		
		socket_type socket(int32 domain, int32 type, int32 protocol)
		{
			return ::socket(domain, type, protocol);
		}
		
		bool bind(socket_type descriptor,const  struct sockaddr * addr, uint32 addrlen)
		{
			if (::bind(descriptor, addr, addrlen) == SOCKET_ERROR)
			{
				ERROR_EX_LOG("bind fail error = %ld, error = %s", errno, strerror(errno));
				return false;
			}
			return true;
		}

		//13 EACCES, EPERM 用户试图在套接字广播标志没有设置的情况下连接广播地址或由于防火墙策略导致连接失败。
		//98 EADDRINUSE本地地址处于使用状态。
		//97 EAFNOSUPPORT参数serv_add中的地址非合法地址。
		//11 EAGAIN没有足够空闲的本地端口。
		//114 EALREADY套接字为非阻塞套接字，并且原来的连接请求还未完成。
		//9EBADF 非法的文件描述符。
		//111 ECONNREFUSED远程地址并没有处于监听状态。
		//14 EFAULT 指向套接字结构体的地址非法。
		//115 EINPROGRESS套接字为非阻塞套接字，且连接请求没有立即完成。
		//4 EINTR 系统调用的执行由于捕获中断而中止。
		//106 EISCONN 已经连接到该套接字。
		//101 ENETUNREACH网络不可到达。
		//88 ENOTSOCK 文件描述符不与套接字相关。
		//110 ETIMEDOUT 连接超时
		bool async_connect(socket_type descriptor, const sockaddr * addr, uint32 addrlen)
		{
			if (::connect(descriptor, addr, addrlen) == SOCKET_ERROR)
			{
				bool socket_ret = false;
#if defined(__GCC__)

			//	服务器开发中网络编程是一项基本技能也是重要的技能，在高并发socket编程中我们经常会使用到异步socket也就是非阻塞socket。下面记录下非阻塞socket中如何判断connect连接是否成。在非阻塞socket调用connect函数之后可能出现的情况有：
			//
			//	1 connect函数直接返回0，那么恭喜你连接直接成功了，不用考虑其他问题了。
			//
			//	2 连接返回错误，errno等于EINPROGRESS，这种情况表示socket正在连接，需要后续使用epoll_wait 或者select函数进行确认，当然也有很多其他不常用的方法比如write ，send等等，但是服务器异步socket几乎都不可能用这些方法。准确的来说Linux中高并发服务器都是使用epoll模型，所以使用epoll_wait函数。
			//
			//	3 连接返回错误，errno 不等于EINPROGRESS，表示connect函数调用失败了。
			//
			//	对于connect函数出现上述中的第二种情况那应该如何确认socket是否连接成功了，下面将讲述下epoll_wait 和select函数调用的不同处理情况：
			//
			//	1 epoll 模型：将连接的socket加入epoll的监听事件中，然后再调用epoll_wait函数等待事件触发，如果连接的socket的端口没有监听或者连接的ip不可达都会触发EPOLLERR事件，socket连接成功一般触发EPOLLOUT事件，但是如果对方在连接成功后立马给你回一条数据，那么可能同时触发EPOLLIN 和EPOLLOUT事件，网络编程的书上一般都说连接失败socket会变的可读可写，但是epoll模型中我不知道怎么触发这种情况，调试没有试出来。
			//
			//	2 select模型：同样将连接的socket加入读写错误三个事件中，调用select函数，如果socket连接失败那么socket会变的同时可读和可写，如果socket连接成功可能会出现两种情况，第一是socket可写，第二就是socket同时可读可写。那么现在问题来了，当socket同时可读可写的时候我们怎么判断socket到底是连接成功了还是失败了呢。所以我们在socket同时可读可写的时候需要调用另外一个函数来确定socket是否连接成功，那就是getsockopt函数，getsockopt函数有一个选项为SO_ERROR，通过该选项我们可以判断出socket是否连接成功，注意不是获取该函数的返回值而是函数的参数返回值，函数调用中的第四个参数会返回socket连接错误的错误码，如果成功错误码为0，否则不为0。该函数书上说调用之后会把错误同时赋值给errno但是我发现没有，只能通过函数调用的参数获取错误。就记录这么多了，该洗洗睡了。
					
				switch (errno) 
				{
					case EALREADY:
						break;
					case EINPROGRESS:  //套接字为非阻塞套接字，且连接请求没有立即完成。
					{
						struct timeval time_out;
						time_out.tv_sec = 0;
						time_out.tv_usec = 15000;
						fd_set write;
						FD_ZERO(&write);
						FD_SET(descriptor, &write);
						int32 ret = 0;
						ret = csocket_ops::select(0, NULL, &write, NULL, &time_out);
						FD_ZERO(&write);
						if (ret > 0)
						{
							//return true;
							//break;
							socket_ret = true;
						}
						break;
					}
					case ECONNREFUSED:
						break;
					case EISCONN:
						break;
					case ETIMEDOUT: //连接超时
					ERROR_EX_LOG("connection refused,");
					break;
					case ENETUNREACH:
						break;
					case EADDRINUSE:
						break;
					case EBADF:
						break;
					case EFAULT:
						break;
					case ENOTSOCK:
						break;
					default:
					{
						break;
					}
				}//end of switch
#elif  defined(_MSC_VER)

			//	说明：第一个参数是套接字，第二个是包含地址信息的sockaddr_in结构体，第三个 是sockaddr_in结构体长度；
			//
			//		返回值 : 正常返回0， 错误返回SOCKET_ERR, 也就是 - 1， 
			//
			//		可用WSAGetlastError()获取到错误码；
			//
			//		需要注意的是 :
			//	1.只有对于阻塞套接字，返回值才可以直接表示连接尝试成功或失败。使用非阻塞套接字时，无法立即完成连接尝试。 在这种情况下，connect将返回SOCKET_ERROR，WSAGetLastError将返回WSAEWOULDBLOCK。
			//		2.对于非阻塞套接字，无法立即完成连接尝试，也就是说， 在这种情况下，connect将返回SOCKET_ERROR，WSAGetLastError将返回WSAEWOULDBLOCK。这时要判断是否连接成功，可以使用select函数, 通过检查套接字是否可写来确定连接请求的是否已 成功。也就是设置select的第三个参数，判断返回的可写的套接字个数，如果大于0，则 表示有成功连接 :
			//	int nRet = select(0, 0, &fd, 0, &tConnectTimeOut);
			//
			//	if (nRet > 0)
			//		return true;
			//
			//另外两种方法判断非阻塞套接字是否成功，如:
			//
			//	1.  如果应用程序使用WSAAsyncSelect表示对连接事件感兴趣，则应用程序将收到FD_CONNECT通知，指示连接操作已完成（成功与否）
			//
			//	2. 如果应用程序使用WSAEventSelect来指示对连接事件的兴趣，那么将发出关联的事件对象的信号，指示连接操作已完成（成功与否）。
					
				int32 iErr = WSAGetLastError();
				//ERROR_EX_LOG("WSAGetLastError = %d", iErr); /// 10035  ip port --> 网络不通
				switch (iErr)
				{
					case WSANOTINITIALISED:
						break;
					case WSAENETDOWN:
						break;
					case WSAEADDRINUSE:
						break;
					case WSAEINTR:
						break;
					case WSAEINPROGRESS:
						break;
					case WSAEALREADY:
						break;
					case WSAEADDRNOTAVAIL:
						break;
					case WSAEAFNOSUPPORT:
						break;
					case WSAECONNREFUSED:
						break;
					case WSAEFAULT:
						break;
					case WSAEINVAL:
						break;
					case WSAEISCONN:
						break;
					case WSAENETUNREACH:
						break;
					case WSAENOBUFS:
						break;
					case WSAENOTSOCK:
						break;
					case WSAETIMEDOUT: //连接超时
						ERROR_EX_LOG("connection refused,");
						break;
					case WSAEWOULDBLOCK: // server accept  时 
					{
						//ERROR_EX_LOG(" not service");
						struct timeval time_out;
						time_out.tv_sec = 0;
						time_out.tv_usec = 15000;
						fd_set write;
						FD_ZERO(&write);
						FD_SET(descriptor, &write);
						int32 ret = 0;
						ret = csocket_ops::select(0, NULL, &write, NULL, &time_out);
						FD_ZERO(&write);
						if (ret > 0)
						{
							//return true;
							//break;
							socket_ret = true;
						}
						break;
					};
					default:
					{
						break;
					};
				};//end of switch
#endif
				return socket_ret;
			}

			return true;
		}
		// socket_type accept(socket_type sockfd, struct sockaddr *addr, socklen_t *addrlen, int32 &error_code);
		socket_type accept(socket_type descriptor, struct sockaddr *addr, socklen_t *addrlen, int32 &error_code)
		{
			socket_type tmep = ::accept(descriptor, addr, addrlen);
#if defined(_MSC_VER)

			error_code = WSAGetLastError();
			if (error_code == WSAEWOULDBLOCK)
			{
			}
#endif // #if defined(_MSC_VER)			
			
			return tmep;
		}
	

		int32 listen(socket_type descriptor, int32 backlog)
		{
			return ::listen(descriptor, backlog);
		}
		
		int64 async_send(socket_type descriptor, const void *buf, int32 len, int32 flags)
		{
			return ::send(descriptor,(const char *) buf, len, flags);
		}
		 
		 // async recv
		 int64 async_recv(socket_type descriptor, void *buf, int32 len, int32 flags)
		 {
			 int64 recv_len = ::recv(descriptor, (char *)buf, len, flags);

			 //if (recv_len == SOCKET_ERROR) {
				// int32 id = WSAGetLastError();
				// switch (id)
				// {
				// case WSANOTINITIALISED: ERROR_EX_LOG("not initialized\n"); break;
				// case WSASYSNOTREADY: ERROR_EX_LOG("sub sys not ready\n"); break;
				// case WSAHOST_NOT_FOUND: ERROR_EX_LOG("name server not found\n"); break;
				// case WSATRY_AGAIN: ERROR_EX_LOG("server fail\n"); break;
				// case WSANO_RECOVERY: ERROR_EX_LOG("no recovery\n"); break;
				// case WSAEINPROGRESS: ERROR_EX_LOG("socket blocked by other prog\n"); break;
				// case WSANO_DATA: ERROR_EX_LOG("no data record\n"); break;
				// case WSAEINTR: ERROR_EX_LOG("blocking call canciled\n"); break;
				// case WSAEPROCLIM: ERROR_EX_LOG("limit exceeded\n");
				// case WSAEFAULT: ERROR_EX_LOG("lpWSAData in startup not valid\n");
				// default: ERROR_EX_LOG("unknown error id = %d\n", id); break;
				// };
				// ERROR_EX_LOG("receive error.\n");
				// 
			 //}
			 return recv_len;
		 }
		 
		 int32 select(int32 maxfd, fd_set* read, fd_set *write, fd_set *exce, const struct timeval* time_out)
		 {
			 return ::select(maxfd, read, write, exce, (struct timeval*)time_out);
		 }
		 
		 bool set_nonblocking(socket_type descriptor, bool on)
		 {

			 /***设套接字为非阻塞模式***/ 
#if defined(_MSC_VER)
			 unsigned long ul = 1;
			 int32	flags = ::ioctlsocket(descriptor, FIONBIO, &ul); //设置套接字非阻塞模式
			 if (flags == -1)
			 {
				 return false;
			 }

#elif defined(__GNUC__)

			 int32 flags = ::fcntl(descriptor, F_GETFL, 0);
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
			 if (-1 == ::fcntl(descriptor, F_SETFL, flags))
			 {
				 return false;
			 }
			
#else
#pragma error "unknow platform!!!"

#endif
			
			 return true;
		 }

		 bool close_socket(socket_type descriptor)
		 {
#if defined(_MSC_VER)

			 if (::closesocket(descriptor) == SOCKET_ERROR)
			 {
				 //int32 iErr = WSAGetLastError() ;
				 return false;
			 }

#elif defined(__GNUC__)

			 if (::close(descriptor) < 0)
			 {
				 //errno
				return false;
			}
#else

#pragma error "unknow platform!!!"

#endif
			 return true;
		 }

		 int32 getsocketopt(socket_type descriptor, int32 level, int32 optname, void * optval, int32 optlen)
		 {
			 
			 int result = ::getsockopt(descriptor, level, optname, (char*)optval, (socklen_t*)&optlen);
			 return result;
		 }
		 
		 int32 setsocketopt(socket_type descriptor, int32 level, int32 optname, void * optval, int32 optlen)
		 {
			 int32 result = ::setsockopt(descriptor, level, optname, (char*)optval, optlen);
			 return result;
		 }

		 int32 shutdown(socket_type descriptor, int32 how)
		 {
			 return ::shutdown(descriptor, how);
		 }
	}
}
