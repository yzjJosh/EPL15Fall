#include <iostream>
#include <assert.h>
#include <functional>
#include "../../Valarray.h"

using epl::valarray;
using std::cout;
using std::endl;
using std::complex;
using namespace epl;

int InstanceCounter::counter = 0;

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
struct _strip_const_{ using type = T; };

template <typename T>
struct _strip_const_<const T>{ using type = T; };

template <typename T1, typename T2>
bool equals(T1 const& a, T2 const& b){
    if(abs(a) < 0.00001 && abs(b) < 0.00001) return true;
    return std::abs(((double)a - b)/(((double)a + b)/2)) < 0.00001;
}

template <typename T1, typename T2>
bool equals(complex<T1> const& a, complex<T2> const& b){
    double d1 = (double)a.real()-b.real();
    double d2 = (double)a.imag()-b.imag();
    double da = sqrt((double)a.real()*a.real()+(double)a.imag()*a.imag());
    double db = sqrt((double)b.real()*b.real()+(double)b.imag()*b.imag());
    if(da < 0.00001 && db < 0.00001) return true;
    return std::sqrt(d1*d1+d2*d2)/((da+db)/2) < 0.00001;
}


template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
void test(T1 const& a, T2 const& b, T3 const& c, T4 const& d, T5 const& e, T6 const& f){
    valarray<double> d1{9.4, 8.7, 1.9, -3.7};
    int d2 = 3;
    double d3 = 9.7;
    valarray<int> d4{1, 2, 3, 4, 5};
    auto res1 = a + (b + c + d) + (e + f);
    auto res2 = res1 * a * (b * c * d) * (e * f);
    auto res3 = res2 / d1 / d2 / (d3 / d4);
    auto res4 = res3 - a - (b - c - d) - (e - f);
    auto res = (res1 + res2) + (res3 * res4) - (res2 + res1);
    using type = typename _strip_const_<typename std::iterator_traits<decltype(res.begin())>::value_type>::type;
    valarray<type> array = res;
    uint64_t len = std::min(_size_(a), std::min(_size_(b), std::min(_size_(c), std::min(_size_(d), std::min(_size_(e), _size_(f))))));
    len = std::min(len, std::min(_size_(d1), _size_(d4)));
    for(uint64_t i =0; i<len; i++){
        auto item1 = _get_(a, i);
        auto item2 = _get_(b, i);
        auto item3 = _get_(c, i);
        auto item4 = _get_(d, i);
        auto item5 = _get_(e, i);
        auto item6 = _get_(f, i);
        using type1 = typename _strip_const_<decltype(item1)>::type;
        using type2 = typename _strip_const_<decltype(item2)>::type;
        using type3 = typename _strip_const_<decltype(item3)>::type;
        using type4 = typename _strip_const_<decltype(item4)>::type;
        using type5 = typename _strip_const_<decltype(item5)>::type;
        using type6 = typename _strip_const_<decltype(item6)>::type;
        type a_ = _typeCast_<type1, type>(item1);
        type b_ = _typeCast_<type2, type>(item2);
        type c_ = _typeCast_<type3, type>(item3);
        type d_ = _typeCast_<type4, type>(item4);
        type e_ = _typeCast_<type5, type>(item5);
        type f_ = _typeCast_<type6, type>(item6);
        type d1_ = _typeCast_<double, type>(d1[i]);
        type d2_ = _typeCast_<int, type>(d2);
        type d3_ = _typeCast_<double, type>(d3);
        type d4_ = _typeCast_<int, type>(d4[i]);
        type temp1 = a_ + (b_ + c_ + d_) + (e_ + f_);
        type temp2 = temp1 * a_ * (b_ * c_ * d_) * (e_ * f_);
        type temp3 = temp2 / d1_ / d2_ / (d3_ / d4_);
        type temp4 = temp3 - a_ - (b_ - c_ - d_) - (e_ - f_);
        type temp = (temp1 + temp2) + (temp3 * temp4) - (temp2 + temp1);
        assert(equals(temp, array[i]));
        assert(equals(temp, res[i]));
    }
}

