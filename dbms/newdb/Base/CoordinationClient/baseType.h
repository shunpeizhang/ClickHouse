#pragma once

#include <string>
#include <memory>
#include <map>
#include <queue>
#include <mutex>
#include <vector>
#include <thread>



namespace coordination
{


	enum COORDINATION_EVENT_TYPE
	{
		CET_NodeCreate,
		CET_ValueChange,
		CET_NodeDELETE
	};



	typedef char int8;
	typedef short int16;
	typedef int int32;
	typedef long long int64;

	typedef unsigned char uint8;
	typedef unsigned short uint16;
	typedef unsigned int uint32;
	typedef unsigned long long uint64;

	typedef float float32;
	typedef double float64;


	typedef std::pair<std::string, std::string> KeyValue;




}







