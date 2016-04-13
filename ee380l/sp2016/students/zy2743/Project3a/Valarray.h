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
    class valarray;
    
    
    template <typename T, typename Type>
    struct ReturnArray{ using type = valarray<typename choose_type<T, Type>::type>; };
    
    template <typename T, typename Type>
    struct ReturnArray<T, valarray<Type>>{ using type = valarray<typename choose_type<T, Type>::type>; };
    
    template <typename T, typename Type>
    using RetType = typename ReturnArray<T, Type>::type;
    
    
    template <typename Type>
    uint64_t size(Type const& val){ return UINT64_MAX; }
    
    template <typename Type>
    uint64_t size(valarray<Type> const& array){ return array.size(); }
    
    template <typename Type>
    Type get(Type const& val, const uint64_t k){ return val; }
    
    template <typename Type>
    Type get(valarray<Type> const& array, const uint64_t k){ return array[k]; }
    
    
    template <typename From, typename To>
    To typeCast(From const& from) { return from; }
    
    template <typename From, typename To>
    To typeCast(complex<int> const& from){ return To(from.real(), from.imag()); }
    
    
    template <typename Type>
    struct strip_const{ using type = Type; };
    
    template <typename Type>
    struct strip_const<const Type>{ using type = Type; };
    
    
    template <typename T, typename Type, typename OP>
    RetType<T, Type>& apply_op(RetType<T, Type>& result, valarray<T> const& x, Type const& y, OP op){
        using RetItem = typename RetType<T, Type>::valType;
        uint64_t len = std::min(x.size(), size(y));
        for(uint64_t i=0; i<len; i++){
            RetItem x_item = typeCast<T, RetItem>(x[i]);
            auto item = get(y, i);
            using YType = typename strip_const<decltype(item)>::type;
            RetItem y_item = typeCast<YType, RetItem>(item);
            result[i] = op(x_item, y_item);
        }
        return result;
    }
    
    template <typename T, typename Type>
    RetType<T, Type> operator+ (valarray<T> const& array, Type const& that) {
        using RetItem = typename RetType<T, Type>::valType;
        RetType<T, Type> res(array.size());
        return apply_op(res, array, that, std::plus<RetItem>());
    }
    
    template <typename T, typename Type>
    RetType<T, Type> operator+ (Type const& that, valarray<T> const& array) {
        return array + that;
    }
    
    template <typename T1, typename T2>
    RetType<T1, T2> operator+ (valarray<T1> const& array1, valarray<T2> const& array2){
        using RetItem = typename RetType<T1, T2>::valType;
        RetType<T1, T2> res(std::min(array1.size(), array2.size()));
        return apply_op(res, array1, array2, std::plus<RetItem>());
    }
    
    template <typename T, typename Type>
    RetType<T, Type> operator- (valarray<T> const& array, Type const& that) {
        using RetItem = typename RetType<T, Type>::valType;
        RetType<T, Type> res(array.size());
        return apply_op(res, array, that, std::minus<RetItem>());
    }
    
    template <typename T, typename Type>
    RetType<T, Type> operator- (Type const& that, valarray<T> const& array) {
        using RetItem = typename RetType<T, Type>::valType;
        RetType<T, Type> res(array.size());
        return apply_op(res, array, that, [](RetItem const& x, RetItem const& y){ return std::minus<RetItem>()(y, x); });
    }
    
    template <typename T1, typename T2>
    RetType<T1, T2> operator- (valarray<T1> const& array1, valarray<T2> const& array2){
        using RetItem = typename RetType<T1, T2>::valType;
        RetType<T1, T2> res(std::min(array1.size(), array2.size()));
        return apply_op(res, array1, array2, std::minus<RetItem>());
    }
    
    template <typename T, typename Type>
    RetType<T, Type> operator* (valarray<T> const& array, Type const& that) {
        using RetItem = typename RetType<T, Type>::valType;
        RetType<T, Type> res(array.size());
        return apply_op(res, array, that, std::multiplies<RetItem>());
    }
    
    template <typename T, typename Type>
    RetType<T, Type> operator* (Type const& that, valarray<T> const& array) {
        return array * that;
    }
    
    template <typename T1, typename T2>
    RetType<T1, T2> operator* (valarray<T1> const& array1, valarray<T2> const& array2){
        using RetItem = typename RetType<T1, T2>::valType;
        RetType<T1, T2> res(std::min(array1.size(), array2.size()));
        return apply_op(res, array1, array2, std::multiplies<RetItem>());
    }
    
    template <typename T, typename Type>
    RetType<T, Type> operator/ (valarray<T> const& array, Type const& that) {
        using RetItem = typename RetType<T, Type>::valType;
        RetType<T, Type> res(array.size());
        return apply_op(res, array, that, std::divides<RetItem>());
    }
    
    template <typename T, typename Type>
    RetType<T, Type> operator/ (Type const& that, valarray<T> const& array) {
        using RetItem = typename RetType<T, Type>::valType;
        RetType<T, Type> res(array.size());
        return apply_op(res, array, that, [](RetItem const& x, RetItem const& y){ return std::divides<RetItem>()(y, x); });
    }
    
    template <typename T1, typename T2>
    RetType<T1, T2> operator/ (valarray<T1> const& array1, valarray<T2> const& array2){
        using RetItem = typename RetType<T1, T2>::valType;
        RetType<T1, T2> res(std::min(array1.size(), array2.size()));
        return apply_op(res, array1, array2, std::divides<RetItem>());
    }
    
    template<typename Val>
    std::ostream& operator<<(std::ostream& out, const valarray<Val>& arr){
        auto start = arr.begin();
        auto tail = arr.end();
        const char* ref = "";
        while(start!=tail){
            std::cout<< ref << (*start);
            ref = ", ";
            start++;
        }
        return out;
    }
    
    
    template <typename T>
    class valarray : public vector<T>{
    public:
        using vector<T>::vector;
        using vector<T>::size;
        using valType = T;
        
        valarray<T> operator- (void) const{
            valarray<T> res{*this};
            for(auto& x: res) x = -x;
            return res;
        }
        
        template <typename Fun>
        T accumulate(Fun fun){
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

