#ifndef __SLAB_H_
#define __SLAB_H_

#include "slab.h"
#include "Singleton.h"
    
#include <iostream>
#include <typeinfo>

template <typename T>
class _Slab
{
protected:
	friend class Singleton< _Slab<T> >;
public:
    _Slab()
    {
        
        std::cout <<"T:"<<typeid(T).name()<<" size:"<<sizeof(T)<<std::endl;
        if((m_pSlabCache = umem_cache_create(typeid(T).name(),sizeof(T),ALIGN4,SLAB_NOSLEEP,NULL,NULL)) == NULL)
            std::cout <<"Create Slab Error"<<std::endl;
    }
    
    ~_Slab()
    {
        umem_cache_destroy(m_pSlabCache);
    }

    void* alloc(std::size_t size)
    {
        return umem_cache_alloc(m_pSlabCache);
    }
    
    void dealloc(void *p,std::size_t size)
    {
        umem_cache_free(m_pSlabCache,p);
    }
    
private:
    umem_cache_t* m_pSlabCache;
};

template <typename T>
class Slab
{
public:
    static void* operator new(std::size_t size)
    {
        std::cout <<"operator new:"<<size<<std::endl;
        return Singleton < _Slab <T> >::getInstance()->alloc(size);
    }
    static void operator delete(void *p,std::size_t size)
    {
        std::cout << "delete" << std::endl;
        Singleton< _Slab <T> >::getInstance()->dealloc(p,size);
    }
        
    virtual ~Slab()
    {
    }
};



#endif
