#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>

#include "Common/char_pool.hpp"


namespace core
{

/// @brief 网络消息最小长度
#define MIN_NET_MESSAGE_LEN 10
/// @brief 网络消息最大长度
#define MAX_NET_MESSAGE_LEN (1024 * 1024 * 10)
	
	
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

    /// @brief 连接fd类型
	typedef uint16 SOCKET;
	
	
    /// @brief 消息长度类型
	typedef uint32 MessageLen;
    /// @brief 消息id类型
	typedef uint32 MessageID;
    /// @brief 连接唯一标识类型
	typedef uint32 SocketChannelID;
    /// @brief 服务实例唯一标识类型
	typedef uint32 ServerUniqueID;
	
    
    /// @brief 内存池，内存空间复用
	typedef char_pool_t<MIN_NET_MESSAGE_LEN, MAX_NET_MESSAGE_LEN> SharedMemoryPool;
	typedef std::shared_ptr<SharedMemoryPool>  SharedMemoryPoolPtr;
	
	
    /// @brief 服务节点类型
	enum class ServerNodeType
	{
		INVALID = 0,
    	/// 连接节点
		LINK_NODE,
    	/// 计算节点
		CALCUTE_NODE,
    	/// 数据节点
		DATA_NODE,
    	/// 协调节点
		COORDINATON_NODE,
    	/// 索引节点
		INDEX_NODE,
    	/// 集群管理器节点
		MANAGE_NODE,		
	};
	
	
	
}





