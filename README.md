This is a part of a bigger project for an Entity Component System implementation . This is my implementation
of a deffered messenger system. I needed a way to send messages to receivers, but the receivers should only receive messages at certain times. It uses a signals and slot system. Messages are sent to a hub, which can then be invoked to send these messages as signals to the slots that are set up to recieve these signals. A message can take any parameters, but can only have a void return type.

Usage should be relatively simple:

To create a new type of message: inherit from Message using CRTP and the signature (that is a list of all parameters the message takes) for the message. The message can only return void. Say you have a message called AwesomeThingHappened, that is an int describing how awesome that particular message was, and a string describing the message itself:

```c++
  class AwesomeThingHappened : public Message<AwesomeThingHappened, int, std::string>
  {
    AwesomeThingHappened(int awesomeness, const std::string& str)
      :Message(awesomeness, string)  
    {}
  }
```

As you can see the constructor for the Message-class will accept the arguments specified in the template following
Derived.

To send this message you need a MessageManager. The MessageManager should only be created using the static make()-method:

```c++
  auto manager = MessageManager::make();
```

You can send messages to that manager like this:

```c++
  manager->addMessage<AwesomeThingHappened>(1337, "Pretty big deal.");
```

To listen to messages you need to set up a receiver. This is should be easy enough. There are two ways, the way I describe here is safer and should be prefered. It uses a handler object around a reciever ID which is managed using RAII. The receiver handle will remove the receiver ID after it's destroyed. The function, addHandledReceiver, takes a template argument which is the message that it listenes to, as well as an std::function of the signature corresponding to the message:

```c++
  auto handle = manager->addHandledReceiver<AwesomeThingHappened>(std::function<void (int i, const std::string& str>
    ([&]() {std::cout << "The message: \"" << str << "\"" has awesomeness level << i";});
```  
  
The above receiver reads the message and outputs the string and the awesomeness level. 

There is one more thing. As the messages are not sent to the receievers until they are connected, you must call the connect()-function in the manager:

```
  manager->connect();
```

Output: "The message: "Pretty big deal." has awesomness level 1337".

Not until this line will the text be output to the console window. When connect is called, all the messages are
connected to their respective slots and removed from the manager, but the receivers are left (should be obvious).
