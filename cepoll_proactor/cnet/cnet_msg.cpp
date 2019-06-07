/***********************************************************************************************
			created: 		2019-05-19

			author:			chensong

			purpose:		net_msg
************************************************************************************************/

#include "cnet_msg.h"


namespace chen {
	cnet_msg::cnet_msg()
		:m_session_id(0)
		,m_status(0)
		,m_msg_id(0)
		,m_size(0)
		,m_buffer(NULL)
	{
	}
	cnet_msg::~cnet_msg()
	{
	}
	bool cnet_msg::alloc_me(uint32 msg_size)
	{
		if (!m_buffer)
		{
			m_buffer = new char[msg_size];
			if (!m_buffer)
			{
				return false;
			}
		}
		m_size = msg_size;
		return true;
	}
	void cnet_msg::free_me()
	{
		if (m_buffer)
		{
			delete []m_buffer;
			m_buffer = NULL;
		}
	}
}//namespace chen