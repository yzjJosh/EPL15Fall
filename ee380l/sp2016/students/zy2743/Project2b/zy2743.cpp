#include <cmath>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <iterator>
#include <string>

#include "zy2743.h"
#include "Event.h"
#include "ObjInfo.h"
#include "Params.h"
#include "Random.h"
#include "Window.h"

using namespace std;
using String = std::string;

double const zy2743::speed_lo = 2.123456;
double const zy2743::speed_hi = 7.123456;
double const zy2743::speed_delta = 1e-7;
String const zy2743::default_name = "Josh:";
Initializer<zy2743> __zy2743_initializer;

/*
 * REMEMBER: do not call set_course, set_speed, perceive, or reproduce
 * from inside the constructor!!!!
 * you must wait until the object is actually alive
 */
zy2743::zy2743(void) {
    cheatAlgae = drand48() < 0.30? true: false;
    isFakingAlgae = false;
    isFakingEnemy = false;
    stop_faking_enemy_event = nullptr;
    hunt_event = nullptr;
    fake_name = "";
    pos = Point(0.0, 0.0);
    pos_update_time = Event::now();
    top = 0.0;
    bottom = 0.0;
    left = 0.0;
    right = 0.0;
    
    SmartPointer<zy2743> self = SmartPointer<zy2743>(this);
    new Event(0, [self](void) { self->startup(); });
}

zy2743::~zy2743() {}

String zy2743::player_name(void) const{
    return "zy2743";
}

String zy2743::species_name(void) const{
    if(isFakingEnemy)
        return fake_name;
    if(isFakingAlgae)
        return "Algae";
    else
        return default_name + to_string(pos_update_time) + " " + to_string(pos.xpos) + " " + to_string(pos.ypos) + " "
            + to_string(top) + " " + to_string(bottom) + " " + to_string(left) + " " + to_string(right);
}

SmartPointer<LifeForm> zy2743::create(void) {
    return new zy2743;
}

void zy2743::initialize(void) {
    LifeForm::add_creator(zy2743::create, "zy2743");
}

Action zy2743::encounter(const ObjInfo& info){
    if(health() == 0.0) return LIFEFORM_IGNORE;
    if(hunt_event != nullptr) hunt_event->cancel();
    update_pos();
    SmartPointer<zy2743> self = SmartPointer<zy2743>{this};
    if (isFriend(info)) {
        update_bound_from_friend(info);
        set_course(get_course() + M_PI_2);
        avoid_cross_bound();
        hunt_event = new Event(fmin(2.0, max_move_time()), [self](void) { self->hunt(); });
        return LIFEFORM_IGNORE;
    }
    else {
        double next_hunt_time = 0.0;
        if(!isAlgae(info)){
            isFakingEnemy = true;
            fake_name = info.species;
            if(stop_faking_enemy_event != nullptr) stop_faking_enemy_event->cancel();
            stop_faking_enemy_event = new Event(0.0, [self](void){ self->isFakingEnemy = false; self->stop_faking_enemy_event = nullptr;});
            
            double die_chance = eatChance(info.health, health(), info.their_speed, get_speed(), false);
            if(die_chance > 0.7){
                set_course(info.bearing + M_PI);
                set_speed(speed_hi);
                avoid_cross_bound();
                const double dodge_distance = 50.0;
                next_hunt_time = fmin(dodge_distance/get_speed(), max_move_time());
            }
        }
        hunt_event = new Event(next_hunt_time, [self](void) { self->hunt(); });
        return LIFEFORM_EAT;
    }
}

void zy2743::startup(void) {
    SmartPointer<zy2743> self = SmartPointer<zy2743>(this);
    hunt_event = new Event(0, [self](void) { self->hunt(); });
}

void zy2743::spawn(void) {
    SmartPointer<zy2743> child = new zy2743;
    reproduce(child);
}

Color zy2743::my_color(void) const {
   // if(isFakingAlgae) return YELLOW;
    return ORANGE;
}

bool zy2743::isAlgae(const ObjInfo & info){
    return info.species == "Algae" && info.their_speed == 0.0 && info.health < 3.0;
}

bool zy2743::isFriend(const ObjInfo & info){
    bool is_self = info.species.substr(0, default_name.length()) == default_name || ((abs(info.their_speed-speed_lo)<speed_delta || abs(info.their_speed-speed_hi)<speed_delta));
    const double threshold = 10000.0;
    bool is_ally = false;
    if(Event::now() < threshold)
        is_ally |=   info.species == "mq2373" || info.species == "yz9234";
    return is_self || is_ally;
}

double zy2743::eatChance(double h1, double h2, double s1, double s2, bool fake){
    double c1 = eat_success_chance(h1, h2);
    double c2 = eat_success_chance(h2, h1);
    double chance_eat_directly = c1*(1-c2*(fake? 0.5: 1.0));
    double chance_both_eat = c1*c2*(fake? 0.5: 1.0);
    double chance_win_when_both_eat = 0.0;
    switch(encounter_strategy){
        case EVEN_MONEY:
            chance_win_when_both_eat = 0.5;
            break;
        case BIG_GUY_WINS:
            chance_win_when_both_eat = h1>h2? 1.0: 0.0;
            break;
        case UNDERDOG_IS_HERE:
            chance_win_when_both_eat = h1<h2? 1.0: 0.0;
            break;
        case FASTER_GUY_WINS:
            chance_win_when_both_eat = s1 > s2? 1.0: 0.0;
            break;
        case SLOWER_GUY_WINS:
            chance_win_when_both_eat = s1 < s2? 1.0: 0.0;
            break;
        default:
            break;
    }
    return chance_eat_directly + chance_both_eat*chance_win_when_both_eat;
}

