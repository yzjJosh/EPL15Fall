#include <cmath>
#include <cstdlib>
#include <iostream>
#include <string>

#include "SuperLiviaYue.h"
#include "Event.h"
#include "ObjInfo.h"
#include "Params.h"
#include "Random.h"
#include "Window.h"

using namespace std;
using String = std::string;

Initializer<SuperLiviaYue> __SuperLiviaYue_initializer;

String SuperLiviaYue::species_name(void) const
{
    return "SuperLiviaYue";
  
}

String SuperLiviaYue::player_name(void) const
{
    return "SuperLiviaYue";
}


Action SuperLiviaYue::encounter(const ObjInfo& info)
{
    if(health()==0.0) return LIFEFORM_IGNORE;
    
    if (info.species == species_name()) {
        /* don't be cannibalistic */
        set_course(info.bearing + M_PI_2);
        hunt_event->cancel();
        SmartPointer<SuperLiviaYue> self = SmartPointer<SuperLiviaYue>(this);
        hunt_event = new Event(4.0,[self](void) {self->hunt(); });
        return LIFEFORM_IGNORE;
    }
    else {

            hunt_event->cancel();
            SmartPointer<SuperLiviaYue> self = SmartPointer<SuperLiviaYue>(this);
            hunt_event = new Event(0.0, [self](void) { self->hunt(); });
            return LIFEFORM_EAT;

    }
    
}


void SuperLiviaYue::initialize(void) {
    LifeForm::add_creator(SuperLiviaYue::create, "SuperLiviaYue");
}

/*
 * REMEMBER: do not call set_course, set_speed, perceive, or reproduce
 * from inside the constructor!!!!
 * you must wait until the object is actually alive
 */

SuperLiviaYue::SuperLiviaYue() {
  
    SmartPointer<SuperLiviaYue> self = SmartPointer<SuperLiviaYue>(this);
    new Event(0, [self](void) { self->startup(); });
}

SuperLiviaYue::~SuperLiviaYue() {}

void SuperLiviaYue::startup(void) {
    set_course(drand48() * 2.0 * M_PI);
    set_speed(2 + 5.0 * drand48());
    SmartPointer<SuperLiviaYue> self = SmartPointer<SuperLiviaYue>(this);
    hunt_event = new Event(0, [self](void) { self->hunt(); });
}

void SuperLiviaYue::spawn(void) {
    SmartPointer<SuperLiviaYue> child = new SuperLiviaYue;
    reproduce(child);
}


Color SuperLiviaYue::my_color(void) const {
    return MAGENTA;
}


SmartPointer<LifeForm> SuperLiviaYue::create(void) {
    return new SuperLiviaYue;
}


void SuperLiviaYue::hunt(void) {
    const String fav_food = "Algae";
    hunt_event = nullptr;
    if (health() == 0.0) { return; } // we died
    
    ObjList prey = perceive(40.0);
    double weight[8];
    for(auto& i : weight){
        i = 0.0;
    }
    
    int count = 0;
    for (ObjList::iterator i = prey.begin(); i != prey.end(); ++i) {
        if ((*i).species == fav_food || ((*i).species!=species_name() && (*i).their_speed == 0.0 && (*i).health < this->health())) {
            for(int k = 0; k < 8; k++){
                weight[k] += cos((*i).bearing-M_PI_4*k)/((*i).distance);
            }
            count++;
        }
    }
    
  
        double planned_course = get_course()+M_PI/2;
        double max = 0.0;
        
        for(int j=0; j<8; j++){
            if(max < weight[j]){
                max = weight[j];
                planned_course = M_PI_4*j;
            }
        }
        
        double best_course = planned_course;
        double optimal_weight = 0.0;
        
        for(ObjList::iterator i = prey.begin(); i != prey.end(); ++i) {
            if ((*i).species == fav_food || ((*i).their_speed == 0.0 && (*i).health< this->health() && (*i).species!= species_name())) {
                double temp_weight = cos((*i).bearing-planned_course)/((*i).distance);
                if(temp_weight > optimal_weight){
                    optimal_weight = temp_weight;
                    
                    best_course = (*i).bearing;
                }
            }
        }
        
        set_course(best_course);
    

    SmartPointer<SuperLiviaYue> self = SmartPointer<SuperLiviaYue>(this);
    hunt_event = new Event(10.0, [self](void) { self->hunt(); });
    if (health() >= 4.0) spawn();
}
