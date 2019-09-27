#include "request.h"

#include <arpa/inet.h>


#include "Common/logger_adapt.h"
#include "IServer.h"
#include "Common/crc32.h"


using namespace core;


static const uint32 MESSAGE_VERSION = 1;


void printHex(const char *buf, int32 len)
{
    printf("hex data %d:", len);
    for (int32 iPos = 0; len > iPos; ++iPos)
    {
        printf("%02x ", buf[iPos]);
    }
    printf("\n");
}



MessageHeader::MessageHeader()
{
    ret_code_ = -1;
    timestamp_ = time(NULL);
    message_id_ = 0;

    is_encryption_ = 0;
    is_compress_ = 0;
    version_ = MESSAGE_VERSION;

    const char *flagBuf = PROTOMSG_HEAD_FLAG_INFO;
    msg_flag_[0] = flagBuf[0];
    msg_flag_[1] = flagBuf[1];
    msg_flag_[2] = flagBuf[2];
    msg_flag_[3] = flagBuf[3];
}



Request::Request()
{
    body_data_buf_ = NULL;
    body_data_len_ = 0;
}

Request::~Request()
{
}


bool Request::encodeHeader(const MessageHeader& header, char *headerBuf, uint32 headerBufLen)
{
    uint32 needLen = sizeof(MessageHeader);
    if (headerBufLen < needLen)
    {
        LOG_ERROR(&g_serverInstance->logger(), "headerBufLen:%u < needLen:%u error!", headerBufLen, needLen);
        return false;
    }

    char *curBuf = headerBuf;

    //msg_flag_
    memcpy(curBuf, header.msg_flag_, sizeof(header.msg_flag_));
    curBuf += sizeof(header.msg_flag_);

    //version_
    uint16 version = htons(header.version_);
    memcpy(curBuf, (char*)&version, sizeof(version));
    curBuf += sizeof(version);

    //is_compress_
    uint8 is_compress = header.is_compress_;
    memcpy(curBuf, (char*)&is_compress, sizeof(is_compress));
    curBuf += sizeof(is_compress);

    //is_compress_
    uint8 is_encryption = header.is_encryption_;
    memcpy(curBuf, (char*)&is_encryption, sizeof(is_encryption));
    curBuf += sizeof(is_encryption);

    //message_id
    uint32 message_id = htonl(header.message_id_);
    memcpy(curBuf, (char*)&message_id, sizeof(message_id));
    curBuf += sizeof(message_id);

    //timestamp
    uint32 timestamp = htonl(header.timestamp_);
    memcpy(curBuf, (char*)&timestamp, sizeof(timestamp));
    curBuf += sizeof(timestamp);

    //ret_code
    uint32 ret_code = htonl(header.ret_code_);
    memcpy(curBuf, (char*)&ret_code, sizeof(ret_code));
    curBuf += sizeof(ret_code);

    if (curBuf - headerBuf != needLen) return false;
    
    return true;
}

bool Request::decodeHeader(const char *headerBuf, uint32 headerBufLen, MessageHeader& header)
{
    uint32 needLen = sizeof(MessageHeader);
    if (headerBufLen < needLen)
    {
        LOG_ERROR(&g_serverInstance->logger(), "headerBufLen:%u < needLen:%u error!", headerBufLen, needLen);
        return false;
    }

    const char *curBuf = headerBuf;

    //msg_flag_
    if(curBuf[0] != header.msg_flag_[0] || curBuf[1] != header.msg_flag_[1] || curBuf[2] != header.msg_flag_[2] || curBuf[3] != header.msg_flag_[3])
    {
        LOG_ERROR(&g_serverInstance->logger(), "msg_flag_ error!");
        return false;
    }
    curBuf += sizeof(header.msg_flag_);

    //version_
    header.version_ = ntohs(*(uint16*)curBuf);
    if (MESSAGE_VERSION != header.version_)
    {
        LOG_ERROR(&g_serverInstance->logger(), "header.version_:%d error!", header.version_);
        return false;
    }
    curBuf += sizeof(header.version_);

    //is_compress_
    header.is_compress_ = curBuf[0];
    if (1 < header.is_compress_)
    {
        LOG_ERROR(&g_serverInstance->logger(), "1 < header.is_compress_:%u error!", header.is_compress_);
        return false;
    }
    curBuf += sizeof(header.is_compress_);

    //is_compress_
    header.is_encryption_ = curBuf[0];
    if (1 < header.is_encryption_)
    {
        LOG_ERROR(&g_serverInstance->logger(), "1 < header.is_encryption_:%u error!", header.is_encryption_);
        return false;
    }
    curBuf += sizeof(header.is_encryption_);

    //message_id
    header.message_id_ = ntohl(*(uint32*)curBuf);
    curBuf += sizeof(header.message_id_);

    //timestamp_
    header.timestamp_ = ntohl(*(uint32*)curBuf);
    curBuf += sizeof(header.timestamp_);

    //ret_code_
    header.ret_code_ = ntohl(*(uint32*)curBuf);
    curBuf += sizeof(header.ret_code_);

    if (curBuf - headerBuf != needLen) return false;
    
    
    return true;
}

