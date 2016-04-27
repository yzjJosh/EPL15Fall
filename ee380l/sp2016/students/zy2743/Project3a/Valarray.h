// Valarray.h

/* Put your solution in this file, we expect to be able to use
 * your epl::valarray class by simply saying #include "Valarray.h"
 *
 * We will #include "Vector.h" to get the epl::vector<T> class 
 * before we #include "Valarray.h". You are encouraged to test
 * and develop your class using std::vector<T> as the base class
 * for your epl::valarray<T>
 * you are required to submit your project with epl::vector<T>
 * as the base class for your epl::valarray<T>
 */

#ifndef _Valarray_h
#define _Valarray_h

#include "Vector.h"
//#include <vector>
#include <complex>
#include <functional>

using std::complex;
//using std::vector; // during development and testing
using epl::vector; // after submission
using namespace std::rel_ops;


namespace epl {
    
    template <typename T> class Wrap;
    template <typename T> using valarray = Wrap<vector<T>>;
    
    template <typename T> struct SRank { static constexpr int value = 0; };
    template<> struct SRank<int> { static constexpr int value = 1; };
    template<> struct SRank<float> { static constexpr int value = 2; };
    template<> struct SRank<double> { static constexpr int value = 3; };
    template<typename Comp> struct SRank<complex<Comp>> { static constexpr int value = SRank<Comp>::value; };
    
    
    template<int value> struct SType;
    template<> struct SType<1> { using type = int; };
    template<> struct SType<2> { using type = float; };
    template<> struct SType<3> { using type = double; };
    
    
    template <typename T> struct is_complex { static constexpr bool value = false; };
    template <typename T> struct is_complex<complex<T>> { static constexpr bool value = true; };
    
    
    template <bool, typename T> struct wrap_complex{ using type = T; };
    template <typename T> struct wrap_complex<true, T>{ using type = complex<T>; };
    
    
    template <typename T1, typename T2>
    struct choose_type{
        static constexpr int t1_rank = SRank<T1>::value;
        static constexpr int t2_rank = SRank<T2>::value;
        static constexpr int max_rank = t1_rank > t2_rank? t1_rank: t2_rank;
        
        using my_type = typename SType<max_rank>::type;
        
        static constexpr bool t1_c = is_complex<T1>::value;
        static constexpr bool t2_c = is_complex<T2>::value;
        static constexpr bool c = t1_c || t2_c;
        
        using type = typename wrap_complex<c, my_type>::type;
    };
    
    
    template <typename T> struct get_val_type{ using type = typename T::valType; };
    template <typename T> struct get_val_type<vector<T>>{ using type = T; };

    
    template <typename From, typename To> To typeCast(From const& from) { return from; }
    template <typename From, typename To> To typeCast(complex<int> const& from){ return To(from.real(), from.imag()); }
    
    template <bool p, typename T> using EnableIf = typename std::enable_if<p, T>::type;
    template <typename T> using EnableIfScalar = EnableIf<SRank<T>::value != 0, T>;
    
    template <typename T> struct is_vector :public std::false_type {};
    template <typename T> struct is_vector<vector<T>> :public std::true_type {};
    
    
    template <typename T>
    class const_iterator: public std::iterator<std::random_access_iterator_tag, typename get_val_type<T>::type>{
    private:
        const T* home;
        uint64_t index;
        
        using valType = typename T::valType;
    public:
        const_iterator(T const& home, uint64_t const index){
            this->home = &home;
            this->index = index;
        }
        
        const valType operator*(void) const{
            return (*home)[index];
        }
        
        const_iterator<T> operator+ (const uint64_t step) const{
            const_iterator<T> res{*this};
            res.index += step;
            return res;
        }
        
        friend const_iterator<T> operator+(const uint64_t step, const_iterator<T> const& it) {
            return it + step;
        }
        
        const_iterator<T> operator- (const uint64_t step) const{
            return operator+ (-step);
        }
        
        const_iterator<T>& operator+= (const uint64_t step) {
            index += step;
            return *this;
        }
        
        const_iterator<T>& operator-= (const uint64_t step) {
            index -= step;
            return *this;
        }
        
