This is a part of a bigger project for an Entity Component System implementation . This is my implementation 
of some sort of delayed messenger system. I needed a way to send messages to receivers, but the receivers 
should only receive messages at certain times.

Usage should be relatively simple:

To create a new type of message inherit from Message using CRTP and the signature (sort of) for the message. The 
message can only return void. Say you have a message called AwesomeThingHappened, that is an int describing how
awesome that particular message was, and a string describing the message itself:

  class AwesomeThingHappened : public Message<AwesomeThingHappened, int, std::string>
  {
    AwesomeThingHappened(int awesomeness, const std::string& str)
      :Message(awesomeness, string)  
    {}
  }
  
As you can see the constructor for the Message-class will accept the arguments specified in the template following
Derived.

To send this message you need a MessageManager:

  auto manager = MessageManager::make();
  
And you can send messages using that manager like this:

  manager->addMessage<AwesomeThingHappened>(1337, "Pretty big deal.");
  
To listen to messages you need to set up a receiver. This is a pretty labourious process, but should be easy enough.
There are two ways, the way I describe here should be safer and is managed using RAII. The receiver handle will remove
the receiver after it's destroyed. The function, addHandledReceiver, takes a template argument which is the message
that it listeneds to, as well as an std::function of the signature corresponding to the message:

  auto handle = manager->addHandledReceiver<AwesomeThingHappened>(std::function<void (int i, const std::string& str>
    ([&]() {std::cout << "The message: \"" << str << "\" has awesomeness level << i";});
  
The above receiver reads the message and outputs the string and the awesomeness level. 

There is one more thing. The messages are not sent to the receievers until they are connected using the connect()-
function in the manager:

  manager->connect();
  
Not until this line will the text be output to the console window. When connect is called, all the messages are
removed from the manager, but the receivers are left (should be obvious).
