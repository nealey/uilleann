include <../../common.scad>

plunge = 103;                   // How far in can the butt go?
height = 264;                   // as measured by my calipers

tubeh = 120;
tubed = 16;

endh = height - plunge;
endid = 7.5;
endod = 13.7;

module drone_baritone_tuner_barrel() {
  brass_tube(h=tubeh, d=tubed);
}

module drone_baritone_tuner_end() {
  stopod = tubed - (clearance * 2);
  stoph = height / 30;
  stop_elevation = endh - (height - tubeh);

  union() {
    derlin_tube(h=endh, id=endid, od=endod);
    derlin_tube(h=stop_elevation, id=endid, od=tubed - clearance*2);
    translate([0, 0, stop_elevation]) derlin_stop(h=stoph, id=endod, od=tubed + brass_thickness*2);
    translate([0, 0, endh-bell_height]) derlin_bell(id=endod);
  }
}

module drone_baritone_tuner() {
  drone_baritone_tuner_barrel();
  translate([0, 0, plunge]) drone_baritone_tuner_end();
}

module drone_baritone() {
  drone_baritone_tuner();
}
drone_baritone();
