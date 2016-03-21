#include <iostream>
#include <cstdint>

/* REQUIRED OUTPUT (blank lines added for clairty) :
 Test Move Construct
 acquisition
 move construct
 valid use
 copy construct
 copy
 valid use
 destructor
 destructor
 END
 
 Test Use After Move
 acquisition
 move construct
 valid use
 INVALID use attempt
 destructor
 INVALID use attempt
 END
 
 Test Move Assign
 acquisition
 move assign
 valid use
 destructor
 END
 
 Test Move Sloppy
 acquisition
 move construct
 INVALID use attempt
 move construct
 copy
 copy construct
 copy
 valid use
 valid use
 valid use
 destructor
 destructor
 destructor
 p holds no resource, no destructor
 END
 
 */

using std::cout;
using std::endl;

class Resource {
    
    Resource(void) {}
    int x = 0;
    static constexpr int magic_number = 0x123456;
public:
    static Resource* acquire(void) {
        cout << "acquisition\n";
        Resource* res = new Resource;
        res->x = magic_number;
        return res;
    }
    Resource(Resource const & r) { x = r.x; cout << "copy\n"; }
    ~Resource(void) { x = 0; cout << "destructor\n"; }
    Resource& operator=(Resource const&) = delete;
    void use(void) {
        if (valid()) { cout << "valid use\n"; } else { cout << "INVALID use\n"; }
    }
    
    bool valid(void) const {
        return x == magic_number;
    }
};


/* You must implement this class. You are permitted to make no other changes to this file */
class SafeMove {
    Resource* ptr;
    /* you may add additional data members (i.e., fields) to this class, provided they are private */
    /* you may write additional member functions, provide they are private */
    bool isAlive = true;
    
    bool isValid(void) const { // you must rewrite this function, but cannot change the parameters or return value
        return isAlive;
    }
    
    void destroy(){
        if(isAlive) delete ptr;
        isAlive = false;
    }
    
    void copy(SafeMove const& rhs){
        ptr = new Resource(*(rhs.ptr));
        isAlive = true;
    }
    
    void move(SafeMove&& rhs){
        ptr = rhs.ptr;
        rhs.isAlive = false;
        isAlive = true;
    }
    
public:
    /* you cannot add any public methods. You cannot add any constructors */
    /* you cannot add any print statements. you cannot modify any of the existing print statements */
    
    SafeMove(void) { ptr = Resource::acquire(); } // you must not change this constructor
    
    void use(void) { // you must not change this function
        if (isValid()) { ptr->use(); } else { cout << "INVALID use attempt\n"; }
    }
    
    SafeMove(SafeMove const& rhs) { // you must write this function, you cannot remove the print statement
        cout << "copy construct\n";
        copy(rhs);
    }
    
    SafeMove(SafeMove&& rhs) { // you must write this function, you cannot remove the print statement
        cout << "move construct\n";
        if(rhs.isValid())
            move(std::move(rhs));
        else
            copy(rhs);
    }
    
    
    ~SafeMove(void) { // you must write this function
        destroy();
    }
    
    SafeMove& operator=(SafeMove&& rhs) { // you must write this function, you cannot remove the print statement
        cout << "move assign\n";
        if(rhs.isValid())
            move(std::move(rhs));
        else
            copy(rhs);
        return * this;
    }
    
    SafeMove& operator=(SafeMove const& rhs) { // you must write this function, you cannot remove the print statement
        cout << "copy assign\n";
        if(this!= & rhs){
            destroy();
            copy(rhs);
        }
        return *this;
    }
};

bool glob = true;

SafeMove make(void) {
    /* this ? statement supresses compiler optimizations that interfere with the test program */
    return glob ? SafeMove{} : SafeMove{};
}

/* this is a normal program with no errors
 * the object should be constructed inside the make() function and then move constructed to create p
 * there should be exactly two resources created
 * p.use() should be a valid use, and the resource should be deallocated when this function returns */
void testMoveConstruct(void) {
    cout << "Test Move Construct\n";
    {
        SafeMove p = make(); // resource created inside make, then moved to p
        p.use();
        SafeMove q{ p }; // deep copy, new resource created
        q.use();
    } // two destructors
    cout << "\tEND\n";
}

/* this is a program with a typical error, an object is used after it has been moved
 * a single resource should be created. The resource should be moved to q
 * (i.e., q is a shallow copy of p). q.use() should be a valid use
 * p.use() should be an invalid use and must be detected as such
 * the resource should be deallocated when q is destructed
 */
void testUseAfterMove(void) {
    cout << "Test Use After Move\n";
    {
        SafeMove p;
        {
            SafeMove q{ std::move(p) };
            q.use(); // valid
            p.use(); // invalid
        }
        p.use(); // (still) invalid
    }
    cout << "\tEND\n";
}

/* this is a program with a minor error. An object is moved on top of itself
 * a single resource should be constructed.
 * p.use() should be a valid use, and the resource should be deallocated when this function returns */
void testMoveAssign(void) {
    cout << "Test Move Assign\n";
    {
        SafeMove p;
        
        p = std::move(p);
        
        p.use();
    }
    cout << "\tEND\n";
}

void testMoveSloppy(void) {
    cout << "Test Move Sloppy\n";
    {
        SafeMove p;
        {
            SafeMove q{ std::move(p) };
            p.use(); // p is invalid
            SafeMove r{ std::move(p) }; // r is created by deep copy
            SafeMove s{ p }; // s is created by deep copy
            q.use(); // valid
            r.use(); // valid
            s.use(); // valid
        } // three destructors
        cout << "p holds no resource, no destructor\n";
    }
    cout << "\tEND\n";
}

int main(void) {
    testMoveConstruct();
    testUseAfterMove();
    testMoveAssign();
    testMoveSloppy();
}