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
#include <vector>
#include <complex>
#include <functional>

using std::complex;
//using std::vector; // during development and testing
using epl::vector; // after submission

namespace epl {
    
    template <typename T> struct SRank;
    template<> struct SRank<int> { static constexpr int value = 1; };
    template<> struct SRank<float> { static constexpr int value = 2; };
    template<> struct SRank<double> { static constexpr int value = 3; };
    template<typename Comp> struct SRank<complex<Comp>> { static constexpr int value = SRank<Comp>::value; };
    
    template<int value> struct SType;
    template<> struct SType<1> { using type = int; };
    template<> struct SType<2> { using type = float; };
    template<> struct SType<3> { using type = double; };
    
    template <typename T> struct complex_depth { static constexpr int value = 0; };
    template <typename T> struct complex_depth<complex<T>> { static constexpr int value = complex_depth<T>::value + 1; };
    
    template <int depth, typename T> struct wrap_complex{
        using inner_type = typename wrap_complex<depth-1, T>::type;
        using type = complex<inner_type>;
    };
    template <typename T> struct wrap_complex<0, T> { using type = T; };
    
    template <typename T1, typename T2>
    struct choose_type{
        static constexpr int t1_rank = SRank<T1>::value;
        static constexpr int t2_rank = SRank<T2>::value;
        static constexpr int max_rank = t1_rank > t2_rank? t1_rank: t2_rank;
        
        using my_type = typename SType<max_rank>::type;
        
        static constexpr int t1_depth = complex_depth<T1>::value;
        static constexpr int t2_depth = complex_depth<T2>::value;
        static constexpr int max_depth = t1_depth>t2_depth? t1_depth: t2_depth;
        
        using type = typename wrap_complex<max_depth, my_type>::type;
    };
    
    
    template <typename T>
    class valarray : public vector<T>{
    private:
        template <typename Type>
        struct ReturnArray{
            using type = valarray<typename choose_type<T, Type>::type>;
        };
        
        template <typename Type>
        struct ReturnArray<valarray<Type>>{
            using type = valarray<typename choose_type<T, Type>::type>;
        };
        
        template <typename Type>
        using RetType = typename ReturnArray<Type>::type;
        
        template <typename Type>
        uint64_t size(Type const& val) const {
            return UINT64_MAX;
        }
        
        template <typename Type>
        uint64_t size(valarray<Type> const& array) const {
            return array.size();
        }
        
        template <typename Type>
        Type get(Type const& val, const uint64_t k) const {
            return val;
        }
        
        template <typename Type>
        Type get(valarray<Type> const& array, const uint64_t k) const {
            return array[k];
        }
        
        template <typename Type>
        struct strip_const{
            using type = Type;
        };
        
        template <typename Type>
        struct strip_const<const Type>{
            using type = Type;
        };
        
        template <typename From, typename To>
        static To typeCast(From const& from) {
            return from;
        }
        
        template <typename From, typename To>
        static To typeCast(complex<int> const& from){
            return To(from.real(), from.imag());
        }
        
    public:
        using vector<T>::vector;
        using vector<T>::size;
        using valType = T;
        
        valarray<T> operator- (void) const{
            valarray<T> res{*this};
            for(auto& x: res) x = -x;
            return res;
        }
        
        template <typename Type>
        RetType<Type> operator+ (Type const& that) const{
            RetType<Type> res;
            using RetValueType = typename RetType<Type>::valType;
            uint64_t length = std::min(size(), size(that));
            for(uint64_t i=0; i<length; i++){
                auto item = get(that, i);
                using type = typename strip_const<decltype(item)>::type;
                res.push_back(typeCast<T, RetValueType>((*this)[i]) + typeCast<type, RetValueType>(item));
            }
            return res;
        }
        
        template <typename Scalar, typename Array>
        friend typename Array::template RetType<Scalar> operator+ (Scalar const& scalar, Array const& array){
            return array + scalar;
        }
        
        template <typename Type>
        RetType<Type> operator- (Type const& that) const{
            RetType<Type> res;
            using RetValueType = typename RetType<Type>::valType;
            uint64_t length = std::min(size(), size(that));
            for(uint64_t i=0; i<length; i++){
                auto item = get(that, i);
                using type = typename strip_const<decltype(item)>::type;
                res.push_back(typeCast<T, RetValueType>((*this)[i]) - typeCast<type, RetValueType>(item));
            }
            return res;
        }
        
        template <typename Scalar, typename Array>
        friend typename Array::template RetType<Scalar> operator- (Scalar const& scalar, Array const& array){
            return -array + scalar;
        }
        
        template <typename Type>
        RetType<Type> operator* (Type const& that) const{
            RetType<Type> res;
            using RetValueType = typename RetType<Type>::valType;
            uint64_t length = std::min(size(), size(that));
            for(uint64_t i=0; i<length; i++){
                auto item = get(that, i);
                using type = typename strip_const<decltype(item)>::type;
                res.push_back(typeCast<T, RetValueType>((*this)[i]) * typeCast<type, RetValueType>(item));
            }
            return res;
        }
        
        template <typename Scalar, typename Array>
        friend typename Array::template RetType<Scalar> operator* (Scalar const& scalar, Array const& array){
            return array * scalar;
        }
        
        template <typename Type>
        RetType<Type> operator/ (Type const& that) const{
            RetType<Type> res;
            using RetValueType = typename RetType<Type>::valType;
            uint64_t length = std::min(size(), size(that));
            for(uint64_t i=0; i<length; i++){
                auto item = get(that, i);
                using type = typename strip_const<decltype(item)>::type;
                res.push_back(typeCast<T, RetValueType>((*this)[i]) / typeCast<type, RetValueType>(item));
            }
            return res;
        }
        
        template <typename Scalar, typename Array>
        friend typename Array::template RetType<Scalar> operator/ (Scalar const& scalar, Array const& array){
            using RetType = typename Array::template RetType<Scalar>;
            using RetValueType = typename RetType::valType;
            RetType res;
            for(uint64_t i=0; i<array.size(); i++)
                res.push_back(typeCast<Scalar, RetValueType>(scalar) / typeCast<typename Array::valType, RetValueType>(array[i]));
            return res;
        }
        
        T accumulate(std::function<T(T const&, T const&)> fun){
            if(size() == 0) return T();
            else if(size() == 1) return (*this)[0];
            else{
                T res = fun((*this)[0], (*this)[1]);
                for(uint64_t i=2; i<size(); i++)
                    res = fun(res, (*this)[i]);
                return res;
            }
        }
        
        T sum(void){
            T res(0);
            for(auto& x: *this) res = res + x;
            return res;
        }

        
        
    };
    
}

#endif /* _Valarray_h */