        const_iterator<T>& operator++ (void){
            return operator+=(1);
        }
        
        const_iterator<T>& operator-- (void){
            return operator-=(1);
        }
        
        const_iterator<T> operator++ (int){
            const_iterator<T> res{*this};
            operator++();
            return res;
        }
        
        const_iterator<T> operator-- (int){
            const_iterator<T> res{*this};
            operator--();
            return res;
        }
        
        int64_t operator- (const_iterator<T> const& that) const{
            return ((int64_t)index) - ((int64_t)that.index);
        }
        
        bool operator== (const_iterator<T> const& that) const{
            return this->home == that.home && this->index == that.index;
        }
        
        bool operator< (const_iterator<T> const& that) const{
            if(home != that.home) return false;
            return index < that.index;
        }
        
        valType& operator[] (const uint64_t step) const{
            return *(*this + step);
        }
        
        valType* operator->(void) const{
            return &(home[index]);
        }
        
        const_iterator<T>& operator= (const_iterator<T> const& it){
            home = it.home;
            index = it.index;
            return *this;
        }
    };
    
    template <typename T>
    class Scalar{
    private:
        T val;
    public:
        using valType = T;
        
        Scalar(void): val(T()) {}
        Scalar(T const& value): val(value) {}
        T operator[] (uint64_t const k) const { return val; }
        uint64_t size(void) const { return UINT64_MAX; }
        const_iterator<Scalar<T>> begin(void) const { return const_iterator<Scalar<T>>(*this, 0); }
        const_iterator<Scalar<T>> end(void) const { return const_iterator<Scalar<T>>(*this, 1); }
    };

    
    template <typename T> struct choose_ref{ using type = T; };
    template <typename T> struct choose_ref<vector<T>> { using type = vector<T> const&; };
    template <typename T> using ChooseRef = typename choose_ref<T>::type;
    
    
    template <typename T1, typename T2, typename OP>
    class Proxy{
    private:
        using LeftType = ChooseRef<T1>;
        using RightType = ChooseRef<T2>;
        using Left_Val_Type = typename get_val_type<T1>::type;
        using Right_Val_Type = typename get_val_type<T2>::type;
        
        LeftType left;
        RightType right;
        OP operation;
        
    public:
        Proxy(T1 const& lhs, T2 const& rhs, OP op = OP{}): left(lhs), right(rhs), operation(op){}
        
        using valType = typename choose_type<Left_Val_Type, Right_Val_Type>::type;
        
        uint64_t size(void) const{
            return std::min(left.size(), right.size());
        }
        
        valType operator[] (uint64_t const k) const {
            valType x = typeCast<Left_Val_Type, valType>(left[k]);
            valType y = typeCast<Right_Val_Type, valType>(right[k]);
            return operation(x, y);
        }
        
        const_iterator<Proxy<T1, T2, OP>> begin(void) const { return const_iterator<Proxy<T1, T2, OP>>(*this, 0); }
        
        const_iterator<Proxy<T1, T2, OP>> end(void) const { return const_iterator<Proxy<T1, T2, OP>>(*this, size()); }
        
    };
    
    template <typename Type, typename OP>
    struct UnaryOP{
        Type operator()(Type const& val, Type const&) const {
            return std::plus<Type>{}(OP{}(val), 0);
        }
    };
    
    
    template <typename T, typename OP, typename ValType = typename get_val_type<T>::type>
    struct UnaryProxy: public Proxy<T, Scalar<int>, UnaryOP<ValType, OP>>{
        UnaryProxy(T const& inner): Proxy<T, Scalar<int>, UnaryOP<ValType, OP>>(inner, Scalar<int>(0)) {}
    };
    
    template <typename Type>
    struct my_sqrt{
        using retType = typename choose_type<double, Type>::type;
        retType operator()(Type const& val) const { return std::sqrt(typeCast<Type, retType>(val)); }
    };
    
    template <typename T>
    class Wrap : public T{
    public:
        using T::T;
        using valType = typename get_val_type<T>::type;
        
        Wrap(void): T(){}
        
        Wrap(T const& arg): T(arg){}
        
