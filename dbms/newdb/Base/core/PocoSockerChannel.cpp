#include "core/PocoSocketChannel.h"


#include <arpa/inet.h>


#include "Common/logger_adapt.h"
#include "core/request.h"
#include "core/IServer.h"


namespace core
{


	
    PocoSocketChannel::PocoSocketChannel(std::shared_ptr<Poco::Net::StreamSocket> socket, 
        std::shared_ptr<ISocketRector<std::shared_ptr<Poco::Net::Socket>,
        Poco::AbstractObserver>> reactor,
        SharedMemoryPoolPtr sharedMemoryPoolPtr)
        : socket_(socket)
        , reactor_(reactor)
        , sharedMemoryPoolPtr_(sharedMemoryPoolPtr)
    {
        bClose_ = false;
        this->socketChannelState_ = SocketChannelState::ready;
        this->recvMessageBuf_ = NULL;

        addObserver();
    }
	
    PocoSocketChannel::~PocoSocketChannel()
    {
        if (socket_.get() && !bClose_)
        {			
            this->close();
        }
    }

    bool PocoSocketChannel::send(std::shared_ptr<Request> resp)
    {
        bool ret = true;
		
        if (NULL == this->socket_.get()) return false;
		
        std::shared_ptr<Request> realResp = outFilterHandle(resp);
        if (!realResp.get())
        {
            LOG_WARNING(&g_serverInstance->logger(), "outFilterHandle failed!");
            return false;
        }
		
        int32 msgLen = resp->getRequestNeedBufLen();
        char* pBuf = this->sharedMemoryPoolPtr_->alloc(msgLen);
        if (NULL == pBuf)
        {
            LOG_ERROR(&g_serverInstance->logger(), "sharedMemoryPoolPtr_->alloc failed! len: %d", msgLen);
            return false;
        }
		
        //encode
        if(!resp->encodeRequest(pBuf, msgLen))
        {
            LOG_ERROR(&g_serverInstance->logger(), "resp->encodeRequest failed! msgLen: %d", msgLen);
            return false;
        }
        
        try
        {
            int32 totalSendLen = 0;
            do
            {
                int32 sendLen = this->socket_->sendBytes(pBuf + totalSendLen, msgLen - totalSendLen);
                if (0 >= sendLen)
                {
                    LOG_ERROR(&g_serverInstance->logger(), "sendBytes failed! errorCode:%d", sendLen);
                    errorHandle("sendBytes failed!");
                    ret = false;
                    break;
                }
                totalSendLen += sendLen;
            } while (totalSendLen < msgLen);

        }
        catch (Poco::Exception& exception)
        {
            LOG_ERROR(&g_serverInstance->logger(), "sendBytes failed! %s", exception.what());
            errorHandle("sendBytes failed!");
            ret = false;
        }
		
        this->sharedMemoryPoolPtr_->free(pBuf);
		
        return ret;
    }

    void PocoSocketChannel::setTimeout(int sTime)
    {
		
    }

    void PocoSocketChannel::close()
    {
        this->socketChannelState_ = SocketChannelState::Invalid;
        if (this->socket_.get() && !bClose_)
        {
            try
            {
                removeObserver();
                socket_->close();
            }
            catch (...)
            {
        		
            }
        }
        ISocketChannel::close();
    }
	
    void PocoSocketChannel::errorHandle(const std::string& errMsg)
    {
        close();
        this->socketChannelState_ = SocketChannelState::Invalid;
        if (this->linkEvent_.get())
        {
            char buf[1024];
            snprintf(buf, sizeof(buf) - 1, "PocoSocketChannel::errorHandle channelID:%d error: %s", channelID_, errMsg.c_str());			
            this->linkEvent_->onError(this, buf);
        }
    }

    std::shared_ptr<Request> PocoSocketChannel::inFilterHandle(std::shared_ptr<Request> request)
    {
        std::shared_ptr<Request> result = std::shared_ptr<Request>(request);
        for (int iPos = 0; inFilterCount_ > iPos; ++iPos)
        {
            result = inFilters_[iPos]->onFilter(this, result);
        }
		
        return result;
    }
	
    std::shared_ptr<Request> PocoSocketChannel::outFilterHandle(std::shared_ptr<Request> request)
    {
        std::shared_ptr<Request> result = std::shared_ptr<Request>(request);
        for (int iPos = 0; outFilterCount_ > iPos; ++iPos)
        {
            result = outFilters_[iPos]->onFilter(this, result);
        }
		
        return result;
    }
	
    const std::shared_ptr<Poco::Net::Socket> PocoSocketChannel::getSocket() const
    {
        return this->socket_;
    }

