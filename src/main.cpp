#include <iostream>

#include "observer.h"

class MouseListener {
public:
  virtual ~MouseListener() {}
  virtual void onLeftMouseButton(int, int) = 0;
};

class KeyboardListener {
public:
  virtual ~KeyboardListener() {}
  virtual void onKeyPressed(int) = 0;
};

class MouseAndKeyboardSource
  : public Observer::RawSource<MouseListener, KeyboardListener> {
public:
  void test() 
  {
    std::cout << "notification MouseListener::onLeftMouseButton\n";
    notify(&MouseListener::onLeftMouseButton, 25, 48);
    std::cout << "notification KeyboardListener::onKeyPressed\n";
    notify(&KeyboardListener::onKeyPressed, 65);
  }
};

class MouseOnlySource
  : public Observer::RawSource<MouseListener> {
public:
  void test() 
  {
    std::cout << "notification MouseListener::onLeftMouseButton\n";
    notify(&MouseListener::onLeftMouseButton, 95, 105);
  }
};

class SmartMouseOnlySource
  : public Observer::SmartSource<MouseListener> {
public:
  void test()
  {
    std::cout << "notification MouseListener::onLeftMouseButton\n";
    notify(&MouseListener::onLeftMouseButton, 27, 163);
  }
};
     
class MouseOnlyObserver : public Observer::Listener<MouseListener> {
public:
  void onLeftMouseButton(int a, int b) 
  {
    std::cout << " - MouseOnlyObserver::onLeftMouseButton(" << a << ", " << b << ") received\n";
  }
};

class MouseAndKeyboardObserver 
  : public Observer::Listener<MouseListener, KeyboardListener> {
public:
  void onLeftMouseButton(int a, int b)
  {
    std::cout << " - MouseAndKeyboardObserver::onLeftMouseButton(" << a << ", " << b << ") received\n";
  }
  
  void onKeyPressed(int k) 
  {
    std::cout << " - MouseAndKeyboardObserver::onKeyPressed(" << k << ") received\n";
  }
};


int main()
{
  std::cout << "Mouse and keyboard test\n";
  {
    // Source object, which emits notifications for mouse and keyboard listeners
    MouseAndKeyboardSource src;
    // Observer object, which receives only mouse notification
    MouseOnlyObserver mouseOnlyObserver;
    // Observer object, which receives mouse and keyboard notification
    MouseAndKeyboardObserver mouseAndKeyboardObserver;
    // Mouse only listener is registered to the source
    src.attach(&mouseOnlyObserver);
    // Mouse and keyboard listener is registered to the source
    src.attach(&mouseAndKeyboardObserver);
    // Emits left mouse button and key pressed notification
    src.test();
  }
  
  std::cout << "\nMouse only test\n";
  {
    // Source object, which emits notifications for mouse listener
    MouseOnlySource src;
    // Observer object, which receives only mouse notification
    MouseOnlyObserver mouseOnlyObserver;
    // Observer object, which receives mouse and keyboard notification
    MouseAndKeyboardObserver mouseAndKeyboardObserver;
    // Mouse only listener is registered to the source
    src.attach(&mouseOnlyObserver);
    // Mouse listener is registered to the source and keyboard listener is ignored
    src.attach(&mouseAndKeyboardObserver);
    // Emits left mouse button
    src.test();
  }

  std::cout << "\nSmart mouse only test\n";
  {
    // Source object, which emits notifications for mouse listener
    SmartMouseOnlySource src;
    // Observer object, which receives only mouse notification
    auto observer = std::make_shared<MouseOnlyObserver>();
    src.attach(observer);
    // Emits left mouse button, which is received by the observer object
    src.test();
    // Reset observer object, no need to detach it
    observer.reset();
    // Emits left mouse button, but no observer is notified now
    src.test();
  }
  return 0;
}