double zy2743::max_move_time(void){
    double x_speed = get_speed() * cos(get_course());
    double y_speed = get_speed() * sin(get_course());
    double x_time = MAXFLOAT;
    double tolerance = 0.8;
    if(x_speed > 0.0) x_time = (right - pos.xpos - tolerance)/x_speed;
    else if(x_speed < 0.0) x_time = (left - pos.xpos + tolerance)/x_speed;
    double y_time = MAXFLOAT;
    if(y_speed > 0.0) y_time = (top - pos.ypos - tolerance)/y_speed;
    else if(y_speed < 0.0) y_time = (bottom - pos.ypos + tolerance)/y_speed;
    return fmax(fmin(x_time, y_time), 0.0);
}

void zy2743::update_bound(const Point & p){
    if(p.xpos < left) left = p.xpos;
    if(p.xpos > right) right = p.xpos;
    if(p.ypos < bottom) bottom = p.ypos;
    if(p.ypos > top) top = p.ypos;
}

void zy2743::update_bound_from_friend(ObjInfo const& info){
    if(info.species.substr(0, default_name.length()) != default_name)
        return;
    String bounds = info.species.substr(default_name.length());
    istringstream iss{bounds};
    vector<String> tokens{istream_iterator<String>{iss}, istream_iterator<String>{}};
    double update_t = stod(tokens[0]);
    const double t_tolerance = 1.0;
    double delta = Event::now() - update_t;
    if(delta > t_tolerance) return;
    const double d_tolerance = delta * info.their_speed * 1.2;
    const double x_tolerance = d_tolerance * cos(info.their_course);
    const double y_tolerance = d_tolerance * sin(info.their_course);
    Point p = get_pos(info);
    double o_x = p.xpos - stod(tokens[1]);
    double o_y = p.ypos - stod(tokens[2]);
    double t = stod(tokens[3]) + o_y - fmax(y_tolerance, 0.0);
    double b = stod(tokens[4]) + o_y - fmin(y_tolerance, 0.0);
    double l = stod(tokens[5]) + o_x - fmin(x_tolerance, 0.0);
    double r = stod(tokens[6]) + o_x - fmax(x_tolerance, 0.0);
    if(t > top) top = t;
    if(b < bottom) bottom = b;
    if(l < left) left = l;
    if(r > right) right = r;
}

Point zy2743::get_pos(ObjInfo const& info){
    Point p{pos};
    p.xpos += info.distance * cos(info.bearing);
    p.ypos += info.distance * sin(info.bearing);
    return p;
}

void zy2743::update_pos(void){
    if(health() == 0.0) return;
    double delta = Event::now() - pos_update_time;
    pos_update_time = Event::now();
    pos.xpos += delta * get_speed() * cos(get_course());
    pos.ypos += delta * get_speed() * sin(get_course());
    update_bound(pos);
}

void zy2743::avoid_cross_bound(void){
    const double min_dis = 1.0;
    if(top - pos.ypos < min_dis || pos.ypos - bottom < min_dis){
        set_course(2*M_PI - get_course());
    }
    if(pos.xpos - left < min_dis || right - pos.xpos < min_dis){
        double new_course = M_PI - get_course();
        if(new_course < 0.0) new_course += 2*M_PI;
        set_course(new_course);
    }
    
}

void zy2743::hunt(void) {
    hunt_event = nullptr;
    if (health() == 0.0) return; // we died
    
    double radius = fmin(fmax(40.0*health(), 30), 100);
    ObjList objs = perceive(radius);
    
    if(health() == 0.0) return;
    
    update_pos();
    
    ObjList preys;
    ObjList hunters;
    int count_avoid_when_reproduce = 0;
    double weight[32];
    for(int i=0; i<32; i++) weight[i] = 0.0;
    for (auto const& obj: objs){
        double chance = eatChance(health(), obj.health, speed_hi, obj.their_speed, true);
        double die_chance = eatChance(obj.health, health(), obj.their_speed, speed_hi, false);
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
        update_bound(get_pos(obj));
        update_bound_from_friend(obj);
    }
    
    double next_hunt_time = 0.0;
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
            double chance = eatChance(health(), prey.health, speed_hi, prey.their_speed, true);
            double cur_weight = cos(prey.bearing - best_course)/prey.distance * (isAlgae(prey)? 1.0: chance);
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
        set_speed(speed_hi);
        
        next_hunt_time = fmin(max_distance/get_speed()*1.1, max_move_time());
    }else{
        double new_course = get_course();
        
        const double dodge_distance = 20.0;
        bool dodge = false;
        for (auto const& hunter: hunters) {
            if(hunter.distance < dodge_distance){
                new_course = hunter.bearing + M_PI;
                dodge = true;
                break;
            }
        }
        
        set_speed(dodge? speed_hi: speed_lo);
        set_course(new_course);
        
        avoid_cross_bound();
        next_hunt_time = fmin(dodge? dodge_distance/get_speed()*1.2: 10.0, max_move_time());
    }
    SmartPointer<zy2743> self = SmartPointer<zy2743>(this);
    hunt_event = new Event(next_hunt_time, [self](void) { self->hunt(); });
    
    if (((health() >= 4.0 && !cheatAlgae) || (health() >= 100.0 && cheatAlgae)) && count_avoid_when_reproduce <= 2) spawn();
    if (health() >= 10.0 && cheatAlgae) isFakingAlgae = true;
    if (health() < 10.0 && cheatAlgae) isFakingAlgae = false;
    isFakingEnemy = false;
}
