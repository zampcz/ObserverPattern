//! [MouseListener]
class MouseListener {
public:
  virtual ~MouseListener() {}
  virtual void onLeftMouseButtonUp(int x, int y) {}
  virtual void onLeftMouseButtonDown(int x, int y) {}
  virtual void onRightMouseButtonUp(int x, int y) {}
  // ...
};
//! [MouseListener]

//! [NaiveMouseSource]
class MouseSource {
public:
  // ...
  void attachMouseListener(MouseListener* listener);
  void detachMouseListener(MouseListener* listener);
  
protected:
  void notifyLeftMouseButtonUp(int x, int y);
  void notifyLeftMouseButtonDown(int x, int y);
  // ...
  
private:
  std::vector<MouseListener*> m_mouseListeners;
};
//! [NaiveMouseSource]

//! [SimpleMouseExample]
class ConcreteMouseListener : public Observer::Listener<MouseListener> {
public:
  void onLeftMouseButtonUp(int x, int y) override {
    // ...
  }
};

class MouseSource : public Observer::RawSource<MouseListener> {
public:
  void f() {
    int x, y;
    // ...
    notify(&MouseListener::onLeftMouseButtonUp, x, y);
    // ...
  }
};

void test() {
  ConcreteMouseListener listener;
  MouseSource source;
  
  source.attach(&listener);   // attach the listener
  source.f();                 // call f() method, which emits onLeftMouseButtonUp notification
  source.detach(&listener);   // detach the listener
}

//! [SimpleMouseExample]

//! [MultipleListenersExample]
class ConcreteListener : public Observer::Listener<ListenerA, ListenerB> {
public:
  // ...
};

class Source : public Observer::RawSource<ListenerB, ListenerC> {
public:
  // ...
};

void test() {
  ConcreteListener listener;
  Source source;
  
  // Attach the listener. Only abstract ListenerB is actually attached to the source
  // as ListenerA is not supported by the source and ListenerC is not implemented in
  // in the concrete listener.
  source.attach(&listener);
  //...
}

//! [MultipleListenersExample]
