@0x821a648c14ac0fee;

struct ConnectAndComputeDataReq
{
    script @0 : Text;
    block @1 : Data;
}

# Connect Node and Compute Node result data
struct ConnectAndComputeDataResp
{
    result @0 : UInt32;
    block @1 : Data;
}




