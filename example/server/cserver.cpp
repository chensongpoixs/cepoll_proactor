/***********************************************************************************************
	created: 		2019-03-07
	
	author:			chensong
					
	purpose:		server
************************************************************************************************/
#include "cserver.h"


namespace chen {
	
	cserver g_server;
	bool cserver::init(uint32 port)
	{
		m_net_mgr = new cnet_mgr;
		if (!m_net_mgr)
		{
			return false;
		}
		if (!m_net_mgr->init())
		{
			return false;
		}
		m_net_mgr->set_connect_callback();// ??
		m_net_mgr->set_disconnect_callback(); //??
		m_net_mgr->set_msg_callback();//??
		if (!m_net_mgr->startup("0.0.0.0", port);
		return true;
	}
	
	
	bool cserver::Loop()
	{
		return true;
	}
	void 	cserver::set_stoping()
	{
		
	}
	bool cserver::destroy()
	{
		
		if (m_net_mgr)
		{
			m_net_mgr->shutdown();
		}
		return true;
	}
}