#include <iostream>
#include <cstdint>
#include <vector> // hint, helpful for the factory method
#include <random>

class Red; class Blue; class Yellow; class Brown; // forward references, in case you use them inside Pigment

/* you may add abstract (i.e., pure virtual) methods to this class
 * but you may not make any other changes to the class
 *
 * NOTE: My concept of a perfect solution to the problem has the following characteristics
 * the primary pigments are Red, Blue and Yellow. These pigments are special, and every class
 * knows about them. Specifically, your factory method and/or your Pigment base class can make
 * explicit references to Red, Blue or Yellow. You can write functions that only work with
 * these specific primary colors (e.g., a function "blendWithRed(Red*)" would be perfectly fine
 * if that's what you felt you needed).
 *
 * Brown is also special, since it's the basic color (it's what you get when you blend randomly)
 *
 * However, other than the primary pigments and Brown, there should be no references to specific pigments
 * i.e., make your class as scalable as possible, where it could be possible to add new Pigment classes.
 * We won't ask you to design special cases like
 * "Red blended with Orange creates ReddishOrange" where ReddishOrange is a new color, but
 * a good design will make it relatively easy to invent new colors (like ReddishOrange), and I
 * may include this aspect of your design when grading your solution
 * DO NOT MAKE ANY PIGMENTS/COLORS OTHER THAN THE ONES SPECIFIED BELOW
 */
class Pigment {
public:
    virtual const char* name(void) const = 0; // return the name of this pigment (e.g., "Red", "Blue", etc).
    virtual Pigment* blend(Pigment* that) = 0;
    /* you can add additional pure-virtual methods, but nothing else */
    virtual Pigment* blendWithRed(Red* that) = 0;
    virtual Pigment* blendWithBlue(Blue* that) = 0;
    virtual Pigment* blendWithYellow(Yellow* that) = 0;
    virtual ~Pigment(void) = default;
};


/* this function returns a random number between 0 and max-1
 * you can and should use this function in your factory method getPigment */
uint32_t rand(uint32_t max) {
    static std::default_random_engine engine;
    return std::uniform_int_distribution<uint32_t> {0, max - 1}(engine);
}

using CreateFun = Pigment*(*)(void);

std::vector<CreateFun>& getVector(void){
    static std::vector<CreateFun> vec;
    return vec;
}

/* return a random Pigment
 * your code must not have any if statements.
 * to receive full credit, it must be possible for me to invent a new Pigment type as follows
 * I would create a subclass of Pigment
 * I would follow the pattern you used for your Pigments
 * I would not have to modify any of the existing code to add my new Pigment
 * If I followed those steps, then my new Pigment should be included in the possible random
 * returned values from this function
 *
 * in other words, I want a scalable, object-oriented factory method
 */
Pigment* getPigment(void) { // you must write this function
    std::vector<CreateFun>& vec = getVector();
    return vec[rand((uint32_t)vec.size())]();
}

/* Rules for Pigments
 * Implement the following concrete classes. Each class must be a subclass (i.e., inherit from)
 * the Pigment abstract base class.
 * Red, Blue, Yellow, Purple, Green, Orange, Brown
 * Each concrete class must implement the blend method
 * Note that the blend method has two arguments: the formal argument "that" and the implicit argument "this"
 * The Pigment returned by blend will usually depend on both of these arguments as follows:
 * Red and Blue will blend to create Purple
 * Blue and Yellow will blend to create Green
 * Red and Yellow will blend to create Orange
 * any other combination of colors will blend to create Brown
 * e.g., Orange and Green blend to make Brown
 * Brown and Red blend to make Brown, etc.
 *
 * The blend method is symmetric, i.e,. Blue blended with Yellow creates Green
 * and Yellow blended with Blue also creates Green
 *
 * NOTE: under these rules, if you blend a pigment with the same pigment, you will get Brown, i.e.,
 * Red blended with Red creates Brown
 * While that's not a good representation of Pigments, the question is sufficiently complicated already
 * that I want you to implement this simplified behavior --> a color blended with itself will create Brown.
 *
 * You are not permitted any if statements in your solution. You are encouraged to use virtual functions
 * The Pigment classes (and all of the subtypes of Pigments) cannot have any data members (i.e., no state)
 * You are forbidden from using typeinfo, typeid or dynamic_cast in your solution
 * I do not care about memory management, you can leak memory if you wish
 */

class InitBrown{
public:
    InitBrown(void);
};

class Brown: public Pigment{
public:
    
    static InitBrown init;
    
    static Pigment* build(void){
        return new Brown;
    }
    
    virtual const char* name(void) const override{
        return "Brown";
    }
    
    virtual Pigment* blend(Pigment* that) override{
        return new Brown;
    }
    
    virtual Pigment* blendWithRed(Red* that) override{
        return new Brown;
    }
    
    virtual Pigment* blendWithBlue(Blue* that) override{
        return new Brown;
    }
    
    virtual Pigment* blendWithYellow(Yellow* that) override{
        return new Brown;
    }
    
};

InitBrown::InitBrown(void){
    getVector().push_back(&Brown::build);
}

InitBrown Brown::init;

class InitPurple{
public:
    InitPurple(void);
};

class Purple: public Pigment{
public:
    
    static InitPurple init;
    static Pigment* build(void){
        return new Purple;
    }
    
    virtual const char* name(void) const override{
        return "Purple";
    }
    
    virtual Pigment* blend(Pigment* that) override{
        return new Brown;
    }
    
    virtual Pigment* blendWithRed(Red* that) override{
        return new Brown;
    }
    
