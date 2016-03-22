#include "LifeForm.h"
#include "Init.h"
#include <string>

class Josh : public LifeForm {
private:
    void startup(void);
    void spawn(void);
    void hunt(void);
    Event* hunt_event;
    static bool isAlgae(ObjInfo const&);
    static bool isFriend(ObjInfo const&);
    static double eatChance(double, double);
    static SmartPointer<LifeForm> create(void);
    static void initialize(void);
    
    int no_target_count;
    std::string name;
    bool cheatAlgae;
    static double const algea_speed_lo;
    static double const algea_speed_hi;
    static double const speed_delta;
    static const std::string default_name;
public:
  Josh(void);
  ~Josh(void);
  Color my_color(void) const override;   // defines LifeForm::my_color
  virtual std::string species_name(void) const override;
  virtual std::string player_name(void) const override;
  virtual Action encounter(const ObjInfo&) override;
  friend class Initializer<Josh>;
};

