// Vector.h -- header file for Vector data structure project

#pragma once
#ifndef _Vector_h
#define _Vector_h

#include <cstdint>

namespace epl{

template <typename T>
class vector {
    
private:
    T** data;
    uint64_t length;
    uint64_t capacity;
    uint64_t const default_capacity = 8;
    
    void destroy(){
        for(int i=0; i<length; i++)
            delete data[i];
        delete[] data;
    }
    
    void copy(vector const & that){
        length = that.length;
        capacity = that.capacity;
        data = new T*[capacity];
        for(int i=0; i<length; i++)
            data[i] = new T(*that.data[i]);
    }
    
    void move(vector&& tmp){
        length = tmp.length;
        capacity = tmp.capacity;
        data = tmp.data;
    }
    
public:
    vector(void){
        data = new T*[default_capacity];
        length = 0;
        capacity = default_capacity;
    }
    
    vector(uint64_t const n){
        if(n == 0){
            vector();
            return;
        }
        capacity = n;
        length = n;
        data = new T*[n];
        for(int i=0; i<n; i++)
            data[i] = new T;
    }
    
    vector(vector const & that){ copy(that); }
    
    vector(vector&& tmp){ move(std::move(tmp)); }
    
    ~vector(void){ destroy(); }
    
};

} //namespace epl

#endif /* _Vector_h */
