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
#include "cnet_types.h"
#include <vector>



#if defined(__GNUC__)
#include <sys/epoll.h>
# include <sys/timerfd.h>
namespace chen 
{
	
	
	class cepoll_reactor :public cnoncopyable 
	{
	public:
	    explicit	cepoll_reactor(): m_epoll_fd_(-1), m_curfd_count(0), m_maxfd_count(0), m_active(NULL), m_stoped(false){}
		virtual ~cepoll_reactor() {}
		
		bool init(socket_type listenfds, uint32 active_num);
		void destroy();
		
	public:
		
		int32 select(uint32 ms);
	public:

		void * get_para(int64 index) {return m_active[index].data.ptr;}

	public:
		bool	is_read(uint32 index) const;
		bool	is_write(uint32 index)const;
		bool	is_exception(uint32 index) const;
	public:

		/*	bool 	descriptor_read_state(int64 index) {return m_active[index].event | EPOLLIN;}

			int32 register_descriptor(socket_type descriptor, cnet_session *& session);
			void deregister_descriptor(socket_type descriptor);
			socket_type		get_event_descriptor(int32 index) {return m_active[index].data->fd;}*/
			// register
		void	register_read_descriptor(socket_type& descriptor, void* para) { _register_descriptor(descriptor, EPOLLIN, para); }
		void	register_write_descriptor(socket_type &descriptor, void* para) { _register_descriptor(descriptor, EPOLLOUT, para); }
		void	register_readwrite_descriptor(socket_type& descriptor, void* para) { _register_descriptor(descriptor, EPOLLIN | EPOLLOUT, para); }

		void deregister_descriptor(socket_type &descriptor);
	private:
		int32 _register_descriptor(socket_type &descriptor, uint32 st, void * session);
	private:
		// The epoll file descriptor.
		socket_type					m_epoll_fd_;
//		socket_type 				m_maxfd;
		uint32						m_curfd_count;
		uint32						m_maxfd_count;
		epoll_event* 				m_active;
		bool 						m_stoped;
		
	};
} // namespace chen
#endif //!#if defined(__GNUC__)

#endif // !#define _C_EPOLL_REACTOR_H_
