#include <vector>
#include <type_traits>
#include <cassert>
#include <algorithm>
#include <memory>

namespace Observer
{

//! Base class for all listeners.
/*!
  User-defined implementation of listeners must inherit from this class.
  Implemented listeners should be passed as template parameters.
  This class basically defines a collection of listeners.
*/
template <typename... T_Listeners>
class Listener : public T_Listeners... {
public:
  using ListenerType = Listener<T_Listeners...>;

  virtual ~Listener() {}
};

//! A container of listeners, which holds raw pointers.
/*!
  This is a collection of pointers to listener objects of a single type,
  which is defined by the template parameter.
  The container is to be used in Source class.
*/
template <class T_Listener>
class RawContainer
{
public:
  virtual ~RawContainer() {}

  //! Attach listener \a listener to this container.
  /*!
    When attached, notifications are sent to the listener.
  */
  void attach(T_Listener* listener) 
  {
    assert(listener);
    m_listeners.push_back(listener);
  }

  //! Detach listener \a listener from this container.
  /*!
    When detached, notifications aren't sent to the listener anymore.
  */
  void detach(T_Listener* listener)
  {
    m_listeners.erase(std::remove_if(m_listeners.begin(), m_listeners.end(), listener));
  }

protected:
  
  //! Call a notification function as specified by the first parameter.
  /*!
    All listeners registered in this container are notified.
    The parameter pack is forwarded to that function.
  */
  template <typename... Fn_Args>
  void notify(void (T_Listener::*fn)(Fn_Args...), Fn_Args&&... args)
  {    
    for (auto& l : m_listeners)
     (l->*fn)(std::forward<Fn_Args>(args)...);
    }
  
private:
  std::vector<T_Listener*> m_listeners;
};

//! A container of listeners, which holds weak pointers.
/*!
  This is a collection of weak pointers to listener objects of a single type,
  which is defined by the template parameter.
  The container is to be used in Source class.
*/
template <class T_Listener>
class SmartContainer
{
public:
  virtual ~SmartContainer() {}

  //! Attach listener \a listener to this container.
  /*!
    When attached, notifications are sent to the listener.
    Note that a weak pointer to the listener is actually stored.
  */
  void attach(const std::shared_ptr<T_Listener>& listener) 
  {
    assert(listener);
    m_listeners.push_back(listener);
  }

  //! Detach listener \a listener from this container.
  /*!
    When detached, notifications aren't sent to the listener anymore.
  */
  void detach(const std::shared_ptr<T_Listener>& listener)
  {
    // Erase the given listener and all listeners, which have already expired.
    m_listeners.erase(std::remove_if(m_listeners.begin(), m_listeners.end(),
      [&listener](const auto& weakListener)
      { 
         auto ptr = weakListener.lock();
         return ptr == listener || ptr.empty();
      }));
  }

protected:
  
  //! Call a notification function as specified by the first parameter.
  /*!
    All listeners registered in this container are notified.
    The parameter pack is forwarded to that function.
  */  
  template <typename... Fn_Args>
  void notify(void (T_Listener::*fn)(Fn_Args...), Fn_Args&&... args)
  {    
    for (auto& weakListener : m_listeners)
      if (auto listener = weakListener.lock())
        (listener.get()->*fn)(std::forward<Fn_Args>(args)...);
  }
  
private:
  std::vector<std::weak_ptr<T_Listener>> m_listeners;
};


namespace detail
{
//! Convenience struct for identifying if a class \a T is in pack \a Args
/*!
  usage: contains<T, Args...>::value
*/
template <typename T, typename... Args>
struct contains 
  : public std::disjunction<std::is_same<T, Args>...>
{ };

//! Conditionally calls attach and detach methods of the source object
/*!
  This class defines the "true" condition.
*/
template <bool Contains, class T_Container>
struct cond {
  template<class T_Source, class T_Listener>
  static void attach(T_Source* s, const T_Listener& l) {
    static_cast<T_Container*>(s)->attach(l);
  }

