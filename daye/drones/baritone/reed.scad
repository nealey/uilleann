include <../../common.scad>

// 9mm od
// 7.5mm from plane on reed holder to back
// 9 - 7.5 = 2.5 thickness; 4.5-2.5=2 cutaway from center

length = 45;
thickness = 0.55;
d = 9;
base_thickness = 1.5;

scrape_angle = atan((base_thickness-thickness)/length)*2.6;

translate([0, 0, -d/2+base_thickness])
difference() {
  rotate([-88, 0, 0]) cylinder(d=d, h=length);

  // Chop off excess cylinder at the base
  translate([-d/2, -d, 0]) cube(d);

  // Shave off reed
  translate([-d/2, -length, -d/2-base_thickness]) cube([d, length*3, d]);

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
