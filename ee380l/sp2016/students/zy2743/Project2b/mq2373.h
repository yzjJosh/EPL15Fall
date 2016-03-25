#if !(_mq2373_h)
#define _mq2373_h 1

#include <memory>
#include "LifeForm.h"
#include "Init.h"

class mq2373 : public LifeForm {
protected:
  static void initialize(void);
  void spawn(void);
  void hunt(void);
  void startup(void);
  Event* hunt_event;
  bool fakeName;
  bool isFriend(const ObjInfo & info);
  bool huntType = false;
public:
  mq2373(void);
  ~mq2373(void);
  std::string fake_name;
  Color my_color(void) const;   // defines LifeForm::my_color
  static SmartPointer<LifeForm> create(void);
  virtual std::string species_name(void) const;
  virtual Action encounter(const ObjInfo&);
  friend class Initializer<mq2373>;
};

#endif /* !(_mq2373_h) */
