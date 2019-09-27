#pragma once


#include <string>
#include <vector>


#include "CoordinationClient/baseType.h"
#include "CoordinationClient/ICoordinationEventHandler.h"


using std::string;


namespace coordination
{

	class ICoordinationClientApi
	{
	public:
    	ICoordinationClientApi() {}
    	virtual ~ICoordinationClientApi() {}

		
        /**
         *	@brief ��ʼ��
         *
         *  @param coordinationAddress ���� "172.16.11.11:10000, 172.16.11.12:10000, 172.16.11.13:10000"
         */
		virtual bool initialize(const string& coordinationAddress) = 0;

		virtual bool start() = 0;
		virtual void stop() = 0;
		

        /// @brief ���ýڵ�ֵ, ֧��key�������򴴽�(����ָ��)
		virtual bool setNodeValue(const KeyValue& keyValue, bool isCreate = false) = 0;

        /// @brief ɾ���ڵ�
		virtual bool delNode(const string& key) = 0;

        /// @brief ��ȡ�ڵ�ֵ
		virtual bool getNodeValue(const string& key, KeyValue& keyValue) = 0;

        /// @brief �����ڵ���, һ��keyֻ�ܴ���һ������
		virtual bool watchNode(const string& nodeKey, ICoordinationEventHandler *handler) = 0;

        /// @brief ȡ�������ڵ���
		virtual bool cancelWatchNode(const string& nodeKey) = 0;

        /// @brief ��ȡĿ¼child��Ϣ
		virtual bool getChildNode(const string& nodeKey, std::vector<KeyValue>& childNodeInfo) = 0;

        /// @brief ����Ŀ¼�½ڵ���, һ��keyֻ�ܴ���һ������
		virtual bool watchChildNode(const string& nodeKey, ICoordinationEventHandler *handler) = 0;

        /// @brief ȡ������Ŀ¼�½ڵ���
		virtual bool cancelWatchChildNode(const string& nodeKey) = 0;


        /**
         *	@brief ע�����
         *
         *  @param timeOutSTime ��ʾ�೤ʱ��û�����罻���ʹ�������쳣(��λ����)
         */
		virtual bool registerServer(const string& serverKey, const string& serverValue, int32 timeOutSTime) = 0;


        /**
         *	@brief ���ò���master��ѡ
         *
         *  @param name ��ѡ�ٵı�ʶ���������μӾ�ѡ
         *  @param value �Ǿ�ѡ��Ӯ��ѡ��ʱ���õĳ�ʼ������ֵ
         *  @param timeOutSTime ��ʾ�೤ʱ��û�����罻���ʹ�������쳣(��λ����)
         */
		virtual bool campaignLeader(const string& name, const string& value, int32 timeOutSTime) = 0;

		
        /**
         *	@brief ȡ������master��ѡ
         *
         *  @param name ��ѡ�ٵı�ʶ���������μӾ�ѡ
         */
		virtual bool resignLeader(const string& name) = 0;

        /**
         *	@brief ��ȡmaster
         *
         *  @param name ��ѡ�ٵı�ʶ���������μӾ�ѡ
         *  @param leaderInfo ���ص�leader��Ϣ
         *  @param isOwner �������Ƿ�Ϊleader
         */
		virtual bool leader(const string& name, KeyValue& leaderInfo) = 0;

        /// @brief ����leader���
		virtual bool watchLeader(const string& name, ICoordinationEventHandler *handler) = 0;

        /// @brief ȡ������leader���
		virtual bool cancelWatchLeader(const string& name) = 0;


		
        /**
         *	@brief �ڸ����������ϻ�÷ֲ�ʽ������
         *
         *  @param name ��Ҫ��ȡ�ķֲ�ʽ�������ı�ʶ
         *  @param timeOutMTime ����ʱʱ��(��λ����)
         *  @return CEC_Success: ��ʾ�����ɹ�
		 *		false: ��ʾ���ѱ�ռ��
		 *		��������ʾʧ��
         */
		virtual bool lock(const string& name, int32 timeOutSTime) = 0;

		
        /**
         *	@brief �ͷŶ����ĳ���
         *
         *  @param lockKey lock�ӿڷ��ص�
         */
		virtual bool unlock(const string& name) = 0;




	};







}








