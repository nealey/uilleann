include <parts.scad>

module drone_tenor_tuner_brass() {
  brass_tube(h=72, d=9.6);
}

module drone_tenor_tuner_derlin(od=9.8, mortise_od=9.6+brass_thickness*2) {
  derlin_tube(h=69, id=5, od=od);
  translate([0, 0, 6]) derlin_stop(h=5, id=od, od=mortise_od);
  translate([0, 0, 69-bell_height]) derlin_bell(id=od);
}

module drone_tenor_tuner() {
  drone_tenor_tuner_brass();
  translate([0, 0, 66]) drone_tenor_tuner_derlin();
}

drone_tenor_tuner();
