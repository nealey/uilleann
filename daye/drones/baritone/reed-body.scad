include<../../common.scad>

module reed_body() {
  difference() {
    cylinder(h=100, d=8);
    translate([0, 0, 4]) cylinder(h=100, d=7.12);
    translate([-5, 2.6, 0]) rotate([-1.5, 0, 0]) cube([10, 10, 100]);
  }
}

reed_body();
