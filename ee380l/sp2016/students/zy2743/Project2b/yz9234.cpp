#include <cmath>
#include <cstdlib>
#include <iostream>
#include <string>

#include "yz9234.h"
#include "Event.h"
#include "ObjInfo.h"
#include "Params.h"
#include "Random.h"
#include "Window.h"


using namespace std;
using String = std::string;

Initializer<yz9234> __yz9234_initializer;

String yz9234::species_name(void) const
{
    if(isFaking) return enermy_name;
    return "yz9234";
    
}

String yz9234::player_name(void) const
{
    return "yz9234";
    
}


Action yz9234::encounter(const ObjInfo& info)
{
    if(health()==0.0) return LIFEFORM_IGNORE;
    
    if (isFriend(info) || info.species == species_name()) {
        /* don't be cannibalistic */
        set_course(info.bearing + M_PI_2);
        hunt_event->cancel();
        SmartPointer<yz9234> self = SmartPointer<yz9234>(this);
        hunt_event = new Event(4.0,[self](void) {self->hunt(); });
        return LIFEFORM_IGNORE;
    }
    else {
        hunt_event->cancel();
        if(!isAlgae(info)){
            enermy_name = info.species;
            isFaking = true;
            SmartPointer<yz9234> self = SmartPointer<yz9234>(this);
            new Event(0.0, [self](void) { self->isFaking = false;});
        }
       
        SmartPointer<yz9234> self = SmartPointer<yz9234>(this);
        hunt_event = new Event(0.0, [self](void) { self->hunt(); });
        return LIFEFORM_EAT;
        
    }
    
}


void yz9234::initialize(void) {
    LifeForm::add_creator(yz9234::create, "yz9234");
}


/*
 * REMEMBER: do not call set_course, set_speed, perceive, or reproduce
 * from inside the constructor!!!!
 * you must wait until the object is actually alive
 */


yz9234::yz9234() {
    isFaking = false;
    enermy_name = "";
    SmartPointer<yz9234> self = SmartPointer<yz9234>(this);
    new Event(0, [self](void) { self->startup(); });
}

yz9234::~yz9234() {}

void yz9234::startup(void) {
    set_course(drand48() * 2.0 * M_PI);
    set_speed(2 + 5.0 * drand48());
    SmartPointer<yz9234> self = SmartPointer<yz9234>(this);
    hunt_event = new Event(0, [self](void) { self->hunt(); });
}

void yz9234::spawn(void) {
    SmartPointer<yz9234> child = new yz9234;
    reproduce(child);
}


Color yz9234::my_color(void) const {
    return MAGENTA;
}


SmartPointer<LifeForm> yz9234::create(void) {
    return new yz9234;
}

bool yz9234::isAlgae(const ObjInfo & info){
    return info.species == "Algae" && info.their_speed == 0.0 && info.health <3.0;

}

bool yz9234::isFriend(const ObjInfo & info){
    const double speed_lo = 2.123456;
    const double speed_hi = 7.123456;
    const double speed_delta = 1e-7;
    bool is_Josh = info.species.substr(0, 5) == "Josh:" || ((abs(info.their_speed-speed_lo)<speed_delta || abs(info.their_speed-speed_hi)<speed_delta));
    bool is_self = info.species == "yz9234";
    const double threshold = 10000.0;
    bool is_ally = false;
    if(Event::now() < threshold)
        is_ally |= info.species == "YS8797" || info.species == "mq2373" || is_Josh;
    return is_self || is_ally;
}



void yz9234::hunt(void) {
    
    isFaking = false;
    const String fav_food = "Algae";
    hunt_event = nullptr;
    if (health() == 0.0) { return; } // we died
    
    double range = 40.0*(this->health());
    if(range>80) range = 60;
    if(range<30) range = 30;
    ObjList prey = perceive(range);
   
    double weight[32];
    for(auto& i : weight){
        i = 0.0;
    }
    
    int count = 0;
    for (ObjList::iterator i = prey.begin(); i != prey.end(); ++i) {
        double chance_I_eat = eat_success_chance(health(), (*i).health);
        double chance_enemy_eat = eat_success_chance((*i).health, this->health());
        double probability = chance_I_eat * (1-chance_enemy_eat);
        
        if (isAlgae(*i)  || (!isFriend((*i)) && (*i).species!=species_name() && probability>0.6)) {
            for(int k = 0; k < 32; k++){
                weight[k] += cos((*i).bearing-M_PI_4/4*k)/((*i).distance);
            }
            count++;
        }
    }
    
    
    double planned_course = get_course()+M_PI/2;
    double max = 0.0;
    
    for(int j=0; j<32; j++){
        if(max < weight[j]){
            max = weight[j];
            planned_course = M_PI_4/4*j;
        }
    }
    
    double best_course = planned_course;
    double optimal_weight = 0.0;
    
    for(ObjList::iterator i = prey.begin(); i != prey.end(); ++i) {
        double chance_I_eat = eat_success_chance(health(), (*i).health);
        double chance_enemy_eat = eat_success_chance((*i).health, this->health());
        double probability = chance_I_eat * (1-chance_enemy_eat);

        if (isAlgae(*i) || (!isFriend((*i)) && (*i).species!=species_name() && probability>0.6)){
            double temp_weight = cos((*i).bearing-planned_course)/((*i).distance);
            if(temp_weight > optimal_weight){
                optimal_weight = temp_weight;
                
                best_course = (*i).bearing;
            }
        }
    }
    
    double max_distance = 0.0;
    
    for(ObjList::iterator i = prey.begin(); i != prey.end(); ++i) {
        double chance_I_eat = eat_success_chance(health(), (*i).health);
        double chance_enemy_eat = eat_success_chance((*i).health, this->health());
        double probability = chance_I_eat * (1-chance_enemy_eat);
        
        if (isAlgae(*i) || (!isFriend((*i)) && (*i).species!=species_name() && probability>0.6)){
            if(abs((*i).bearing - best_course) < M_2_PI/36 && (*i).distance > max_distance){
                max_distance = (*i).distance;
            }
          
        }
    }
    
    set_course(best_course);
    
    if(optimal_weight == 0.0){
        set_speed(2.0);
    }else{
        set_speed(5 + 5.0 * drand48());
    }
    
    SmartPointer<yz9234> self = SmartPointer<yz9234>(this);
    if(max_distance != 0.0){
        hunt_event = new Event(max_distance/get_speed()*1.1, [self](void) { self->hunt(); });
    }else {
        hunt_event = new Event(10.0, [self](void) { self->hunt(); });
    }
    
    
    if (health() >= 4.0) spawn();
    

    
    
}
