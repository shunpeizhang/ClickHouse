#pragma once




#include "baseType.h"



namespace core
{
    
    
/// @brief filter最大数量
#define MAX_SOCKET_CHANNEL_FILTER_COUNT  10
/// @brief 连接器会有多个，需要一个label进行区分，此为默认创建的连接器
#define CONNECTOR_BASE_LABEL "CONNECTOR_BASE_LABEL"
    
    /// @brief 用于连接唯一标识生成
    extern SocketChannelID g_lastCreateSocketID;
    
    
	class Request;
	
	template <class S>
	struct ISocketChannel;	
	
	/// @brief 网络消息包处理类(协议解析、消息对应业务接口调用)
	/// @tparam S 用户连接对应的实际类型
	template <class S>
	class ILinkHandler {
	public:
		virtual ~ILinkHandler() {}
		
		/**
		 *	@brief 网络消息包处理
		 *
		 *  @param channel 网络连接
		 *  @param req 待处理消息
		 */
		virtual bool onData(ISocketChannel<S> *channel, std::shared_ptr<Request> req) = 0;
	};
	
    
    /// @brief 网络连接的非读写事件处理
	/// @tparam S 用户连接对应的实际类型
	template <class S>
	class ILinkEvent {
	public:
		virtual ~ILinkEvent() {}
		
    	/**
		 *	@brief 网络连接关闭处理
		 *
		 *  @param channel 网络连接
		 */
		virtual void onClose(ISocketChannel<S> *channel) = 0;
    	
    	/**
		 *	@brief 网络连接出现错误处理
		 *
		 *  @param channel 网络连接
		 */
		virtual void onError(ISocketChannel<S> *channel, const std::string& errorMsg) = 0;
		
    	/**
		 *	@brief 网络连接多一定时间内没有消息包交互时处理
		 *
		 *  @param channel 网络连接
		 */
		virtual void onTimeout(ISocketChannel<S> *channel) = 0;
	};
	
    
    /// @brief 此类主要用于在消息包接收后处理前或消息包发送前，进行消息的处理（比如：压缩、加密、过滤等）
	/// @tparam S 用户连接对应的实际类型
	template <class S>
	class IFilter {
	public:
		virtual ~IFilter() {}
		
    	/**
		 *	@brief 进行消息的处理（比如：压缩、加密、过滤等）
		 *
		 *  @param channel 网络连接
		 *  @param messsage 需要处理的消息包
		 *  @return 处理之后的消息包
		 */
		virtual std::shared_ptr<Request> onFilter(ISocketChannel<S> *channel, std::shared_ptr<Request> messsage) = 0;
	};
	
    /// @brief 此类主要用于Session中，对应于value的基类
	class IObject
	{
	public:
		~IObject() {}
		
	};
	
    /// @brief 连接的会话信息管理类
	class Session
	{
	public:
		virtual ~Session() {}
		
    	/**
		 *	@brief 类似于kv中的get
		 *
		 *  @param key kv中的key
		 *  @return kv中的value
		 */
    	virtual std::shared_ptr<IObject> get(const std::string& key)
    	{
        	auto iter = this->objects_.find(key);
        	if (this->objects_.end() == iter) return std::shared_ptr<IObject>(NULL);
        	
        	return iter->second;
    	}
    	
    	/**
		 *	@brief 类似于kv中的get
		 *
		 *  @param key kv中的key
		 *  @param object kv中的value
		 */
    	virtual bool set(const std::string& key, std::shared_ptr<IObject> object) {objects_[key] = object; }
		
	protected:
    	/// 保存一个连接中会话的kv信息
		std::map<std::string, std::shared_ptr<IObject>> objects_;
	};
	
    
    /// @brief 代表一个连接
	/// @tparam S 用户连接对应的实际类型（因为机制上要兼容poco或其他实际网络库）
	template <class S>
	class ISocketChannel
	{
	public:		
		ISocketChannel()
		{
			this->inFilterCount_ = 0;
			this->outFilterCount_ = 0;
			this->peerPort_ = -1;
			this->channelID_ = -1;
			this->bClose_ = true;
    		this->channelID_ = 0;
		
			this->session_ = std::make_shared<Session>();
		}
		
		virtual ~ISocketChannel() {}

    	/**
		 *	@brief 发送消息包(不同网络库的异常在send内部处理)
		 *
		 *  @param resp 待发送的消息包
		 *  @return 是否发送成功
		 */
		virtual bool send(std::shared_ptr<Request> resp) = 0;

