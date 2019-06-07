/***********************************************************************************************
		created: 		2019-04-06

		author:			chensong

		purpose:		select_reactor
************************************************************************************************/


#ifndef _C_SELECT_REACTOR_H_
#define _C_SELECT_REACTOR_H_
#include "cnoncopyable.h"
#include "cnet_types.h"
#include "cnet_session.h"
#include <set>
#include <vector>
#if defined(_MSC_VER)
namespace chen {
	class cselect_reactor /*:private cnoncopyable*/
	{
	private:
		struct csocket_exter_para
		{
			socket_type		socket;
			void*			ptr;
			csocket_exter_para() : socket(INVALID_SOCKET), ptr(NULL) {}
			void reset()
			{
				socket = INVALID_SOCKET;
				ptr = NULL;
			}
		};
		struct csocket_active_para
		{
			uint32	events;
			void*	ptr;
		};

	public:
		explicit cselect_reactor() {}
		~cselect_reactor() {}
	public:
		
	public:
		bool init(socket_type listenfds,uint32 active_num);
		void destroy();
	public:
		void * get_para(uint32 index);
	public:
		bool	is_read(uint32 index) const ;
		bool	is_write(uint32 index)const;
		bool	is_exception(uint32 index) const;
		
	public:
		// register
		void	register_read_descriptor(socket_type& descriptor, void* para) { _register_descriptor(descriptor, E_READFDS, para); }
		void	register_write_descriptor(socket_type& descriptor, void* para) { _register_descriptor(descriptor, E_WRITFDS, para); }
		void	register_readwrite_descriptor(socket_type& descriptor, void* para) { _register_descriptor(descriptor, E_READFDS | E_WRITFDS, para); }
		// remove file
		void deregister_descriptor(socket_type& descriptor);
		/*void shutdown();*/
	public:
		int32 select(uint32 ms);
	private:
		int32 _register_descriptor(socket_type& descriptor, uint32 st, void * session);
	private:
		cselect_reactor(const cselect_reactor&);
		//cnoncopyable &operator =(cnoncopyable &&);
		cselect_reactor& operator=(const cselect_reactor&);
	private:
		socket_type 				m_maxfd;
		uint32						m_curfd_count;
		uint32						m_maxfd_count;
		fd_set						m_readfds; // read
		fd_set						m_writefds;
		fd_set						m_excefds;//exceptfds
		csocket_exter_para*			m_paras;
		csocket_active_para*		m_active_paras;
	};

} // namespace chen
#endif // #if defined(_MSC_VER)
#endif //!#define _C_SELECT_REACTOR_H_