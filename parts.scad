// Make circles circular in final renders.
$fn = $preview ? 0 : 60;

module brass() {
  color("gold") children();
}

module derlin() {
  color("steelblue") children();
}

// tube creates a hollow cylinder
module tube(h=10, id=5, od=10) {
  difference() {
    cylinder(h=h, d=od);
    cylinder(h=h, d=id);
  }
}

// default height of rings
rings_height = 1.8;

// rings of adornment.
// This engraves two rings, 1.4mm apart, at a depth of 0.4.
module rings(h=0.4, d=1, depth=0.4, spacing=1.4) {
  tube(h=h, id=d-depth, od=d);
  translate([0, 0, spacing]) tube(h=h, id=d-depth, od=d);
}

// default brass tube thickness
brass_thickness = 0.75;

// brass_tube creates a chamfered hollow cylinder with given inside diameter,
// and wall thickness of 0.75.
// Decorative rings are etched into the top and bottom.
module brass_tube(h=10, d=10, t=brass_thickness) {
  id = d;
  od = id + (t*2);
  cd = (id + od) / 2; // Chamfer diameter

  brass() {
    difference() {
      tube(h=h, id=id, od=od);

      // Chamfered ends
      translate([0, 0, 0]) cylinder(h=1, d1=cd, d2=id);
      translate([0, 0, h-1]) cylinder(h=1, d1=id, d2=cd);

      translate([0, 0, 2]) rings(d=od);
      translate([0, 0, h-2-rings_height]) rings(d=od);
    }
  }
}

// derlin_tube is a non-chamfered sky blue tube.
// I use blue so it's easier to see in preview.
module derlin_tube(h=10, id=5, od=10) {
  derlin() {
    tube(h=h, id=id, od=od);
  }
}

// derlin_stop is a derlin tube, chamfered on one end.
module derlin_stop(h=10, id=5, od=10) {
  chamferh = h/5;
  derlin() {
    difference() {
      union() {
        cylinder(h=h-chamferh, d=od);
        translate([0, 0, h-chamferh]) cylinder(h=chamferh, d1=od, d2=id);
        }
      cylinder(h=h, d=id);
      }
    }
}

// default bell height
bell_height = 17;

// derlin_bell makes a decorative flared bell.
// od will default to 33% larger than id.
module derlin_bell(h=bell_height, id=5, od=0) {
  chamferh = 1;
  chamferd = 1;
  ringsmargin = 1;
  toph = rings_height + ringsmargin*2 + chamferh;
  flareh = h - toph;
  od = (od == 0) ? (id * 1.33) : od;
  derlin() {
    difference() {
      union() {
        cylinder(h=flareh, d1=id, d2=od);
        translate([0, 0, flareh]) cylinder(h=toph-chamferh, d=od);
        translate([0, 0, flareh+toph-chamferh]) cylinder(h=chamferh, d1=od, d2=od-chamferd);
      }
      cylinder(h=h, d=id);
      translate([0, 0, flareh + ringsmargin]) rings(d=od);
    }
  }
}
