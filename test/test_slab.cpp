#include <iostream>
using namespace std;

#include "Slab.h"

class A:public Slab<A>
{
public:
    A(int s):size(s)
    {
        std::cout << "class A"<<std::endl;
    }
    
    ~A()
    {
        std::cout << "class ~A"<<std::endl;
    }
    
    void show()
    {
        std::cout <<"size:"<<size<<std::endl;
    }
    
private:
    int size;
};

    
int main(void)
{
    //Slab<A> *pA = new Slab<A>();
    A *pA = new A(19);
    
    pA->show();
    
    delete pA;
    
    return 0;
}
