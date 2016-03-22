//
//  jp46466.h
//  Proj3
//
//  Created by Jun Pan on 4/28/14.
//  Copyright (c) 2014 Jun Pan. All rights reserved.
//

#ifndef __Proj3__jp46466__
#define __Proj3__jp46466__

#include "LifeForm.h"
#include "Init.h"
#include <string>

class jp46466 : public LifeForm {
protected:
    int course_changed {1};
    static void initialize(void);
    void spawn(void);
    void hunt(void);
    void hunt2(void);
    void live(void);
    Event* hunt_event;
    double init_speed; 
    int lost{0};
    int persist{0};
    int initpersist {2};
    double initperrange{max_perceive_range};
    double perrange{0};
    std::string myname{"jp46466"};
    bool firstlostflag{true};
    double courserec{M_PI/2};
    double lostchance{10};
    bool coin{true};
    bool coin2{true};
    int huntevent{0};
public:
    jp46466(void);
    ~jp46466(void);
    Color my_color(void) const;   // defines LifeForm::my_color
    static LifeForm* create(void);
    std::string species_name(void) const;
    std::string player_name(void) const; 
    Action encounter(const ObjInfo&);
    friend class Initializer<jp46466>;
};

#endif /* defined(__Proj3__jp46466__) */
