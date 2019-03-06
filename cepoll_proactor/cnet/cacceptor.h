/***********************************************************************************************
	created: 		2019-03-06
	
	author:			chensong
					
	purpose:		acceptor
************************************************************************************************/
#ifndef _C_ACCEPTOR_H_
#define _C_ACCEPTOR_H_
#include "cnoncopyable.h"
#include "csocket_ops.h"

namespace chen {
	class cacceptor : public cnoncopyable
	{
	public:
		explicit cacceptor() ,m_port(0){}
		virtual ~cacceptor() {}
		bool 	init(const char * ip, uint32& port);
		void 	destroy();
	public: 
		// IPv4
		bool			create(); 
		//bool 			open();
		bool 			bind();
		socket_type 	accpet();
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
}// chen

#endif // _C_ACCEPTOR_H_