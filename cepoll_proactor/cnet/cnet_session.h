/***********************************************************************************************
	created: 		2019-03-01
	
	author:			chensong
					
	purpose:		net_session
************************************************************************************************/

#ifndef _C_NET_SESSION_H_
#define _C_NET_SESSION_H_
#include "cnoncopyable.h"
#include "cnet_types.h"
#include <mutex>
namespace chen  {
	class cnet_session :public cnoncopyable 
	{
	public:
		explicit cnet_session():m_sockfd(0), m_event(0) {}
		virtual ~cnet_session() {}
	public:
		bool 	init(socket_type  sockfd);
		void 	destroy();
	public:
		void    set_event(int32 event);
		socket_type get_sockfd() { return m_sockfd; };
	private:
		socket_type		m_sockfd;
		int32 		 	m_event;
		std::mutex 		m_mutex;
	};
	
}  //namespace chen

#endif // _C_NET_SESSION_H_