int32 Request::getRequestNeedBufLen()
{
    return sizeof(MessageLen) + sizeof(uint8) + sizeof(MessageHeader) + sizeof(uint32) + this->body_data_len_ + sizeof(uint32);    
}

bool Request::encodeRequest(char *messageBuf, uint32 messageBufLen)
{
    uint32 needLen = getRequestNeedBufLen();
    if (messageBufLen < needLen)
    {
        LOG_ERROR(&g_serverInstance->logger(), "messageBufLen:%u < needLen:%u error!", messageBufLen, needLen);
        return false;
    }

    char headerBuf[sizeof(MessageHeader)];
    if (!encodeHeader(this->header_, headerBuf, sizeof(MessageHeader)))
    {
        LOG_ERROR(&g_serverInstance->logger(), "encodeHeader error!");
        return false;
    }
    
    char *curBuf = messageBuf;

    //messageLen
    MessageLen covertMessageLen = htonl(needLen);
    memcpy(curBuf, (char*)&covertMessageLen, sizeof(covertMessageLen));
    curBuf += sizeof(covertMessageLen);

    //messageHeaderLen
    uint8 covertHeaderLen = sizeof(MessageHeader);
    memcpy(curBuf, (char*)&covertHeaderLen, sizeof(covertHeaderLen));
    curBuf += sizeof(covertHeaderLen);

    //headerData
    memcpy(curBuf, headerBuf, sizeof(headerBuf));
    curBuf += sizeof(headerBuf);

    //header_crc32
    uint32 header_crc32 = htonl(crc32::calc_crc32(headerBuf, sizeof(headerBuf)));
    memcpy(curBuf, (char*)&header_crc32, sizeof(header_crc32));
    curBuf += sizeof(header_crc32);

    //messageData
    if(this->body_data_buf_)
    {
        memcpy(curBuf, this->body_data_buf_, this->body_data_len_);
        curBuf += this->body_data_len_;

        //data_crc32        
        uint32 data_crc32 = htonl(crc32::calc_crc32(this->body_data_buf_, this->body_data_len_));
        
        memcpy(curBuf, (char*)&data_crc32, sizeof(data_crc32));
        curBuf += sizeof(data_crc32);
    }
    else
    {
        bzero(curBuf, sizeof(uint32));
        curBuf += sizeof(uint32);
    }

    if (curBuf - messageBuf != needLen) return false;

    
    return true;
}



bool Request::decodeRequest(const char *messageBuf, uint32 messageBufLen)
{    
    const char *curBuf = messageBuf;

    //messageLen
    MessageLen messageRealLen = ntohl(*(uint32*)curBuf);
    if (messageRealLen != messageBufLen)
    {
        LOG_ERROR(&g_serverInstance->logger(), "messageRealLen:%u != messageBufLen:%u error!", messageRealLen, messageBufLen);
        return false;
    }
    curBuf += sizeof(MessageLen);

    //messageHeaderLen
    uint8 messageHeaderLen = *(uint8*)curBuf;
    if (sizeof(MessageHeader) > messageHeaderLen)
    {
        LOG_ERROR(&g_serverInstance->logger(), "sizeof(MessageHeader) > messageHeaderLen:%u error!", messageHeaderLen);
        return false;
    }
    curBuf += sizeof(messageHeaderLen);

    //headerData
    const char *headerBuf = curBuf;
    if (!decodeHeader(headerBuf, messageHeaderLen, this->header_))
    {
        LOG_ERROR(&g_serverInstance->logger(), "decodeHeader error!");
        return false;
    }
    curBuf += messageHeaderLen;

    //header_crc32
    uint32 header_crc32 = ntohl(*(uint32*)curBuf);
    uint32 real_header_crc32 = crc32::calc_crc32(headerBuf, messageHeaderLen);
    if (header_crc32 != real_header_crc32)
    {
        LOG_ERROR(&g_serverInstance->logger(), "header_crc32:%u != real_header_crc32:%u error!", header_crc32, real_header_crc32);
        return false;
    }
    curBuf += sizeof(header_crc32);

    //messageData
    this->body_data_len_ = messageBufLen - (curBuf - messageBuf) - sizeof(uint32);
    if (0 < this->body_data_len_)
    {
        this->body_data_buf_ = curBuf;
        curBuf += this->body_data_len_;

        //data_crc32
        uint32 data_crc32 = ntohl(*(uint32*)curBuf);
        uint32 real_data_crc32 = crc32::calc_crc32(this->body_data_buf_, this->body_data_len_);
        if (data_crc32 != real_data_crc32)
        {
            LOG_ERROR(&g_serverInstance->logger(), "data_crc32:%u != real_data_crc32:%u error!", data_crc32, real_data_crc32);
            return false;
        }
    }
    curBuf += sizeof(uint32);

    if (curBuf - messageBuf != messageBufLen) return false;

    
    
    return true;
}






