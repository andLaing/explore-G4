#ifndef geometries_nema_hh
#define geometries_nema_hh

#include "random/random.hh"

#include <G4Event.hh>
#include <G4PVPlacement.hh>

#include <G4SystemOfUnits.hh>
#include <vector>



// ===== Section 3: Spatial Resolution =======================================================

// This is the most boring phantom of the lot: the geometry consists of 6
// pointlike sources, and there are no materials around them that are supposed
// to attenuate or scatter, so the generator is the only thing that matters.

class nema_3_phantom {
public:
  nema_3_phantom(G4double fov_length);
  void generate_primaries(G4Event* event) const;
  G4PVPlacement* geometry() const;
private:
  std::vector<G4ThreeVector> vertices;
};

// ===== NEMA NU-2 2018 Section 7: Image Qualitiy, Accuracy of Corrections ==================

// TODO central cylinder is missing, and its attenuating properties are an
// inherent part of the test.

// TODO if the NEMA NU 2 section 7 analyses are to be performed, the body size
// needs to be increased, in order to accommodate the 12 different background
// ROIs. Maybe the shape should then be adjusted to match the official one, but
// maybe it's a pointless PITA.

class nema_7_phantom {

public:
  G4PVPlacement* geometry() const;
  void generate_primaries(G4Event* event) const;
  G4ThreeVector generate_vertex() const;
  G4ThreeVector generate_vertex_in_body() const; // This one should probably end up private

  G4ThreeVector sphere_position(int n) const;
  bool inside_lung    (G4ThreeVector&) const;
  bool inside_a_sphere(G4ThreeVector&) const;
  bool inside_this_sphere(size_t, G4ThreeVector&) const;
  bool inside_whole(G4ThreeVector&) const { return true; }
  std::optional<size_t> in_which_region(G4ThreeVector&) const;

private:
  struct one_sphere {
    one_sphere(G4double radius, G4double activity) : radius{radius}, activity{activity} {}
    G4double radius;
    G4double activity;
  };

protected:
  std::vector<one_sphere> spheres;
  G4double background  = 1;
  G4double  inner_r    = 114.4*mm;
  G4double    top_r    = 147.0*mm;
  G4double corner_r    =  77.0*mm;
  G4double   lung_r    =  25.0*mm;
  G4double half_length =  90.0*mm;
  G4double to_end      =  70.0*mm; // NEMA requires spheres at 7cm from phantom end
  biased_choice pick_region{{}};
  biased_choice pick_sub_region{{}};

protected:
  std::tuple<G4double, G4double, G4double> sub_volumes() const;
};

// ----- Builder ----------------------------------------------------------------------
class build_nema_7_phantom : private nema_7_phantom {
public:
  build_nema_7_phantom& length(G4double l) { half_length = l / 2;  return *this; }
  build_nema_7_phantom&  inner_radius(G4double r)  {  inner_r = r; return *this; }
  build_nema_7_phantom&    top_radius(G4double r)  {    top_r = r; return *this; }
  build_nema_7_phantom& corner_radius(G4double r)  { corner_r = r; return *this; }
  build_nema_7_phantom& activity(G4double a)     { background = a; return *this; }
  build_nema_7_phantom& sphereR(G4double r, G4double activity);
  build_nema_7_phantom& sphereD(G4double d, G4double activity) { return sphereR(d/2, activity); }
  build_nema_7_phantom& lungD  (G4double d)                    { return   lungR(d/2          ); }
  build_nema_7_phantom& lungR  (G4double r) {  lung_r = r;    return *this; }
  build_nema_7_phantom& inner_diameter(G4double d) { return inner_radius(d/2); }
  build_nema_7_phantom& spheres_from_end(G4double l) { to_end = l; return *this; }
  nema_7_phantom build();
};
// ------------------------------------------------------------------------------------

// TODO this needs to live elsewhere
void generate_back_to_back_511_keV_gammas(G4Event* event, G4ThreeVector position, G4double time);

#endif
