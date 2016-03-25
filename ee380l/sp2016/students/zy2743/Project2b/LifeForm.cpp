/* main test simulator */
#include <iostream>
#include "CraigUtils.h"
#include "Window.h"
#include "tokens.h"
#include "ObjInfo.h"
#include "QuadTree.h" 
#include "Params.h"
#include "LifeForm.h"
#include "Event.h"

using namespace std;

template <typename T>
void bound(T& x, const T& min, const T& max) {
	assert(min < max);
	if (x > max) { x = max; }
	if (x < min) { x = min; }
}

ObjInfo LifeForm::info_about_them(SmartPointer<LifeForm> neighbor) {
	ObjInfo info;
	info.species = neighbor->species_name();
	info.health = neighbor->health();
	info.distance = pos.distance(neighbor->position());
	info.bearing = pos.bearing(neighbor->position());
	info.their_speed = neighbor->speed;
	info.their_course = neighbor->course;
	return info;
}

void LifeForm::update_position(void){
    if(!is_alive) return;
    double delta = Event::now() - update_time;
    if(delta < 0.001) return;
    update_time = Event::now();
    Point newPos{pos};
    newPos.xpos += delta*speed*cos(course);
    newPos.ypos += delta*speed*sin(course);
    if((energy -= movement_cost(speed, delta)) < min_energy || space.is_out_of_bounds(newPos))
        die();
    else{
        space.update_position(pos, newPos);
        pos = newPos;
    }
}

void LifeForm::region_resize(void){
    update_position();
    compute_next_move();
}

void LifeForm::compute_next_move(void){
    if(!is_alive) return;
    if(border_cross_event != nullptr){
        border_cross_event->cancel();
        border_cross_event = nullptr;
    }
    if(speed > 0.0){
        SmartPointer<LifeForm> p{this};
        border_cross_event = new Event(space.distance_to_edge(pos, course)/speed+Point::tolerance, [p](void){p->border_cross();});
    }
}

void LifeForm::set_course(double course){
    if(this->course == course) return;
    update_position();
    this->course = course;
    compute_next_move();
}

void LifeForm::set_speed(double speed){
    if(this->speed == speed) return;
    update_position();
    this->speed = speed;
    compute_next_move();
}

ObjList LifeForm::perceive(double dis){
    std::vector<ObjInfo> res{};
    update_position();
    if(!is_alive) return res;
    if(dis > max_perceive_range) dis = max_perceive_range;
    if(dis < min_perceive_range) dis = min_perceive_range;
    if((energy -= perceive_cost(dis)) < min_energy){
        die();
        return res;
    }
    for(auto& life: space.nearby(pos, dis))
        res.push_back(info_about_them(life));
    return res;
}

void LifeForm::age(void){
    if(!is_alive) return;
    if((energy -= age_penalty) < min_energy){
        die();
        return;
    }
    SmartPointer<LifeForm> p{this};
    new Event(age_frequency, [p](void){ p->age(); });
}

void LifeForm::gain_energy(double gain){
    if(!is_alive) return;
    energy += gain;
}

void LifeForm::eat(SmartPointer<LifeForm> prey){
    if(!is_alive) return;
    if(!prey->is_alive) return;
    if((energy -= eat_cost_function(0, 0)) < min_energy){
        die();
        return;
    }
    SmartPointer<LifeForm> p{this};
    double gain = prey->energy * eat_efficiency;
    new Event(digestion_time, [gain, p](void){ p->gain_energy(gain); });
    prey->die();
}

void LifeForm::check_encounter(void){
    if(!is_alive) return;
    for(auto& life: space.nearby(pos, encounter_distance * 2))
        life->update_position();
    SmartPointer<LifeForm> candidate = space.closest(pos);
    if(candidate->update_time != Event::now())
        candidate->update_position();
    if(candidate->is_alive && pos.distance(candidate->pos) <= encounter_distance)
        resolve_encounter(candidate);
}

void LifeForm::border_cross(void){
    if(!is_alive) return;
    update_position();
    check_encounter();
    compute_next_move();
}

void LifeForm::resolve_encounter(SmartPointer<LifeForm> life){
    if(!is_alive) return;
    if(!life->is_alive) return;
    if((energy -= encounter_penalty) < min_energy) die();
    if((life->energy -= encounter_penalty) < min_energy) life->die();
    if(!(is_alive && life->is_alive)) return;
    Action a1 = encounter(info_about_them(life));
    if(!is_alive && !life->is_alive) return;
    Action a2 = life->encounter(life->info_about_them(SmartPointer<LifeForm>{this}));
    if(!life->is_alive && !is_alive) return;
    double r1 = drand48();
    double r2 = drand48();
    SmartPointer<LifeForm> p{this};
    double chance1 = eat_success_chance(energy, life->energy);
    double chance2 = eat_success_chance(life->energy, energy);
    if(a1 == LIFEFORM_IGNORE && a2 == LIFEFORM_IGNORE){
        return;
    }else if(a1 == LIFEFORM_EAT && a2 == LIFEFORM_IGNORE){
        if(r1 < chance1)
            eat(life);
    }else if(a1 == LIFEFORM_IGNORE && a2 == LIFEFORM_EAT){
        if(r2 < chance2)
            life->eat(p);
    }else{
        if(r1 < chance1 && r2 >= chance2)
            eat(life);
        else if(r1 >= chance1 && r2 < chance2)
            life->eat(p);
        else if(r1 < chance1 && r2 < chance2){
            switch(encounter_strategy){
                case EVEN_MONEY:
                    if(drand48() <= 0.5)
                        eat(life);
                    else
                        life->eat(p);
                    break;
                case BIG_GUY_WINS:
                    if(energy >= life->energy)
                        eat(life);
                    else
                        life->eat(p);
                    break;
                case UNDERDOG_IS_HERE:
                    if(energy <= life->energy)
                        eat(life);
                    else
                        life->eat(p);
                    break;
                case FASTER_GUY_WINS:
                    if(speed >= life->speed)
                        eat(life);
                    else
                        life->eat(p);
                    break;
                case SLOWER_GUY_WINS:
                    if(speed <= life->speed)
                        eat(life);
                    else
                        life->eat(p);
                    break;
                default:
                    break;
            }
        }
    }
}

void LifeForm::reproduce(SmartPointer<LifeForm> child){
    if(Event::now()-reproduce_time < min_reproduce_time || !is_alive)
        return;
    
    update_position();
    if(!is_alive) return;
    
    double next_energy = energy*0.5*(1-reproduce_cost);
    if(next_energy < min_energy){
        die();
        return;
    }
    SmartPointer<LifeForm> nearest;
    int i = 0;
    do{
        child->pos.ypos = pos.ypos + sin(drand48() * 2.0 * M_PI)*drand48()*reproduce_dist;
        child->pos.xpos = pos.xpos + cos(drand48() * 2.0 * M_PI)*drand48()*reproduce_dist;
        nearest = space.closest(child->pos);
        i++;
    }while(i<=5 && ((nearest && nearest->position().distance(child->position()) <= encounter_distance) || space.is_out_of_bounds(child->pos)));
    if(i > 5) return;
    energy = next_energy;
    child->energy = next_energy;
    reproduce_time = Event::now();
    child->start_point = child->pos;
    space.insert(child, child->pos, [child](void) { child->region_resize(); });
    new Event(age_frequency, [child](void) { child->age(); });
    child->is_alive = true;
}

