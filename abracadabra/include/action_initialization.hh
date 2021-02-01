#ifndef action_initialization_hh
#define action_initialization_hh 1

#include <G4VUserActionInitialization.hh>

/// Action initialization class.

class action_initialization : public G4VUserActionInitialization {
public:
  action_initialization() : G4VUserActionInitialization() {}
  virtual ~action_initialization() override{};

  virtual void BuildForMaster() const override;
  virtual void Build()          const override;
};

#endif