  template<class T_Source, class T_Listener>
  static void detach(T_Source* s, const T_Listener& l) {
    static_cast<T_Container*>(s)->detach(l);
  }
};

//! Conditionally calls attach and detach methods of the source object
/*!
  This is an empty specialization for "false" condition.
*/
template<class T_Container>
struct cond<false, T_Container> {
  template<class T_Source, class T_Listener>
  static void attach(T_Source*, T_Listener) {}

  template<class T_Source, class T_Listener>
  static void detach(T_Source*, T_Listener) {}
};
}


//! Base class for all sources
/*!
  The source may be source of events for mutliple listeners. They are
  specified in the \a T_Listeners pack. Objects  of rach listener type 
  are stored in separate containers, however, the user may specify the
  type of the container in \a T_Container template parameter.
  
  Containers must have attach(), detach(), and notify methods. Currently,
  two containers are provided: RawContainer for holding raw pointers 
  and SmartContainer for holding weak pointers of listener objects.
  
  however, users may supply their own implementation.
*/
template <template<class> class T_Container, class... T_Listeners>
class Source
  : public T_Container<T_Listeners>... 
{
  using SourceType = Source<T_Container, T_Listeners...>;
public:

  //! Attach a listener object, which implements listeners given by Args.
  /*!
    This is an overload of attach method, which accepts a raw pointer to the listerner
    object. Note that only those listener types, which are provided in the T_Listeners
    pack are registered, others are ignored.
  */
  template<typename... Args>
  void attach(Listener<Args...>* listener)
  {
    using namespace detail;
    (cond<contains<Args, T_Listeners...>::value, T_Container<Args>>::attach(this, listener), ...);
  }

  //! Attach a listener object, which implements listeners given by Args.
  /*!
    This is an overload of attach method, which accepts a shared pointer to the listerner
    object. Note that only those listener types, which are provided in the T_Listeners
    pack are registered, others are ignored.
  */
  template<typename... Args>
  void attach(const std::shared_ptr<Listener<Args...>>& listener)
  {
    using namespace detail;
    (cond<contains<Args, T_Listeners...>::value, T_Container<Args>>::attach(this, listener), ...);
  }

  //! Convenience method to overcome the covariant issue with smart pointers.
  template<class T>
  void attach(const std::shared_ptr<T>& listener)
  {
    attach(std::static_pointer_cast<typename T::ListenerType>(listener));
  }
  
  //! Detach a listener object, which implements listeners given by Args.
  template<typename... Args>
  void detach(Listener<Args...>* listener)
  {
    using namespace detail;
    (cond<contains<Args, T_Listeners...>::value, T_Container<Args>>::detach(this, listener), ...);
  }
  
  //! Detach a listener object, which implements listeners given by Args.
  template<typename... Args>
  void detach(const std::shared_ptr<Listener<Args...>>& listener)
  {
    using namespace detail;
    (cond<contains<Args, T_Listeners...>::value, T_Container<Args>>::detach(this, listener), ...);
  }
  
  //! Convenience method to overcome the covariant issue with smart pointers.
  template<class T>
  void detach(const std::shared_ptr<T>& listener)
  {
    detach(std::static_pointer_cast<typename T::ListenerType>(listener));
  }

protected:

  //! Call a notification function as specified by the first parameter.
  /*!
    The parameter pack is forwarded to that function.
  */
  template <typename T, typename... Fn_Args>
  void notify(
    void (T::*fn)(Fn_Args...),
    Fn_Args&&... args) 
    {    
      T_Container<T>::notify(fn, std::forward<Fn_Args>(args)...);
    }
};

//! Shortcut for a source operating on raw pointers
template <class... T_Listeners>
using RawSource = Source<RawContainer, T_Listeners...>;

//! Shortcut for a source operating on smart pointers
template <class... T_Listeners>
using SmartSource = Source<SmartContainer, T_Listeners...>;

} // namespace Observer
