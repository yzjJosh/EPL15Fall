
#include <iostream>
#include <assert.h>
#include <functional>
#include "../../Valarray.h"

using epl::valarray;
using std::cout;
using std::endl;
using std::complex;
using namespace epl;

template <typename T>
uint64_t _size_(T const&) { return UINT64_MAX; }

template <typename T>
uint64_t _size_(valarray<T> const& array) { return array.size(); }

template <typename T>
T _get_(T const& val, const uint64_t) { return val; }

template <typename T>
T _get_(valarray<T> const& array, const uint64_t k) { return array[k]; }

template <typename From, typename To>
To _typeCast_(From const& from) {
    return from;
}

template <typename From, typename To>
To _typeCast_(complex<int> const& from){
    return To(from.real(), from.imag());
}

template <typename T>
struct _strip_const_{
    using type = T;
};

template <typename T>
struct _strip_const_<const T>{
    using type = T;
};

template <typename T1, typename T2>
void testPlus(T1 const& a, T2 const& b){
    auto c = a + b;
    using type = typename std::iterator_traits<decltype(c.begin())>::value_type;
    uint64_t len = std::min(_size_(a), _size_(b));
    for(uint64_t i=0; i<len; i++){
        auto item1 = _get_(a, i);
        auto item2 = _get_(b, i);
        using type1 = typename _strip_const_<decltype(item1)>::type;
        using type2 = typename _strip_const_<decltype(item2)>::type;
        assert(c[i] == (_typeCast_<type1, type>(item1) + _typeCast_<type2, type>(item2)));
    }
}

template <typename T1, typename T2>
void testMinus(T1 const& a, T2 const& b){
    auto c = a - b;
    using type = typename decltype(c)::valType;
    uint64_t len = std::min(_size_(a), _size_(b));
    for(uint64_t i=0; i<len; i++){
        auto item1 = _get_(a, i);
        auto item2 = _get_(b, i);
        using type1 = typename _strip_const_<decltype(item1)>::type;
        using type2 = typename _strip_const_<decltype(item2)>::type;
        assert(c[i] == (_typeCast_<type1, type>(item1) - _typeCast_<type2, type>(item2)));
    }
}

template <typename T1, typename T2>
void testMultiply(T1 const& a, T2 const& b){
    auto c = a * b;
    using type = typename decltype(c)::valType;
    uint64_t len = std::min(_size_(a), _size_(b));
    for(uint64_t i=0; i<len; i++){
        auto item1 = _get_(a, i);
        auto item2 = _get_(b, i);
        using type1 = typename _strip_const_<decltype(item1)>::type;
        using type2 = typename _strip_const_<decltype(item2)>::type;
        assert(c[i] == (_typeCast_<type1, type>(item1) * _typeCast_<type2, type>(item2)));
    }
}

template <typename T1, typename T2>
void testDevide(T1 const& a, T2 const& b){
    auto c = a / b;
    using type = typename decltype(c)::valType;
    uint64_t len = std::min(_size_(a), _size_(b));
    for(uint64_t i=0; i<len; i++){
        auto item1 = _get_(a, i);
        auto item2 = _get_(b, i);
        using type1 = typename _strip_const_<decltype(item1)>::type;
        using type2 = typename _strip_const_<decltype(item2)>::type;
        assert(c[i] == (_typeCast_<type1, type>(item1) / _typeCast_<type2, type>(item2)));
    }
}

template <typename T>
void testSum(valarray<T> const& array){
    T sum(0);
    for(auto& x: array)
        sum += x;
    assert(sum == array.sum());
}


template <typename T, typename Fun>
void testAccumulate(valarray<T>& array, Fun fun){
    if(array.size() < 2) return;
    T res(fun(array[0], array[1]));
    for(int i=2; i<array.size(); i++)
        res = fun(res, array[i]);
    assert(res == array.accumulate(fun));
}

template <typename T, typename Fun>
void testConstAccumulate(valarray<T> const& array, Fun fun){
    if(array.size() < 2) return;
    T res(fun(array[0], array[1]));
    for(int i=2; i<array.size(); i++)
        res = fun(res, array[i]);
    assert(res == array.accumulate(fun));
}


