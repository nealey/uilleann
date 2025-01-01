
$fn = $preview ? 0 : 60;

// A washer: flat, like in a garden hose.
module washer(h=1, d1=0, d2=1) {
  id = (d2 > d1) ? d1 : d2;
  od = (d1 > d2) ? d1 : d2;
  difference() {
    cylinder(h=h, d=od);
    cylinder(h=h, d=id);
  }
}

// A fillet is a sort of trumpet bell shape.
module fillet(h=1, d1=1, d2=2) {
    r = abs(d1-d2)/2;
    resize([d1, d1, h]) {
        rotate_extrude() {
            translate([d2/2, 0, 0]) {
                difference() {
                    square([r, r]);
                    translate([r, r]) circle(r=r);
                }
            }
        }
    }
}
