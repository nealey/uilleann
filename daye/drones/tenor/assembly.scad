include <../../common.scad>

plunge = 60;                    // How far in can the butt go?
height = 135.2;                 // as measured by my calipers

tubeh = 72;
tubed = 9.6;

endh = height - plunge;
endid = 5.0;
endod = 10.0;

module drone_tenor_tuner_barrel() {
  brass_tube(h=tubeh, d=tubed);
}

module drone_tenor_tuner_end() {
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

module drone_tenor_tuner() {
  drone_tenor_tuner_barrel();
  translate([0, 0, plunge]) drone_tenor_tuner_end();
}

module drone_tenor() {
  drone_tenor_tuner();
}

drone_tenor();
