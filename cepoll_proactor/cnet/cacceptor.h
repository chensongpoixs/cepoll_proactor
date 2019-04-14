/***********************************************************************************************
	created: 		2019-03-06
	
	author:			chensong
					
	purpose:		acceptor
************************************************************************************************/
#ifndef _C_ACCEPTOR_H_
#define _C_ACCEPTOR_H_
#include "cnoncopyable.h"
#include "csocket_ops.h"
#include "cnet_types.h"
namespace chen {
	class cacceptor : public cnoncopyable
	{
	public:
		explicit cacceptor():m_sockfd(-1), m_ip (NULL),m_port(0){}
		virtual ~cacceptor() {}
		bool 	init(const char * ip, uint32 port);
		void 	destroy();
	public: 
		// IPv4
		bool			create(); 
		bool		    async_connect();
		//bool 			open();
		bool 			bind();
		bool		 	accept(socket_type& socket, struct sockaddr_in& sock_addr, int32& err_code);
		// 并发数
		bool 			listen();
		socket_type& 	get_sockfd();
	private:
		void 			shutdown();
	private:
		socket_type 	m_sockfd; 
		std::string 	m_ip;		// ip
		uint32			m_port;    // port
	};
}// namespace chen

#endif // _C_ACCEPTOR_H_