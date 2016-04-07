#include <iostream>
#include <assert.h>
#include <list>
#include "../../../Vector.h"

using std::cout;
using std::endl;

class Foo{
public:
    const int constructor;
    
    Foo(int& a): constructor(0){}
    Foo(int& a, int& b): constructor(1){}
    Foo(int&& a, int& b): constructor(2){}
    Foo(int& a, int&& b, int&& c): constructor(3){}
    Foo(int& a, int& b, int& c): constructor(4){}
    Foo(int& a, int& b, int& c, int& d): constructor(5){}
    Foo(int&& a, int&& b, int& c, int& d): constructor(6){}
    Foo(Foo const&): constructor(7){}
    Foo(Foo&& that): constructor(that.constructor){}
};

int main(void) {
    epl::vector<int> v;
    assert(v.size() == 0);
    for(int i=0; i<10000; i++){
        v.push_back(i);
        assert(v[i] == i);
        assert(v.size() == i+1);
    }
    try{
        *(v.end()+1);
        assert(false);
    }catch(epl::invalid_iterator e){
        assert(e.what() == "Severe");
    }
    auto begin = v.begin();
    auto end = v.end();
    for(int i=0; begin != end; begin++, i++)
        assert(*begin == i);
    epl::vector<int> copy(v.begin(), v.end());
    begin = copy.begin();
    end = copy.end();
    for(int i=0; begin!=end; begin++, i++){
        assert(*begin == copy[i]);
        assert(v[i] == copy[i]);
    }
    std::list<int> l{0, 1, 2, 3, 4, 5, 6, 7};
    copy = epl::vector<int>(l.begin(), l.end());
    assert(copy.size() == l.size());
    begin = copy.begin();
    end = copy.end();
    auto lbegin = l.begin();
    for(int i=0; begin!=end; begin++, i++,lbegin++)
        assert(*begin == *lbegin);
    begin = v.begin();
    for(int i=0; i<10000; i++){
        v.push_front(i);
        assert(v[0] == i);
        assert(v.size() == 10001+i);
    }
    try{
        *begin;
        assert(false);
    }catch(epl::invalid_iterator e){
        assert(e.what() == "Moderate");
    }
    begin = v.begin();
    int value = v[v.size()-1];
    v.pop_back();
    v.emplace_back(value);
    assert(v[v.size()-1] == value);
    try{
        *begin;
        assert(false);
    }catch(epl::invalid_iterator e){
        assert(e.what() == "Mild");
    }
    begin = v.begin();
    end = v.end();
    for(int i=0; begin != end; begin++, i++)
        assert(v[i] == *begin);
    for(int i=9999; i>=0; i--){
        assert(v[v.size()-1] == i);
        v.pop_back();
        assert(v.size() == 10000+i);
    }
    begin = v.begin();
    end = v.end();
    for(int i=0; begin != end; begin++, i++)
        assert(v[i] == *begin);
    assert(*(v.begin()+10) == v[10]);
    assert(*(v.end()-10) == v[v.size()-10]);
    assert(v.end()-v.begin() == v.size());
    assert(v.end()-epl::vector<int>(v).begin() == v.size());
    begin = v.begin();
    assert(*(begin++) == v[0]);
    assert(*begin == v[1]);
    assert(*(++begin) == v[2]);
    assert(*begin == v[2]);
    assert(*(begin--) == v[2]);
    assert(*begin == v[1]);
    assert(*(--begin) == v[0]);
    assert(*begin == v[0]);
    assert(*(begin += 10) == v[10]);
    assert(*begin == v[10]);
    assert(begin - v.begin() == 10);
    assert(*(begin -= 5) == v[5]);
    assert(*begin == v[5]);
    assert(begin - v.begin() == 5);
    assert(begin - 5 + v.size() == v.end());
    assert(begin - v.begin() == 5);
    assert(*begin++ == v[5]);
    assert(*begin == v[6]);
    assert(*begin-- == v[6]);
    assert(*begin == v[5]);
    assert(begin[0] == v[5]);
    assert(begin[1] == v[6]);
    assert(begin[-1] == v[4]);
    assert(*(1+begin) == v[6]);
    for(int i=9999; i>=0; i--){
        assert(v[0] == i);
        v.pop_front();
        assert(v.size() == i);
    }
    assert(v.size() == 0);
    assert(v.begin() == v.end());
    for(int i=0; i<10000; i++){
        v.push_back(std::move(i));
        assert(v[v.size()-1] == i);
        v.push_front(std::move(i));
        assert(v[0] == i);
        assert(v.size() == (i+1)*2);
    }
    begin = v.begin();
    end = v.end();
    for(int i=0; begin != end; begin++, i++)
        assert(v[i] == *begin);
    while(v.size() > 0) v.pop_front();
    begin = v.begin();
    end = v.end();
    for(int i=0; begin != end; begin++, i++)
        assert(v[i] == *begin);
    assert(v.size() == 0);
    assert(v.begin() == v.end());
    assert(v.begin() != epl::vector<int>(v).begin());
    assert(!(v.begin() < epl::vector<int>(v).begin()));
    assert(v.end()-epl::vector<int>(v).begin() == 0);
    v = epl::vector<int>(6);
    begin = v.begin();
    v.push_back(0);
    try{
        *begin;
        assert(false);
    }catch(epl::invalid_iterator e){
        assert(e.what() == "Moderate");
    }
    try{
        begin + 10;
        assert(false);
    }catch(epl::invalid_iterator e){
        assert(e.what() == "Moderate");
    }
    try{
        begin - 10;
        assert(false);
    }catch(epl::invalid_iterator e){
        assert(e.what() == "Moderate");
    }
    try{
        begin ++;
        assert(false);
    }catch(epl::invalid_iterator e){
        assert(e.what() == "Moderate");
    }
    try{
        begin --;
        assert(false);
    }catch(epl::invalid_iterator e){
        assert(e.what() == "Moderate");
    }
    try{
        ++ begin;
        assert(false);
    }catch(epl::invalid_iterator e){
        assert(e.what() == "Moderate");
    }
    try{
        -- begin;
        assert(false);
    }catch(epl::invalid_iterator e){
        assert(e.what() == "Moderate");
    }
    try{
        begin += 10;
        assert(false);
    }catch(epl::invalid_iterator e){
        assert(e.what() == "Moderate");
    }
    try{
        begin -= 10;
        assert(false);
    }catch(epl::invalid_iterator e){
        assert(e.what() == "Moderate");
    }
    try{
        v.begin() == begin;
        assert(false);
    }catch(epl::invalid_iterator e){
        assert(e.what() == "Moderate");
    }
    try{
        v.begin() < begin;
        assert(false);
    }catch(epl::invalid_iterator e){
        assert(e.what() == "Moderate");
    }
    try{
        v.begin() - begin;
        assert(false);
    }catch(epl::invalid_iterator e){
        assert(e.what() == "Moderate");
    }
    try{
        begin = v.end() + 5;
    }catch(epl::invalid_iterator e){
        assert(false);
    }
    v = {1, 3, 5, 0 , 5};
    assert(v.size() == 5);
    assert(v.end() - v.begin() == v.size());
    begin = v.begin();
    end = v.end();
    for(int i=0; begin != end; begin++, i++){
        assert(v[i] == *begin);
        *begin = -i;
        assert(*begin == -i);
        assert(v[i] == -i);
    }
    const epl::vector<int>& const_v = v;
    epl::vector<int>::const_iterator const_begin = const_v.begin();
    epl::vector<int>::const_iterator const_end = const_v.end();
    assert(const_end-const_begin == const_v.size());
    for(int i=0; const_begin != const_end; const_begin+=1, i++){
        assert(v[i] == *const_begin);
       // *const_begin = i;
    }
    const_begin = v.begin();
    const_end = v.end();
    for(int i=0; const_begin != const_end; const_begin+=1, i++)
        assert(v[i] == *const_begin);
    const_begin = const_v.begin();
    assert(*(const_begin ++) == const_v[0]);
    assert(*const_begin == const_v[1]);
    assert(*(++ const_begin) == const_v[2]);
    assert(*const_begin == const_v[2]);
    assert(const_begin - const_v.begin() == 2);
    assert(const_v.end() - const_v.begin() == const_v.size());
    assert(*(const_begin--) == const_v[2]);
    assert(*const_begin == const_v[1]);
    assert(*(--const_begin) == const_v[0]);
    assert(*const_begin == const_v[0]);
    assert(*const_begin++ == v[0]);
    assert(*const_begin == v[1]);
    assert(*const_begin-- == v[1]);
    assert(*const_begin == v[0]);
    assert(*(const_begin += 1) == v[1]);
    assert(const_begin[0] == v[1]);
    assert(const_begin[1] == v[2]);
    assert(const_begin[-1] == v[0]);
    assert(*(1+const_begin) == v[2]);
    const_begin = const_end - 1;
    assert(const_begin - const_end == -1);
    epl::vector<int>::const_iterator const_copy{const_begin-1};
    assert(const_copy - const_begin == -1);
//    auto c = const_copy + 1;
//    c = begin;
//    assert(c == const_copy+1);
    //begin = const_begin;
    v = {};
    assert(v.size() == 0);
    try{
        v[-1] = 0;
        assert(false);
    }catch(std::exception e){}
    try{
        v[0] = 0;
        assert(false);
    }catch(std::exception e){}
    try{
        v.pop_back();
        assert(false);
    }catch(std::exception e){}
    try{
        v.pop_front();
        assert(false);
    }catch(std::exception e){}
    epl::vector<int> w(10000);
    assert(w.size() == 10000);
    for(int i=0; i<w.size(); i++){
        w[i] = i;
        assert(w[i] == i);
    }
    epl::vector<int32_t> x(10);
    auto B = x.begin();
    auto E = x.end();
    assert(B!=E);
    assert(E-B == 10);
    int32_t k = 0;
    for (auto& v : x) {
        v = k++;
    }
    const epl::vector<int32_t>& y = x;
    int32_t s = 0;
    for (const auto& v : y) {
        s += v;
    }
    assert(s==45);
    epl::vector<int32_t> X(1);
    auto itr = X.begin();
    X.pop_back();
    try{
        int a;
        a = *itr;
        assert(false);
    } catch (epl::invalid_iterator ii){
        assert(ii.level == epl::invalid_iterator::MILD);
    }
    v = w;
    assert(v.size() == w.size());
    assert(w.begin() != v.begin());
    assert(w.end() != v.end());
    assert(!(w.begin() < v.end()));
    for(int i=0; i<v.size(); i++)
        assert(v[i] == w[i]);
    const epl::vector<int> p(v);
    assert(p.size() == v.size());
    for(int i=0; i<p.size(); i++)
        assert(p[i] == v[i]);
    w = epl::vector<int>(p);
    assert(w.size() == p.size());
    for(int i=0; i<w.size(); i++)
        assert(w[i] == p[i]);
    epl::vector<int> q(std::move(w));
    assert(q.size() == p.size());
    for(int i=0; i<q.size(); i++)
        assert(q[i] == p[i]);
    q = q;
    for(int i=0; i<q.size(); i++)
        assert(q[i] == p[i]);
    bool same = std::is_same<epl::vector<int>::iterator::iterator_category, std::random_access_iterator_tag>::value;
    assert(same);
    same = std::is_same<epl::vector<int>::iterator::value_type, int>::value;
    assert(same);
    same = std::is_same<epl::vector<int>::const_iterator::iterator_category, std::random_access_iterator_tag>::value;
    assert(same);
    same = std::is_same<epl::vector<int>::const_iterator::value_type, const int>::value;
    assert(same);
    epl::vector<epl::vector<int>> vec;
    vec.push_front(epl::vector<int>());
    for(int i=0; i<100; i++)
        vec.push_back(vec[0]);
    epl::vector<Foo> f;
    int arg = 0;
    f.emplace_back(arg);
    assert(f.size() == 1);
    assert(f[0].constructor == 0);
    assert(f.begin()->constructor == 0);
    assert(((const epl::vector<Foo>&)f).begin()->constructor == 0);
    f.emplace_back(arg, arg);
    assert(f.size() == 2);
    assert(f[1].constructor == 1);
    assert((f.begin()+1)->constructor == 1);
    assert((((const epl::vector<Foo>&)f).begin()+1)->constructor == 1);
    f.emplace_back(0, arg);
    assert(f.size() == 3);
    assert(f[2].constructor == 2);
    assert((f.begin()+2)->constructor == 2);
    assert((((const epl::vector<Foo>&)f).begin()+2)->constructor == 2);
    f.emplace_back(arg, 0, 0);
    assert(f.size() == 4);
    assert(f[3].constructor == 3);
    assert((f.begin()+3)->constructor == 3);
    assert((((const epl::vector<Foo>&)f).begin()+3)->constructor == 3);
    f.emplace_back(arg, arg, arg);
    assert(f.size() == 5);
    assert(f[4].constructor == 4);
    assert((f.begin()+4)->constructor == 4);
    assert((((const epl::vector<Foo>&)f).begin()+4)->constructor == 4);
    f.emplace_back(arg, arg, arg, arg);
    assert(f.size() == 6);
    assert(f[5].constructor == 5);
    assert((f.begin()+5)->constructor == 5);
    assert((((const epl::vector<Foo>&)f).begin()+5)->constructor == 5);
    f.emplace_back(0, 0, arg, arg);
    assert(f.size() == 7);
    assert(f[6].constructor == 6);
    assert((f.begin()+6)->constructor == 6);
    assert((((const epl::vector<Foo>&)f).begin()+6)->constructor == 6);
    f.emplace_back(f[0]);
    assert(f.size() == 8);
    assert(f[7].constructor == 7);
    assert((f.begin()+7)->constructor == 7);
    assert((((const epl::vector<Foo>&)f).begin()+7)->constructor == 7);
    f.emplace_back(f[0]);
    assert(f.size() == 9);
    assert(f[8].constructor == 7);
    assert((f.begin()+8)->constructor == 7);
    assert((((const epl::vector<Foo>&)f).begin()+8)->constructor == 7);
    f.emplace_back(f[0]);
    assert(f.size() == 10);
    assert(f[9].constructor == 7);
    assert((f.begin()+9)->constructor == 7);
    assert((((const epl::vector<Foo>&)f).begin()+9)->constructor == 7);
    std::cout<<"pass!"<<std::endl;
    
    return 0;
}