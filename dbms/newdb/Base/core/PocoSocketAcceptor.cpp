#include "core/PocoSocketAcceptor.h"


#include "core/PocoServer.h"
#include "core/PocoSocketChannel.h"
#include "Common/logger_adapt.h"
#include "core/PocoSocketChannel.h"


namespace core
{
	
	
	
PocoSocketAcceptor::PocoSocketAcceptor(std::shared_ptr<ISocketRector<std::shared_ptr<Poco::Net::Socket>, Poco::AbstractObserver>> socketRector,
		std::shared_ptr<ILinkHandler<std::shared_ptr<Poco::Net::Socket>>> linkHandler, 
		std::shared_ptr<ILinkEvent<std::shared_ptr<Poco::Net::Socket>>> linkEvent,
		SharedMemoryPoolPtr sharedMemoryPoolPtr)
		: ISocketAcceptor(socketRector, linkHandler, linkEvent)
		, sharedMemoryPoolPtr_(sharedMemoryPoolPtr)
	{
		
	}
	
	PocoSocketAcceptor::~PocoSocketAcceptor()
	{
		try
		{
			if (this->socketRector_.get())
			{
				this->socketRector_->removeEventHandler(*this->serverSocket_.get(), Observer(*this, &PocoSocketAcceptor::onAccept));
			}
		}
		catch (...)
		{
			LOG_ERROR(&g_serverInstance->logger(), "socketRector_->removeEventHandler failed!");
		}
	}
	
	bool PocoSocketAcceptor::initialize(int serverPort)
	{
		try
		{
    		this->serverSocket_ = std::make_shared<PocoServerSocketChannel>(std::make_shared<Poco::Net::ServerSocket>(serverPort));
			  this->socketRector_->addEventHandler(*this->serverSocket_.get(), Observer(*this, &PocoSocketAcceptor::onAccept));
		}
		catch (Poco::Exception e)
		{
			LOG_ERROR(&g_serverInstance->logger(), "%s", e.what());
			return false;
		}	
		
		return true;
	}
	

	void PocoSocketAcceptor::onAccept(Poco::Net::ReadableNotification* pNotification)
	{
		pNotification->release();
			
		Poco::Net::StreamSocket sock = this->serverSocket_->getServerSocket()->acceptConnection();
		auto socketChannelManager = ((PocoServer*)g_serverInstance.get())->getSocketChannelManager();
		if (NULL == socketChannelManager.get()) return;
		
		auto channel = new PocoSocketChannel(std::make_shared<Poco::Net::StreamSocket>(sock), 
			this->socketRector_, this->sharedMemoryPoolPtr_);
		channel->setHandler(this->linkHandler_);
		channel->setLinkEvent(this->linkEvent_);
    	channel->setChannelID(g_serverInstance->createUniqueID());
		
		for (int iPos = 0; inFilterCount_ > iPos; ++iPos)
		{
			channel->addInFilter(this->inFilters_[iPos]);
		}
		
		for (int iPos = 0; outFilterCount_ > iPos; ++iPos)
		{
			channel->addOutFilter(this->outFilters_[iPos]);
		}
		
		socketChannelManager->addSocketChannel(channel);
	}

	
	
}
	