#pragma once

#include <string>


#include "CoordinationClient/baseType.h"
#include "CoordinationClient/ICoordinationEventHandler.h"


using std::string;


namespace coordination{

    class ICoordinationReactor
    {
    public:
        ICoordinationReactor() = default;
        virtual ~ICoordinationReactor() = default;


        /// @brief 监听节点变更
		    virtual bool watchNode(const string& nodeKey, ICoordinationEventHandler *handler) = 0;

        /// @brief 取消监听节点变更
		    virtual bool cancelWatchNode(const string& nodeKey) = 0;


        /// @brief 监听目录下节点变更
		    virtual bool watchChildNode(const string& nodeKey, ICoordinationEventHandler *handler) = 0;

        /// @brief 取消监听目录下节点变更
		    virtual bool cancelWatchChildNode(const string& nodeKey) = 0;


        /// @brief 监听leader变更
		    virtual bool watchLeader(const string& name, ICoordinationEventHandler *handler) = 0;

        /// @brief 取消监听leader变更
		    virtual bool cancelWatchLeader(const string& name) = 0;

    };



}











