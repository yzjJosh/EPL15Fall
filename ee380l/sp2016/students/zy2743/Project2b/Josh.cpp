#include <cmath>
#include <cstdlib>
#include <iostream>
#include <string>

#include "Josh.h"
#include "Event.h"
#include "ObjInfo.h"
#include "Params.h"
#include "Random.h"
#include "Window.h"

using namespace std;
using String = std::string;

double const Josh::algea_speed_lo = 1.01;
double const Josh::algea_speed_hi = 7.01;
double const Josh::speed_delta = 0.00001;
String const Josh::default_name = "Josh";
Initializer<Josh> __Josh_initializer;

String Josh::player_name(void) const{
    return default_name;
}

String Josh::species_name(void) const{
    return name;
}

Action Josh::encounter(const ObjInfo& info){
    if(health() == 0.0) return LIFEFORM_IGNORE;
    if(hunt_event != nullptr) hunt_event->cancel();
    SmartPointer<Josh> self = SmartPointer<Josh>{this};
    if (isFriend(info)) {
        set_course(get_course() + M_PI_2);
        hunt_event = new Event(2.0, [self](void) { self->hunt(); });
        return LIFEFORM_IGNORE;
    }
    else {
        hunt_event = new Event(0.0, [self](void) { self->hunt(); });
        return LIFEFORM_EAT;
    }
}

void Josh::initialize(void) {
    LifeForm::add_creator(Josh::create, default_name);
}

/*
 * REMEMBER: do not call set_course, set_speed, perceive, or reproduce
 * from inside the constructor!!!!
 * you must wait until the object is actually alive
 */
Josh::Josh() {
    name = default_name;
    no_target_count = 0;
    cheatAlgae = drand48() < 0.25? true: false;
    SmartPointer<Josh> self = SmartPointer<Josh>(this);
    new Event(0, [self](void) { self->startup(); });
}

Josh::~Josh() {}

void Josh::startup(void) {
    SmartPointer<Josh> self = SmartPointer<Josh>(this);
    hunt_event = new Event(0, [self](void) { self->hunt(); });
}

void Josh::spawn(void) {
    SmartPointer<Josh> child = new Josh;
    reproduce(child);
}


Color Josh::my_color(void) const {
   // if(health() >= 10.0 && name != default_name) return YELLOW;
    return ORANGE;
}

SmartPointer<LifeForm> Josh::create(void) {
    return new Josh;
}

bool Josh::isAlgae(const ObjInfo & info){
    return info.species == "Algae" && info.their_speed == 0.0 && info.health < 3.0;
}

bool Josh::isFriend(const ObjInfo & info){
    return info.species == default_name || (info.species == "Algae" && (abs(info.their_speed-algea_speed_lo)<speed_delta || abs(info.their_speed-algea_speed_hi)<speed_delta));
}

double Josh::eatChance(double h1, double h2){
    double c1 = eat_success_chance(h1, h2);
    double c2 = eat_success_chance(h2, h1);
    return c1*(1-c2);
}

void Josh::hunt(void) {
    hunt_event = nullptr;
    if (health() == 0.0) return; // we died
    
    double radius = fmin(fmax(40.0*health(), 30), 100);
    ObjList objs = perceive(radius);
    
    if(health() == 0.0) return;
    
    double next_hunt_time = 10.0;
    ObjList preys;
    ObjList hunters;
    int count_avoid_when_reproduce = 0;
    double weight[32];
    for(int i=0; i<32; i++) weight[i] = 0.0;
    for (auto const& obj: objs){
        double chance = eatChance(health(), obj.health);
        double die_chance = eatChance(obj.health, health());
        if (isAlgae(obj) || (!isFriend(obj) && chance > 0.7)) {
            for(int j=0; j<32; j++)
                weight[j] += cos((obj).bearing - M_PI_4/4*j)/obj.distance * (isAlgae(obj)? 1.0: chance);
            preys.push_back(obj);
        }else if(isFriend(obj) || (!isAlgae(obj) && die_chance > 0.7)){
            for(int j=0; j<32; j++)
                weight[j] -= cos(obj.bearing - M_PI_4/4*j)/obj.distance * (isFriend(obj)? 1.0: die_chance/0.5);
            if(obj.distance <= reproduce_dist) count_avoid_when_reproduce ++;
            if(!isFriend(obj)) hunters.push_back(obj);
        }
    }
    if(preys.size() > 0){
        double best_course = get_course();
        double max_weight = -MAXFLOAT;
        for(int i=0; i<32; i++){
            if(weight[i] > max_weight){
                best_course = M_PI_4/4*i;
                max_weight = weight[i];
            }
        }
        double new_course = best_course;
        double new_course_weight = -MAXFLOAT;
        for (auto const& prey: preys) {
            double cur_weight = cos(prey.bearing - best_course)/prey.distance * (isAlgae(prey)? 1.0: eatChance(health(), prey.health));
            if(cur_weight > new_course_weight){
                new_course_weight = cur_weight;
                new_course = prey.bearing;
            }
        }
        
        double max_distance = 0.0;
        for (auto const& prey: preys)
            if(abs(prey.bearing - new_course) <= M_PI_2/36 && prey.distance > max_distance)
                max_distance = prey.distance;

        set_course(new_course);
        if(name == default_name)
            set_speed(7.0);
        else
            set_speed(algea_speed_hi);
        
        next_hunt_time = fmin(max_distance/get_speed()*1.1, next_hunt_time);
        no_target_count = 0;
    }else{
        double new_course = get_course();
        if(++no_target_count == 3){
            new_course += M_PI_2;
            no_target_count = 0;
        }
        const double dodge_distance = 20.0;
        bool dodge = false;
        for (auto const& hunter: hunters) {
            if(hunter.distance < dodge_distance){
                new_course = hunter.bearing + M_PI_2;
                dodge = true;
                break;
            }
        }
        if(name == default_name)
            set_speed(dodge? 7.0: 1.0);
        else
            set_speed(dodge? algea_speed_hi: algea_speed_lo);
        set_course(new_course);
        
        next_hunt_time = dodge? dodge_distance/get_speed()*1.2: next_hunt_time;
    }
    SmartPointer<Josh> self = SmartPointer<Josh>(this);
    hunt_event = new Event(next_hunt_time, [self](void) { self->hunt(); });
    
    if (((health() >= 4.0 && !cheatAlgae) || (health() >= 100.0 && cheatAlgae)) && count_avoid_when_reproduce <= 2) spawn();
    if (health() >= 10.0 && cheatAlgae) name = "Algae";
    if (health() < 10.0 && cheatAlgae) name = default_name;
}
