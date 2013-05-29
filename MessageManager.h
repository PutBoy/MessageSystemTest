#pragma once

#include "Message.h"
#include <functional>
#include <vector>
#include <memory>
#include "Utility.h"

class BaseMessage;

namespace ECS
{
  class BaseReceiver;
	
	class MessageManager;

	template <typename MessageType, typename FunctionType>
	class ReceiverHandle
	{
	public:
		ReceiverHandle(std::shared_ptr<MessageManager> manager, std::shared_ptr<FunctionType> function)
			:mManager(manager)
			,mFunction(function)
		{
		}

		~ReceiverHandle()
		{
			if (mManager.expired())
				return;

			mManager.lock()->removeReceiver<MessageType>(mFunction);
		}
	private:
		//weak in case the manager has gone been destroyed already.
		std::weak_ptr<MessageManager> mManager;
		std::shared_ptr<FunctionType> mFunction;
	};

	class MessageManager : public std::enable_shared_from_this<MessageManager> , public Noncopyable
	{
	public:
		typedef std::vector<std::pair<BaseMessage::Family, std::shared_ptr<BaseMessage>>> MessageVector;

		static std::shared_ptr<MessageManager> make()
		{
			return std::make_shared<MessageManager>();
		}

		template <typename MessageType>
		void addMessage(std::shared_ptr<MessageType> message)
		{
			mMessages.emplace_back(MessageType::family(), message);
		}

		template <typename MessageType, typename ... Args>
		void addMessage(Args ... args)
		{
			mMessages.emplace_back(MessageType::family(), std::make_shared<MessageType>(args ...));
		}

		//this adds as a RAII-powered way to add a Receiver.
		//A shared_ptr of the function will be created and will be kept in the
		//ReceiverHandle-Object returned.
		//The ReceiverHandle-object will remove the Receiver when it goes out
		//of scope.
		//The Receiver must be an std::function object.
		template <typename MessageType, typename FunctionType>
		std::shared_ptr<ReceiverHandle<MessageType, FunctionType>> addHandledReceiver(FunctionType function) 
		{
			auto functionPtr = std::make_shared<FunctionType>(function);
			addReceiver<MessageType>(functionPtr);
			
			return std::make_shared<ReceiverHandle<MessageType, FunctionType>>
				(getThis(), functionPtr);
		}

		//Adds a Receiver which is a functions that listens to a message. 
		//Send in a shared_ptr to your function. This is then used when removing
		//the Receiver too.
		//The Receiver must be an std::shared_ptr<std::function> object.
		template <typename MessageType, typename FunctionType>
		void addReceiver(std::shared_ptr<FunctionType> function)
		{
			std::shared_ptr<BaseReceiver> receiver = std::make_shared<Receiver<MessageType, FunctionType>>(function);
			mReceivers.push_back(receiver);
		}
		
		template <typename MessageType, typename FunctionType>
		void removeReceiver(std::shared_ptr<FunctionType> function)
		{
			for (auto iter = mReceivers.begin(); iter != mReceivers.end(); ++iter)
			{
				if ((*iter)->family() == MessageType::family())
				{
					std::shared_ptr<Receiver<MessageType, FunctionType>> receiver = 
						std::static_pointer_cast<Receiver<MessageType, FunctionType>>(*iter);
					if (receiver->getFunction() == function)
					{
						mReceivers.erase(iter);
						break;
					}
				}
			}
		}

		//this function calls all the Receivers with the messages as arguments.
		void connect();


		MessageManager()
		{
		};

	private:
		std::shared_ptr<MessageManager> getThis()
		{
			return shared_from_this();
		}

		MessageVector mMessages;
		std::vector<std::shared_ptr<BaseReceiver>> mReceivers;
	};

	class BaseReceiver
	{
	public:
		virtual ~BaseReceiver() {};
		virtual void connect(MessageManager::MessageVector& vector) = 0;
		virtual BaseMessage::Family family() = 0;
	};

	
	template <typename MessageType, typename FunctionType>
	class Receiver : public BaseReceiver
	{
	public:
		Receiver(){};

		Receiver(std::shared_ptr<FunctionType> function)
			:mFunction(function)
		{
		};

		virtual BaseMessage::Family family()
		{
			auto fam = MessageType::family();
			return MessageType::family();
		};

		virtual void connect(MessageManager::MessageVector& messages)
		{
			for (auto iter = messages.begin(); iter != messages.end(); ++iter)
			{
				if (iter->first == MessageType::family())
				{
					if (!mFunction.expired())
					{
						auto thisMessage = std::static_pointer_cast<MessageType>(iter->second);
						thisMessage->connect(*mFunction.lock());
					}
				}
			}
		};

		std::shared_ptr<FunctionType> getFunction() {return mFunction.lock();};
	private:
		typedef void F (int);

		std::weak_ptr<FunctionType> mFunction;
	};

};