#pragma once


#include "DataTypeDef.h"


namespace ultradb
{
  //平台大小端标志：是否小端
  extern bool pf_little_flag;
  //报文头标识字符串数量
  #define PROTOMSG_HEAD_FLAG_LENGTH 4
  //报文头数据标记长度
  #define PROTOMSG_DATA_FLAG_LENGTH 4

  const unsigned short PROTOMSG_VER = 1;

  #pragma pack(1)
  //报文标志
  struct ProtocolMessageFlag {
    char little;
    char comp;
    char crypt;
    char reserve1;
  };
  #pragma pack()

  #pragma pack(1)
  //发送报文头结构（当前报文头36字节）
  struct ProtocolMessage
  {
    //MSG头：FLAG
    char msg_flag[PROTOMSG_HEAD_FLAG_LENGTH];
    //数据报文长度
    unsigned int size;
    //标志位：{大小端标记；压缩标记；加密标识，其他预留}{当前仅实现了小端序}
    ProtocolMessageFlag flag;
    //数据报文版本。当前默认为1，预留
    unsigned short ver;
    //命令编号
    unsigned int cmd_id;
    //时间戳
    unsigned int timestamp;
    //返回值
    unsigned int ret_val;
    //原始数据块长度
    unsigned int original_size;
    //当前body的数据块长度，如果flag不为0
    unsigned int body_size;
    //数据报头crc32位校验码
    unsigned int header_crc32;
    //数据体
    char body[0];
    ////数据体crc32位校验码（此字段位于body[body_size]处开始的4个字节，如果body_size为0，则此字段无数据）
    //unsigned int body_crc32;
  };
  #pragma pack()

  //#报文头原始长度（不包含头部校验字节）
  #define PROTOMSG_RAW_HEADER_SIZE() (sizeof(char) * (PROTOMSG_HEAD_FLAG_LENGTH) + sizeof(unsigned int) + sizeof(char) * sizeof(ProtocolMessageFlag) + sizeof(unsigned short) + sizeof(unsigned int) * 5)
  //报文体头部长度（包含头部校验字节）
  #define PROTOMSG_HEADER_SIZE() (PROTOMSG_RAW_HEADER_SIZE() + sizeof(unsigned int))
  //报文体数据体及尾部长度(x=body_size)
  #define PROTOMSG_BODY_TAIL_SIZE(m) (m->body_size + ((m->body_size > 0) ? sizeof(int) : 0))
  //报文体总长度（x=body_size)
  #define PROTOMSG_TOTAL_LENGTH(m) (PROTOMSG_HEADER_SIZE() + PROTOMSG_BODY_TAIL_SIZE(m))
  //计算消息体总空间长度。如果body_size为0，则body_crc32字段无数据
  #define CALC_PROTOMSG_TOTAL_SPACE(size) (PROTOMSG_HEADER_SIZE() + size + ((size > 0) ? sizeof(int) : 0))
}