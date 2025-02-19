include <../common.scad>

od = 16.0;

module tonehole(h=10, d=10, undercut=0) {
  translate([od/2, 0, h]) {
    rotate([0, -90+undercut, 0]) {
      // Scoot it down a hair so it will penetrate the entire tube when rotated
      translate([0, 0, -3]) {
        cylinder(h=od/2, d=d);
      }
    }
  }
}

module chanter() {
  difference () {
    union() {
      // The core part of the instrument is this brass tube
      brass() cylinder(h=360.0, d=od);

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
    tonehole(h=57.1, d=6.68); // E-
    tonehole(h=87.0, d=4.25); // E
    tonehole(h=118.0, d=8.33); // F#
    tonehole(h=149.0, d=5.51, undercut=-15); // G
    tonehole(h=184.5, d=7.75, undercut=15); // A
    tonehole(h=184.5, d=7.75, undercut=-15); // A (wide undercut)
    tonehole(h=219.0, d=6.72, undercut=15); // B
    tonehole(h=250.0, d=5.50, undercut=30); // C
    rotate([0, 0, 180]) tonehole(h=270.0, d=7.11, undercut=30); // D
  }
}

module cut() {
    rotate([0, 45, 0]) cube(240, center=true);
}

chanter();