    	/**
		 *	@brief 设置多长时间没有网络交互就关闭连接
		 *
		 *  @param sTime 时长（单位：秒）
		 */
		virtual void setTimeout(int sTime) = 0;

    	/**
		 *	@brief 得到session管理器
		 *
		 *  @return 返回session管理器
		 */
		virtual std::shared_ptr<Session> getSession() {return this->session_;}
    	
    	/**
		 *	@brief 设置消息包的实际处理类
		 *
		 *  @param linkHandler 消息包实际处理类
		 */
		virtual void setHandler(std::shared_ptr<ILinkHandler<S>> linkHandler) {this->linkHandler_ = linkHandler; }
		
    	/**
		 *	@brief 设置连接的非读写事件处理类
		 *
		 *  @param linkEvent 连接的非读写事件处理类
		 */
		virtual void setLinkEvent(std::shared_ptr<ILinkEvent<S>> linkEvent) {this->linkEvent_ = linkEvent; }
    	
    	/**
		 *	@brief 添加接收消息包后的filter处理类
		 *
		 *  @param filter filter类
		 */
		virtual bool addInFilter(std::shared_ptr<IFilter<S>> filter) {
			if (MAX_SOCKET_CHANNEL_FILTER_COUNT <= this->inFilterCount_) return false;
			this->inFilters_[this->inFilterCount_++] = filter;
		}
		
    	/**
		 *	@brief 添加发送消息包前的filter处理类
		 *
		 *  @param filter filter类
		 */
		virtual bool addOutFilter(std::shared_ptr<IFilter<S>> filter) {
			if (MAX_SOCKET_CHANNEL_FILTER_COUNT <= this->outFilterCount_) return false;		
			this->outFilters_[this->outFilterCount_++] = filter;
		}

    	
		/// @brief 得到此连接对应的ip		 
		virtual const std::string& getPeerIp() const {return this->peerIp_;}
    	
    	/// @brief 得到此连接对应的port
		virtual int16 getPeerPort() const {return this->peerPort_;}
    	
    	/// @brief 得到此连接对应的唯一标识（因为fd系统会重用）
		virtual SocketChannelID getChannelID() const {return this->channelID_;}
    	virtual void setChannelID(SocketChannelID channelID) {this->channelID_ = channelID; }
    	
    
    	
    	/// @brief 得到网络库中实际连接对象
		virtual const S getSocket() const = 0;
		
    	/// @brief 关闭此连接
		virtual void close() {this->bClose_ = true; }
    	
    	/// @brief 是否此连接已关闭
		virtual	bool isClosed() {return this->bClose_;}
		
	protected:
    	/// @brief 消息包处理类
		std::shared_ptr<ILinkHandler<S>> linkHandler_;
    	/// @brief 连接非读写事件处理类
		std::shared_ptr<ILinkEvent<S>> linkEvent_;
    	/// @brief 接收消息包后的filter处理类
		std::shared_ptr<IFilter<S>> inFilters_[MAX_SOCKET_CHANNEL_FILTER_COUNT];
    	/// @brief 发送消息包前的filter处理类
		std::shared_ptr<IFilter<S>> outFilters_[MAX_SOCKET_CHANNEL_FILTER_COUNT];
    	/// @brief 接收消息包后的filter处理类实际数量
		int8 inFilterCount_;
    	/// @brief 发送消息包前的filter处理类实际数量
		int8 outFilterCount_;
		
    	/// @brief 此连接ip
		std::string peerIp_;
    	/// @brief 此连接port
		int16 peerPort_;
    	/// @brief 此连接唯一标识
		SocketChannelID channelID_;
    	/// @brief 此连接是否已关闭
		bool bClose_;
		
    	/// @brief 连接会话信息管理器
		std::shared_ptr<Session> session_;    	
	};

    
    /// @brief 反应器模式中的rector
	/// @tparam S 用户连接对应的实际类型
    /// @tparam T 观察者模式中的观察者
	template <class S, class T>
	class ISocketRector
	{
	public:
		virtual ~ISocketRector() {}
		
    	/// @brief 开始事件分发
		virtual void run() = 0;
    	/// @brief 停止事件分发
		virtual void stop() = 0;
		
    	/**
		 *	@brief 连接对应的添加观察者
		 *
		 *  @param socket 连接
		 *  @param observer 观察者
		 */
		virtual void addEventHandler(const ISocketChannel<S>& socket, const T& observer) = 0;
    	
