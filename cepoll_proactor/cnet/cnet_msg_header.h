/***********************************************************************************************
				created: 		2019-05-15

				author:			chensong

				purpose:		net_msg_header
************************************************************************************************/


#ifndef _C_NET_MSG_HEADER_H_
#define _C_NET_MSG_HEADER_H_
#include "cnet_types.h"
namespace chen {
	/**
	** 信息包头信息
	**/
	struct cnet_msg_header
	{
		int32	m_msg_id;
		int32	m_size;
		int32	m_extra;
		cnet_msg_header() 
			:m_msg_id(0)
			, m_size(0)
			, m_extra(0) 
		{}
	};
	//#pragma pack(push,1)
	//	struct cmsg_header
	//	{
	//		uint16 msg_id;
	//		uint32 msg_size;
	//	};
	//
	//#pragma pack(pop)

	static const uint32 MSG_ID_SIZE = sizeof(uint16);
	static const uint32 MSG_HEADER_SIZE = sizeof(cnet_msg_header);

	inline uint32 get_session_index(uint32 id) { return id & 0x0000FFFF; }

	inline uint32 get_session_serial(uint32 id) { return (id & 0xFFFF0000) >> 16; }
}//namespace chen

#endif // !#define _C_NET_MSG_HEADER_H_