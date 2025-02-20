include <../common.scad>

height = 360.0;
od = 16.0;
od_derlin = od + 0.5;
id_bot = 13.2;
id_top = 5.51;
cut_height = 155; // Where to cut it in half

module tonehole(h=10, d=10, undercut=0) {
  rotate([0, 0, -90])
  translate([od/2, 0, h]) {
    rotate([0, -90+undercut, 0]) {
      // Scoot it down a hair so it will penetrate the entire tube when rotated
      translate([0, 0, -od/2]) {
        cylinder(h=od, d=d);
      }
    }
  }
}


// Something like David Daye's Penny Chanter,
// based on my measurements and undercut hints on David's web site.
// This is symmetrical, so both pieces can be printed at the same time,
// with color changes
module chanter() {
  difference () {
    union() {
      // The core part of the instrument is this brass tube
      difference() {
        brass() cylinder(h=height, d=od);

        // Seam guides
        translate([0, od/2, 0]) cylinder(d=1, h=400);
      }

      // David adds this outer Derlin tube as an upgrade option
      difference() {
        derlin() translate([0, 0, 40.0]) cylinder(h=290, d=od_derlin);

        // The slits up the sides serve three purposes:
        // 1. directs the slicer to place seams in it (and not on the face)
        // 2. provides alignment guides for attaching the two pieces
        // 3. looks like an intentional stylistic thing
        rotate([0, 0, 85]) translate([0, od_derlin/2, 0]) cylinder(d=1, h=cut_height+20); // overlap
        rotate([0, 0, -85]) translate([0, od_derlin/2, cut_height-20]) cylinder(d=1, h=400);
      }

      // These little doodads are probably just to hide the Derlin seam
      ivory() translate([0, 0, 30.0]) cylinder(h=12, d=22.0);
      ivory() translate([0, 0, height - 30.0 - 12]) cylinder(h=12, d=22.0);
    }

    // Seam guides
    translate([0, 11, 0]) cylinder(d=1, h=400);

    // Inner bore, which runs up to the place where the reed drops in
    translate([0, 0,   0]) cylinder(h=337.01, d1=id_bot, d2=id_top);
    translate([0, 0, 337]) cylinder(h=23.1, d1=id_top, d2=7.1);

    // I totally dig David's minimalist aesthetic
    translate([0, 0, 3]) rings(d=16.0);

    // Stamp my name on it
    difference() {
      translate([0, 0, 307]) {
        rotate([90, 0, 180]) {
          linear_extrude(50) {
            translate([0, 5]) text("neale", font="Fontdiner Swanky", valign="bottom", size=3, halign="center");
            translate([0, 0]) text("2024-2", font="Fontdiner Swanky", valign="bottom", size=3, halign="center");
          }
        }
      }
      cylinder(h=360, d=od-0.5);
    }


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

module tenon(top=false, h=cut_height, od=od, depth=20, thickness=2) {
  module cut() {
        cylinder(h=cut_height+depth, d=od-thickness-clearance);
        cylinder(h=cut_height, d=50);
  }

  if (top) {
    difference() {
      children();
      cut();
    }
  } else {
    intersection() {
      children();
      cut();
    }
  }
}

chanter();
//translate([0, 50, 360]) rotate([180, 0, 0]) chanter();

