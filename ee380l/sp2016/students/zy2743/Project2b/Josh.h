#include "LifeForm.h"
#include "Init.h"
#include <string>

class Josh : public LifeForm {
private:
    void startup(void);
    void spawn(void);
    void hunt(void);
    bool isAlgae(ObjInfo const&);
    bool isFriend(ObjInfo const&);
    Event* hunt_event;
    static SmartPointer<LifeForm> create(void);
    static void initialize(void);
    int no_target_count;
    std::string name;
    bool cheatAlgae;
    double const algea_speed_lo = 1.01;
    double const algea_speed_hi = 7.01;
    double const speed_delta = 0.00001;
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

