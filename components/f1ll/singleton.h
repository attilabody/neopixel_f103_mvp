#ifndef SINGLETON_H_
#define SINGLETON_H_

#include <utility>

template<typename T> class Singleton {
public:
    static T &Instance() { return *m_instance; }
    template<typename ... Args>
    static T &Init(Args &&... args)
    {
        static T instance{ std::forward<Args>(args)... };
       	m_instance = &instance;
        return instance;
    }

protected:
    Singleton() = default;
    Singleton(const Singleton &) = delete;
    Singleton &operator=(const Singleton &) = delete;
    static T *m_instance;
};

template<typename T> T* Singleton<T>::m_instance = nullptr;

#endif /* SINGLETON_H_ */
