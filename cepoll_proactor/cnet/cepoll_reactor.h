/***********************************************************************************************
	created: 		2019-02-24
	
	author:			chensong
					
	purpose:		epoll_reactor
	
	later tasks:	null
	
	   The struct epoll_event is defined as :

           typedef union epoll_data {
               void    *ptr;
               int      fd;
               uint32_t u32;
               uint64_t u64;
           } epoll_data_t;

           struct epoll_event {
               uint32_t     events;    // Epoll events 
               epoll_data_t data;      // User data variable 
           };
************************************************************************************************/
#ifndef _C_EPOLL_REACTOR_H_
#define _C_EPOLL_REACTOR_H_
#include "cnoncopyable.h"
#include "cnet_type.h"

namespace chen 
{
	
	
	class cepoll_reactor :public cnoncopyable 
	{
	public:
	    explicit	cepoll_reactor() {}
		virtual ~cepoll_reactor() {}
		
		bool init();
		void Destroy();
		
	public:
		
		int64 select(uint32 csleep);
		void * get_ptr(int64 index) {return m_active[i].ptr;}
		bool 	descriptor_read_state(int64 index) {return m_active[i].event | EPOLLIN;}
		//void shutdown();
		int32 register_descriptor(socket_type descriptor, cnet_session *& session);
		void deregister_descriptor(socket_type descriptor);
		socket_type		get_event_descriptor(int32 index) {return m_active[index].data.fd;}
	private:
		enum { epoll_size = 20000 };
		 // Create the epoll file descriptor. Throws an exception if the descriptor
		// cannot be created.
		static socket_type _do_epoll_create();
		
		// Create the timerfd file descriptor. Does not throw.
		//static int32 do_timerfd_create();
	
		//cdescriptor_state_queue* _allocate_descriptor_state()
	
	private:
		// The epoll file descriptor.
		socket_type m_epoll_fd_;
		
		// Mutex to protect access to internal data.
		//std::mutex m_mutex_;
		std::vector<epoll_event*> 			m_active;
		// The timer file descriptor.
		//socket_type m_timer_fd_;
		// Mutex to protect access to the registered descriptors.
	//	std::mutex m_registered_descriptors_mutex_;

  // Keep track of all registered descriptors.
		//cobject_pool<cdescriptor_state_queue> m_registered_descriptors_;
		 // Whether the service has been shut down.
		bool 		m_stoped;
	}
} // chen

#endif // _C_EPOLL_REACTOR_H_
