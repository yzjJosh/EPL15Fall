#include <cmath>
#include <cstdlib>
#include <iostream>
#include <string>

#include "mq2373.h"
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

Initializer<mq2373> __mq2373_initializer;

String mq2373::species_name(void) const
{
    return "mq2373";
}

bool mq2373::isFriend(const ObjInfo & info){
    const double speed_lo = 2.123456;
    const double speed_hi = 7.123456;
    const double speed_delta = 1e-7;
    bool is_Josh = info.species.substr(0, 5) == "Josh:" || ((abs(info.their_speed-speed_lo)<speed_delta || abs(info.their_speed-speed_hi)<speed_delta));
    bool is_self = info.species == "mq2373";
    const double threshold = 10000.0;
    bool is_ally = false;
    if(Event::now() < threshold)
        is_ally |= info.species == "YS8797" || info.species == "yz9234" || is_Josh;
    return is_self || is_ally;
}

Action mq2373::encounter(const ObjInfo& info)
{
	const String fav_food = "Algae";
    if (info.species == species_name()) {
        /* don't be cannibalistic */
		set_speed(3);
		if(get_course() > M_PI)
			set_course(get_course() - 0.05 * M_PI);
		else
			set_course(get_course() + 0.05 * M_PI);
        return LIFEFORM_IGNORE;
    }
    else {
		if(info.species == fav_food){
			hunt_event->cancel();
			SmartPointer<mq2373> self = SmartPointer<mq2373>(this);
			hunt_event = new Event(0.0, [self](void) { self->hunt(); });
			return LIFEFORM_EAT;
		}else{
			if(health() > 1.5 && !(isFriend(info))){
				fakeName = true;
				fake_name = info.species;
				hunt_event->cancel();
				SmartPointer<mq2373> self = SmartPointer<mq2373>(this);
				hunt_event = new Event(0.0, [self](void) { self->hunt(); });
				return LIFEFORM_EAT;
			}else{
				if(!(isFriend(info))){
					fakeName = true;
					fake_name = info.species;
				}
				spawn();
				set_speed(7);
				if(get_course() > M_PI)
					set_course(get_course() - 0.1 * M_PI);
				else
					set_course(get_course() + 0.1 * M_PI);
				return LIFEFORM_IGNORE;
			}
		}
    }
}

void mq2373::initialize(void) {
    LifeForm::add_creator(mq2373::create, "mq2373");
}

/*
 * REMEMBER: do not call set_course, set_speed, perceive, or reproduce
 * from inside the constructor!!!!
 * you must wait until the object is actually alive
 */
mq2373::mq2373() {
    SmartPointer<mq2373> self = SmartPointer<mq2373>(this);
    new Event(0, [self](void) { self->startup(); });
}

mq2373::~mq2373() {}

void mq2373::startup(void) {
    set_course(drand48() * 2.0 * M_PI);
    set_speed(1);
    SmartPointer<mq2373> self = SmartPointer<mq2373>(this);
    hunt_event = new Event(0, [self](void) { self->hunt(); });
}

void mq2373::spawn(void) {
    SmartPointer<mq2373> child = new mq2373;
    reproduce(child);
}


Color mq2373::my_color(void) const {
    return MAGENTA;
}

SmartPointer<LifeForm> mq2373::create(void) {
    return new mq2373;
}


void mq2373::hunt(void) {
    const String fav_food = "Algae";

    hunt_event = nullptr;
    if (health() == 0.0) { return; } // we died
	
	ObjList prey = perceive(35.0);
	if(get_speed() == 3)
		prey = perceive(45.0);

    double best_d = HUGE;
	double largest_d = 0;
	fakeName = false;
	if(prey.begin()!= prey.end()){
		for (ObjList::iterator i = prey.begin(); i != prey.end(); ++i) {
			if ((*i).species == fav_food) {
				if (best_d > (*i).distance) {
					set_speed(7);
					set_course((*i).bearing);
					best_d = (*i).distance;
				}
			}
		}
		if(best_d == HUGE){
			for (ObjList::iterator i = prey.begin(); i != prey.end(); ++i) {
				if((*i).species != species_name()){
					if (largest_d < (*i).distance) {
						set_speed(1);
						set_course((*i).bearing);
						largest_d = (*i).distance;
					}
				}
			}
			if(largest_d != 0){
				SmartPointer<mq2373> self = SmartPointer<mq2373>(this);
				hunt_event = new Event((largest_d/2), [self](void) { self->hunt(); });
			}else{
				set_speed(1);
				if(get_course() > M_PI)
					set_course(get_course() - 0.1 * M_PI);
				else
					set_course(get_course() + 0.1 * M_PI);
				SmartPointer<mq2373> self = SmartPointer<mq2373>(this);
				hunt_event = new Event(10.0, [self](void) { self->hunt(); });					
			}
		} else {
			SmartPointer<mq2373> self = SmartPointer<mq2373>(this);
			hunt_event = new Event((best_d/6.5), [self](void) { self->hunt(); });
		}
	} else {
		if(get_speed() != 35){
			set_speed(3);
			if(get_course() < M_PI)
				set_course(get_course() + M_PI);
			else
				set_course(get_course() - M_PI);
		} else {
			set_speed(3);
			if(get_course() < M_PI)
				set_course(get_course() + 0.1 * M_PI);
			else
				set_course(get_course() - 0.1 * M_PI);
		}
		SmartPointer<mq2373> self = SmartPointer<mq2373>(this);
		hunt_event = new Event(30.0, [self](void) { self->hunt(); });
	}
    if (health() >= 3.0) spawn();
}
