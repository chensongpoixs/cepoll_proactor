/***********************************************************************************************
				created: 		2019-03-01

				author:			chensong

				purpose:		net_types
************************************************************************************************/
#ifndef _C_NET_TYPES_H_
#define _C_NET_TYPES_H_
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <new>
//#include <codecvt>
#include <locale>
#include <string>

#if defined(_MSC_VER)

	// win 
#   include <windows.h>
#include <WinSock2.h>
# include <winsock2.h>
# include <ws2tcpip.h>
//#   pragma comment(lib, "ws2.lib")
#   pragma comment(lib, "ws2_32.lib")
#    pragma comment(lib, "mswsock.lib")

#elif defined(__GNUC__)

	// unix
	
# include <sys/types.h>
# include <sys/stat.h>
# include <fcntl.h>
#  include <sys/time.h>
#  include <sys/select.h>
# include <sys/socket.h>
# include <sys/uio.h>
# include <sys/un.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <netdb.h>
# include <net/if.h>
# include <limits.h>
#include <unistd.h>
#else
#pragma error "unknow platform!!!"

#endif
namespace chen 
{
	typedef signed char		int8;
	typedef unsigned char	uint8;

	typedef signed short	int16;
	typedef unsigned short	uint16;

	typedef signed int		int32;
	typedef unsigned int	uint32;

#if defined(_MSC_VER)

	typedef signed long long	int64;
	typedef unsigned long long	uint64;

#elif defined(__GNUC__)

	typedef signed long		int64;
	typedef unsigned long	uint64;
#else
#pragma error "unknow platform!!!"

#endif
}

namespace chen
{

	enum CSOCKET_TYPE 
	{
		E_NONE		= 0x00,     /* No events registered. */
		E_READFDS	= 0x01, /* Fire when descriptor is readable. */
		E_WRITFDS	= 0x02,  /* Fire when descriptor is writable. */
		E_EXCEFDS	= 0x04
	};

#if defined(_MSC_VER)
#define FUNCTION __FUNCTION__
	typedef SOCKET socket_type;

#elif defined(__GNUC__)
#define FUNCTION __PRETTY_FUNCTION__

	typedef int socket_type;
	#define  INVALID_SOCKET  (-1)
	#define		SOCKET_ERROR		(-1)
#else
#pragma error "unknow platform!!!"

#endif
	
}  //namespace chen

#endif // !_C_NET_TYPES_H_
