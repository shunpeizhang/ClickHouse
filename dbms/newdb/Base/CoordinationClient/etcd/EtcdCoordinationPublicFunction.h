#pragma once

#include <string>

#include "CoordinationClient/baseType.h"

namespace coordination
{
	namespace etcd {

	
        /// @brief 得到child node范围的end key
		const std::string getChildNodeEndKey(const std::string& startKey);

        /// @brief 创建一个lease, 则出租人(也就是etcd server)选择一个ID
		bool leaseGrant(std::shared_ptr<::grpc::Channel> etcdChannel, int64 TTL, int64& ID);

        /// @brief 通过路径得到父节点
		bool getParentNode(const std::string& node, std::string& parentNode);

	}

}





