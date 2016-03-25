#include <memory>
#include "LifeForm.h"
#include <string>
#include "Init.h"

class yz9234 : public LifeForm {
protected:
    static void initialize(void);
    void spawn(void);
    void hunt(void);
    void startup(void);
    Event* hunt_event;
    bool isFriend(const ObjInfo & info);
    std::string enermy_name;
    bool isFaking;
    bool isAlgae(const ObjInfo & info);
public:
    yz9234(void);
    ~yz9234(void);
    Color my_color(void) const;   // defines LifeForm::my_color
    static SmartPointer<LifeForm> create(void);
    virtual std::string species_name(void) const;
    virtual std::string player_name(void) const;
    virtual Action encounter(const ObjInfo&);
    friend class Initializer<yz9234>;
};