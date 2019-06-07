/***********************************************************************************************
				created: 		2019-05-17

				author:			chensong

				purpose:		cnet_msg_reserve
************************************************************************************************/
#ifndef _C_NET_MSG_RESERVE_H_
#define _C_NET_MSG_RESERVE_H_
namespace chen {

	enum EMsgIDReserve
	{
		EMIR_Close = 0,
		EMIR_Connect, //建立连接
		EMIR_Disconnect,

		EMIR_Max,
	};


	/*enum ESOCKET_STATE
	{
		
		EMIR_Connect,
		EMIR_Disconnect

	};*/

}//namespace chen

#endif // !#define _C_NET_MSG_RESERVE_H_