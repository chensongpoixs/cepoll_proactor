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
		void shutdown();
		int32 register_descriptor(socket_type descriptor, cdescriptor_state_queue *& descriptor_state);
		void deregister_descriptor(socket_type descriptor, cdescriptor_state_queue *& descriptor_state, bool closing = false);
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
		std::mutex m_mutex_;

		// The timer file descriptor.
		//socket_type m_timer_fd_;
		// Mutex to protect access to the registered descriptors.
		std::mutex m_registered_descriptors_mutex_;

  // Keep track of all registered descriptors.
		//cobject_pool<cdescriptor_state_queue> m_registered_descriptors_;
		 // Whether the service has been shut down.
		bool 		m_shutdown_;
	}
} // chen

#endif // _C_EPOLL_REACTOR_H_