    	/**
		 *	@brief 是否存在某观察者
		 *
		 *  @param socket 连接
		 *  @param observer 观察者
		 */
		virtual bool hasEventHandler(const ISocketChannel<S>& socket, const T& observer) = 0;
    	
    	/**
		 *	@brief 删除某观察者
		 *
		 *  @param socket 连接
		 *  @param observer 观察者
		 */
		virtual void removeEventHandler(const ISocketChannel<S>& socket, const T& observer) = 0;
	};
	
    
    /// @brief 接收器-连接器模式中的接收器
	/// @tparam S 用户连接对应的实际类型
    /// @tparam T 观察者模式中的观察者
	template <class S, class T>
	class ISocketAcceptor {
	public:
    	/**
		 *  @param socketRector 反应器
		 *  @param linkHandler 消息包处理类，用于新接收的连接中
		 *  @param linkEvent 连接非读写事件处理类，用于新接收的连接中
		 */
		ISocketAcceptor(std::shared_ptr<ISocketRector<S, T>> socketRector, 
			std::shared_ptr<ILinkHandler<S>> linkHandler, 
			std::shared_ptr<ILinkEvent<S>> linkEvent)
			: socketRector_(socketRector)
			, linkHandler_(linkHandler)
			, linkEvent_(linkEvent)
		{
		}
		
		virtual ~ISocketAcceptor() {}
		
    	/**
		 *	@brief 初始化监听
		 *
		 *  @param serverPort 监听的端口
		 *  @return 是否监听成功，失败有可能是端口被占用
		 */
		virtual bool initialize(int serverPort) = 0;
		
    	/// @brief 添加接收消息后的filter类，用于新接收的连接中
		virtual bool addInFilter(std::shared_ptr<IFilter<S>> filter) {
			if (MAX_SOCKET_CHANNEL_FILTER_COUNT <= this->inFilterCount_) return false;		
			this->inFilters_[this->inFilterCount_++] = filter;
		}
		
    	/// @brief 添加发送消息前的filter类，用于新接收的连接中
		virtual bool addOutFilter(std::shared_ptr<IFilter<S>> filter) {
			if (MAX_SOCKET_CHANNEL_FILTER_COUNT <= this->outFilterCount_) return false;		
			this->outFilters_[this->outFilterCount_++] = filter;
		}

	protected:
    	/// @brief 消息包处理类，用于新接收的连接中
		std::shared_ptr<ILinkHandler<S>> linkHandler_;
    	/// @brief 连接非读写事件处理类，用于新接收的连接中
		std::shared_ptr<ILinkEvent<S>> linkEvent_;
    	/// @brief 事件分发器
		std::shared_ptr<ISocketRector<S, T>> socketRector_;
		
    	/// @brief 接收消息后的filter类，用于新接收的连接中
		std::shared_ptr<IFilter<S>> inFilters_[MAX_SOCKET_CHANNEL_FILTER_COUNT];
    	/// @brief 发送消息前的filter类，用于新接收的连接中
		std::shared_ptr<IFilter<S>> outFilters_[MAX_SOCKET_CHANNEL_FILTER_COUNT];
    	/// @brief 接收消息包后的filter处理类实际数量
		int8 inFilterCount_;
    	/// @brief 发送消息包前的filter处理类实际数量
		int8 outFilterCount_;
	};	
	
    /// @brief 接收器-连接器模式中的连接器
	/// @tparam S 用户连接对应的实际类型
    /// @tparam T 观察者模式中的观察者
	template <class S, class T>
	class ISocketConnector {
	public:
    	/**
		 *  @param socketRector 反应器
		 *  @param linkHandler 消息包处理类，用于新接收的连接中
		 *  @param linkEvent 连接非读写事件处理类，用于新接收的连接中
		 *  @param timeMTimeOut 连接超时（单位：毫秒）
		 */
		ISocketConnector(std::shared_ptr<ISocketRector<S, T>> socketRector, 
			std::shared_ptr<ILinkHandler<S>> linkHandler, 
			std::shared_ptr<ILinkEvent<S>> linkEvent,
			uint32 timeMTimeOut)
			: socketRector_(socketRector)
			, linkHandler_(linkHandler)
			, linkEvent_(linkEvent)
			, timeMTimeOut_(timeMTimeOut)
		{
		}
		
		virtual ~ISocketConnector() {}		
		
    	/**
		 *	@brief 创建一个新客户端连接
		 *
		 *  @return 如果失败返回NULL，成功则返回新连接
		 */
		virtual ISocketChannel<S>* createConnect(const std::string& ip, uint16 port) = 0;
		