        template <typename Type>
        Wrap(Wrap<Type> const& that): T() {
            uint64_t size = that.end() - that.begin();
            for(uint64_t i=0; i<size; i++)
                this->push_back(typeCast<typename get_val_type<Type>::type, valType>(that[i]));
        }
        
        template <typename Type>
        Wrap<T>& operator= (Wrap<Type> const& that){
            if((void*)this != (void*)&that){
                uint64_t size = std::min(this->size(), that.size());
                for(uint64_t i=0; i<size; i++)
                    (*this)[i] = typeCast<typename get_val_type<Type>::type, valType>(that[i]);
            }
            return *this;
        }
        
        Wrap<T>& operator= (Wrap<T> const& that){
            if((void*)this != (void*)&that){
                uint64_t size = std::min(this->size(), that.size());
                for(uint64_t i=0; i<size; i++)
                    (*this)[i] = typeCast<typename get_val_type<T>::type, valType>(that[i]);
            }
            return *this;
        }
        
        template <typename S, typename Enable = EnableIfScalar<S>>
        Wrap<T>& operator=(S const& that){
            return operator=(Wrap<Scalar<S>>(Scalar<S>(that)));
        }

        
        template <typename Fun>
        valType accumulate(Fun const fun) const{
            if(this->size() == 0) return valType();
            else if(this->size() == 1) return (*this)[0];
            else{
                valType res = fun((*this)[0], (*this)[1]);
                for(uint64_t i=2; i<this->size(); i++)
                    res = fun(res, (*this)[i]);
                return res;
            }
        }
        
        template <typename Fun>
        valType accumulate(Fun const fun) {
            if(this->size() == 0) return valType();
            else if(this->size() == 1) return (*this)[0];
            else{
                valType res = fun((*this)[0], (*this)[1]);
                for(uint64_t i=2; i<this->size(); i++)
                    res = fun(res, (*this)[i]);
                return res;
            }
        }
        
        valType sum(void) const{ return accumulate(std::plus<valType>()); }
        
        template <typename Fun>
        Wrap<UnaryProxy<T, Fun>> apply(Fun const) const { return Wrap<UnaryProxy<T, Fun>>(UnaryProxy<T, Fun>(*this)); }
        
        template <typename Fun>
        Wrap<UnaryProxy<T, Fun>> apply(Fun const) { return Wrap<UnaryProxy<T, Fun>>(UnaryProxy<T, Fun>(*this)); }
        
        Wrap<UnaryProxy<T, std::negate<valType>>> operator- (void) const{ return apply(std::negate<valType>());}
        
        Wrap<UnaryProxy<T, my_sqrt<valType>>> sqrt(void) const { return apply(my_sqrt<valType>()); }
    };
    
    
    template <typename T1, typename T2, typename T3 = typename choose_type<typename Wrap<T1>::valType, typename Wrap<T2>::valType>::type>
    Wrap<Proxy<T1, T2, std::plus<T3>>> operator+ (Wrap<T1> const& left, Wrap<T2> const& right) {
        T1 const& l(left);
        T2 const& r(right);
        Proxy<T1, T2, std::plus<T3>> proxy(l, r);
        return Wrap<Proxy<T1, T2, std::plus<T3>>> (proxy);
    }
    
    template <typename T1, typename T2, typename T3 = typename choose_type<typename Wrap<T1>::valType, EnableIfScalar<T2>>::type>
    Wrap<Proxy<T1, Scalar<T2>, std::plus<T3>>> operator+ (Wrap<T1> const& left, T2 const& right){
        Scalar<T2> scalar(right);
        return left + Wrap<Scalar<T2>>(scalar);
    }
    
    template <typename T1, typename T2, typename T3 = typename choose_type<EnableIfScalar<T1>, typename Wrap<T2>::valType>::type>
    Wrap<Proxy<Scalar<T1>, T2, std::plus<T3>>> operator+ (T1 const& left, Wrap<T2> const& right){
        Scalar<T1> scalar(left);
        return Wrap<Scalar<T1>>(scalar) + right;
    }
    
