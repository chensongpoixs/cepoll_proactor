/***********************************************************************************************
	created: 		2019-03-02
	
	author:			chensong
					
	purpose:		log
************************************************************************************************/


#ifndef _C_LOG_H_
#define _C_LOG_H_
#include "cnet_type.h"
#include <cstdio>
#include <cstdlib>
namespace chen {
	
	
#define NORMAL_LOG(format, ...)		printf(format, ##__VA_ARGS__)
#define ERROR_LOG(format, ...)		printf(format, ##__VA_ARGS__)
#define WARNING_LOG(format, ...)	printf(format, ##__VA_ARGS__)
#define SYSTEM_LOG(format, ...)		printf(format, ##__VA_ARGS__)
#define DEBUG_LOG(format, ...)		printf(format, ##__VA_ARGS__)


#define ERROR_EX_LOG(format, ...)	ERROR_LOG("[%s][%d][ERROR]" format, FUNCTION, __LINE__, ##__VA_ARGS__)
#define WARNING_EX_LOG(format, ...)	ERROR_LOG("[%s][%d][WARNING]" format, FUNCTION, __LINE__, ##__VA_ARGS__)

} // chen

#endif // _C_LOG_H_