#include "core/ILink.h"


#include <Poco/Net/Socket.h>
#include <Poco/AbstractObserver.h>

#include "Poco/Util/Util.h"
#include "Poco/Util/TimerTask.h"
#include "Poco/Event.h"
#include "Poco/Util/Timer.h"
#include "Poco/Util/TimerTaskAdapter.h"



namespace core
{
	class IServerWatch;
	class PocoSocketChannelPoolItem;
	
	
	/// @brief 见基类
	class PocoSocketChannelPool : public ISocketChannelPool<std::shared_ptr<Poco::Net::Socket>, Poco::AbstractObserver>
	{
		typedef std::recursive_mutex this_mutex;
		typedef std::lock_guard<std::recursive_mutex> this_guard;
		
	public:
    	/// @brief 见基类
		PocoSocketChannelPool(std::shared_ptr<ISocketConnector<std::shared_ptr<Poco::Net::Socket>, Poco::AbstractObserver>> socketConnector, 
			uint16 minChannelCount, uint16 maxChannelCount, std::shared_ptr<IServerWatch> serverWatcher);
		~PocoSocketChannelPool();
		
		
    	/// @brief 见基类
		virtual bool setMinChannelCount(ServerUniqueID serverUniqueID, uint16 minChannelCount);
    	/// @brief 见基类
		virtual bool setMaxChannelCount(ServerUniqueID serverUniqueID, uint16 maxChannelCount);
    	/// @brief 见基类
		virtual bool setSocketConnector(ServerUniqueID serverUniqueID, std::shared_ptr<ISocketConnector<std::shared_ptr<Poco::Net::Socket>, Poco::AbstractObserver>> socketConnector);
		
    	/// @brief 心跳消息设置
		virtual bool setHeartbeat(const std::string& heartbeatMessageData, MessageID heartbeatMessageID);
		
    	/// @brief 见基类
		virtual ISocketChannel<std::shared_ptr<Poco::Net::Socket>>* getSocketChannel(ServerUniqueID serverUniqueID);
    	/// @brief 见基类
		virtual void releaseSocketChannel(ISocketChannel<std::shared_ptr<Poco::Net::Socket>> *socketChannel);
		
		
	protected:
    	/// @brief 见基类
		virtual void onConnected(ISocketChannel<std::shared_ptr<Poco::Net::Socket>> *channel);
    	/// @brief 见基类
		virtual void onClose(ISocketChannel<std::shared_ptr<Poco::Net::Socket>> *channel);
    	/// @brief 见基类
		virtual void onError(ISocketChannel<std::shared_ptr<Poco::Net::Socket>> *channel, const std::string& errorMsg);
		
    	/// @brief 见基类
		virtual void onTimeout(ISocketChannel<std::shared_ptr<Poco::Net::Socket>> *channel);
		
    	/// @brief 见基类
		void onHeartbeatTimer(Poco::Util::TimerTask& task);
		
    	/// @brief 得到服务实例对应PocoSocketChannelPoolItem
		std::shared_ptr<PocoSocketChannelPoolItem> getOrCreatePoolItem(ServerUniqueID serverUniqueID);
    	/// @brief 得到服务实例对应PocoSocketChannelPoolItem
		std::shared_ptr<PocoSocketChannelPoolItem> getItemByChannel(ISocketChannel<std::shared_ptr<Poco::Net::Socket>> *channel);
		
	protected:
    	/// @brief 服务发现
		std::shared_ptr<IServerWatch> serverWatcher_;
    	/// @brief 服务实例连接池
		std::map<ServerUniqueID, std::shared_ptr<PocoSocketChannelPoolItem>> channelPoolItems_;
    	/// @brief poco中的定时器
		Poco::Util::Timer heartbeatTimer_;
		
    	/// @brief 心跳消息数据
		std::string heartbeatMessageData_;
    	/// @brief 心跳消息ID
		MessageID heartbeatMessageID_;
		
		this_mutex mutex_;
	};
	
	
    /// @brief 见基类
	class PocoSocketChannelHandleApi : public ISocketChannelHandleApi
	{
	public:
    	PocoSocketChannelHandleApi(std::shared_ptr<ISocketChannelPool<std::shared_ptr<Poco::Net::Socket>, Poco::AbstractObserver>> channelPool);
		~PocoSocketChannelHandleApi();
		
