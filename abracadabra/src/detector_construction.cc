#include "detector_construction.hh"

#include <G4Box.hh>
#include <G4Cons.hh>
#include <G4LogicalVolume.hh>
#include <G4NistManager.hh>
#include <G4Orb.hh>
#include <G4PVPlacement.hh>
#include <G4RunManager.hh>
#include <G4Sphere.hh>
#include <G4SystemOfUnits.hh>
#include <G4ThreeVector.hh>
#include <G4Trd.hh>

// Create logical volume from solid and material
G4LogicalVolume* logical(G4VSolid* solid, G4Material* material) {
  return new G4LogicalVolume{solid, material, solid->GetName()};
}

G4VPhysicalVolume* detector_construction::Construct() {
  // Lookup-by-name of materials from NIST database
  G4NistManager* nist = G4NistManager::Instance();

  auto material = [nist](auto const& name) { return nist->FindOrBuildMaterial(name); };

  auto place = [](G4ThreeVector position, G4LogicalVolume* logical, G4LogicalVolume* parent = nullptr) {
    auto name           = logical->GetName();
    bool bool_op        = false;
    bool check_overlaps = true;
    return new G4PVPlacement{nullptr, position, logical, name, parent, bool_op, check_overlaps};
  };

  // Envelope parameters
  G4double env_size_xy = 20 * cm;
  G4double env_size_z  = 30 * cm;

  // World ----------------------------------------------------------------------
  G4double world_size_xy = 1.2 * env_size_xy;
  G4double world_size_z  = 1.2 * env_size_z;

  auto logic_world = logical
    (new G4Box{"World",
               0.5 * world_size_xy,
               0.5 * world_size_xy,
               0.5 * world_size_z  },
     material("G4_AIR"));

  G4VPhysicalVolume* phys_world = place({}, logic_world);

  // Envelope ----------------------------------------------------------------------
  auto logic_env = logical
    (new G4Box{"Envelope",
               0.5 * env_size_xy,
               0.5 * env_size_xy,
               0.5 * env_size_z},
     material("G4_WATER"));

  place({}, logic_env, logic_world);

  // Cone --------------------------------------------------------------------------
  G4double cone_rmin_a  = 0 * cm,   cone_rmax_a = 2 * cm;
  G4double cone_rmin_b  = 0 * cm,   cone_rmax_b = 4 * cm;
  G4double cone_hz      = 3 * cm;
  G4double cone_phi_min = 0 * deg, cone_phi_max = 360 * deg;

  place({0, 2*cm, -7*cm},
        logical(new G4Cons{"TissueCone",
                           cone_rmin_a, cone_rmax_a,
                           cone_rmin_b, cone_rmax_b,
                           cone_hz, cone_phi_min, cone_phi_max},
                material("G4_A-150_TISSUE")),
        logic_env);

  // Trapezoid -------------------------------------------------------------------
  G4double trapezoid_dxa = 12 * cm, trapezoid_dxb = 12 * cm;
  G4double trapezoid_dya = 10 * cm, trapezoid_dyb = 16 * cm;
  G4double trapezoid_dz  =  6 * cm;

  auto trapezoid = logical
    (new G4Trd{"BoneTrapezoid",
               0.5 * trapezoid_dxa, 0.5 * trapezoid_dxb, 0.5 * trapezoid_dya,
               0.5 * trapezoid_dyb, 0.5 * trapezoid_dz},
     material("G4_BONE_COMPACT_ICRU"));

  place({0, -1*cm, 7*cm}, trapezoid, logic_env);

  // --------------------------------------------------------------------------------

  // Set Shape2 as scoring volume
  this -> scoring_volume = trapezoid;

  //always return the physical World
  return phys_world;
}