    	/// @brief 添加接收消息后的filter类，用于新接收的连接中
		virtual bool addInFilter(std::shared_ptr<IFilter<S>> filter) {
			if (MAX_SOCKET_CHANNEL_FILTER_COUNT <= this->inFilterCount_) return false;		
			this->inFilters_[this->inFilterCount_++] = filter;
		}
		
    	/// @brief 添加发送消息前的filter类，用于新接收的连接中
		virtual bool addOutFilter(std::shared_ptr<IFilter<S>> filter) {
			if (MAX_SOCKET_CHANNEL_FILTER_COUNT <= this->outFilterCount_) return false;
			this->outFilters_[this->outFilterCount_++] = filter;
		}
		
    	/// @brief 设置连接非读写事件处理类
		virtual void setLinkEvent(std::shared_ptr<ILinkEvent<S>> linkEvent)
		{
			linkEvent_ = linkEvent;
		}
		
	protected:
    	/// @brief 消息包处理类，用于新接收的连接中
		std::shared_ptr<ILinkHandler<S>> linkHandler_;
    	/// @brief 连接非读写事件处理类，用于新接收的连接中
		std::shared_ptr<ILinkEvent<S>> linkEvent_;
    	/// @brief 事件分发器
		std::shared_ptr<ISocketRector<S, T>> socketRector_;
		
    	/// @brief 接收消息后的filter类，用于新接收的连接中
		std::shared_ptr<IFilter<S>> inFilters_[MAX_SOCKET_CHANNEL_FILTER_COUNT];
    	/// @brief 发送消息前的filter类，用于新接收的连接中
		std::shared_ptr<IFilter<S>> outFilters_[MAX_SOCKET_CHANNEL_FILTER_COUNT];
    	/// @brief 接收消息包后的filter处理类实际数量
		int8 inFilterCount_;
    	/// @brief 发送消息包前的filter处理类实际数量
		int8 outFilterCount_;
		
    	/// @brief 连接超时时长（单位：毫秒）
		uint32 timeMTimeOut_;
	};
	
    /// @brief 所以连接的管理器，负责连接的生命周期管理
	/// @tparam S 用户连接对应的实际类型
    /// @tparam T 观察者模式中的观察者
	template <class S, class T>
	class ISocketChannelManager : public ILinkEvent<S> {
	public:
		virtual ~ISocketChannelManager() {}

    	/// @brief 添加一个连接
		virtual void addSocketChannel(ISocketChannel<S> *channel) = 0;
    	/// @brief 清理一个连接
		virtual void removeSocketChannel(ISocketChannel<S> *channel) = 0;
    	/// @brief 清理一个连接
    	virtual void removeSocketChannel(SocketChannelID channelID) = 0;
    	/// @brief 只是将连接从管理器中删除，不析构连接
		virtual void eraseSocketChannelNotDelete(SocketChannelID channelID) = 0;
		
    	/// @brief 通过连接唯一标识得到连接
		virtual ISocketChannel<S>* getSocketChannelByChannelId(SocketChannelID channelID) = 0;

    	/// @brief 添加接收器
		virtual void addSocketAcceptor(const std::string& label, std::shared_ptr<ISocketAcceptor<S, T>> socketAcceptor) {
			this->socketAcceptors_[label] = socketAcceptor; 
		}
		
    	/// @brief 添加连接器
		virtual void addSocketConnector(const std::string& label, std::shared_ptr<ISocketConnector<S, T>> socketConnector) {
			this->socketConnector_[label] = socketConnector; 
		}
		
    	/// @brief 设置分发器
		virtual void setSocketRector(std::shared_ptr<ISocketRector<S, T>> socketRector) {
			this->socketRector_ = socketRector; 
		}
		
    	/// @brief 得到分发器
		std::shared_ptr<ISocketRector<S, T>> getSocketRector() {return this->socketRector_; }

    	/// @brief 通过label得到对应的接收器
		virtual std::shared_ptr<ISocketAcceptor<S, T>> getSocketAcceptor(const std::string& label)
		{
			auto iter = this->socketAcceptors_.find(label);
			if (this->socketAcceptors_.end() != iter)
			{
				return iter->second;
			}
		
			return std::shared_ptr<ISocketAcceptor<S, T>>(NULL);
		}
		
