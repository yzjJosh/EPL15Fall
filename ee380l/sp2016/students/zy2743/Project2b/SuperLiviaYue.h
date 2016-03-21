#include <memory>
#include "LifeForm.h"
#include <string>
#include "Init.h"

class SuperLiviaYue : public LifeForm {
protected:
    static void initialize(void);
    void spawn(void);
    void hunt(void);
    void startup(void);
    Event* hunt_event;
    
public:
    SuperLiviaYue(void);
    ~SuperLiviaYue(void);
    Color my_color(void) const;   // defines LifeForm::my_color
    static SmartPointer<LifeForm> create(void);
    virtual std::string species_name(void) const;
    virtual std::string player_name(void) const;
    virtual Action encounter(const ObjInfo&);
    friend class Initializer<SuperLiviaYue>;
};