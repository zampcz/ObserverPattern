/*!
  \mainpage

  \section intro Introduction

  This is a C++ implementation of the observer pattern as you may know it from Java listeners. 
  Thanks to C++ template magic, client implementation of an observer source class is 
  very simple and requires almost no coding at all.

  \section download Download

  The source code of this library is available on <a href="https://github.com/zampcz/ObserverPattern">GitHub</a>.
  The code is licenced under MIT licence.

  \section naive Possible implementation

  A naive implementation of the pattern require the developer to add three methods and one
  container for each listener. Say we have a \a MouseListener interface, which listens to mouse events 
  and \a MouseSource class, which emits those events. The implementation of the \a MouseListener is
  straightforward:
  
  \snippet snippets.cpp MouseListener
  
  The implementation of the source class, however, requires some manual work:

  \snippet snippets.cpp NaiveMouseSource
  
  As you can see, there is a lot of repetitive manual work to implement all the notify functions as well as 
  attach and detach methods. When we later decide that the source should be actually source of keyboard event as well, 
  things get much worse. Now the class would have to keep two lists of listeners, two pairs of attach and detach 
  methods and number of notify functions, all of them iterating listeners and calling correct listener function.
  
  \section simple Implementation using the Observer namespace
  
  If one uses the provided Observer library, the implementation of the \a MouseListener interface doesn't 
  have to change at all. The Observer library makes the source implementation is much simpler though. The concrete 
  mouse listener implementation must inherit from Observer::Listener and not from the listener directly as shown 
  in the following example.

  \snippet snippets.cpp SimpleMouseExample

  As you can see the source implementation doesn't have to define \a attach, \a detach, or \a notify* functions.
  They are all implemented in the Observer::RawSource class. \a RawSource denotes that raw pointers are used for 
  storing the listenres. If you prefer smart pointers instead, please use Observer::SmartSource.
  
  \section complex Support of mutiple listeners
  
  The aforementioned example is already an improvement but what if the source should support multiple listeners,
  or the concrete listener is composed of multiple abstract listeners? This is not a problem for the Observer
  library and it supports both scenarios. Consider the following example:

  \snippet snippets.cpp MultipleListenersExample

  Say we have three abstract listeners \a ListenerA, \a ListenerB, and \a ListenerC. The concrete listener implemens
  only \a ListenerA and \a ListenerB, whereas the source implementation supports \a ListenerB and \a ListenerC.
  Clearly, the only abstract listener supported by the concrete listener and the source is \a ListenerB.
  Still, the user may attach the concrete listener to the source without a compiler error and only the correct
  abstract interfaces are taken into account and attached to the source as one would expect.
*/
