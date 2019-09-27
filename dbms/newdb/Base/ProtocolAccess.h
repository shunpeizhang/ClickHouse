#pragma once
#include <vector>
using std::vector;

#include "ProtocolMessage.h"

namespace ultradb
{
namespace protocol_access
{
using CharArray = std::vector<char>;

//创建一个ProtocolMessage
ProtocolMessage *allocProtoMessage(ProtocolMessageFlag flag, unsigned int original_size = 0, unsigned int body_size = 0);
//通过空间初始化ProtocolMessage消息
ProtocolMessage *initProtoMessage(char *ptr, ProtocolMessageFlag flag, unsigned int original_size = 0, unsigned int body_size = 0);
//释放ProtocolMessage空间
void freeProtoMessage(ProtocolMessage *&msg);
//检测消息头标识，是否等于: FY
bool checkMessageHeaderFlag(const char *msg);
//查找消息头标志位置
bool findMessageHeaderFlagPos(const char *msg, unsigned int size, unsigned int start_pos, unsigned int &pos);
//验证是否可以解析出命令
bool canParseMessageHeaderCmd(const char *msg, unsigned int size, unsigned int &cmd_id);
//检测消息头标识，是否等于: FY
bool checkMessageHeaderFlag(ProtocolMessage *msg);
//设置消息头命令编号
void setMessageHeaderCmd(ProtocolMessage *msg, unsigned int cmd_id);
//获取消息头部的命令编号
bool checkMessageHeaderCmd(ProtocolMessage *msg, unsigned int cmd_id);
//解析消息命令
bool parseMessageHeaderCmd(ProtocolMessage *msg, unsigned int &cmd_id);
//设置消息头时间戳
void setMessageHeaderTimestamp(ProtocolMessage *msg);
//解析消息头时间戳
bool parseMessageHeaderTimestamp(ProtocolMessage *msg, unsigned int &timestamp);
//设置报文返回值
void setMessageRetval(ProtocolMessage *msg, unsigned int retval);
//获取报文的返回值
bool parseMessageRetval(ProtocolMessage *msg, unsigned int &retval);
//设置消息头部crc32校验值
void setMessageHeaderCrc32(ProtocolMessage *msg);
//检测消息头部crc32校验值
bool checkMessageHeaderCrc32(ProtocolMessage *msg);
//检测tcp流中的数据是否已经完成
bool checkMessageUncomplate(ProtocolMessage *msg, unsigned int size);
//验证消息全部长度是否与指定长度相等
bool checkMessageTotalSize(ProtocolMessage *msg, unsigned int size);
//设置报文体数据
void setMessageBody(ProtocolMessage *msg, const char *buff, unsigned int buff_size);
//设置报文体数据
void setMessageBody(ProtocolMessage *msg, const CharArray &buffer);
//验证报文体是否包含体数据
bool checkMessageHaveBody(ProtocolMessage *msg);
//获取报文体数据
bool parseMessageBody(ProtocolMessage *msg, CharArray &buffer);
//设置报文体的crc32校验值
void setMessageBodyCrc32(ProtocolMessage *msg);
//校对报文体的crc32
bool checkMessageBodyCrc32(ProtocolMessage *msg);
} // namespace protocol_access
} // namespace ultradb