    virtual Pigment* blendWithBlue(Blue* that) override{
        return new Brown;
    }
    
    virtual Pigment* blendWithYellow(Yellow* that) override{
        return new Brown;
    }
    
};

InitPurple::InitPurple(void){
    getVector().push_back(&Purple::build);
}

InitPurple Purple::init;

class InitGreen{
public:
    InitGreen(void);
};

class Green: public Pigment{
public:
    
    static InitGreen init;
    static Pigment* build(void){
        return new Green;
    }
    
    virtual const char* name(void) const override{
        return "Green";
    }
    
    virtual Pigment* blend(Pigment* that) override{
        return new Brown;
    }
    
    virtual Pigment* blendWithRed(Red* that) override{
        return new Brown;
    }
    
    virtual Pigment* blendWithBlue(Blue* that) override{
        return new Brown;
    }
    
    virtual Pigment* blendWithYellow(Yellow* that) override{
        return new Brown;
    }
    
};

InitGreen::InitGreen(void){
    getVector().push_back(&Green::build);
}

InitGreen Green::init;

class InitOrange{
public:
    InitOrange(void);
};

class Orange: public Pigment{
public:
    
    static InitOrange init;
    static Pigment* build(void){
        return new Orange;
    }
    
    virtual const char* name(void) const override{
        return "Orange";
    }
    
    virtual Pigment* blend(Pigment* that) override{
        return new Brown;
    }
    
    virtual Pigment* blendWithRed(Red* that) override{
        return new Brown;
    }
    
    virtual Pigment* blendWithBlue(Blue* that) override{
        return new Brown;
    }
    
    virtual Pigment* blendWithYellow(Yellow* that) override{
        return new Brown;
    }
    
};

InitOrange::InitOrange(void){
    getVector().push_back(&Orange::build);
}

InitOrange Orange::init;

class InitRed{
public:
    InitRed(void);
};

class Red: public Pigment{
public:
    
    static InitRed init;
    static Pigment* build(void){
        return new Red;
    }
    
    virtual const char* name(void) const override{
        return "Red";
    }
    
    virtual Pigment* blend(Pigment* that) override{
        return that->blendWithRed(this);
    }
    
    virtual Pigment* blendWithRed(Red* that) override{
        return new Brown;
    }
    
    virtual Pigment* blendWithBlue(Blue* that) override{
        return new Purple;
    }
    
    virtual Pigment* blendWithYellow(Yellow* that) override{
        return new Orange;
    }
    
};

InitRed::InitRed(void){
    getVector().push_back(&Red::build);
}

InitRed Red::init;


class InitBlue{
public:
    InitBlue(void);
};

class Blue: public Pigment{
public:
    
    static InitBlue init;
    static Pigment* build(void){
        return new Blue;
    }
    
    virtual const char* name(void) const override{
        return "Blue";
    }
    
    virtual Pigment* blend(Pigment* that) override{
        return that->blendWithBlue(this);
    }
    
    virtual Pigment* blendWithRed(Red* that) override{
        return new Purple;
    }
    
    virtual Pigment* blendWithBlue(Blue* that) override{
        return new Brown;
    }
    
    virtual Pigment* blendWithYellow(Yellow* that) override{
        return new Green;
    }
    
};

InitBlue::InitBlue(void){
    getVector().push_back(&Blue::build);
}

InitBlue Blue::init;

class InitYellow{
public:
    InitYellow(void);
};

class Yellow: public Pigment{
public:
    
    static InitYellow init;
    static Pigment* build(void){
        return new Yellow;
    }
    
    virtual const char* name(void) const override{
        return "Yellow";
    }
    
    virtual Pigment* blend(Pigment* that) override{
        return that->blendWithYellow(this);
    }
    
    virtual Pigment* blendWithRed(Red* that) override{
        return new Orange;
    }
    
    virtual Pigment* blendWithBlue(Blue* that) override{
        return new Green;
    }
    
    virtual Pigment* blendWithYellow(Yellow* that) override{
        return new Brown;
    }
    
};

InitYellow::InitYellow(void){
    getVector().push_back(&Yellow::build);
}

InitYellow Yellow::init;


int main(void) {
    /* check primary blends */
    {
        Pigment* p1 = new Blue;
        Pigment* p2 = new Yellow;
        std::cout << p1->name() << " mixed with " << p2->name() << " makes " << p1->blend(p2)->name() << std::endl;
        std::cout << p2->name() << " mixed with " << p1->name() << " makes " << p2->blend(p1)->name() << std::endl;
    }
    
    /* check primary blends */
    {
        Pigment* p1 = new Blue;
        Pigment* p2 = new Red;
        std::cout << p1->name() << " mixed with " << p2->name() << " makes " << p1->blend(p2)->name() << std::endl;
        std::cout << p2->name() << " mixed with " << p1->name() << " makes " << p2->blend(p1)->name() << std::endl;
    }
    
    /* check primary blends */
    {
        Pigment* p1 = new Red;
        Pigment* p2 = new Yellow;
        std::cout << p1->name() << " mixed with " << p2->name() << " makes " << p1->blend(p2)->name() << std::endl;
        std::cout << p2->name() << " mixed with " << p1->name() << " makes " << p2->blend(p1)->name() << std::endl;
    }
    
    /* check factory and random blends (almost all of which are brown */
    for (int k = 0; k < 100; k += 1) {
        Pigment* p1 = getPigment();
        Pigment* p2 = getPigment();
        std::cout << p1->name() << " mixed with " << p2->name() << " makes " << p1->blend(p2)->name() << std::endl;
    }
}