    	/// @brief 见基类
		virtual bool sendMessageToServer(ServerUniqueID serverUniqueID, std::shared_ptr<Request> req);
    	/// @brief 见基类
		virtual bool sendMessageToClient(SocketChannelID channelID, std::shared_ptr<Request> req);
		
	protected:
    	/// @brief 作为客户端的连接池
		std::shared_ptr<ISocketChannelPool<std::shared_ptr<Poco::Net::Socket>, Poco::AbstractObserver>> channelPool_;
	};
	
	
    /// @brief 服务实例对应的连接池
	class PocoSocketChannelPoolItem
	{
		typedef std::recursive_mutex this_mutex;
		typedef std::lock_guard<std::recursive_mutex> this_guard;
		
		
    	/// @brief 连接相关信息
		struct ChannelInfo
		{
    		/// @brief 连接
			ISocketChannel<std::shared_ptr<Poco::Net::Socket>> *channel_;
    		/// @brief 连接创建时间
			uint32 createSTime_;
    		/// @brief 连接最后发送心跳时间
			uint32 lastHeartbeatSTime_;
			
			ChannelInfo();
			~ChannelInfo();
		};
		
	public:
		PocoSocketChannelPoolItem(std::shared_ptr<ISocketConnector<std::shared_ptr<Poco::Net::Socket>, Poco::AbstractObserver>> socketConnector, 
			uint16 minChannelCount,
			uint16 maxChannelCount,
			ServerUniqueID serverUniqueID, 
			std::shared_ptr<IServerWatch> serverWatcher);
		
		virtual ~PocoSocketChannelPoolItem();
		
    	/// @brief 同PocoSocketChannelPool
		virtual ISocketChannel<std::shared_ptr<Poco::Net::Socket>>* getSocketChannel();
    	/// @brief 同PocoSocketChannelPool
		virtual void releaseSocketChannel(ISocketChannel<std::shared_ptr<Poco::Net::Socket>> *socketChannel);
    	/// @brief 同PocoSocketChannelPool
		virtual void removeChannel(ISocketChannel<std::shared_ptr<Poco::Net::Socket>> *socketChannel);
		
    	/// @brief 连接是否在此实例中
		virtual bool hasSocketChannel(SocketChannelID channelID);
		
    	/// @brief 同PocoSocketChannelPool
		virtual bool setMinChannelCount(uint16 minChannelCount);
    	/// @brief 同PocoSocketChannelPool
		virtual bool setMaxChannelCount(uint16 maxChannelCount);
    	/// @brief 同PocoSocketChannelPool
		virtual bool setSocketConnector(std::shared_ptr<ISocketConnector<std::shared_ptr<Poco::Net::Socket>, Poco::AbstractObserver>> socketConnector);
		
    	/// @brief 同PocoSocketChannelPool
		virtual bool setHeartbeat(const std::string& heartbeatMessageData, MessageID heartbeatMessageID);
		
    	/// @brief 处理此实例中所以连接的心跳
		virtual void sendHeartbeat();
		
	protected:
		virtual bool isSocketChannelHealth(std::shared_ptr<ChannelInfo> channelInfo);
		virtual bool sendHeartbeat(std::shared_ptr<ChannelInfo> channelInfo);
		
		virtual ISocketChannel<std::shared_ptr<Poco::Net::Socket>>* getOrCreateChannel();
		
		
	protected:
    	/// @brief 连接器
		std::shared_ptr<ISocketConnector<std::shared_ptr<Poco::Net::Socket>, Poco::AbstractObserver>> socketConnector_;
		uint16 minChannelCount_;
		uint16 maxChannelCount_;
    	/// @brief 服务发现
		std::shared_ptr<IServerWatch> serverWatcher_;
    	/// @brief 服务实例唯一标识
		ServerUniqueID serverUniqueID_;
		
    	/// @brief 空闲的连接
		std::map<SocketChannelID, std::shared_ptr<ChannelInfo>> freeSocketChannel_;
    	/// @brief 外部正在使用的连接
		std::map<SocketChannelID, std::shared_ptr<ChannelInfo>> useSocketChannel_;
    	/// @brief 异常连接，待释放
		std::map<SocketChannelID, std::shared_ptr<ChannelInfo>> needRemoveSocketChannel_;
		
    	/// @brief 心跳消息数据
		std::string heartbeatMessageData_;
    	/// @brief 心跳消息ID
		MessageID heartbeatMessageID_;
		
		this_mutex mutex_;
	};
	
	
	
	
	
	
	
	
	
	
}



