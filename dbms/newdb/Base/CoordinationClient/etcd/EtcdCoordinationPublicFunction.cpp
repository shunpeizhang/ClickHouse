#include <grpc/grpc.h>
#include <grpcpp/channel.h>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>

#include "etcdGrpc/rpc.grpc.pb.h"

#include "EtcdCoordinationPublicFunction.h"



const std::string coordination::etcd::getChildNodeEndKey(const std::string& startKey)
{
	std::string endKey(startKey);
	endKey[endKey.size() - 1] = (char)((int8)(endKey[endKey.size() - 1]) + 1);

	return endKey;
}


bool coordination::etcd::leaseGrant(std::shared_ptr<::grpc::Channel> etcdChannel, int64 TTL, int64& ID)
{
	std::unique_ptr<etcdserverpb::Lease::Stub> leaseStub = etcdserverpb::Lease::NewStub(etcdChannel);

	grpc::ClientContext context;
	etcdserverpb::LeaseGrantRequest request;
	request.set_ttl(TTL);
	request.set_id(ID);

	etcdserverpb::LeaseGrantResponse response;
	grpc::Status status = leaseStub->LeaseGrant(&context, request, &response);
	if (!status.ok())
	{
		printf("leaseStub->LeaseGrant failed!\n");
		return false; 
	}

	ID = response.id();

	return true;
}

bool coordination::etcd::getParentNode(const std::string& node, std::string& parentNode)
{
	auto pos = node.find("/");
	if (0 == pos)
	{
		return false;
	}

	parentNode = node.substr(0, pos);

	return true;
}






