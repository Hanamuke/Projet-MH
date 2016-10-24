#ifndef GREEDYLIST_HPP_INCLUDED
#define GREEDYLIST_HPP_INCLUDED
#include <iostream>
#include <memory>

//the greedyList is a data structure that is of fixed size, chosen at creation. It is to be used exactly the same way as a list on the front-end.
//On the back end, it never allocate or deallocate memory, it is thus effectively faster than a list.
//It is especially useful if you have the possibility to reuse it, i.e. if you'd normally use .clear() on a list.
//In the particular event of chess programming, or recursive programming that uses lists in general, it is possible to use a single greedylist per depth (and per thread)
//which is a huge time benefit.

template<typename T>
class GreedyList
{
public:
    GreedyList():arr(NULL),maxSize(0),curr_size(0){}
    GreedyList(size_t s):arr(new T[s]),maxSize(s),curr_size(0){} //size in number of elements
    GreedyList(const GreedyList<T> &g) //copy constructor, reallocate memory and copy the content
    {
        if(g.begin()!=NULL && g.max_size()!=0)
        {
            maxSize=g.max_size();
            curr_size=g.size();
            arr=new T[maxSize];
            memcpy(arr,g.begin(),maxSize*sizeof(T));
        }
        else
        {
            arr=NULL;
            maxSize=0;
            curr_size=0;
        }
    }
    GreedyList& operator=(GreedyList<T> &g) //copy constructor, reallocate memory and copy the content
    {
        clear();
        if(g.begin()!=NULL && g.max_size()!=0)
        {
            maxSize=g.max_size();
            curr_size=g.size();
            arr=new T[maxSize];
            memcpy(arr,g.begin(),maxSize*sizeof(T));
        }
        else
        {
            arr=NULL;
            maxSize=0;
            curr_size=0;
        }
        return *this;
    }
    T* begin() const {return arr;}
    T* end() const {return arr+curr_size;}
    bool empty() const {return curr_size==0;}
    void clear() {curr_size=0;}
    size_t size() const {return curr_size;}
    size_t max_size() const {return maxSize;}
    void push_back(T const & _obj){arr[curr_size]=_obj; curr_size++;}
    void push_back(T && _obj){arr[curr_size]=_obj; curr_size++;}
    void pop_back(){if(curr_size) curr_size--;}
    void erase(T* it){curr_size--; *it=arr[curr_size];}///WARNING : CHANGES THE ORDER OF THE ELEMENTS
    void resize(size_t s)//FOR THE MOMENT IT DELETES THE CONTENT
    {
        if(arr!=NULL)
        delete[] arr;
        arr=new T[s];
        maxSize=s;
    }
    T& operator[](size_t i){return arr[i];}
    const T& operator[](size_t i) const {return arr[i];}
    ~GreedyList(){if(arr!=NULL) delete[] arr; arr=NULL;}
private:
    T* arr;
    size_t maxSize;
    size_t curr_size; //number of active elements.
};

#endif
