//usage example

#include <iostream>
#include <memory>
#include <string>
#include "MessageManager.h"

class Bar : public ECS::Message<Bar, std::string>
{
public:
  Bar(std::string str)
		:Message(str)
	{}
};

class Foo : public ECS::Message<Foo, int, int>
{
public:
	Foo(int i, int j)
		:Message(i, j)
	{}
};

int main()
{
    std::shared_ptr<ECS::MessageManager> messages = ECS::MessageManager::make();
    
    int total = 0;
	std::string full;
	{
		auto ReceiverHandle0 = messages->addHandledReceiver<Bar>(std::function<void (const std::string&)>([&](const std::string& str) {full += str;}));
		auto ReceiverHandle1 = messages->addHandledReceiver<Foo>(std::function<void (int, int)>([&](int i, int j) {total -= i * j;}));
	
		messages->addMessage<Foo>(1, 2);
		messages->addMessage<Foo>(2, 4);
		messages->addMessage<Foo>(3, 8);

	
		messages->addMessage<Bar>("Hello ");
		messages->addMessage<Bar>("World!");
		messages->addMessage<Bar>("!");

		std::cout << "Messages not connected to the receivers yet!\n";
		std::cout << total << "\n";
		std::cout << "\"" << full  << "\"" << "\n";

		messages->connect();

		std::cout << "\nThough they are now!\n";
		std::cout << total << "\n";
		std::cout << "\"" << full  << "\"" << "\n";
	}

	messages->connect();
	std::cout << "\nAll reciever destroyed! :(\n";
	std::cout << total << "\n";
	std::cout << "\"" << full  << "\"" << "\n";
}
