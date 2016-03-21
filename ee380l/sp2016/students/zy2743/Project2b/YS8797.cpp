//
//  YS8797.cpp
//  LifeForm
//
//  Created by mackbook on 3/12/16.
//  Copyright © 2016 utaustin. All rights reserved.
//

#include "YS8797.h"

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <string>
#include <unordered_map>
#include "CraigUtils.h"
#include "Event.h"
#include "ObjInfo.h"
#include "Params.h"
#include "Random.h"
#include "Window.h"

#ifdef _MSC_VER
using namespace epl;
#endif
using namespace std;
using String = std::string;

Initializer<YS8797> __YS_initializer;

String YS8797::species_name(void) const
{
    return "YS8797";
}

Action YS8797::encounter(const ObjInfo& info)
{
    if (info.species == species_name()) {
        /* don't be cannibalistic */
//        set_course(info.bearing + M_PI);
        return LIFEFORM_IGNORE;
    }
    else {
        hunt_event->cancel();
        SmartPointer<YS8797> self = SmartPointer<YS8797>(this);
        hunt_event = new Event(0.0, [self](void) { self->hunt(); });
        return LIFEFORM_EAT;
    }
}

void YS8797::initialize(void) {
    LifeForm::add_creator(YS8797::create, "YS8797");
}

/*
 * REMEMBER: do not call set_course, set_speed, perceive, or reproduce
 * from inside the constructor!!!!
 * you must wait until the object is actually alive
 */
YS8797::YS8797() {
    SmartPointer<YS8797> self = SmartPointer<YS8797>(this);
    new Event(0, [self](void) { self->startup(); });
}

YS8797::~YS8797() {}

void YS8797::startup(void) {
    set_course(drand48() * 2.0 * M_PI);
    set_speed(2 + 5.0 * drand48());
    SmartPointer<YS8797> self = SmartPointer<YS8797>(this);
    hunt_event = new Event(0, [self](void) { self->hunt(); });
}

void YS8797::spawn(void) {

        SmartPointer<YS8797> child = new YS8797;
        reproduce(child);
    
}


Color YS8797::my_color(void) const {
    return CYAN;
}

SmartPointer<LifeForm> YS8797::create(void) {
    return new YS8797;
}


void YS8797::hunt(void) {
    const String fav_food = "Algae";
    
    hunt_event = nullptr;
    if (health() == 0.0) { return; } // we died
    
    ObjList prey = perceive(30.0);
    
    
    double best_d = HUGE;
    bool meetAlgae = false;
    unordered_map<double, double> myMap;
    
    for (ObjList::iterator i = prey.begin(); i != prey.end(); ++i) {
        if ((*i).species == fav_food) {
            meetAlgae = true;
            if (myMap.find((*i).bearing) == myMap.end()) {
                myMap[(*i).bearing] = 1.0;
            } else {
                myMap[(*i).bearing] += 1.0;
            }
            
            for (const auto& item : myMap) {
                myMap[(*i).bearing] += 1.0 - log(abs(item.first - (*i).bearing) + 1);
            }
            
        }
    }
    if (meetAlgae) {
        set_speed(2 + 5.0 * drand48());
        double best_course = 0;
        double maxScore = 0.0;
        for (const auto& item : myMap) {
            if(item.second > maxScore)
                best_course = item.first;
        }
        set_course(best_course);
    }
    else
        set_course(get_course() + M_PI / 2);
    
 
    SmartPointer<YS8797> self = SmartPointer<YS8797>(this);
    hunt_event = new Event(10.0, [self](void) { self->hunt(); });
    
    if (health() >= 4.0) spawn();
}

