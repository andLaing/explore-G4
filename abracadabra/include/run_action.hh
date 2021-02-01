#ifndef run_action_hh
#define run_action_hh 1

#include <G4Accumulable.hh>
#include <G4UserRunAction.hh>
#include <globals.hh>

class G4Run;

/// EndOfRunAction(), calculates the dose in the selected volume from the energy
/// deposit accumulated via stepping and event actions. The computed dose is
/// then printed on the screen.

class run_action : public G4UserRunAction {
public:
  run_action();
  virtual ~run_action() override {}

  //virtual G4Run* GenerateRun() override;
  virtual void BeginOfRunAction(const G4Run*) override;
  virtual void   EndOfRunAction(const G4Run*) override;

  void AddEdep (G4double edep);

private:
  G4Accumulable<G4double> edep;
  G4Accumulable<G4double> edep2;
};

#endif