    void PocoSocketChannel::onReadable(Poco::Net::ReadableNotification* pNf)
    {
        pNf->release();
		
        try
        {    	
            if (this->socket_.get() && !bClose_ && (SocketChannelState::ready == this->socketChannelState_ 
                || SocketChannelState::readingData == this->socketChannelState_))
            {
                if (sizeof(MessageLen) + sizeof(MessageID) >= this->socket_->available()) return;
			
                //recv msg len and id
                if(SocketChannelState::ready == this->socketChannelState_)
                {
                    //read msg len
                    char buf[sizeof(MessageLen)];
                    {                    	
                        int readLen = this->socket_->receiveBytes(buf, sizeof(buf));
                        if (0 >= readLen)
                        {
                            LOG_ERROR(&g_serverInstance->logger(), "this->socket_->receiveBytes failed! errorCode: %d", readLen);
                            errorHandle("this->socket_->receiveBytes failed");
                            return;
                        }
                        this->messageLen_ = ntohl(*(uint32*)buf);
                        if (this->messageLen_ >= MAX_NET_MESSAGE_LEN)
                        {
                            LOG_ERROR(&g_serverInstance->logger(), "messageLen:%u >= MAX_NET_MESSAGE_LEN error!", this->messageLen_);
                            errorHandle("messageLen >= MAX_NET_MESSAGE_LEN error");
                            return;
                        }
                    }
				
                    if (this->recvMessageBuf_)
                    {
                        this->sharedMemoryPoolPtr_->free(this->recvMessageBuf_);
                        this->recvMessageBuf_ = NULL;
                    }
                	
                    this->recvMessageBuf_ = this->sharedMemoryPoolPtr_->alloc(this->messageLen_);
                    if (NULL == this->recvMessageBuf_)
                    {
                        LOG_ERROR(&g_serverInstance->logger(), "sharedMemoryPoolPtr_->alloc failed! len: %d", this->messageLen_);
                        errorHandle("sharedMemoryPoolPtr_->alloc failed!");
                        return;
                    }
                    memcpy(this->recvMessageBuf_, buf, sizeof(buf));
                    recvMessageLen_ += sizeof(buf);
				
                    this->socketChannelState_ = SocketChannelState::readingData;
                }			
			
                //read data
                if(SocketChannelState::readingData == this->socketChannelState_)
                {
                    int readLen = this->socket_->receiveBytes(this->recvMessageBuf_ + this->recvMessageLen_, this->messageLen_ - this->recvMessageLen_);
                    if (0 >= readLen)
                    {
                        LOG_ERROR(&g_serverInstance->logger(), "this->socket_->receiveBytes failed! errorCode: %d", readLen);
                        errorHandle("this->socket_->receiveBytes failed");
                        return;
                    }
                    this->recvMessageLen_ += readLen;
				
                    if (this->messageLen_  == this->recvMessageLen_)
                    {
                        this->socketChannelState_ = SocketChannelState::handlingMessage;
                    }
                    else
                    {
                        LOG_ERROR(&g_serverInstance->logger(), "this->messageLen_ - sizeof(MessageLen) - sizeof(MessageID) - this->recvMessageLen error");
                        errorHandle("this->socket_->receiveBytes failed");
                        return;
                    }
                }
			
                //handle msg
                if(SocketChannelState::handlingMessage == this->socketChannelState_)
                {
                    std::shared_ptr<Request> request = std::make_shared<Request>();
                    if (!request->decodeRequest(this->recvMessageBuf_, this->messageLen_))
                    {
                        LOG_ERROR(&g_serverInstance->logger(), "request->decodeRequest error!");
                        errorHandle("request->decodeRequest failed");
                        return;
                    }
				
                    std::shared_ptr<Request> realRequst = inFilterHandle(request);
                    if (!realRequst.get())
                    {
                        LOG_WARNING(&g_serverInstance->logger(), "inFilterHandle failed!");
                        this->socketChannelState_ = SocketChannelState::ready;
                        return;
                    }
			
                    if (this->linkHandler_.get())
                    {
                        this->linkHandler_->onData(this, realRequst);
                    }

                    // clear
                    {
                        this->messageLen_ = 0;
                        this->recvMessageLen_ = 0;
                        if (this->recvMessageBuf_)
                        {
                            this->sharedMemoryPoolPtr_->free(this->recvMessageBuf_);
                            this->recvMessageBuf_ = NULL;
                        }
                    }
                	
                    this->socketChannelState_ = SocketChannelState::ready;
                }			
            }	
        }
        catch (...)
        {
            LOG_ERROR(&g_serverInstance->logger(), "this->socket_->receiveBytes failed! ");
            errorHandle("this->socket_->receiveBytes failed");
            return;
        }
    }
		
    void PocoSocketChannel::onShutdown(Poco::Net::ShutdownNotification* pNf)
    {
        pNf->release();
		
        close();
        if (this->linkEvent_.get())
        {
            this->linkEvent_->onClose(this);
        }
    }

    void PocoSocketChannel::onTimeout(Poco::Net::TimeoutNotification* pNf)
    {
        pNf->release();
    }

    void PocoSocketChannel::onError(Poco::Net::ErrorNotification* pNf)
    {
        pNf->release();
        errorHandle("onError");
    }
	
    bool PocoSocketChannel::addObserver()
    {
        if (this->socket_.get() && !bClose_)
        {
            reactor_->addEventHandler(*this, ReadableObserver(*this, &PocoSocketChannel::onReadable));
            reactor_->addEventHandler(*this, ShutdownObserver(*this, &PocoSocketChannel::onShutdown));
            reactor_->addEventHandler(*this, ErrorObserver(*this, &PocoSocketChannel::onError));
			
            return true;
        }
		
        return false;
    }
	
    bool PocoSocketChannel::removeObserver()
    {
        if (this->socket_.get() && !bClose_)
        {
            reactor_->removeEventHandler(*this, ReadableObserver(*this, &PocoSocketChannel::onReadable));
            reactor_->removeEventHandler(*this, ShutdownObserver(*this, &PocoSocketChannel::onShutdown));
            reactor_->removeEventHandler(*this, ErrorObserver(*this, &PocoSocketChannel::onError));
			
            return true;
        }
		
        return false;
    }
	
	
	
}
	
	
	
	
	
	