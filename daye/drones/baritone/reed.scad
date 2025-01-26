include <../../common.scad>

// 9mm od
// 7.5mm from plane on reed holder to back
// 9 - 7.5 = 2.5 thickness; 4.5-2.5=2 cutaway from center

length = 45;
thickness = 0.5;
d = 9;
base_thickness = 1.5;

scrape_angle = atan((base_thickness-thickness)/length)*1.5;

module reed_chopper(d) {
  difference() {
    translate([-d/2, 0, 0]) cube([d, d/2, d]);
    cylinder(d=d, h=d);
  }
}

difference() {
  cylinder(d=d, h=length);
  translate([-5, -d/2+base_thickness, 0]) cube([10, 10, length*2]);
  translate([-5, -d/2+base_thickness-thickness, length]) rotate([-scrape_angle, 180, 180]) cube([10, 10, 50]);
  translate([0, 0, length-d/2]) rotate([90, 0, 0]) reed_chopper(d);
}
