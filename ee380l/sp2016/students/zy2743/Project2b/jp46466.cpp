//
//  jp46466.cpp
//  Proj3
//
//  Created by Jun Pan on 4/28/14.
//  Copyright (c) 2014 Jun Pan. All rights reserved.
//

#include "jp46466.h"
#include <iostream>
#include <cstdlib>
#include <cmath>
#include "CraigUtils.h"
#include "Params.h"
#include "Event.h"
#include "ObjInfo.h"
#include "Window.h"

using namespace std;
using String = std::string;

Initializer<jp46466> __jp46466_initializer;

//int Praveen::total_num = 0 ; // sorry, no statics allowed

String jp46466::species_name(void) const
{
	return myname;
}

String jp46466::player_name(void) const {

   return "jp46466";
}
Action jp46466::encounter(const ObjInfo& info)
{
	if (info.species == species_name()) {
		/* don't be cannibalistic */
		set_course(info.bearing + M_PI);
		return LIFEFORM_IGNORE;
	}
	else {
		hunt_event->cancel();
		SmartPointer<jp46466> me{this};
                if (huntevent ==1){
                    hunt_event = new Event(0.1, [me] (void) { me->hunt2(); });
                } else{
                    hunt_event = new Event(0.1, [me] (void) { me->hunt(); });
                }
                   
                //set_speed(0);
		return LIFEFORM_EAT;
	}
}

void jp46466::initialize(void)
{
	LifeForm::add_creator(jp46466::create, "jp46466");
}

/*
 * REMEMBER: do not call set_course, set_speed, perceive, or reproduce
 * from inside the constructor!!!!
 * you must wait until the object is actually alive
 */
jp46466::jp46466()
{
    hunt_event = Nil<Event>();
    SmartPointer<jp46466> me{this};
    (void) new Event(0.0, [me] (void) { me->live();});
}


jp46466::~jp46466()
{
}

void jp46466::spawn(void)
{
	jp46466* child = new jp46466;
	reproduce(child);
        if(child->health() >0) set_course(child->get_course() +M_PI/3);
        //if(child->health()>6.0) child->spawn();
}


Color jp46466::my_color(void) const
{
  	return BLUE;
}

LifeForm* jp46466::create(void)
{
	jp46466* res = Nil<jp46466>();
	res = new jp46466;
	res->display();
	return res;
}


/*
 * this event is only called once, when the object is "born"
 *
 * I don't want to call "set_course" and "set_speed" from the constructor
 * 'cause OBJECTS ARE NOT ALIVE WHEN THEY ARE CONSTRUCTED (remember!)
 * so... in the constructor I schedule this event to happen in 0 time
 * units
 */
void jp46466::live(void)
{
	set_course(drand48() * 2.0 * M_PI);
	set_speed(2 +5  * drand48());
    init_speed = get_speed();
        perrange = initperrange; 
	SmartPointer<jp46466> me{this};
	hunt_event = new Event(5.0, [me] (void) { me->hunt();});
      huntevent = 0;
         //if (health()>=0.5)me->spawn();
}

void jp46466::hunt(void)
{

    const String fav_food = "Algae";
    const String mefind = "jp46466:lol";
  
    if (health() == 0) { return; }
     
    ObjList prey = perceive(perrange);
     int friendfind = false; 
    //bool friendly = false;
    ObjInfo opp;
    double bfriendd = HUGE;
    double best_d = HUGE;
    int count = 0 ;
    for (ObjInfo i : prey) {
        if (i.species == fav_food) {
        count++ ;
            course_changed = 0 ;
            if (best_d > i.distance) {
                set_course(i.bearing);
                best_d = i.distance;
            }
        }else if (i.species == mefind &&course_changed && bfriendd > i.distance){
            set_course(i.bearing);
            friendfind = true; 
            bfriendd = i.distance; 
        }else{
           //set_course(i.their_course+M_PI);
           //friendly = true; 
        }
    }
    if (health()> 10.0&&count <1) {
        set_speed(0.0);
        set_course(0.0);
        myname = "Algae";
        SmartPointer<jp46466> me{this};
        hunt_event = new Event(10.0, [me] (void) { me->hunt2();});
        huntevent = 1; 
        return; 
    }


    if(best_d == HUGE){
       myname = "jp46466";
       
       //set_speed(init_speed); 
        if(course_changed == 0){
            course_changed = 1 ;
            set_course(get_course() + M_PI) ;
           set_speed(init_speed);
           persist = 5; 
         //  lost = 0;
        }else {
            //myname = "jp46466:hungary";
          if(!friendfind){
                lost++; 
                //perrange *= 1.2;rse ( get_course()+M_PI/2);
                  // }el
                //set_speed (get_speed()/2);
                if (lost < lostchance ){
                    if (persist-- <= 0){
                       if(coin){
                           set_course(get_course() + courserec) ;
                       }else{
                           set_course(get_course() - courserec);
                       }
                       coin = !coin; 
                       persist = initpersist; 
                       //courserec*=2;
                       //if (courserec > M_PI) courserec = M_PI/10; 
                    } else {
                       //set_course( M_PI*2*drand48());
                    }
                }else {
                   persist = initpersist;
                   if (coin2) {
                      set_course ( get_course()+M_PI);
                   }else {
                      set_course ( get_course()+M_PI);
                   }
                   coin2 = !coin2; 
                   initpersist *=2;     
                   lost = 0;
                   lostchance *= 1.5;
                }
               // set_speed(get_speed()*1.2);
           // }
          }else {
           // myname = "jp46466:lol";
          }
        }
    } else{
       courserec = M_PI/8;
       lostchance = 10;
       lost = 0 ; 
       if (count>4)myname = "jp46466:lol";
       double nottofast = max_speed>15? 15 : max_speed;
       set_speed(nottofast);
       //perrange = initperrange; 
      
    }
    SmartPointer<jp46466> me{this};
    hunt_event = new Event(10.0, [me] (void) { me->hunt();});
    huntevent = 0; 
    if (health() >= 6.0 && count >5) spawn();
}

void jp46466::hunt2(void)
{

    const String fav_food = "Algae";
      hunt_event = Nil<Event>();
    if (health() == 0) { return; }
     
    ObjList prey = perceive(perrange);
    int count = 0 ;
    for (ObjInfo i : prey) {
        if (i.species == fav_food) {
        count++ ;
        }
    }

    if (health()< 10.0 || count>5) {
	set_course(drand48() * 2.0 * M_PI);
	set_speed(2 + 5 * drand48());
        myname = "jp46466";
        SmartPointer<jp46466> me{this};
        hunt_event = new Event(0.0, [me] (void) { me->hunt();});
        huntevent = 0;
        return; 
    }

   // if (count >=4 &&health()>6.0)  spawn();

    SmartPointer<jp46466> me{this};
    hunt_event = new Event(30.0, [me] (void) { me->hunt2();});
    huntevent = 1; 
}












