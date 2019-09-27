@0x834a1af6e438a22c;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("ultradb::protocol::base");



struct RequestHeader
{
	sessionID @0 :UInt32;
	
}



struct ResponseHeader
{
	sessionID @0 :UInt32;
}




struct HeatbeatRequest
{
	const messageID :Int32 = 0x0F000001;

	header @0 :RequestHeader;
	
	timestamp @1 :UInt32;
	
}

struct HeatbeatResponse
{
	const messageID :Int32 = 0x0F000002;

	header @0 :ResponseHeader;

	
}










