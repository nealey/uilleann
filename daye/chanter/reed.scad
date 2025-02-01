include <../common.scad>

// Evertjan 't Hart's guide was super helpful:
// https://www.hartdd.com/reedmaking/reed1.html
//
// Using those dimensions (base thickness = 2, cane_d = 24),
// the width comes out to 13.2mm,
// which is what the page says it should be.

length = 50;
thickness = 0.55;
cane_d = 24;
base_thickness = 2;

scrape_angle = atan((base_thickness-thickness)/length)*2.6;

difference() {
  translate([0, 0, -cane_d/2+base_thickness]) {
    difference() {
      // Start with a solid piece of cane.
      rotate([-90, 0, 0]) cylinder(d=cane_d, h=length);

      // Chisel out reed
      translate([-cane_d/2, -length, -cane_d/2-base_thickness]) cube([cane_d, length*3, cane_d]);
    }
  }

  // Cut the tails
  // I totally guessed at all of these
  #translate([0, 10, 0]) rotate([0, 0, -30]) cube(20);
  

  // Sand inside to 1.5mm thickness
  sander_d = 82;
  translate([0, 0, base_thickness - 1.5 - sander_d/2]) rotate([-90, 0, 0]) cylinder(d=sander_d, h=length);

  // Scrape reed
  translate([0, length, d/2-base_thickness+thickness]) rotate([-scrape_angle, 0, 0]) translate([-d/2, -length, 0]) cube([d, length, d]);

  // Reed chopper
  translate([0, length, 0]) {
    translate([0, -d/2, 0]) {
      difference() {
        translate([-d/2, 0, 0]) cube([d, d/2, d]);
        cylinder(d=d, h=d);
      }
    }
  }
}
