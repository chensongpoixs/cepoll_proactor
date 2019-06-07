/***********************************************************************************************
			created: 		2019-05-19

			author:			chensong

			purpose:		net_msg
************************************************************************************************/
#ifndef _C_NET_MSG_H_
#define _C_NET_MSG_H_
#include "cnoncopyable.h"
#include "cnet_types.h"
namespace chen {
	class cnet_msg : private cnoncopyable
	{
	public:
	    explicit	cnet_msg();
		~cnet_msg();
	public:
		bool alloc_me(uint32 msg_size);
		void free_me();
	public:
		uint32 get_session_id() const { return m_session_id; }
		void set_session_id(uint32 value) { m_session_id = value; }
		
		uint32 get_status() const { return m_status; }
		void set_status(int32 value) { m_status = value; }
		uint16 get_msg_id() const { return m_msg_id; }
		void set_msg_id(uint16 value) { m_msg_id = value; }


		uint32 get_size() const { return m_size; }
		const char* get_buf() const { return m_buffer; }
		char* get_buf() { return m_buffer; }
	private:
		uint32				m_session_id;
		uint32				m_status;
		uint16				m_msg_id;
		uint32				m_size;
		char*				m_buffer;
	};

}//namespace chen


#endif // !#define _C_NET_MSG_H_