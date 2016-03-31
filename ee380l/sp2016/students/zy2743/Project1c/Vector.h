#ifndef _VECTOR_H_
#define _VECTOR_H_

#include <cstdint>
#include <stdexcept>
#include <utility>

//Utility gives std::rel_ops which will fill in relational
//iterator operations so long as you provide the
//operators discussed in class.  In any case, ensure that
//all operations listed in this website are legal for your
//iterators:
//http://www.cplusplus.com/reference/iterator/RandomAccessIterator/
using namespace std::rel_ops;

namespace epl{

    class invalid_iterator {
    public:
        enum SeverityLevel {SEVERE,MODERATE,MILD,WARNING};
        SeverityLevel level;
        
        invalid_iterator(SeverityLevel level = SEVERE){ this->level = level; }
        virtual const char* what() const {
            switch(level){
                case WARNING:   return "Warning"; // not used in Spring 2015
                case MILD:      return "Mild";
                case MODERATE:  return "Moderate";
                case SEVERE:    return "Severe";
                default:        return "ERROR"; // should not be used
            }
        }
    };

    template <typename T>
    class vector {
    private:
        T* data;
        uint64_t length;
        uint64_t capacity;
        uint64_t start;
        uint64_t version;
        uint64_t reallocation_version;
        static uint64_t const default_capacity = 8;
        
        //Definition of iterator_helper
        template <typename Elem>
        class iterator_helper: public std::iterator<std::random_access_iterator_tag, Elem>{
        protected:
            Elem* ptr;
            uint64_t version;
            uint64_t index;
            vector<T>* home;
            
            void check_valid(void) const{
                if(index > home->length)
                    throw invalid_iterator{invalid_iterator::SEVERE};
                if(home->version != version){
                    if(version < home->reallocation_version)
                        throw invalid_iterator{invalid_iterator::MODERATE};
                    else
                        throw invalid_iterator{invalid_iterator::MILD};
                }
            }
            
        public:
            iterator_helper(vector<T> const& home, const uint64_t index){
                this->ptr = home.data + vector::getIndex(index, home.start, home.capacity);
                this->version = home.version;
                this->index = index;
                this->home = &home;
            }
            
            Elem& operator*(void) const{
                check_valid();
                return *ptr;
            }
            
            iterator_helper<Elem> operator+ (const uint64_t step) const{
                check_valid();
                iterator_helper<Elem> res{*this};
                res.ptr = home->data + vector::getIndex(res.index += step, home->start, home->capacity);
                return res;
            }
            
            iterator_helper<Elem> operator- (const uint64_t step) const{
                return operator+ (-step);
            }
            
            iterator_helper<Elem>& operator+= (const uint64_t step) {
                check_valid();
                ptr = home->data + vector::getIndex(index += step, home->start, home->capacity);
                return *this;
            }
            
            iterator_helper<Elem>& operator-= (const uint64_t step) {
                check_valid();
                ptr = home->data + vector::getIndex(index -= step, home->start, home->capacity);
                return *this;
            }
            
            iterator_helper<Elem>& operator++ (void){
                return operator+=(1);
            }
            
            iterator_helper<Elem>& operator-- (void){
                return operator-=(1);
            }
            
            iterator_helper<Elem> operator++ (int){
                iterator_helper<Elem> res{*this};
                operator++();
                return res;
            }
            
            iterator_helper<Elem> operator-- (int){
                iterator_helper<Elem> res{*this};
                operator--();
                return res;
            }
            
            int64_t operator- (iterator_helper<Elem> const& that) const{
                check_valid();
                that.check_valid();
                return ((int64_t)index) - ((int64_t)that.index);
            }
            
            bool operator== (iterator_helper<Elem> const& that) const{
                check_valid();
                that.check_valid();
                return this->ptr == that.ptr;
            }
            
            bool operator< (iterator_helper<Elem> const& that) const{
                check_valid();
                that.check_valid();
                if(home != that.home) return false;
                return index < that.index;
            }
        
        };
        
    public:
        using iterator = iterator_helper<T>;
        
    private:
        class const_iterator: public iterator_helper<const T>{
        private:
            void copy(iterator const& that){
                this->ptr = that.ptr;
                this->index = that.index;
                this->version = that.version;
                this->home = that.home;
            }
            
        public:
            const_iterator(vector<T> const& home, const uint64_t index):
                iterator_helper<T>(home, index){}
            
            const_iterator(iterator const& that){
                copy(that);
            }
            
            const_iterator& operator= (iterator const& that){
                if(this != &that)
                    copy(that);
                return *this;
            }
            
        };
        
