#include "ProtocolAccess.h"

#include <cassert>
#include <time.h>
#include <string.h>

#include <Common/memcpySmall.h>
#include <Common/crc32.h>

namespace ultradb
{
namespace protocol_access
{
namespace detail
{
//查找子字符串位置
bool findStringPos(const char *source, unsigned int source_size, const char *sub_str,
                   unsigned int sub_size, unsigned int start_pos, unsigned int &pos)
{
  assert(source && source_size > 0 && sub_str && sub_size > 0);
  if (source && source_size > 0 && sub_str && sub_size > 0 && ((start_pos + sub_size <= source_size)))
  {
    //开始索引位置、匹配到位置
    unsigned int index_ = start_pos;
    unsigned int match_pos_ = 0;
    char msg_ch_ = '\0', flag_ch_ = '\0';
    while (index_ < source_size && match_pos_ < sub_size)
    {
      msg_ch_ = source[index_];
      flag_ch_ = sub_str[match_pos_];
      if (msg_ch_ == flag_ch_)
      {
        match_pos_++;
        index_++;
        //如果已经全部匹配，则返回
        if (match_pos_ == sub_size)
        {
          pos = (index_ - sub_size);
          return true;
        }
        continue;
      }
      else
      {
        match_pos_ = 0;
        index_++;
      }
    }
  }
  return false;
}
} // namespace detail
///消息头部标识
const char MSG_FLAG[PROTOMSG_HEAD_FLAG_LENGTH] = {0x67, 0x38, 0x21, 0x13}; // 0x67382113; //1731731731
union {
  short number;
  char s;
} test;
bool testBigEndin()
{
  test.number = 0x0102;
  return (test.s == 0x01);
}
//判断平台是否小端
bool pf_little_flag = testBigEndin();
//消息体头标识

//创建一个ProtocolMessage
ProtocolMessage *allocProtoMessage(ProtocolMessageFlag flag,
                                   unsigned int original_size /*= 0*/, unsigned int body_size /*= 0*/)
{
  unsigned int total_space_ = CALC_PROTOMSG_TOTAL_SPACE(body_size);
  char *buff_ = new (std::nothrow) char[total_space_];
  if (!buff_)
  {
    return nullptr;
  }

  memset(buff_, 0, total_space_);
  ProtocolMessage *msg_ = (ProtocolMessage *)buff_;
  memcpy(msg_->msg_flag, MSG_FLAG, PROTOMSG_HEAD_FLAG_LENGTH);
  //设置端序
  msg_->flag.little = pf_little_flag;
  msg_->flag.comp = flag.comp;
  msg_->flag.crypt = flag.crypt;

  msg_->ver = PROTOMSG_VER;
  msg_->size = total_space_;
  msg_->original_size = original_size;
  msg_->body_size = body_size;
  return msg_;
}
//通过空间初始化ProtocolMessage消息
ProtocolMessage *initProtoMessage(char *ptr, ProtocolMessageFlag flag,
                                  unsigned int original_size /*= 0*/, unsigned int body_size /*= 0*/)
{
  assert(ptr);
  if (!ptr)
  {
    return nullptr;
  }

  ProtocolMessage *msg_ = (ProtocolMessage *)ptr;
  memcpy(msg_->msg_flag, MSG_FLAG, PROTOMSG_HEAD_FLAG_LENGTH);
  //设置端序
  msg_->flag.little = pf_little_flag;
  msg_->flag.comp = flag.comp;
  msg_->flag.crypt = flag.crypt;
  msg_->ver = PROTOMSG_VER;
  msg_->size = CALC_PROTOMSG_TOTAL_SPACE(body_size);
  msg_->original_size = original_size;
  msg_->body_size = body_size;
  return msg_;
}
//释放ProtocolMessage空间
void freeProtoMessage(ProtocolMessage *&msg)
{
  assert(msg);
  if (!msg)
  {
    return;
  }

  char *buff_ = (char *)msg;
  delete[] buff_;
  msg = 0;
}
//检测消息头标识，是否等于: FY
bool checkMessageHeaderFlag(const char *msg)
{
  assert(msg);
  if (!msg)
  {
    return false;
  }
  //消息头：FY
  for (short i = 0; i < PROTOMSG_HEAD_FLAG_LENGTH; i++)
  {
    if (msg[i] != MSG_FLAG[i])
      return false;
  }
  return true;
}
//查找消息头标志位置
bool findMessageHeaderFlagPos(const char *msg, unsigned int size, unsigned int start_pos, unsigned int &pos)
{
  return detail::findStringPos(msg, size, MSG_FLAG, PROTOMSG_HEAD_FLAG_LENGTH, start_pos, pos);
}
//验证是否可以解析出命令
bool canParseMessageHeaderCmd(const char *msg, unsigned int size, unsigned int &cmd_id)
{
  assert(msg);
  if (!msg)
  {
    return false;
  }

  //cmd所在位置
  const int CMD_ID_POS_ = 10;
  if (checkMessageHeaderFlag(msg) && size >= CMD_ID_POS_)
  {
    ProtocolMessage *msg_ptr_ = (ProtocolMessage *)msg;
    cmd_id = msg_ptr_->cmd_id;
    return true;
  }
  return false;
}
//检测消息头标识，是否等于: FY
bool checkMessageHeaderFlag(ProtocolMessage *msg)
{
  assert(msg);
  if (!msg)
  {
    return false;
  }

  //消息头：FY
  for (short i = 0; i < PROTOMSG_HEAD_FLAG_LENGTH; i++)
  {
    if (msg->msg_flag[i] != MSG_FLAG[i])
      return false;
  }
  return true;
}
//设置消息头命令编号
void setMessageHeaderCmd(ProtocolMessage *msg, unsigned int cmd_id)
{
  assert(msg);
  if (!msg)
  {
    return;
  }

  msg->cmd_id = cmd_id;
}
//获取消息头部的命令编号
bool checkMessageHeaderCmd(ProtocolMessage *msg, unsigned int cmd_id)
{
  assert(msg);
  if (!msg)
  {
    return false;
  }

  return (msg->cmd_id == cmd_id);
}
//解析消息命令
bool parseMessageHeaderCmd(ProtocolMessage *msg, unsigned int &cmd_id)
{
  assert(msg);
  if (!msg)
  {
    return false;
  }

  cmd_id = msg->cmd_id;
  return true;
}
//设置消息头时间戳
void setMessageHeaderTimestamp(ProtocolMessage *msg)
{
  assert(msg);
  if (!msg)
  {
    return;
  }

  time_t t_ = time(NULL);
  msg->timestamp = t_;
}
//解析消息头时间戳
bool parseMessageHeaderTimestamp(ProtocolMessage *msg, unsigned int &timestamp)
{
  assert(msg);
  if (!msg)
  {
    return false;
  }

  timestamp = msg->timestamp;
  return true;
}
//设置报文返回值
void setMessageRetval(ProtocolMessage *msg, unsigned int retval)
{
  assert(msg);
  if (!msg)
  {
    return;
  }

  msg->ret_val = retval;
}
//获取报文的返回值
bool parseMessageRetval(ProtocolMessage *msg, unsigned int &retval)
{
  assert(msg);
  if (!msg)
  {
    return false;
  }

  retval = msg->ret_val;
  return true;
}
//设置消息头部crc32校验值
void setMessageHeaderCrc32(ProtocolMessage *msg)
{
  assert(msg);
  if (!msg)
  {
    return;
  }

  unsigned int header_size_ = PROTOMSG_RAW_HEADER_SIZE();
  unsigned int val_ = crc32::calc_crc32((char *)msg, header_size_);
  msg->header_crc32 = val_;
}
//检测消息头部crc32校验值
bool checkMessageHeaderCrc32(ProtocolMessage *msg)
{
  assert(msg);
  if (!msg)
  {
    return false;
  }
  unsigned int header_size_ = PROTOMSG_RAW_HEADER_SIZE();
  unsigned int val_ = crc32::calc_crc32((char *)msg, header_size_);
  return (val_ == msg->header_crc32);
}
//检测tcp流中的数据是否已经完成
bool checkMessageUncomplate(ProtocolMessage *msg, unsigned int size)
{
  assert(msg);
  assert(size > 0);
  if (!msg || size == 0)
  {
    return false;
  }

  unsigned int total_size = PROTOMSG_TOTAL_LENGTH(msg);
  //接收到报文总长度小于消息头总长度
  return size < total_size;
}
//验证消息全部长度是否与指定长度相等
bool checkMessageTotalSize(ProtocolMessage *msg, unsigned int size)
{
  assert(msg);
  assert(size > 0);
  if (!msg || size == 0)
  {
    return false;
  }
  unsigned int total_size = PROTOMSG_TOTAL_LENGTH(msg);
  //验证报文总长度与消息头总长度字段相等
  return ((size == CALC_PROTOMSG_TOTAL_SPACE(msg->body_size)) && (size == total_size));
}
//设置报文体数据
void setMessageBody(ProtocolMessage *msg, const char *buffer, unsigned int buffer_size)
{
  assert(msg);
  assert(buffer);
  assert(buffer_size == msg->body_size);
  if (!msg || (msg->body_size != buffer_size))
  {
    return;
  }
  if (!buffer)
  {
    return;
  }

  memcpySmallAllowReadWriteOverflow15(msg->body, buffer, buffer_size);
}
//设置报文体数据
void setMessageBody(ProtocolMessage *msg, const CharArray &buffer)
{
  assert(msg);
  assert(!buffer.empty());
  assert(buffer.size() == msg->body_size);
  if (!msg || (msg->body_size != buffer.size()))
  {
    return;
  }
  if (buffer.empty())
  {
    return;
  }

  memcpySmallAllowReadWriteOverflow15(msg->body, buffer.data(), buffer.size());
}
//验证报文体是否包含体数据
bool checkMessageHaveBody(ProtocolMessage *msg)
{
  assert(msg);
  return (msg != nullptr) ? (msg->body_size > 0) : false;
}
//获取报文体数据
bool parseMessageBody(ProtocolMessage *msg, CharArray &buffer)
{
  assert(msg);
  assert(msg->body_size > 0);
  if (!msg || msg->body_size == 0)
  {
    return false;
  }
  //获取端序
  if (!msg->flag.little)
  {
    return false;
  }
  //当前仅处理非压缩、非加密情况
  if (msg->flag.comp || msg->flag.crypt)
  {
    return false;
  }

  buffer.resize(msg->body_size);
  memcpySmallAllowReadWriteOverflow15((char *)buffer.data(), msg->body, msg->body_size);
  return true;
}
//设置报文体的crc32校验值
void setMessageBodyCrc32(ProtocolMessage *msg)
{
  assert(msg);
  if (!msg)
  {
    return;
  }
  unsigned int crc32_val_ = crc32::calc_crc32(msg->body, msg->body_size);
  memcpySmallAllowReadWriteOverflow15(&msg->body[msg->body_size], &crc32_val_, sizeof(unsigned int));
}
//校对报文体的crc32
bool checkMessageBodyCrc32(ProtocolMessage *msg)
{
  assert(msg);
  if (!msg)
  {
    return false;
  }

  unsigned int crc32_val_ = crc32::calc_crc32(msg->body, msg->body_size);
  unsigned int *old_crc32_ = (unsigned int *)&msg->body[msg->body_size];
  return (*old_crc32_ == crc32_val_);
}
} // namespace protocol_access
} // namespace ultradb