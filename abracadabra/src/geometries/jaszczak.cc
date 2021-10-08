#include "geometries/jaszczak.hh"
#include "utils/enumerate.hh"

#include "nain4.hh"

#include <G4Tubs.hh>
#include <G4Box.hh>


using nain4::material;
using nain4::place;
using nain4::volume;

using CLHEP::pi; using CLHEP::twopi;

jaszczak_phantom build_jaszczak_phantom::build() {
  return std::move(*this);
}


G4PVPlacement* jaszczak_phantom::geometry() const {
  auto air   = material("G4_AIR");
  auto water = material("G4_WATER"); // The radioactive source is floating around in water
  auto pmma  = material("G4_LEAD"); // TODO replace more appropriate material

  auto env_half_length = height_cylinder * 1.1;
  auto env_half_width  = radius_cylinder * 1.1;

  auto cylinder = volume<G4Tubs>("Cylinder", water, 0.0, radius_cylinder, height_cylinder/2, 0.0, twopi);
  auto envelope = volume<G4Box> ("Envelope", air , env_half_width, env_half_width, env_half_length);

  for (const auto [n, r] : enumerate(radii_spheres)) {
    rod_sector(n, r, cylinder, pmma);
  }
  // TODO spheres
  // TODO orientation of whole
  place(cylinder).in(envelope).now();
  return place(envelope).now();
}


void jaszczak_phantom::rod_sector(unsigned long n, G4double r,
                                  G4LogicalVolume* cylinder, G4Material* material) const {
  auto d = 2 * r;
  auto z = (height_rods - height_cylinder) / 2;
  G4RotationMatrix around_z_axis{{0,0,1}, n*pi/3};

  // Sector displacement from centre, to accommodate gap between sectors
  auto dx = gap * cos(pi/6);
  auto dy = gap * sin(pi/6);
  // Displacement of first rod WRT sector corner
  dx += r * sqrt(3);
  dy += r;
  // Basis vectors of rod lattice
  const auto Ax = 2.0, Ay = 0.0;
  const auto Bx = 1.0, By = sqrt(3);
  auto a = 0;
  for (bool did_b=true ; did_b; a+=1) {
    did_b = false;
    for (auto b = 0; /*break in body*/; b+=1, did_b = true) {
      auto x = (a*Ax + b*Bx) * d + dx;
      auto y = (a*Ay + b*By) * d + dy;
      if (sqrt(x*x + y*y) + r + margin >= radius_cylinder) { break; }
      auto label = std::string("Rod-") + std::to_string(n);
      auto rod = volume<G4Tubs>(label, material, 0.0, r, height_rods/2, 0.0, twopi);
      place(rod).in(cylinder).at(x,y,z).rotate(around_z_axis).now();
    }
  }
}

G4ThreeVector jaszczak_phantom::generate_vertex() const {

  return {};
}

world_geometry_inspector* jaszczak_phantom::inspector() {
  if (! inspector_) {
    inspector_.reset(new world_geometry_inspector{run_manager -> get()});
  }
  return inspector_.get();
}
