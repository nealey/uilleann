include <../../common.scad>

plunge = 106;                   // How far in can the butt go?
height = 308;                   // as measured by my calipers

tubeh = 120;
tubed = 17.5;

bendod = 12.7;
bendh = 102;
bendr = 16;
bendplunge = 18;

endh = height - plunge;
endid = bendod + clearance*2;
endod = 16.5;

module drone_bass_tuner_barrel() {
  brass_tube(h=tubeh, id=tubed);
}

module drone_bass_tuner_end() {
  stopod = tubed - (clearance * 2);
  stoph = height / 30;
  stop_elevation = endh - (height - tubeh);

  union() {
    derlin_tube(h=endh, id=endid, od=endod);
    derlin_tube(h=stop_elevation, id=endid, od=tubed - clearance*2);
    translate([0, 0, stop_elevation]) derlin_stop(h=stoph, id=endod, od=tubed + brass_thickness*2);
  }
}

module drone_bass_bend() {
  union() {
    translate([bendr, 0, 0]) brass_bend(od=bendod, bend_r=bendr);
    translate([bendr*2, 0, -bendh]) brass_tube(h=bendh, id=bendod-brass_thickness*2);
    translate([0, 0, -bendplunge]) brass_tube(h=bendplunge, id=bendod-brass_thickness*2);
  }
}

module drone_bass_resonator() {
  mortise_id = bendod + clearance*2;

  color("ivory") {
    difference() {
      union() {
        cylinder(h=18, d=33.5);
        cylinder(h=19.2, d=15.6);
      }
      translate([0, 0, 9]) cylinder(h=18, d=mortise_id);
      translate([0, 0, 9]) rotate([0, 90, 0]) cylinder(h=30, d=mortise_id);
      translate([0, 0, 9]) sphere(d=mortise_id);
    }
  }
}

module drone_bass_tuner() {
  union() {
    drone_bass_tuner_barrel();
    translate([0, 0, plunge]) drone_bass_tuner_end();
    translate([0, 0, height]) drone_bass_bend();
    translate([bendr*2 - 9, 0, height-bendh-7])
      rotate([0, -90, 180]) drone_bass_resonator();
  }
}

module drone_bass() {
  drone_bass_tuner();
}

drone_bass();

