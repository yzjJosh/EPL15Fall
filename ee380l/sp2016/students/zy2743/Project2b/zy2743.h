#include "LifeForm.h"
#include "Init.h"
#include <string>

class zy2743 : public LifeForm {
private:
    
    static double const speed_lo;
    static double const speed_hi;
    static double const speed_delta;
    static const std::string default_name;
    
    Event* hunt_event;
    bool cheatAlgae;
    bool isCheating;
    Point pos;
    double pos_update_time;
    double top;
    double bottom;
    double left;
    double right;
    int no_target_count;
    
    static bool isAlgae(ObjInfo const&);
    static bool isFriend(ObjInfo const&);
    static double eatChance(double, double);
    static SmartPointer<LifeForm> create(void);
    static void initialize(void);
    
    void startup(void);
    void spawn(void);
    void hunt(void);
    void update_pos(void);
    void update_bound(Point const&);
    void update_bound_from_friend(ObjInfo const&);
    void avoid_cross_bound(void);
    double max_move_time(void);
    Point get_pos(ObjInfo const&);

public:
  zy2743(void);
  ~zy2743(void);
  Color my_color(void) const override;   // defines LifeForm::my_color
  virtual std::string species_name(void) const override;
  virtual std::string player_name(void) const override;
  virtual Action encounter(const ObjInfo&) override;
  friend class Initializer<zy2743>;
};