        void destroy(T* array, uint64_t const len, uint16_t const lo, uint64_t const cap){
            for(uint64_t i=0; i<len; i++)
                array[getIndex(i, lo, cap)].~T();
            ::operator delete (array);
        }
        
        void copy(vector<T> const & that){
            length = that.length;
            capacity = that.capacity;
            data = (T*)::operator new (capacity*sizeof(T));
            start = 0;
            version ++;
            reallocation_version = version;
            for(uint64_t i=0; i<length; i++)
                new (data+i) T{that.data[getIndex(i, that.start, that.capacity)]};
        }
        
        void move(vector<T>&& tmp){
            length = tmp.length;
            capacity = tmp.capacity;
            data = tmp.data;
            start = tmp.start;
            tmp.length = 0;
            tmp.data = nullptr;
            version ++;
            reallocation_version = version;
        }
        
        void resize(void){
            T* old_data = data;
            uint64_t old_c = capacity;
            capacity *= 2;
            data = (T*)::operator new (capacity*sizeof(T));
            for(uint64_t i=0; i<length; i++)
                new (data+i) T{std::move(old_data[getIndex(i, start, old_c)])};
            ::operator delete (old_data);
            start = 0;
            reallocation_version = version;
        }
        
        static uint64_t getIndex(uint64_t const k, uint64_t const lo, uint64_t const cap){
            return (lo+k+cap) % cap;
        }
        
        template <typename It>
        vector(It begin, It end, std::forward_iterator_tag): vector(){
            while(end != begin)
                this->push_back(*(begin++));
        }
        
        template <typename It>
        vector(It begin, It end, std::random_access_iterator_tag){
            capacity = length = end - begin;
            data = (T*)::operator new (length*sizeof(T));
            start = 0;
            version = 0;
            reallocation_version = 0;
            for(uint64_t i=0; i<length; i++, begin++)
                new (data+i) T{*begin};
        }
        
    public:
        
        vector(void){
            data = (T*)::operator new (default_capacity*sizeof(T));
            length = 0;
            capacity = default_capacity;
            start = 0;
            version = 0;
            reallocation_version = 0;
        }
        
        vector(uint64_t const n){
            if(n == 0){
                vector();
                return;
            }
            capacity = n;
            length = n;
            data = (T*)::operator new (n*sizeof(T));
            start = 0;
            version = 0;
            reallocation_version = 0;
            for(uint64_t i=0; i<n; i++)
                new (data+i) T;
        }
        
        template <typename It>
        vector(It begin, It end){
            typename std::iterator_traits<It>::iterator_category tag{};
            vector(begin, end, tag);
        }
        
        vector(std::initializer_list<T> list): vector(list.begin(), list.end()){}
        
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
            return data[getIndex(k, start, capacity)];
        }
        
        const T& operator[](uint64_t const k) const{
            if(k >= length) throw std::out_of_range{"Index out of bound: " + std::to_string(k)};
            return data[getIndex(k, start, capacity)];
        }
        
        void push_back(T const & item){
            T temp{item};
            version ++;
            if(length == capacity) resize();
            new (data + getIndex(length++, start, capacity)) T{std::move(temp)};
        }
        
        void push_back(T&& item){
            version ++;
            if(length == capacity) resize();
            new (data + getIndex(length++, start, capacity)) T{std::move(item)};
        }
        
        void push_front(T const & item){
            T temp{item};
            version ++;
            if(length == capacity) resize();
            new (data + (start = getIndex(-1, start, capacity))) T{std::move(temp)};
            length ++;
        }
        
        void push_front(T&& item){
            version ++;
            if(length == capacity) resize();
            new (data + (start = getIndex(-1, start, capacity))) T{std::move(item)};
            length ++;
        }
        
        void pop_back(void){
            if(length == 0) throw std::out_of_range{"Unable to pop from empty vector!"};
            data[getIndex(--length, start, capacity)].~T();
            version ++;
        }
        
        void pop_front(void){
            if(length == 0) throw std::out_of_range("Unable to pop from empty vector!");
            data[start].~T();
            start = getIndex(1, start, capacity);
            length--;
            version ++;
        }
        
        iterator begin(void){
            return iterator{*this, 0};
        }
        
        const_iterator begin(void) const{
            return const_iterator{*this, 0};
        }
        
        iterator end(void) {
            return iterator{*this, length};
        }
        
        const_iterator end(void) const{
            return const_iterator{*this, length};
        }
        
        template <typename... Args>
        void emplace_back(Args&&... args){
            T temp{std::forward<Args>(args)...};
            version ++;
            if(length == capacity) resize();
            new (data + getIndex(length++, start, capacity)) T{std::move(temp)};
        }
    };
    
} //namespace epl

#endif