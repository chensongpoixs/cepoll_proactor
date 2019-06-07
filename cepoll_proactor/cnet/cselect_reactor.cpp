/***********************************************************************************************
			created: 		2019-04-06

			author:			chensong

			purpose:		select_reactor
************************************************************************************************/

#include "cselect_reactor.h"
#include "clog.h"
#include "csocket_ops.h"
#if defined(_MSC_VER)
namespace chen {
	bool cselect_reactor::init(socket_type listenfds, uint32 active_num)
	{
		if (active_num == 0 || active_num  > 0xFFFF)
		{
			ERROR_EX_LOG(" max fd you not active_num =%lu", active_num);
			return false;
		}
		
		//将set集合清空
		FD_ZERO(&m_readfds);
		FD_ZERO(&m_writefds);
		FD_ZERO(&m_excefds);

		m_paras = new csocket_exter_para[active_num];
		if (!m_paras)
		{
			ERROR_EX_LOG(" alloc exter para active_num = %u", active_num);
			return false;
		}

		m_active_paras = new csocket_active_para[active_num];
		if (!m_active_paras)
		{
			ERROR_EX_LOG(" alloc active para maxnum = %lu", active_num);
			return false;
		}

		/*FD_SET(listenfds, &m_readfds);
		FD_SET(listenfds, &m_writefds);
		FD_SET(listenfds, &m_writefds);*/
		m_curfd_count = 0;
		m_maxfd_count = active_num;
		m_maxfd = listenfds;
		return true;
	}
	void cselect_reactor::destroy()
	{

		FD_ZERO(&m_readfds);
		FD_ZERO(&m_writefds);
		FD_ZERO(&m_excefds);

		if (m_paras)
		{
			delete[] m_paras;
			
		}
		if (m_active_paras)
		{
			delete[] m_active_paras;
		}
		m_maxfd_count = 0;
		m_maxfd = 0;
	}
	void * cselect_reactor::get_para(uint32 index)
	{
		return m_active_paras[index].ptr;
	}
	bool cselect_reactor::is_read(uint32 index) const
	{
		return  ( 0 != (m_active_paras[index].events & E_READFDS) );
	}
	bool cselect_reactor::is_write(uint32 index) const
	{
		return (0 != (m_active_paras[index].events & E_WRITFDS));
	}
	bool cselect_reactor::is_exception(uint32 index) const
	{
		return (0 != (m_active_paras[index].events & E_EXCEFDS));
	}
	int32 cselect_reactor::_register_descriptor(socket_type& descriptor, uint32 st, void * session)
	{
		//需要修改 m_maxfd 的值
		if (descriptor == INVALID_SOCKET)
		{
			ERROR_EX_LOG("descriptor is invalid");
			return -1;
		}

		if (m_curfd_count >= m_maxfd_count)
		{
			ERROR_EX_LOG("reach max fd count %lu", m_maxfd_count);
			return -1;
		}
		if (( st& E_READFDS) != 0)
		{
			if (FD_ISSET(descriptor, &m_readfds))
			{
				ERROR_EX_LOG("socket has been set");
				return -1;
			}
			FD_SET(descriptor, &m_readfds);
		}

		FD_SET(descriptor, &m_writefds);
		FD_SET(descriptor, &m_excefds);
		m_maxfd = ((m_maxfd == INVALID_SOCKET) ? descriptor : max(descriptor, m_maxfd));

		m_paras[m_curfd_count].socket = descriptor;
		m_paras[m_curfd_count].ptr = session;

		++m_curfd_count;
		return 0;
	}
	void cselect_reactor::deregister_descriptor(socket_type& descriptor)
	{
		//需要修改 m_maxfd 的值
		if (descriptor == INVALID_SOCKET)
		{
			ERROR_EX_LOG("ERROR NetMgr::deregister_descriptor descriptor (socket_type=%d)", descriptor);
			return;
		}

		if (m_curfd_count <= 0)
		{
			ERROR_EX_LOG("m_curfd_count < 0 when deregister_descriptor descriptor (socket_type=%d)", descriptor);
			return;
		}


		if (!FD_ISSET(descriptor, &m_readfds))
		{
			ERROR_EX_LOG("ERROR NetMgr::deregister_descriptor(socket_type=%d)", descriptor);
			return;
		}

		FD_CLR(descriptor, &m_readfds);
		FD_CLR(descriptor, &m_writefds);
		FD_CLR(descriptor, &m_excefds);

		//寻找最大socked
		m_maxfd = INVALID_SOCKET;
		int32 descriptor_index = -1;
		for (int32 i = 0; i < m_curfd_count; ++i)
		{
			//寻找fd
			if (m_paras[i].socket == descriptor)
			{
				descriptor_index = i;
			}
			else if (m_maxfd == INVALID_SOCKET || m_paras[i].socket > m_maxfd)
			{
				//寻找最大socket
				m_maxfd = m_paras[i].socket;
			}
		}
		m_curfd_count--;

		if (descriptor_index == -1)
		{
			ERROR_EX_LOG("socket not found");
			return;
		}
		//把最后一个socket移动到删除的那个socket位置
		if (descriptor_index != m_curfd_count)
		{
			m_paras[descriptor_index] = m_paras[m_curfd_count];
			m_paras[m_curfd_count].reset();
		}
	}
	/*void cselect_proactor::shutdown()
	{
	}*/
	int32 cselect_reactor::select(uint32 ms)
	{
		if (m_curfd_count == 0)
		{
			return 0;
		}
		struct timeval time_out;
		time_out.tv_sec = 0;
		time_out.tv_usec = ms * 1000;
		
		fd_set  readfds = m_readfds;
		fd_set  writefds = m_writefds;
		fd_set	excefds = m_excefds;
		int32	num_event = 0;
		//SYSTEM_LOG("numevent = start");
		// 后期优化 write 文件集合 使用 IO使用两个线程  select 使用一个线程 阻塞在select上和epoll_wait 还有一个线程专门读取和写入的操作
		int32 numevent = csocket_ops::select(m_maxfd + 1, &readfds,  &writefds, &excefds, (const timeval *)&time_out);
		//SYSTEM_LOG("numevent  = %ld - end", numevent);
		if (numevent > 0)
		{
			//ERROR_EX_LOG("numevent  = %ld , m_curfd_count = %ld", numevent, m_curfd_count);
			int32 events;
			for (int32 i = 0; i < m_curfd_count; ++i)
			{
				events = 0;
				/*if (m_active_paras[i].events == E_NONE)
				{
					continue;
				}*/
				if (/*m_active_paras[i].events & E_READFDS &&*/0 != FD_ISSET(m_paras[i].socket, &readfds))
				{
					//ERROR_EX_LOG(" read m_curfd_count = %ld,  sockfd =%lu", m_curfd_count, m_paras[i].socket);
					events |= E_READFDS;
				}
				if (/*m_active_paras[i].events & E_WRITFDS &&*/0 != FD_ISSET(m_paras[i].socket, &writefds))
				{
					//ERROR_EX_LOG(" write m_curfd_count = %ld,  sockfd =%lu", m_curfd_count, m_paras[i].socket);
					events |= E_WRITFDS;
				}

				if (/*m_active_paras[i].events & E_EXCEFDS &&*/0 != FD_ISSET(m_paras[i].socket, &excefds))
				{
					//ERROR_EX_LOG(" exec m_curfd_count = %ld,  sockfd =%lu", m_curfd_count, m_paras[i].socket);
					events |= E_EXCEFDS;
				}
				/*m_active_paras[i].socket = i;
				m_active_paras[i].events = events;*/
				if (events != 0)
				{
					m_active_paras[num_event].events = events;
					m_active_paras[num_event].ptr = m_paras[i].ptr;
					++num_event;
				}
			}
		}
		return num_event;
	}

} // namespace chen 

#endif // #if defined(_MSC_VER)