    template <typename T1, typename T2, typename T3 = typename choose_type<typename Wrap<T1>::valType, typename Wrap<T2>::valType>::type>
    Wrap<Proxy<T1, T2, std::minus<T3>>> operator- (Wrap<T1> const& left, Wrap<T2> const& right) {
        T1 const& l(left);
        T2 const& r(right);
        Proxy<T1, T2, std::minus<T3>> proxy(l, r);
        return Wrap<Proxy<T1, T2, std::minus<T3>>> (proxy);
    }
    
    template <typename T1, typename T2, typename T3 = typename choose_type<typename Wrap<T1>::valType, EnableIfScalar<T2>>::type>
    Wrap<Proxy<T1, Scalar<T2>, std::minus<T3>>> operator- (Wrap<T1> const& left, T2 const& right){
        Scalar<T2> scalar(right);
        return left - Wrap<Scalar<T2>>(scalar);
    }
    
    template <typename T1, typename T2, typename T3 = typename choose_type<EnableIfScalar<T1>, typename Wrap<T2>::valType>::type>
    Wrap<Proxy<Scalar<T1>, T2, std::minus<T3>>> operator- (T1 const& left, Wrap<T2> const& right){
        Scalar<T1> scalar(left);
        return Wrap<Scalar<T1>>(scalar) - right;
    }
    
    template <typename T1, typename T2, typename T3 = typename choose_type<typename Wrap<T1>::valType, typename Wrap<T2>::valType>::type>
    Wrap<Proxy<T1, T2, std::multiplies<T3>>> operator* (Wrap<T1> const& left, Wrap<T2> const& right) {
        T1 const& l(left);
        T2 const& r(right);
        Proxy<T1, T2, std::multiplies<T3>> proxy(l, r);
        return Wrap<Proxy<T1, T2, std::multiplies<T3>>> (proxy);
    }
    
    template <typename T1, typename T2, typename T3 = typename choose_type<typename Wrap<T1>::valType, EnableIfScalar<T2>>::type>
    Wrap<Proxy<T1, Scalar<T2>, std::multiplies<T3>>> operator* (Wrap<T1> const& left, T2 const& right){
        Scalar<T2> scalar(right);
        return left * Wrap<Scalar<T2>>(scalar);
    }
    
    template <typename T1, typename T2, typename T3 = typename choose_type<EnableIfScalar<T1>, typename Wrap<T2>::valType>::type>
    Wrap<Proxy<Scalar<T1>, T2, std::multiplies<T3>>> operator* (T1 const& left, Wrap<T2> const& right){
        Scalar<T1> scalar(left);
        return Wrap<Scalar<T1>>(scalar) * right;
    }
    
    template <typename T1, typename T2, typename T3 = typename choose_type<typename Wrap<T1>::valType, typename Wrap<T2>::valType>::type>
    Wrap<Proxy<T1, T2, std::divides<T3>>> operator/ (Wrap<T1> const& left, Wrap<T2> const& right) {
        T1 const& l(left);
        T2 const& r(right);
        Proxy<T1, T2, std::divides<T3>> proxy(l, r);
        return Wrap<Proxy<T1, T2, std::divides<T3>>> (proxy);
    }
    
    template <typename T1, typename T2, typename T3 = typename choose_type<typename Wrap<T1>::valType, EnableIfScalar<T2>>::type>
    Wrap<Proxy<T1, Scalar<T2>, std::divides<T3>>> operator/ (Wrap<T1> const& left, T2 const& right){
        Scalar<T2> scalar(right);
        return left / Wrap<Scalar<T2>>(scalar);
    }
    
    template <typename T1, typename T2, typename T3 = typename choose_type<EnableIfScalar<T1>, typename Wrap<T2>::valType>::type>
    Wrap<Proxy<Scalar<T1>, T2, std::divides<T3>>> operator/ (T1 const& left, Wrap<T2> const& right){
        Scalar<T1> scalar(left);
        return Wrap<Scalar<T1>>(scalar) / right;
    }
    
    template<typename T>
    std::ostream& operator<<(std::ostream& out, const Wrap<T>& arr){
        const char* ref = "";
        for(uint64_t i=0; i<arr.size(); i++){
            std::cout<< ref << arr[i];
            ref = ", ";
        }
        return out;
    }
}

#endif /* _Valarray_h */