template <typename T>
void testSum(T const& array){
    using type = typename _strip_const_<typename std::iterator_traits<decltype(array.begin())>::value_type>::type;
    type sum(0);
    for(auto x: array)
        sum += x;
    assert(sum == array.sum());
}


template <typename T, typename Fun>
void testAccumulate(T array, Fun fun){
    if(array.size() < 2) return;
    using type = typename _strip_const_<typename std::iterator_traits<decltype(array.begin())>::value_type>::type;
    type res(fun(array[0], array[1]));
    for(int i=2; i<array.size(); i++)
        res = fun(res, array[i]);
    assert(res == array.accumulate(fun));
}

template <typename T, typename Fun>
void testConstAccumulate(T const& array, Fun fun){
    if(array.size() < 2) return;
    using type = typename _strip_const_<typename std::iterator_traits<decltype(array.begin())>::value_type>::type;
    type res(fun(array[0], array[1]));
    for(int i=2; i<array.size(); i++)
        res = fun(res, array[i]);
    assert(res == array.accumulate(fun));
}

template <typename T>
T fun1(T const x, T const y){ return x+y; }

template <typename T>
T fun2(T const x, T const y){ return x*y; }

template <typename T>
T fun3(T x, T y) {return x*T(2)+y*y+x*y; }

template <typename T>
T fun4(T x, T y) { x = x; y = y; return std::sin(x+y);}

template <typename T>
T fun5(T x) { return x; };


