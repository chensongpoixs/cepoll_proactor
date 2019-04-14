/***********************************************************************************************
	created: 		2019-03-06
	
	author:			chensong
					
	purpose:		net_mgr
************************************************************************************************/
#ifndef _C_NET_MGR_H_
#define _C_NET_MGR_H_
#include "cnoncopyable.h"
#include "creactor.h"
#include "cnet_types.h"
#include "cacceptor.h"
#include <iostream>
#include <vector>
#include "cnet_session.h"
#include <atomic>
#include "cqueue.h"
namespace chen {
	
	typedef struct cmsg_data
	{
			uint32 	m_client;
			uint32 	m_msg_id;
			void 	*m_ptr;
			uint64 	m_size;
			cmsg_data():m_client(0) , m_msg_id(0), m_ptr(NULL),m_size(0){}
	}cmsg_data;
	class cnet_mgr :public cnoncopyable
	{
	public:
		//连接回调
		typedef std::function<void(uint32 session_id, uint32 para, const char* buf)> cconnect_cb;
		//断开连接回调
		typedef std::function<void(uint32 session_id)> 								cdisconnect_cb;
		//新消息回调
		typedef std::function<void(uint32 session_id, const void* p, uint32 size)> cmsg_cb;
	private:
		typedef std::vector<cnet_session*> 											csessions;
		typedef std::vector<std::thread> 											cthreads;
		typedef std::atomic_bool 													catomic_bool;
	public:
		explicit	cnet_mgr():m_connect_callback(NULL), m_disconnect_callback(NULL), m_msg_callback(NULL), m_shuting(false) {}
		virtual		~cnet_mgr() {}
		
		bool 		init();
		void 		destroy();
	public:
		bool 		startup(const char *ip, uint16 port);
	public:
		//连接回调
		void set_connect_callback(cconnect_cb callback) { m_connect_callback = callback; }

		//断开连接回调
		void set_disconnect_callback(cdisconnect_cb callback) { m_disconnect_callback = callback; }

		//消息回调
		void set_msg_callback(cmsg_cb callback) { m_msg_callback = callback; }
	public:
		void 		process_msg();
		bool 		send_msg(uint32 sessionId, const void* msg_ptr, uint32 msg_size);
		void 		shutdown();
		void 		close(uint32 sessionId);
	private:
		void 		_work_thread();
		void		_clearup_session(cnet_session * psession);
		void		_new_connect();
	private: 	
		cacceptor*					m_acceptor_ptr;		// 监听socket
		creactor*					m_reactor;
		// callback			
		cconnect_cb					m_connect_callback;
		cdisconnect_cb				m_disconnect_callback;
		cmsg_cb						m_msg_callback;
		csessions					m_sessions;
		catomic_bool				m_shuting;
		cthreads 					m_threads;
		cqueue<cmsg_data>			m_msgs;
	};
} // chen 

#endif // _C_NET_MGR_H_