#pragma once

#include <string>

#include "baseType.h"

#define PROTOMSG_HEAD_FLAG_LENGTH 4
#define PROTOMSG_HEAD_FLAG_INFO "FLAG"

namespace core
{
    #pragma pack(1)
    struct MessageHeader
    {
        /// @brief MSG头：FLAG
        char msg_flag_[PROTOMSG_HEAD_FLAG_LENGTH];

        /// @brief 消息版本号。当前默认为1，预留
        uint16 version_;
        // @brief data数据是否压缩，0：未压缩  1：压缩  其他为数据异常
        uint8 is_compress_;
        // @brief data数据是否加密，0：未加密  1：加密  其他为数据异常
        uint8 is_encryption_;
        
        /// @brief 消息ID
        MessageID message_id_;
        /// @brief 时间戳
        uint32 timestamp_;
        // @brief 返回值
        uint32 ret_code_;

        
    public:
        MessageHeader();
    };
    #pragma pack()


    /**
    *	@brief 消息包
    *  @details
    *      |----------------------------------------------------------------------------------------------------------------------
    *      |messageLen(4 byte) | messageHeaderLen(1 byte) | headerData | header_crc32(4 byte) | messageData | data_crc32(4 byte) |
    *      |----------------------------------------------------------------------------------------------------------------------
    *
    */
    struct Request
    {
        MessageHeader header_;
        
        /// @brief 消息序列化内容, 此空间生命周期由外部处理
        const char *body_data_buf_;
        //消息序列化长度
        MessageLen body_data_len_;

        
    public:
        Request();
        virtual ~Request();

        virtual bool encodeHeader(const MessageHeader& header, char *headerBuf, uint32 headerBufLen);
        virtual bool decodeHeader(const char *headerBuf, uint32 headerBufLen, MessageHeader& header);

        virtual int32 getRequestNeedBufLen();
        virtual bool encodeRequest(char *messageBuf, uint32 messageBufLen);
        virtual bool decodeRequest(const char *messageBuf, uint32 messageBufLen);
    };


	
	
	
}