int main(void) {
    valarray<int> a{-1, 1, 2, 3, 4, 5, 6, -3, 56, 90, -4};
    valarray<float> b{0.1f, 8.3f, -3.4f, 1.0f, 4.3f, 1.3f, -0.1f, 0.0003f, 343.98f};
    valarray<double> c{2.5, 2.5, 2.5, 2.5, -0.2, 0.5, 1.43, 3.1415926};
    valarray<complex<int>> d{ complex<int>(3, -1), complex<int>(-3, 0), complex<int>(0, 1)};
    valarray<complex<float>> e{ complex<float>(0.0f, 1.1f), complex<float>(1.2f, -2.3f), complex<float>(2.8, 1.9)};
    valarray<complex<double>> f{ complex<double>(0.0, -3.2), complex<double>(1.3), complex<double>(1.565673, 34343)};
    valarray<complex<double>> g;
    int _a = 3;
    float _b = -4.67f;
    double _c = 3.141592653;
    
 /*   test(_b,a,f,c,_c,_c);
    test(_c,_a,b,c,e,e);
    test(_a,e,_c,c,c,_b);
    test(b,a,_c,e,b,a);
    test(e,_a,_c,_c,_c,e);
    test(f,_a,b,_a,_b,b);
    test(_a,b,f,a,a,c);
    test(c,_a,b,_a,_c,f);
    test(f,_a,e,a,b,b);
    test(_a,a,_a,c,f,b);
    test(a,f,_c,f,_a,f);
    test(e,_c,b,f,b,b);
    test(_b,e,_b,f,_a,f);
    test(_a,a,f,_b,e,e);
    test(_a,b,e,c,b,e);
    test(c,c,c,a,a,c);
    test(_a,f,e,_a,a,f);
    test(_a,a,e,b,c,_c);
    test(e,_c,_c,c,_b,e);
    test(_a,b,c,e,c,c);
    test(c,f,_a,e,b,_a);
    test(a,_c,_b,_a,c,b);
    test(c,_a,_a,_c,f,f);
    test(f,a,c,a,e,f);
    test(_c,c,b,_a,_a,f);
    test(b,c,b,c,e,f);
    test(c,_b,_b,_c,c,f);
    test(_b,_c,f,a,_b,a);
    test(a,_c,b,_b,c,c);
    test(b,b,_c,_b,_b,e);
    test(e,f,e,e,a,_c);
    test(_c,_c,b,_a,b,_c);
    test(e,_b,c,_a,f,a);
    test(_a,c,a,_b,_c,_b);
    test(_a,e,e,_c,f,b);
    test(b,a,_b,b,f,f);
    test(e,c,_b,e,a,_c);
    test(f,b,a,_c,_a,f);
    test(a,c,_c,_b,c,c);
    test(_c,_a,c,_b,a,_b);
    test(_a,a,_a,_a,a,a);
    test(_b,e,f,_a,b,c);
    test(f,a,f,a,e,a);
    test(_a,a,_c,c,f,b);
    test(e,f,_b,b,_a,a);
    test(f,c,_b,_c,_c,a);
    test(c,_b,_b,e,f,c);
    test(_c,a,a,a,a,b);
    test(c,_b,_a,c,_c,c);
    test(b,_b,_a,b,f,f);
    test(_a,c,c,b,e,_b);
    test(_b,_a,b,_b,c,_a);
    test(_a,f,a,_c,e,_a);
    test(a,b,_b,_c,e,b);
    test(c,f,b,c,b,f);
    test(a,b,_b,f,_b,_a);
    test(b,f,f,e,e,_b);
    test(b,_b,b,f,_c,c);
    test(b,a,_b,_b,b,f);
    test(b,e,_c,a,c,b);
    test(e,a,c,e,_b,_b);
    test(_a,_b,c,_a,_a,_b);
    test(f,e,b,b,e,c);
    test(e,_b,e,_c,_c,c);
    test(_a,_c,f,c,f,_c);
    test(e,_c,e,_a,_a,e);
    test(a,b,_b,a,a,f);
    test(_c,b,a,a,b,e);
    test(e,c,a,f,_a,_c);
    test(b,c,_b,c,c,f);
    test(f,f,e,_b,_c,b);
    test(c,_c,a,b,e,c);
    test(_a,b,_a,c,c,e);
    test(_c,f,f,_c,e,e);
    test(_b,_c,c,_b,_c,b);
    test(_c,f,f,b,a,b);
    test(_a,b,a,c,c,_c);
    test(_c,f,a,c,_a,c);
    test(e,_a,e,c,_c,_a);
    test(c,c,c,_c,e,a);
    test(_b,_c,a,_b,b,a);
    test(_b,_c,_a,b,_a,b);
    test(e,_c,_b,e,_c,e);
    test(a,_a,a,_b,_b,c);
    test(_a,e,c,b,_a,f);
    test(_a,a,a,c,b,_b);
    test(_c,_c,_c,c,e,_a);
    test(_b,_c,_a,_c,c,_c);
    test(e,_a,e,c,a,b);
    test(_c,e,_a,a,e,_c);
    test(_a,e,_c,f,b,_b);
    test(_b,e,_b,c,a,_c);
    test(f,f,c,b,a,a);
    test(f,c,_b,_b,_b,e);
    test(_c,c,b,c,f,a);
    test(e,f,_c,b,f,a);
    test(a,_c,f,_a,b,f);
    test(a,b,a,b,f,a);
    test(b,f,b,_b,f,_b);
    test(a,a,e,_a,_a,a);
    test(c,c,_b,f,c,f);
    test(c,b,_c,_a,c,c);
    test(_a,_c,c,_b,_b,_c);
    test(_a,c,_a,_b,_b,a);
    test(_a,_b,_a,b,_c,e);
    test(a,e,f,e,e,c);
    test(f,e,a,c,_b,f);
    test(e,_a,a,e,_c,_b);
    test(_b,b,c,_a,e,_a);
    test(c,_a,_a,c,_b,f);
    test(f,b,c,c,_a,_a);
    test(_b,_a,_c,e,e,a);
    test(a,b,a,a,b,a);
    test(f,c,b,a,_c,_c);
    test(a,_a,_b,_c,a,_c);
    test(a,_b,_b,_b,_c,c);
    test(e,_b,_b,_b,b,c);
    test(a,e,_c,c,f,b);
    test(_c,a,_a,a,_b,c);
    test(_c,e,a,c,b,f);
    test(f,b,a,_a,_a,f);
    test(a,b,e,_a,e,c);
    test(_c,_c,a,b,_a,b);
    test(c,f,b,e,a,_a);
    test(a,_b,e,_b,f,b);
    test(c,a,f,f,e,e);
    test(_a,f,f,c,b,e);
    test(b,f,f,b,c,_a);
    test(b,_c,c,e,c,_c);
    test(e,b,c,_a,a,_c);
    test(a,_a,f,c,c,a);
    test(b,b,_b,e,e,_a);
    test(b,_a,c,_c,_c,_a);
    test(e,_c,e,c,_b,_b);
    test(_b,a,_b,_b,f,f);
    test(_b,e,_c,b,f,a);
    test(b,a,b,_c,_a,c);
    test(f,f,e,a,e,f);
    test(a,a,e,_c,f,_a);
    test(b,b,f,_c,e,a);
    test(_b,_a,_a,c,e,_c);
    test(e,_c,b,_a,_c,e);
    test(_a,_a,a,c,_b,f);
    test(e,e,f,a,_a,_b);
    test(b,e,b,a,_a,c);
    test(_b,_c,a,_c,a,e);
    test(b,_c,f,a,e,c);
    test(_a,a,_a,e,b,_a);
    test(e,f,e,c,c,_c);
    test(a,_a,b,_c,_a,_c);
    test(_a,_c,f,_b,e,_c);
    test(_b,f,_c,b,e,_a);
    test(f,_c,b,e,c,_a);
    test(a,_b,b,_c,f,b);
    test(b,_b,_c,f,a,b);
    test(c,a,f,a,c,_b);
    test(c,c,c,f,_b,_c);
    test(b,e,_a,f,_a,c);
    test(f,b,b,b,b,_b);
    test(_a,_b,f,f,a,b);
    test(e,_b,_c,c,a,f);
    test(b,_a,_a,_b,_c,c);
    test(f,b,_b,b,b,_a);
    test(a,e,_a,e,c,a);
    test(_b,_b,e,_b,e,_b);
    test(a,_b,e,f,_a,e);
    test(_b,b,e,_c,_c,_c);
    test(_c,_c,_b,a,_a,e);
    test(b,_a,f,c,a,_c);
    test(a,_b,_b,_a,b,_b);
    test(f,e,a,_b,_b,_b);
    test(_b,f,b,a,_a,b);
    test(f,b,_c,_c,a,c);
    test(c,_b,f,_a,b,a);
    test(e,c,f,b,a,a);
    test(b,b,c,_a,e,_a);
    test(f,_a,e,b,f,_b);
    test(e,_b,_b,c,f,f);
    test(a,e,f,_b,_a,c);
    test(c,b,_a,b,f,_a);
    test(c,_b,e,b,b,e);
    test(_b,e,_b,_a,b,_c);
    test(e,_a,b,e,_a,_a);
    test(_c,e,b,_a,e,_c);
    test(_b,f,c,_b,_c,_a);
    test(_a,f,_c,b,b,_b);
    test(a,a,c,_b,b,b);
    test(_c,f,e,_c,a,f);
    test(c,e,_b,e,a,_a);
    test(_a,_c,f,_b,c,c);
    test(f,f,_c,b,_b,f);
    test(_c,_b,_b,c,e,a);
    test(_b,b,_c,_a,c,b);
    test(_b,f,_b,_c,_a,_a);
    test(b,_b,b,a,b,f);
    test(_a,_a,a,_a,a,a);
    test(f,c,e,_a,a,_b);
    test(_a,c,a,f,e,a);
    test(f,a,c,_b,_a,f);
    test(_b,_b,_a,_a,_c,a);
    test(f,f,b,_c,e,_a);
    test(_a,f,_b,_c,_b,f);
    test(b,_a,a,_c,_c,f);
    test(_c,_b,a,f,c,e);
    test(a,e,f,f,b,_b);
    test(a,a,a,_c,f,_b);
    test(_a,_a,b,b,e,_a);
    test(c,b,_b,_b,_c,e);
    test(_c,_a,_c,_c,a,e);
    test(f,_a,_b,_a,_c,_a);
    test(b,f,f,_b,b,_a);
    test(_b,b,b,c,_b,_a);
    test(a,c,f,c,_c,b);
    test(e,f,e,_c,a,b);
    test(_b,f,f,b,_c,_b);
    test(b,e,b,f,e,_c);
    test(b,c,_a,_a,_b,c);
    test(b,_c,e,_b,e,c);
    test(a,f,c,b,f,_a);
    test(c,_a,a,_b,_b,_a);
    test(c,b,b,_c,e,f);
    test(e,f,_c,_b,a,_b);
    test(f,b,c,c,b,c);
    test(f,b,b,f,b,f);
    test(_b,c,_a,_a,_b,f);
    test(_c,f,a,b,b,c);
    test(_c,_c,_b,b,_b,_b);
    test(e,e,_c,c,e,b);
    test(b,e,_b,c,a,_a);
    test(_b,c,f,_b,b,c);
    test(_a,a,c,_a,e,c);
    test(_a,f,a,b,_a,_b);
    test(c,a,a,f,a,c);
    test(f,a,f,a,_c,b);
    test(_a,a,_a,c,_b,f);
    test(_c,c,_a,e,e,_b);
    test(a,b,f,b,_c,e);
    test(_b,a,_a,_b,c,a);
    test(e,c,_b,e,_c,_b);
    test(a,f,_c,_a,_a,_a);
    test(_b,_b,_b,e,e,a);
    test(e,c,b,_b,f,_b);
    test(b,a,_a,c,c,_b);
    test(b,f,c,_b,c,_b);
    test(_b,b,e,a,c,c);
    test(a,_b,b,_a,a,b);
    test(_a,_a,_c,e,_b,b);
    test(b,c,_b,a,f,_c);
    test(e,b,a,e,c,f);
    test(_c,_b,_c,_c,c,_c);
    test(_b,f,e,_b,_a,_c);
    test(_c,_b,e,c,_b,f);
    test(_c,a,b,e,c,e);
    test(c,f,_b,_a,_b,f);
    test(_a,a,a,_c,_b,e);
    test(f,a,e,f,c,e);
    test(_b,_a,c,a,_b,b);
    test(_b,_c,b,c,_b,f);
    test(e,b,b,_b,b,_a);
    test(f,f,_b,b,_b,a);
    test(c,e,_c,_c,_b,b);
    test(b,_a,a,_b,_a,_b);
    test(f,e,f,f,a,a);
    test(_a,c,_b,f,_c,e);
    test(_a,c,a,b,b,a);
    test(_a,e,a,_a,e,_c);
    test(c,b,e,b,_b,e);
    test(b,e,_b,f,c,_a);
    test(c,_a,a,_b,e,f);
    test(_c,_b,c,a,c,_c);
    test(a,b,_b,e,_c,_a);
    test(_c,e,b,f,f,_b);
    test(c,_a,b,b,_c,a);
    test(c,b,_a,_c,f,f);
    test(b,f,b,_c,b,a);
    test(e,a,_a,_b,_a,_b);
    test(_b,_b,e,e,_b,a);
    test(a,f,_a,a,_c,f);
    test(_c,c,c,_c,c,e);
    test(f,c,c,_a,_c,f);
    test(_b,_c,_c,_a,e,f);
    test(_c,a,_b,a,b,_c);
    test(_c,b,b,f,f,e);
    test(_c,b,_c,e,_c,f);
    test(a,_c,f,f,a,f);
    test(b,f,f,_c,e,b);
    test(_a,f,a,_b,e,c);
    test(b,_b,b,_a,_c,f);
    test(e,_b,c,_a,b,_b);
    test(_b,b,b,c,f,f);
    test(_b,_a,_b,b,_a,e);
    test(_a,f,e,a,e,c);
    test(_a,a,c,_a,e,c);
    test(_a,_c,_b,_b,a,a);
    test(b,a,e,_a,_c,_c);
    test(a,_a,c,f,_c,a);
    test(f,a,a,c,c,_b);
    test(a,_b,_c,c,_b,b);
    test(e,b,c,a,f,f);
    test(a,_a,a,_a,e,f);
    test(c,f,c,_c,c,c);
    test(f,a,f,b,c,f);
    test(f,e,_c,_b,a,f);
    test(_c,_a,_c,a,_a,a);
    test(_b,e,c,c,c,_a);
    test(_a,a,b,a,_c,a);
    test(e,_b,b,_b,_b,_c);
    test(e,_a,_b,_a,f,f);
    test(a,c,f,_a,e,a);
    test(_a,_b,b,e,a,_b);
    test(f,e,a,_b,_b,_c);
    test(f,a,_a,a,a,b);
    test(_c,a,b,_c,c,e);
    test(f,e,a,c,_c,c);
    test(_b,c,f,a,e,_a);
    test(a,a,_c,b,_c,a);
    test(_c,f,c,b,e,_a);
    test(_c,_b,_c,a,_c,_a);
    test(f,f,e,e,e,b);
    test(b,_c,c,b,_b,_b);
    test(f,a,_b,_b,f,e);
    test(c,_b,a,_c,_c,a);
    test(e,e,a,b,_a,c);
    test(_a,_a,_b,a,_c,_a);
    test(c,b,_b,b,b,_a);
    test(a,f,_b,_c,_b,e);
    test(_c,_a,_c,b,_a,b);
    test(_c,a,_b,_a,_b,_a);
    test(_a,a,_b,b,_c,_a);
    test(a,f,_a,e,_b,c);
    test(e,e,c,c,c,e);
    test(a,b,b,_c,c,a);
    test(_c,f,e,c,e,_a);
    test(a,_b,a,a,b,c);
    test(e,c,_a,c,a,f);
    test(c,_a,f,e,_c,_b);
    test(f,e,_b,f,_c,c);
    test(e,_b,_a,a,_c,_b);
    test(c,e,b,e,b,a);
    test(b,_c,b,_a,_b,e);
    test(_a,_b,_b,_b,a,_a);
    test(_c,f,_a,a,c,_a);
    test(c,b,a,c,c,_c);
    test(f,_c,e,b,b,_a);
    test(_a,_a,e,b,_b,f);
    test(b,_b,b,e,f,a);
    test(_c,c,c,_c,c,_a);
    test(e,_a,a,a,a,f);
    test(e,_c,_b,b,c,_c);
    test(_b,_a,e,b,_c,e);
    test(f,f,_b,e,f,_b);
    test(_c,_b,e,_c,e,f);
    test(e,_b,_b,_a,e,f);
    test(a,_a,e,a,a,_c);
    test(c,f,b,e,e,b);
    test(a,a,_b,_a,f,_b);
    test(f,b,a,b,_a,e);
    test(f,_c,e,_b,a,f);
    test(c,a,b,b,b,_b);
    test(_a,_a,_a,c,b,b);
    test(a,e,a,a,_c,b);
    test(b,f,_b,_c,a,_a);
    test(e,a,_a,_a,_c,f);
    test(_a,c,e,f,f,b);
    test(a,e,c,b,b,_c);
    test(_a,e,_a,b,e,_a);
    test(e,a,_b,_a,b,f);
    test(c,c,b,_c,f,c);
    test(_b,_c,b,_c,f,a);
    test(_b,_b,b,b,e,_b);
    test(_c,a,a,a,_c,f);
    test(_c,_a,_b,a,b,_c);
    test(f,e,_b,_a,f,_a);
    test(_a,_b,_a,e,a,a);
    test(_a,_c,b,e,e,_a);
    test(a,_b,f,_b,_a,_a);
    test(f,e,_c,e,c,a);
    test(_a,c,f,_b,_c,a);
    test(b,_b,a,c,_a,a);
    test(_b,_b,b,c,e,_a);
    test(f,e,_b,e,c,_a);
    test(a,b,_b,_c,a,b);
    test(_b,_a,c,a,e,a);
    test(e,f,a,_b,_a,_a);
    test(e,a,a,b,a,_a);
    test(_b,e,e,f,_b,a);
    test(f,c,b,_a,_a,a);
    test(f,a,a,e,_b,_a);
    test(c,e,b,b,f,c);
    test(_a,_a,a,_c,a,b);
    test(f,e,a,_a,b,b);
    test(e,c,a,e,b,_a);
    test(b,_a,_b,_b,_a,_c);
    test(f,_b,c,a,e,c);
    test(e,b,_a,c,f,_c);
    test(_a,f,a,f,_c,_a);
    test(f,_b,_c,_c,f,_c);
    test(f,b,_a,_c,_c,_c);
    test(e,_b,_a,_b,f,_a);
    test(c,a,_a,e,c,f);
    test(b,_b,_a,a,_b,b);
    test(c,_a,a,a,f,b);
    test(c,e,_c,a,b,b);
    test(_b,_c,f,e,f,_b);
    test(a,e,f,_c,e,f);
    test(f,c,c,_a,_c,f);
    test(e,c,a,e,c,_a);
    test(_b,_c,e,_c,b,_b);
    test(f,a,c,f,c,_c);
    test(b,_b,e,f,a,_b);
    test(f,e,c,b,e,f);
    test(f,c,c,_b,b,e);
    test(a,b,_c,b,c,b);
    test(a,_c,f,_b,_a,_a);
    test(a,b,b,f,e,b);
    test(b,_b,b,a,c,f);
    test(a,_c,c,_c,_b,c);
    test(_c,f,f,e,_b,c);
    test(_a,b,c,a,_b,_c);
    test(f,_a,c,e,e,_a);
    test(_c,c,c,b,_b,f);
    test(_b,_b,e,e,f,b);
    test(a,a,a,a,a,_b);
    test(b,_b,e,_c,_c,b);
    test(a,b,e,a,_c,_b);
    test(_c,f,a,_b,a,c);
    test(_c,f,b,c,f,e);
    test(e,c,f,_a,_c,f);
    test(_c,_b,e,_b,_a,_c);
    test(b,b,_b,c,_c,_a);
    test(b,_b,e,_b,f,e);
    test(f,e,a,c,_a,f);
    test(e,e,_a,b,f,e);
    test(a,c,_a,f,c,_a);
    test(a,c,a,f,f,c);
    test(_a,_b,c,e,f,_a);
    test(a,b,e,b,a,a);
    test(c,_b,_b,b,e,b);
    test(_c,a,_a,_b,b,_a);
    test(f,f,_a,_b,c,f);
    test(_a,_b,_c,b,_c,_c);
    test(f,b,f,a,b,a);
    test(b,_b,_c,_b,f,_b);
    test(_c,_a,c,b,_b,_c);
    test(f,_c,b,c,f,_a);
    test(f,a,f,f,e,_c);
    test(_c,_c,_c,f,b,_a);
    test(_b,_a,a,_c,_c,a);
    test(f,c,a,b,_b,c);
    test(b,a,_b,_c,_a,_c);
    test(e,_c,_b,c,e,_b);
    test(_a,_c,_b,a,e,_b);
    test(e,_c,_b,_b,_b,_c);
    test(b,f,f,c,_a,_a);
    test(_a,b,_a,b,c,a);
    test(b,_c,b,c,e,c);
    test(f,_c,_a,b,_a,_b);
    test(e,_c,e,e,c,e);
    test(a,_c,c,f,b,f);
    test(_a,_a,e,e,_b,a);
    test(f,_c,b,a,_c,_a);
    test(_c,f,b,b,_c,f);
    test(_b,e,c,a,a,e);
    test(b,e,a,c,e,b);
    test(b,b,c,_c,f,c);
    test(b,_b,b,_b,_a,e);
    test(_b,f,a,c,c,f);
    test(c,c,c,b,a,a);
    test(b,f,b,_a,a,a);
    test(f,_a,e,e,e,_a);
    test(c,f,e,c,e,a);
    test(e,c,e,_c,_c,_c);
    test(f,f,f,a,e,c);
    test(e,_b,e,f,e,c);
    test(e,b,f,b,_b,_b);
    test(e,a,f,_a,_a,f);
    test(a,c,c,_b,_a,b);
    test(_c,f,a,f,_a,a);
    test(_b,b,c,a,_a,e);
    test(a,f,_c,a,_a,b);
    test(a,a,f,c,_c,f);
    test(a,a,_c,_b,_c,c);
    test(_c,_c,f,b,a,_b);
    test(a,_b,c,b,b,e);
    test(_c,b,a,b,e,c);
    test(e,b,f,f,b,_c);
    test(c,_a,e,c,_c,_a);
    test(c,_c,f,e,e,_b);
    test(_b,f,_a,c,c,a);
    test(_b,_c,c,_c,b,f);
    test(e,e,e,_b,_c,_c);
    test(_b,f,b,a,a,f);
    test(b,f,_b,_a,_b,c);
    test(b,f,e,_c,a,c);
    test(c,_b,_b,f,c,b);
    test(_a,b,_b,b,c,f);
    test(b,_a,_a,c,c,b);
    test(_a,_a,_c,_b,_b,b);
    test(_b,b,a,a,_c,f);
    test(e,c,_a,c,a,_a);*/
    
    testSum(a);
    testSum(b);
    testSum(c);
    testSum(d);
    testSum(e);
    testSum(f);
    testSum(a+b);
    testSum(c-d);
    testSum(f-_a);
    testSum(_c-a);
    testSum(a*c);
    testSum(_c*f);
    testSum(a/c);
    testSum(_a/f);
    testSum(c/_b);
    
    
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
    
    testAccumulate(a+b, fun1<float>);
    testAccumulate(a+b, fun2<float>);
    testAccumulate(a+b, fun3<float>);
    testAccumulate(a+b, fun4<float>);
    
    testAccumulate(a-b, fun1<float>);
    testAccumulate(a-b, fun2<float>);
    testAccumulate(a-b, fun3<float>);
    testAccumulate(a-b, fun4<float>);
    
    testAccumulate(a*b, fun1<float>);
    testAccumulate(a*b, fun2<float>);
    testAccumulate(a*b, fun3<float>);
    testAccumulate(a*b, fun4<float>);
    
    testAccumulate(a/b, fun1<float>);
    testAccumulate(a/b, fun2<float>);
    testAccumulate(a/b, fun3<float>);
    testAccumulate(a/b, fun4<float>);
    
    testAccumulate(a+_b, fun1<float>);
    testAccumulate(_a+b, fun2<float>);
    testAccumulate(a+_b, fun3<float>);
    testAccumulate(_a+b, fun4<float>);
    
    testAccumulate(a-_b, fun1<float>);
    testAccumulate(_a-b, fun2<float>);
    testAccumulate(a-_b, fun3<float>);
    testAccumulate(_a-b, fun4<float>);
    
    testAccumulate(a*_b, fun1<float>);
    testAccumulate(_a*b, fun2<float>);
    testAccumulate(a*_b, fun3<float>);
    testAccumulate(_a*b, fun4<float>);
    
    testAccumulate(a/_b, fun1<float>);
    testAccumulate(_a/b, fun2<float>);
    testAccumulate(a/_b, fun3<float>);
    testAccumulate(_a/b, fun4<float>);
    
    a.sqrt();
    b.sqrt();
    c.sqrt();
    d.sqrt();
    e.sqrt();
    f.sqrt();
    a.apply(fun5<int>);
    b.apply(fun5<float>);
    c.apply(fun5<double>);
    d.apply(fun5<complex<int>>);
    e.apply(fun5<complex<float>>);
    f.apply(fun5<complex<double>>);
    (a+b).sqrt();
    (c+d).sqrt();
    (a+_b).sqrt();
    (_a+d).sqrt();
    (c+_c).sqrt();
    (a+b).apply(fun5<float>);
    (c+d).apply(fun5<complex<double>>);
    (a+_b).apply(fun5<float>);
    (_a+d).apply(fun5<complex<int>>);
    (c+_c).apply(fun5<complex<double>>);
    const valarray<int>& const_a = a;
    const_a.sqrt();
    const_a.sum();
    const_a.apply(fun5<int>);
    
    cout << a << endl;
    cout << b << endl;
    cout << c << endl;
    cout << d << endl;
    cout << e << endl;
    cout << f << endl;
    
    cout << "Pass!" << endl;
    return 0;
}
