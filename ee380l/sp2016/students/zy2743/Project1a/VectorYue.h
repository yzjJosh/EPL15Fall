// Vector.h -- header file for Vector data structure project

#pragma once
#ifndef _Vector_h
#define _Vector_h

#include <cstdint>
#include <stdexcept>

namespace epl{
    
    template <typename T>
    class vector {
    private:
        uint64_t length;
        uint64_t capacity;
        T*   data;
        uint64_t begin;
        
        
        void copy(vector<T> const&  that){
            
            this->length = that.length;
            this->capacity = that.capacity;
            this->begin = 0;
            
            if(that.length==0){
                
                data = (T*)::operator new(8*sizeof(T));
                
            }else{
                data = (T*)::operator new(that.capacity*sizeof(T));
                
                for(uint64_t i = 0; i<that.length; i++){
                    uint64_t index = getIndex(i);
                    
                    new (data+i) T{that.data[index]};
                }
            }
        }
        
        
        
        
        void destroy(){
            
            if(length==0){
                ::operator delete(data);
            }else{
                for(uint64_t i = 0; i<length; i++){
                    uint64_t index = getIndex(i);
                    data[index].~T();
                }
                ::operator delete(data);
                
            }
            
        }
        
        void move(vector<T>&& that){
            this->length = that.length;
            this->capacity = that.capacity;
            this->begin = that.begin;
            
            this->data = that.data;
            that.length = 0;
            that.data = nullptr;
            
        }
        
    public:
        vector(void){
            capacity = 8;
            length = 0;
            data = (T*) ::operator new(8*sizeof(T));
            begin = 0;
        }
        
        vector(uint64_t n){
            if (n==0) {
                vector();
                return;
            }
            capacity = n;
            length = n;
            begin = 0;
            data = (T*) ::operator new(capacity*sizeof(T));
            
            for(uint64_t i = 0; i<capacity; i++){
                new (data+i) T{};
            }
            
        }
        
        vector(vector<T> const& that){
            copy(that);
        }
        
        vector(vector<T>&& that){
            move(std::move(that));
            
        }
        
        uint64_t getIndex(uint64_t i) const{
            return (i+(this->begin)+(this->capacity))%(this->capacity);
            
        }
        
        uint64_t getIndexForResize (uint64_t i, uint64_t begin, uint64_t capacity) const{
            
            return (i+begin+capacity)%capacity;
            
        }
        
        ~vector(void){
            destroy();
        }
        
        
        vector<T>& operator=(const vector<T>& that){
            if(this != &that){
                destroy();
                copy(that);
                
            }
            return *this;
            
        }
        
        
        
        vector<T>& operator=(vector<T>&& that){
            if(this != &that){
                destroy();
                move(std::move(that));
            }
            
            return *this;
        }
        
        
        
        T& operator[](uint64_t k){
            
            if(k>=length){
                throw std::out_of_range("index out of boubd");
            }
            else{
                k = getIndex(k);
                return data[k];
            }
        }
        
        const T& operator[](uint64_t k) const{
            
            if(k>=length){
                throw std::out_of_range("index out of boubd");
            }
            else{
                k = getIndex(k);
                return data[k];
            }
        }
        
        
        uint64_t size(void) const{
            return length;
        }
        
        
        
        void resize(){
            
            T* old_data = data;
            uint64_t old_begin = begin;
            uint64_t old_capacity = capacity;
            
            this->capacity = capacity*2;
            
            this->data = (T*) ::operator new(this->capacity * sizeof(T));
            this->begin = 0;
            
            for(uint64_t i=0; i<length; i++){
                uint64_t old_index = getIndexForResize(i,old_begin,old_capacity);
                new (data+i) T{std::move(old_data[old_index])};
                
            }
            ::operator delete (old_data);
        }
        
        void push_back(const T& elem){
            
            if(length==capacity){
                this->resize();
            }
            
            new (data+getIndex(length)) T{elem};
            length++;
            
        }
        
        void push_front(const T& elem){
            
            if(length==capacity){
                this->resize();
            }
            
            new (data+getIndex(-1)) T{elem};
            begin = getIndex(-1);
            length++;
        }
        
        void push_back(const T&& elem){
            
            if(length==capacity){
                this->resize();
            }
            
            new (data+(getIndex(length))) T{std::move(elem)};
            length++;
            
        }
        
        void push_front(const T&& elem){
            
            if(length==capacity){
                this->resize();
            }
            
            new (data+getIndex(-1)) T{std::move(elem)};
            begin = getIndex(-1);
            length++;
        }
        
        
        void pop_back(void){
            if(length==0){
                throw std::out_of_range("this vector is empty");
                
            }else{
                
                data[getIndex(length-1)].~T();
                length--;
                
            }
        }
        
        void pop_front(void){
            if(length==0){
                throw std::out_of_range("this vector is empty");
            }else{
                
                data[getIndex(0)].~T();
                begin = getIndex(1);
                length--;
            }
            
            
        }
    };
    
}   //namespace epl

#endif /* _Vector_h */