    	/// @brief 通过label得到对应的连接器
		virtual std::shared_ptr<ISocketConnector<S, T>> getSocketConnector(const std::string& label) 
		{
			auto iter = this->socketConnector_.find(label);
			if (this->socketConnector_.end() != iter)
			{
				return iter->second;
			}
		
			return std::shared_ptr<ISocketConnector<S, T>>(NULL);
		}
		
    	/// @brief 关闭连接管理器中管理的所有对象
		virtual void closeAll() = 0;
		
	protected:    	
    	/// @brief 所有接收器
		std::map<std::string, std::shared_ptr<ISocketAcceptor<S, T>>> socketAcceptors_;
    	/// @brief 所有连接器
		std::map<std::string, std::shared_ptr<ISocketConnector<S, T>>> socketConnector_;
		
    	/// @brief 事件分发器
		std::shared_ptr<ISocketRector<S, T>> socketRector_;
	};
	
	
    /// @brief 作为客户端的连接池
	/// @tparam S 用户连接对应的实际类型
    /// @tparam T 观察者模式中的观察者
	template <class S, class T>
	class ISocketChannelPool : public ILinkEvent<S>
	{
	public:
    	/**
		 *  @param socketConnector 连接器
		 *  @param minChannelCount 服务实例连接池的最小连接数
		 *  @param maxChannelCount 服务实例连接池的最大连接数
		 */
		ISocketChannelPool(std::shared_ptr<ISocketConnector<S, T>> socketConnector, uint16 minChannelCount, uint16 maxChannelCount)
			: socketConnector_(socketConnector)
			, minChannelCount_(minChannelCount)
			, maxChannelCount_(maxChannelCount)
		{}
		virtual ~ISocketChannelPool() {}		
		
    	/**
		 *	@brief 设置某服务实例的连接池最小连接数
		 *
		 *  @param serverUniqueID 服务实例唯一标识
		 *  @param minChannelCount 连接最小数量
		 */
		virtual bool setMinChannelCount(ServerUniqueID serverUniqueID, uint16 minChannelCount) = 0;
    	
    	/**
		 *	@brief 设置某服务实例的连接池最大连接数
		 *
		 *  @param serverUniqueID 服务实例唯一标识
		 *  @param minChannelCount 连接最大数量
		 */
		virtual bool setMaxChannelCount(ServerUniqueID serverUniqueID, uint16 maxChannelCount) = 0;
    	
    	/**
		 *	@brief 连接器对于连接不同服务可能会有差异，通过此接口来支持此差异
		 *
		 *  @param serverUniqueID 服务实例唯一标识
		 *  @param minChannelCount 连接器
		 */
		virtual bool setSocketConnector(ServerUniqueID serverUniqueID, std::shared_ptr<ISocketConnector<S, T>> socketConnector) = 0;
		
    	/**
		 *	@brief 从一个服务实例连接池中得到一个可用连接
		 *
		 *  @param serverUniqueID 服务实例唯一标识
		 *  @return 获取失败返回NULL，成功返回可用连接
		 */
		virtual ISocketChannel<S>* getSocketChannel(ServerUniqueID serverUniqueID) = 0;
    	
    	/**
		 *	@brief 连接使用完成的回收接口
		 *
		 *  @param socketChannel  之前从池中获取的连接
		 */
		virtual void releaseSocketChannel(ISocketChannel<S> *socketChannel) = 0;

    /// @brief 心跳消息设置
    virtual bool setHeartbeat(const std::string& heartbeatMessageData, MessageID heartbeatMessageID) = 0;
    	
	protected:
    	/// @brief 连接器
		std::shared_ptr<ISocketConnector<S, T>> socketConnector_;
    	/// @brief 服务实例连接池的最小连接数
		uint16 minChannelCount_;
    	/// @brief 服务实例连接池的最大连接数
		uint16 maxChannelCount_;
	};
	
    
    /// @brief 上层业务使用的类，不用关心底层细节，主要处理消息的发送
	class ISocketChannelHandleApi
	{
	public:
		virtual ~ISocketChannelHandleApi() {}
		
    	/**
		 *	@brief 向某服务实例发送消息
		 *
		 *  @param serverUniqueID 服务实例唯一标识
		 *  @param req 请求包
		 */
		virtual bool sendMessageToServer(ServerUniqueID serverUniqueID, std::shared_ptr<Request> req) = 0;
    	
    	/**
		 *	@brief 通过连接发送消息
		 *
		 *  @param channelID 连接唯一标识
		 *  @param req 请求包
		 */
		virtual bool sendMessageToClient(SocketChannelID channelID, std::shared_ptr<Request> req) = 0;
	};
	
	

    









}













