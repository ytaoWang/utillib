#ifndef __SINGLETON_H_
#define __SINGLETON_H_
#include <iostream>
#include <typeinfo>

template <typename T>
class Singleton
{
public:
    static T* getInstance()
    {
        static T t;
        std::cout << typeid(T).name() << endl;
        return &t;
    }
private:
    Singleton()
    {
    }
    Singleton(const Singleton&);
    Singleton& operator=(const Singleton &);
    ~Singleton()
    {
    }
};

    

#endif
