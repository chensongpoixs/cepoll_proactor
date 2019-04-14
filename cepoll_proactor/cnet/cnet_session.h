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
	private:
		enum ESESSIONSTATUS
		{
			ESS_NONE = 0,
			ESS_INIT,
			ESS_OPEN,
			ESS_CLOSE,
		};
	public:
		explicit cnet_session():m_sockfd(0), m_event(0) {}
		virtual ~cnet_session() {}
	public:
		bool 	init(socket_type  sockfd);
		void 	destroy();
	public:
		bool	is_init() const { return m_status == ESS_INIT; }
		bool	is_open() const { return m_status == ESS_OPEN; }
		bool	is_close() const { return m_status == ESS_CLOSE; }
	public:
		void	clearup();
		void    set_event(int32 event);
		socket_type get_sockfd() { return m_sockfd; };
	private:
		uint32			m_status;
		socket_type		m_sockfd;
		int32 		 	m_event;
		std::mutex 		m_mutex;
	};
	
}  //namespace chen

#endif // !_C_NET_SESSION_H_