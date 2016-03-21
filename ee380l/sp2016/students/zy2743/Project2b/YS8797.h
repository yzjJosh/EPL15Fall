//
//  YS8797.h
//  LifeForm
//
//  Created by mackbook on 3/12/16.
//  Copyright © 2016 utaustin. All rights reserved.
//

#ifndef YS8797_h
#define YS8797_h
#include <memory>

#include "LifeForm.h"
#include "Init.h"

class YS8797 : public LifeForm {
protected:
    static void initialize(void);
    void spawn(void);
    void hunt(void);
    void startup(void);
    Event* hunt_event;
public:
    YS8797(void);
    ~YS8797(void);
    Color my_color(void) const;   // defines LifeForm::my_color
    static SmartPointer<LifeForm> create(void);
    virtual std::string species_name(void) const;
    virtual Action encounter(const ObjInfo&);
    friend class Initializer<YS8797>;
};

#endif /* YS8797_h */
