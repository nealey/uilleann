include <../common.scad>

module tonehole(h=10, d=10) {
  translate([0, 0, h]) rotate([0, 90, 0]) cylinder(h=20, d=d);
}

difference () {
  union() {
    // The core part of the instrument is this brass tube
    brass() cylinder(h=360.0, d=16.0);

    // David adds this outer Derlin tube as an upgrade option
    derlin() translate([0, 0, 40.0]) cylinder(h=290, d=16.5);

    // These little doodads are probably just to hide the Derlin seam
    ivory() translate([0, 0, 30.0]) cylinder(h=12, d=22.0);
    ivory() translate([0, 0, 322.0]) cylinder(h=10, d=22.0);
  }

  // The bore for the reed: no harm in running this the entire length
  cylinder(h=400, d=6.3);

  // Inner bore, which runs up to the place where the reed drops in
  cylinder(h=327, d=13.5);

  // I totally dig David's minimalist aesthetic
  translate([0, 0, 3]) rings(d=16.0);

  // Tone Holes!
  // XXX: Undercut angle should be taken into account here
  tonehole(h=57.1, d=6.68);
  tonehole(h=87.0, d=4.25);
  tonehole(h=118.0, d=8.33);
  tonehole(h=149.0, d=5.51);
  tonehole(h=184.5, d=7.75);
  tonehole(h=219.0, d=6.72);
  tonehole(h=250.0, d=5.50);
}
