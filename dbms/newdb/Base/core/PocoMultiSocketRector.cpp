#include "core/PocoMultiSocketRector.h"

#include <Poco/Thread.h>


namespace core
{


	PocoMultiSocketRector::PocoMultiSocketRector(int32 handleThreadCount)
	{
		this->handleThreadCount_ = handleThreadCount;
		for (int iPos = 0; this->handleThreadCount_ > iPos; ++iPos)
		{
			this->reactors_.push_back(std::make_shared<Poco::Net::SocketReactor>());
			this->threads_.push_back(std::make_shared<Poco::Thread>());
		}
		
		this->isStart_ = false;
	}
	
	PocoMultiSocketRector::~PocoMultiSocketRector()
	{
		
	}
		
		
	void PocoMultiSocketRector::run()
	{
		if (!this->isStart_)
		{
			for (int iPos = 0; this->handleThreadCount_ > iPos; ++iPos)
			{
				this->threads_[iPos]->start(*this->reactors_[iPos]);
			}
		}
	}
	
	void PocoMultiSocketRector::stop()
	{
		if (this->isStart_)
		{
			for (int iPos = 0; this->handleThreadCount_ > iPos; ++iPos)
			{
				this->reactors_[iPos]->stop();
			}
			
			for (int iPos = 0; this->handleThreadCount_ > iPos; ++iPos)
			{
				this->threads_[iPos]->join();
			}
		}
	}
		
		
	void PocoMultiSocketRector::addEventHandler(const ISocketChannel<std::shared_ptr<Poco::Net::Socket>>& socket, const Poco::AbstractObserver& observer)
	{
		std::shared_ptr<Poco::Net::SocketReactor> reactor = getReactor(socket);
    	
		if (reactor.get()) reactor->addEventHandler(*socket.getSocket().get(), observer);
	}
	
	bool PocoMultiSocketRector::hasEventHandler(const ISocketChannel<std::shared_ptr<Poco::Net::Socket>>& socket, const Poco::AbstractObserver& observer)
	{
		std::shared_ptr<Poco::Net::SocketReactor> reactor = getReactor(socket);		
		if (reactor.get()) return reactor->hasEventHandler(*socket.getSocket().get(), observer);
		
		return false;
	}
	
	void PocoMultiSocketRector::removeEventHandler(const ISocketChannel<std::shared_ptr<Poco::Net::Socket>>& socket, const Poco::AbstractObserver& observer)
	{
		std::shared_ptr<Poco::Net::SocketReactor> reactor = getReactor(socket);
		if (reactor.get()) reactor->removeEventHandler(*socket.getSocket().get(), observer);
	}
		
		
	std::shared_ptr<Poco::Net::SocketReactor> PocoMultiSocketRector::getReactor(const ISocketChannel<std::shared_ptr<Poco::Net::Socket>>& socket)
	{
		int iPos = socket.getChannelID() % handleThreadCount_;
		return this->reactors_[iPos];
	}


	
	
	
}


