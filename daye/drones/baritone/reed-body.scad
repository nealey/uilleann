include<../../common.scad>

id = 7.12;
od = 8;
pluglen = 5;  // Length of the plug at the end
opening = [2, 10, 6]; // Opening size
cutaway = [10, 0.74, 28]; // Tongue cutaway

module reed_body() {
  difference() {
    // The main piece
    cylinder(h=100, d=od);

    // Inner bore
    translate([0, 0, pluglen]) {
      difference() {
        cylinder(h=100, d=id);

        // Leave material for a platform on the tongue cutaway
        translate([-5, id/2 - cutaway[1], 0]) cube(cutaway);
      }
    }

    // Tongue cutaway
    translate([-5, od/2 - cutaway[1], 0]) cube(cutaway);

    // Opening
    translate([-opening[0]/2+.6, 0, pluglen]) cube(opening);
  }
}

reed_body();
