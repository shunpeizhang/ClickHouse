#pragma once




#include "baseType.h"

namespace core
{

	/// @brief 消息对应处理的包装类
	class IBaseEntry;

    /// @brief 此类只用于框架中，保存框架内全局信息; 业务中Context各有差异，让上层自己处理
	struct Context
	{
		std::map<MessageID, std::shared_ptr<IBaseEntry>> entrys_;
	};
	
	

	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
}







