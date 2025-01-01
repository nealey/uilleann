include <common.scad>


rotate([0, 180, 0]) translate([0, 0, -135])
difference() {
  union() {
    color("skyblue") {
      cylinder(h=135.0, d=10.0);
      translate([0, 0, 128]) cylinder(h=7, d=14);
      translate([0, 0, 120]) cylinder(h=8, d1=10, d2=14);
      translate([0, 0, 130]) washer(h=1, d1=9, d2=11);
      translate([0, 0, 66]) cylinder(h=1, d1=11, d2=10);
    }
    color("gold") cylinder(h=72.0, d=11.0);
   }

  // Inner bores
  cylinder(h=66.0, d=9.6);
  cylinder(h=150.0, d=5.0);

  // Decorative engravings
  translate([0, 0, 130]) washer(h=0.4, d1=13.6, d2=15);
  translate([0, 0, 132]) washer(h=0.4, d1=13.6, d2=15);
  translate([0, 0, 69]) washer(h=0.4, d1=10.6, d2=15);
  translate([0, 0, 66]) washer(h=0.4, d1=10.6, d2=15);
  translate([0, 0, 4]) washer(h=0.4, d1=10.6, d2=15);
  translate([0, 0, 2]) washer(h=0.4, d1=10.6, d2=15);
}