template <typename T>
T fun1(T const& x, T const& y){ return x+y; }

template <typename T>
T fun2(T const& x, T const& y){ return x*y; }

template <typename T>
T fun3(T x, T y) {return x*T(2)+y*y+x*y; }

template <typename T>
T fun4(T& x, T& y) { x = x; y = y; return std::sin(x+y);}


int main(void) {
    valarray<int> a{-1, 1, 2, 3, 4, 5, 6, -3, 56, 90, -4};
    valarray<float> b{0.1f, 8.3f, -3.4f, 1.0f, 4.3f, 1.3f, -0.1f, 0.0003f, 343.98f};
    valarray<double> c{2.5, 2.5, 2.5, 2.5, -0.2, 0.5, 1.43, 3.1415926};
    valarray<complex<int>> d{ complex<int>(3, -1), complex<int>(-3, 0), complex<int>(0, 1)};
    valarray<complex<float>> e{ complex<float>(0.0f, 1.1f), complex<float>(1.2f, -2.3f), complex<float>(2.8, 1.9)};
    valarray<complex<double>> f{ complex<double>(0.0, -3.2), complex<double>(1.3), complex<double>(1.565673, 34343)};
    valarray<complex<double>> g;
    testPlus(a, b);
    testPlus(a, c);
    testPlus(a, d);
    testPlus(a, e);
    testPlus(a, f);
    testPlus(a, g);
    testPlus(b, c);
    testPlus(b, d);
    testPlus(b, e);
    testPlus(b, f);
    testPlus(b, g);
    testPlus(c, d);
    testPlus(c, e);
    testPlus(c, f);
    testPlus(c, g);
    testPlus(d, e);
    testPlus(d, f);
    testPlus(d, g);
    testPlus(e, f);
    testPlus(e, g);
    testPlus(f, g);
    testPlus(a, 2.9f);
    testPlus(2.9f, a);
    testPlus(a, -2.7);
    testPlus(-2.7, a);
    testPlus(a, 4);
    testPlus(4, a);
    testPlus(b, 2.9f);
    testPlus(2.9f, b);
    testPlus(b, -2.7);
    testPlus(-2.7, b);
    testPlus(b, 4);
    testPlus(4, b);
    testPlus(c, 2.9f);
    testPlus(2.9f, c);
    testPlus(c, -2.7);
    testPlus(-2.7, c);
    testPlus(c, 4);
    testPlus(4, c);
    testPlus(d, 2.9f);
    testPlus(2.9f, d);
    testPlus(d, -2.7);
    testPlus(-2.7, d);
    testPlus(d, 4);
    testPlus(4, d);
    testPlus(e, 2.9f);
    testPlus(2.9f, e);
    testPlus(e, -2.7);
    testPlus(-2.7, e);
    testPlus(e, 4);
    testPlus(4, e);
    testPlus(f, 2.9f);
    testPlus(2.9f, f);
    testPlus(f, -2.7);
    testPlus(-2.7, f);
    testPlus(f, 4);
    testPlus(4, f);
    testPlus(g, 2.9f);
    testPlus(2.9f, g);
    testPlus(g, -2.7);
    testPlus(-2.7, g);
    testPlus(g, 4);
    testPlus(4, g);
    
    testMinus(a, b);
    testMinus(a, c);
    testMinus(a, d);
    testMinus(a, e);
    testMinus(a, f);
    testMinus(a, g);
    testMinus(b, c);
    testMinus(b, d);
    testMinus(b, e);
    testMinus(b, f);
    testMinus(b, g);
    testMinus(c, d);
    testMinus(c, e);
    testMinus(c, f);
    testMinus(c, g);
    testMinus(d, e);
    testMinus(d, f);
    testMinus(d, g);
    testMinus(e, f);
    testMinus(e, g);
    testMinus(f, g);
    testMinus(a, 2.9f);
    testMinus(2.9f, a);
    testMinus(a, -2.7);
    testMinus(-2.7, a);
    testMinus(a, 4);
    testMinus(4, a);
    testMinus(b, 2.9f);
    testMinus(2.9f, b);
    testMinus(b, -2.7);
    testMinus(-2.7, b);
    testMinus(b, 4);
    testMinus(4, b);
    testMinus(c, 2.9f);
    testMinus(2.9f, c);
    testMinus(c, -2.7);
    testMinus(-2.7, c);
    testMinus(c, 4);
    testMinus(4, c);
    testMinus(d, 2.9f);
    testMinus(2.9f, d);
    testMinus(d, -2.7);
    testMinus(-2.7, d);
    testMinus(d, 4);
    testMinus(4, d);
    testMinus(e, 2.9f);
    testMinus(2.9f, e);
    testMinus(e, -2.7);
    testMinus(-2.7, e);
    testMinus(e, 4);
    testMinus(4, e);
    testMinus(f, 2.9f);
    testMinus(2.9f, f);
    testMinus(f, -2.7);
    testMinus(-2.7, f);
    testMinus(f, 4);
    testMinus(4, f);
    testMinus(g, 2.9f);
    testMinus(2.9f, g);
    testMinus(g, -2.7);
    testMinus(-2.7, g);
    testMinus(g, 4);
    testMinus(4, g);

    testMultiply(a, b);
    testMultiply(a, c);
    testMultiply(a, d);
    testMultiply(a, e);
    testMultiply(a, f);
    testMultiply(a, g);
    testMultiply(b, c);
    testMultiply(b, d);
    testMultiply(b, e);
    testMultiply(b, f);
    testMultiply(b, g);
    testMultiply(c, d);
    testMultiply(c, e);
    testMultiply(c, f);
    testMultiply(c, g);
    testMultiply(d, e);
    testMultiply(d, f);
    testMultiply(d, g);
    testMultiply(e, f);
    testMultiply(e, g);
    testMultiply(f, g);
    testMultiply(a, 2.9f);
    testMultiply(2.9f, a);
    testMultiply(a, -2.7);
    testMultiply(-2.7, a);
    testMultiply(a, 4);
    testMultiply(4, a);
    testMultiply(b, 2.9f);
    testMultiply(2.9f, b);
    testMultiply(b, -2.7);
    testMultiply(-2.7, b);
    testMultiply(b, 4);
    testMultiply(4, b);
    testMultiply(c, 2.9f);
    testMultiply(2.9f, c);
    testMultiply(c, -2.7);
    testMultiply(-2.7, c);
    testMultiply(c, 4);
    testMultiply(4, c);
    testMultiply(d, 2.9f);
    testMultiply(2.9f, d);
    testMultiply(d, -2.7);
    testMultiply(-2.7, d);
    testMultiply(d, 4);
    testMultiply(4, d);
    testMultiply(e, 2.9f);
    testMultiply(2.9f, e);
    testMultiply(e, -2.7);
    testMultiply(-2.7, e);
    testMultiply(e, 4);
    testMultiply(4, e);
    testMultiply(f, 2.9f);
    testMultiply(2.9f, f);
    testMultiply(f, -2.7);
    testMultiply(-2.7, f);
    testMultiply(f, 4);
    testMultiply(4, f);
    testMultiply(g, 2.9f);
    testMultiply(2.9f, g);
    testMultiply(g, -2.7);
    testMultiply(-2.7, g);
    testMultiply(g, 4);
    testMultiply(4, g);

    testDevide(a, b);
    testDevide(a, c);
    testDevide(a, d);
    testDevide(a, e);
    testDevide(a, f);
    testDevide(a, g);
    testDevide(b, c);
    testDevide(b, d);
    testDevide(b, e);
    testDevide(b, f);
    testDevide(b, g);
    testDevide(c, d);
    testDevide(c, e);
    testDevide(c, f);
    testDevide(c, g);
    testDevide(d, e);
    testDevide(d, f);
    testDevide(d, g);
    testDevide(e, f);
    testDevide(e, g);
    testDevide(f, g);
    testDevide(a, 2.9f);
    testDevide(2.9f, a);
    testDevide(a, -2.7);
    testDevide(-2.7, a);
    testDevide(a, 4);
    testDevide(4, a);
    testDevide(b, 2.9f);
    testDevide(2.9f, b);
    testDevide(b, -2.7);
    testDevide(-2.7, b);
    testDevide(b, 4);
    testDevide(4, b);
    testDevide(c, 2.9f);
    testDevide(2.9f, c);
    testDevide(c, -2.7);
    testDevide(-2.7, c);
    testDevide(c, 4);
    testDevide(4, c);
    testDevide(d, 2.9f);
    testDevide(2.9f, d);
    testDevide(d, -2.7);
    testDevide(-2.7, d);
    testDevide(d, 4);
    testDevide(4, d);
    testDevide(e, 2.9f);
    testDevide(2.9f, e);
    testDevide(e, -2.7);
    testDevide(-2.7, e);
    testDevide(e, 4);
    testDevide(4, e);
    testDevide(f, 2.9f);
    testDevide(2.9f, f);
    testDevide(f, -2.7);
    testDevide(-2.7, f);
    testDevide(f, 4);
    testDevide(4, f);
    testDevide(g, 2.9f);
    testDevide(2.9f, g);
    testDevide(g, -2.7);
    testDevide(-2.7, g);
    testDevide(g, 4);
    testDevide(4, g);
    
    testSum(a);
    testSum(b);
    testSum(c);
    testSum(d);
    testSum(e);
    testSum(f);
    testSum(g);
    
    testAccumulate(a, fun1<int>);
    testAccumulate(a, fun2<int>);
    testAccumulate(a, fun3<int>);
    testAccumulate(a, fun4<int>);
    testConstAccumulate(a, fun1<int>);
    testConstAccumulate(a, fun2<int>);
    
    testAccumulate(b, fun1<float>);
    testAccumulate(b, fun2<float>);
    testAccumulate(b, fun3<float>);
    testAccumulate(b, fun4<float>);
    testConstAccumulate(b, fun1<float>);
    testConstAccumulate(b, fun2<float>);
    
    testAccumulate(c, fun1<double>);
    testAccumulate(c, fun2<double>);
    testAccumulate(c, fun3<double>);
    testAccumulate(c, fun4<double>);
    testConstAccumulate(c, fun1<double>);
    testConstAccumulate(c, fun2<double>);
    
    testAccumulate(d, fun1<complex<int>>);
    testAccumulate(d, fun2<complex<int>>);
    testAccumulate(d, fun3<complex<int>>);
    testAccumulate(d, fun4<complex<int>>);
    testConstAccumulate(d, fun1<complex<int>>);
    testConstAccumulate(d, fun2<complex<int>>);
    
    testAccumulate(e, fun1<complex<float>>);
    testAccumulate(e, fun2<complex<float>>);
    testAccumulate(e, fun3<complex<float>>);
    testAccumulate(e, fun4<complex<float>>);
    testConstAccumulate(e, fun1<complex<float>>);
    testConstAccumulate(e, fun2<complex<float>>);
    
    testAccumulate(f, fun1<complex<double>>);
    testAccumulate(f, fun2<complex<double>>);
    testAccumulate(f, fun3<complex<double>>);
    testAccumulate(f, fun4<complex<double>>);
    testConstAccumulate(f, fun1<complex<double>>);
    testConstAccumulate(f, fun2<complex<double>>);
    
    testAccumulate(g, fun1<complex<double>>);
    testAccumulate(g, fun2<complex<double>>);
    testAccumulate(g, fun3<complex<double>>);
    testAccumulate(g, fun4<complex<double>>);
    testConstAccumulate(g, fun1<complex<double>>);
    testConstAccumulate(g, fun2<complex<double>>);

    cout << a << endl;
    cout << b << endl;
    cout << c << endl;
    cout << d << endl;
    cout << e << endl;
    cout << f << endl;
    
    cout << "Pass!" << endl;
    return 0;
}
