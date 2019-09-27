#pragma once

#include <string>

#include "CoordinationClient/baseType.h"

namespace coordination
{
	namespace etcd {

	
        /// @brief �õ�child node��Χ��end key
		const std::string getChildNodeEndKey(const std::string& startKey);

        /// @brief ����һ��lease, �������(Ҳ����etcd server)ѡ��һ��ID
		bool leaseGrant(std::shared_ptr<::grpc::Channel> etcdChannel, int64 TTL, int64& ID);

        /// @brief ͨ��·���õ����ڵ�
		bool getParentNode(const std::string& node, std::string& parentNode);

	}

}





