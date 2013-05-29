#pragma once

#include <functional>
#include <stdint.h>
#include <limits>
#include <cassert>

namespace ECS
{
  //Only use for registration
	class BaseMessage
	{
	public:
		virtual ~BaseMessage() {};
		typedef uint64_t Family;

	protected:
		static Family familyCounter;
	};

	/*	use:
	 *		To create a Message-type for the function signature void ()
	 *		create a class that inherits from Message<Derived>
	 *		So, a message that emits two ints and is called MovedMessage inherits from
	 *		Message<MovedMessage, int, int>
	 *		Messages must have void as return value.
	 *		All Message-classes must provide a default constructor
	 */
	template <typename Derived, typename ... Signature>
	class Message : public BaseMessage
	{
	public:
		typedef void FunctionType (Signature ...); 

		//Sets up the call by binding the arguments
		Message(Signature ... args)
		{
			mConnectFunction = std::bind(&Message::internalConnect, this, std::placeholders::_1, args ...);
		};

		//calls the binded function with the reciever as placeholder
		void connect(std::function<void (Signature ...)>& reciever)
		{
			mConnectFunction(reciever);
		};

		static Family family();
	private:
		//this calls the function with the arguments, this works because it was binded before
		void internalConnect(std::function<void (Signature ...)>& reciever, Signature ... args)
		{
			reciever(args ...);
		}

		std::function<
			void (std::function<void (Signature ...)>)
				> mConnectFunction;
	};

	template <typename Derived, typename ... Signature>
	BaseMessage::Family Message<Derived, Signature ...>::family() 
	{
		static Family thisFamily = BaseMessage::familyCounter++;
		//make sure there are simply not too many Message-types
		assert(thisFamily != std::numeric_limits<uint64_t>::max() - 1);
		auto fam = thisFamily;
		return thisFamily;
	};

};
