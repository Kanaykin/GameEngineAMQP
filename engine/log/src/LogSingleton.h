#pragma once

namespace logger
{

template<class T>
class SingletonWrapper : public T
{
    static bool & getIsDestroyed(){
        // Prefer a static function member to avoid LNK1179.
        // Note: As this is for a singleton (1 instance only) it must be set
        // never be reset (to false)!
        static bool isDestroyedFlag = false;
        return isDestroyedFlag;
    }
public:
    SingletonWrapper(){
        assert(! isDestroyed());
    }
    ~SingletonWrapper(){
        getIsDestroyed() = true;
    }
    static bool isDestroyed(){
        return getIsDestroyed();
    }
};

template <class T>
class LogSingleton
{
public:
    ~LogSingleton() = default;
    static T & getInstance()
    {
        assert(! isDestroyed());
        static SingletonWrapper< T > t;
        return static_cast<T &>(t);
    }
    
    static bool isDestroyed(){
        return SingletonWrapper< T >::isDestroyed();
    }
protected:
    LogSingleton() {}
private:
    static T * m_instance;
    // Copy semantics : OFF
    LogSingleton(LogSingleton const &) = delete;
    LogSingleton& operator=(LogSingleton const &) = delete;

    // Move semantics : ON
    LogSingleton(LogSingleton &&) = default;
    LogSingleton& operator=(LogSingleton &&) = default;
};

template<class T>
T * LogSingleton< T >::m_instance = &LogSingleton< T >::getInstance();

}
