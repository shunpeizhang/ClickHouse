#include "core/PocoSocketConnector.h"


#include <Poco/Net/SocketStream.h>
#include <Poco/Net/SocketAddress.h>
#include <Poco/Timespan.h>


#include "core/PocoServer.h"
#include "core/PocoSocketChannel.h"
#include "Common/logger_adapt.h"

namespace core
{


	PocoSocketConnector::PocoSocketConnector(std::shared_ptr<ISocketRector<std::shared_ptr<Poco::Net::Socket>, Poco::AbstractObserver>> socketRector,
		std::shared_ptr<ILinkHandler<std::shared_ptr<Poco::Net::Socket>>> linkHandler, 
		std::shared_ptr<ILinkEvent<std::shared_ptr<Poco::Net::Socket>>> linkEvent,
		uint32 timeMTimeOut,
		SharedMemoryPoolPtr sharedMemoryPoolPtr)
		: ISocketConnector(socketRector, linkHandler, linkEvent, timeMTimeOut)
		, sharedMemoryPoolPtr_(sharedMemoryPoolPtr)
	{
		
	}
		
	PocoSocketConnector::~PocoSocketConnector()
	{
		
	}
	
	ISocketChannel<std::shared_ptr<Poco::Net::Socket>>* PocoSocketConnector::createConnect(const std::string& ip, uint16 port)
	{
		try
		{
			Poco::Net::StreamSocket socket;
			socket.connect(Poco::Net::SocketAddress(ip, port), Poco::Timespan(this->timeMTimeOut_));
			
			auto socketChannelManager = ((PocoServer*)g_serverInstance.get())->getSocketChannelManager();
			if (NULL == socketChannelManager.get()) return NULL;
		
			auto channel = new PocoSocketChannel(std::make_shared<Poco::Net::StreamSocket>(socket), 
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
			
			return channel;
		}
		catch (...)
		{
			LOG_ERROR(&g_serverInstance->logger(), "createConnect failed!");
		}
		
		return NULL;
	}


	
	
	
}
	




