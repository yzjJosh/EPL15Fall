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
        uint64_t start;
        uint64_t const default_capacity = 8;
        
        void destroy(T** array, uint64_t const len, uint16_t const lo, uint64_t const cap){
            for(uint64_t i=0; i<len; i++)
                delete array[getIndex(i, lo, cap)];
            delete[] array;
        }
        
        void copy(vector<T> const & that){
            length = that.length;
            capacity = that.capacity;
            data = new T*[capacity];
            start = 0;
            for(uint64_t i=0; i<length; i++)
                data[i] = new T(*that.data[getIndex(i, that.start, that.capacity)]);
        }
        
        void move(vector<T>&& tmp){
            length = tmp.length;
            capacity = tmp.capacity;
            data = tmp.data;
            start = tmp.start;
            tmp.length = 0;
            tmp.data = nullptr;
        }
        
        void resize(void){
            T** old_data = data;
            uint64_t old_c = capacity;
            capacity *= 2;
            data = new T*[capacity];
            for(uint64_t i=0; i<length; i++)
                data[i] = old_data[getIndex(i, start, old_c)];
            delete[] old_data;
            start = 0;
        }
        
        uint64_t getIndex(uint64_t const k, uint64_t const lo, uint64_t const cap) const{
            return (lo+k+cap) % cap;
        }
        
    public:
        vector(void){
            data = new T*[default_capacity];
            length = 0;
            capacity = default_capacity;
            start = 0;
        }
        
        vector(uint64_t const n){
            if(n == 0){
                vector();
                return;
            }
            capacity = n;
            length = n;
            data = new T*[n];
            start = 0;
            for(int i=0; i<n; i++)
                data[i] = new T;
        }
        
        vector(vector<T> const & that){ copy(that); }
        
        vector(vector<T>&& tmp){ move(std::move(tmp)); }
        
        ~vector(void){ destroy(data, length, start, capacity); }
        
        vector<T>& operator=(vector<T> const & that){
            if(this != &that){
                destroy(data, length, start, capacity);
                copy(that);
            }
            return *this;
        }
        
        vector<T>& operator=(vector<T>&& that){
            if(this != &that){
                destroy(data, length, start, capacity);
                move(std::move(that));
            }
            return *this;
        }
        
        uint64_t size() const { return length; }
        
        T& operator[](uint64_t const k){
            if(k >= length) throw std::out_of_range{"Index out of bound: " + std::to_string(k)};
            return *data[getIndex(k, start, capacity)];
        }
        
        const T& operator[](uint64_t const k) const{
            if(k >= length) throw std::out_of_range{"Index out of bound: " + std::to_string(k)};
            return *data[getIndex(k, start, capacity)];
        }
        
        void push_back(T const & item){
            if(length == capacity) resize();
            data[getIndex(length++, start, capacity)] = new T{item};
        }
        
        void push_back(T&& item){
            if(length == capacity) resize();
            data[getIndex(length++, start, capacity)] = new T{std::move(item)};
        }
        
        void push_front(T const & item){
            if(length == capacity) resize();
            data[start = getIndex(-1, start, capacity)] = new T{item};
            length ++;
        }
        
        void push_front(T&& item){
            if(length == capacity) resize();
            data[start = getIndex(-1, start, capacity)] = new T{std::move(item)};
            length ++;
        }
        
        void pop_back(void){
            if(length == 0) throw std::out_of_range{"Unable to pop from empty vector!"};
            delete data[getIndex(--length, start, capacity)];
        }
        
        void pop_front(void){
            if(length == 0) throw std::out_of_range("Unable to pop from empty vector!");
            delete data[start];
            start = getIndex(1, start, capacity);
            length--;
        }
        
    };
    
} //namespace epl

#endif /* _Vector_h */
